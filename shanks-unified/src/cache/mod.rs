//! Async SQLite cache for series and acceleration results.
//!
//! Schema (v2):
//!
//! ```text
//! series        (id, name, precision, x_value, args_json, noise_json, n_points, sum_json)
//! series_data   (series_id → sn/an/dev blobs)
//! accelerations (id, series_id, accel_name, m_value, args_json,
//!                filter_type, filter_args_json, n_points)
//!   — one row per (accel, filter) combination; NULL filter_type means "no filter".
//! accel_data    (accel_id → val/an/dev blobs)
//! events        (accel_id, n, name, description)
//! ```
//!
//! All methods are `async (&self)`. The inner `tokio_rusqlite::Connection` is
//! `Arc`-backed so `Cache` is cheap to clone.

use anyhow::Result;
use rusqlite::params;
use std::path::Path;
use tokio_rusqlite::Connection;

// ---------------------------------------------------------------------------
// Plain data types (no FFI, no UniquePtr)
// ---------------------------------------------------------------------------

/// Binary representation of one Arr (matches ffi::Arr variants).
/// `kind` 0=Real 1=Complex 2=Interval 3=CInterval.
/// For Real:     m[0]=mantissa bytes, e[0]=exponent bytes.
/// For Complex:  m[0]/e[0]=real, m[1]/e[1]=imag.
/// For Interval: m[0]/e[0]=inf,  m[1]/e[1]=sup.
/// For CInterval:m[0]/e[0]=real.inf, m[1]/e[1]=real.sup,
///               m[2]/e[2]=imag.inf, m[3]/e[3]=imag.sup.
#[derive(Debug, Clone, Default)]
pub struct RawArrBlobs {
    pub kind: i64,
    pub len: i64,
    pub m: [Vec<u8>; 4],
    pub e: [Vec<u8>; 4], // reserved for future exponent storage; currently unused
}

/// Common numerical data for cache storage.
#[derive(Debug, Clone, Default)]
pub struct CachedResultData {
    pub values: RawArrBlobs,
    pub an: RawArrBlobs,
    pub deviations: RawArrBlobs,
}

/// Plain series data for cache storage.
#[derive(Debug, Clone, Default)]
pub struct CachedSeriesData {
    pub result: CachedResultData,
}

/// Plain accel data for cache storage.
#[derive(Debug, Clone, Default)]
pub struct CachedAccelData {
    pub start_offset: u64,
    pub result: CachedResultData,
}

/// One event row.
#[derive(Debug, Clone)]
pub struct CachedEvent {
    pub n: u64,
    pub name: String,
    pub description: String,
}

// ---------------------------------------------------------------------------
// Cache
// ---------------------------------------------------------------------------

/// Async SQLite cache. Cheap to clone — all clones share the same connection.
#[derive(Clone)]
pub struct Cache {
    conn: Option<Connection>,
}

impl Cache {
    /// Open (or create) the cache database and run schema migrations.
    pub async fn new(path: &Path) -> Result<Self> {
        let conn = Connection::open(path).await?;

        conn.call(|c| {
            c.execute_batch("PRAGMA journal_mode=WAL; PRAGMA synchronous=NORMAL;")
                .map_err(tokio_rusqlite::Error::Rusqlite)?;

            c.execute_batch(
                r#"
                -- v2 schema: drop old tables so we start fresh
                DROP TABLE IF EXISTS filtered_estimates;
                DROP TABLE IF EXISTS events;
                DROP TABLE IF EXISTS accel_data;
                DROP TABLE IF EXISTS accelerations;
                DROP TABLE IF EXISTS series_data;
                DROP TABLE IF EXISTS series_points;
                DROP TABLE IF EXISTS accel_points;
                DROP TABLE IF EXISTS series;

                -- Series identity
                CREATE TABLE IF NOT EXISTS series (
                    id          INTEGER PRIMARY KEY,
                    name        TEXT    NOT NULL,
                    precision   TEXT    NOT NULL,
                    x_value     TEXT    NOT NULL DEFAULT '',
                    args_json   TEXT    NOT NULL DEFAULT '{}',
                    noise_json  TEXT    NOT NULL DEFAULT '',
                    n_points    INTEGER NOT NULL DEFAULT 0,
                    sum_json    TEXT,
                    UNIQUE(name, precision, x_value, args_json, noise_json)
                );

                -- Series numerical data (blobs for sn/an/dev)
                CREATE TABLE IF NOT EXISTS series_data (
                    series_id  INTEGER PRIMARY KEY REFERENCES series(id),
                    sn_kind    INTEGER NOT NULL DEFAULT 0,
                    sn_len     INTEGER NOT NULL DEFAULT 0,
                    sn_m0 BLOB, sn_m1 BLOB, sn_m2 BLOB, sn_m3 BLOB,
                    an_kind    INTEGER NOT NULL DEFAULT 0,
                    an_len     INTEGER NOT NULL DEFAULT 0,
                    an_m0 BLOB, an_m1 BLOB, an_m2 BLOB, an_m3 BLOB,
                    dev_kind   INTEGER NOT NULL DEFAULT 0,
                    dev_len    INTEGER NOT NULL DEFAULT 0,
                    dev_m0 BLOB, dev_m1 BLOB, dev_m2 BLOB, dev_m3 BLOB
                );

                -- One row per (accel x filter) combination.
                -- filter_type IS NULL means "no filter was applied".
                CREATE TABLE IF NOT EXISTS accelerations (
                    id               INTEGER PRIMARY KEY,
                    series_id        INTEGER NOT NULL REFERENCES series(id),
                    accel_name       TEXT    NOT NULL,
                    m_value          INTEGER NOT NULL DEFAULT -1,
                    args_json        TEXT    NOT NULL DEFAULT '{}',
                    filter_type      TEXT    NOT NULL DEFAULT '',
                    filter_args_json TEXT    NOT NULL DEFAULT '',
                    n_points         INTEGER NOT NULL DEFAULT 0,
                    UNIQUE(series_id, accel_name, m_value, args_json, filter_type, filter_args_json)
                );

                -- Accel numerical data (val=accelerated sn, an, dev)
                CREATE TABLE IF NOT EXISTS accel_data (
                    accel_id  INTEGER PRIMARY KEY REFERENCES accelerations(id),
                    val_kind  INTEGER NOT NULL DEFAULT 0,
                    val_len   INTEGER NOT NULL DEFAULT 0,
                    val_m0 BLOB, val_m1 BLOB, val_m2 BLOB, val_m3 BLOB,
                    an_kind   INTEGER NOT NULL DEFAULT 0,
                    an_len    INTEGER NOT NULL DEFAULT 0,
                    an_m0 BLOB,  an_m1 BLOB,  an_m2 BLOB,  an_m3 BLOB,
                    dev_kind  INTEGER NOT NULL DEFAULT 0,
                    dev_len   INTEGER NOT NULL DEFAULT 0,
                    dev_m0 BLOB, dev_m1 BLOB, dev_m2 BLOB, dev_m3 BLOB,
                    start_n INTEGER NOT NULL DEFAULT 0
                );

                -- Events emitted during accel computation
                CREATE TABLE IF NOT EXISTS events (
                    id          INTEGER PRIMARY KEY,
                    accel_id    INTEGER NOT NULL REFERENCES accelerations(id),
                    n           INTEGER NOT NULL,
                    name        TEXT    NOT NULL,
                    description TEXT    NOT NULL DEFAULT ''
                );

                CREATE INDEX IF NOT EXISTS idx_series_name     ON series(name);
                CREATE INDEX IF NOT EXISTS idx_accel_series    ON accelerations(series_id);
                CREATE INDEX IF NOT EXISTS idx_accel_name      ON accelerations(accel_name);
                CREATE INDEX IF NOT EXISTS idx_events_accel    ON events(accel_id);
            "#,
            )
            .map_err(tokio_rusqlite::Error::Rusqlite)?;

            Ok(())
        })
        .await?;

        Ok(Self { conn: Some(conn) })
    }

    /// Create a no-op cache (all reads return None/empty; writes silently dropped).
    pub fn disabled() -> Self {
        Self { conn: None }
    }

    #[inline]
    pub fn is_enabled(&self) -> bool {
        self.conn.is_some()
    }

    // -----------------------------------------------------------------------
    // Series
    // -----------------------------------------------------------------------

    /// Returns `(id, n_points)` if a matching series row exists.
    pub async fn series_exists(
        &self,
        name: String,
        precision: String,
        x_value: String,
        args_json: String,
        noise_json: Option<String>,
    ) -> Result<Option<(i64, u64, Option<String>)>> {
        let Some(conn) = &self.conn else {
            return Ok(None);
        };
        conn.call(move |c| {
            let res = c.query_row(
                "SELECT id, n_points, sum_json FROM series \
                 WHERE name=?1 AND precision=?2 AND x_value=?3 \
                   AND args_json=?4 AND noise_json=?5",
                params![
                    name,
                    precision,
                    x_value,
                    args_json,
                    noise_json.unwrap_or_default()
                ],
                |r| {
                    Ok((
                        r.get::<_, i64>(0)?,
                        r.get::<_, i64>(1)? as u64,
                        r.get::<_, Option<String>>(2)?,
                    ))
                },
            );
            match res {
                Ok(v) => Ok(Some(v)),
                Err(rusqlite::Error::QueryReturnedNoRows) => Ok(None),
                Err(e) => Err(tokio_rusqlite::Error::Rusqlite(e)),
            }
        })
        .await
        .map_err(Into::into)
    }

    /// Insert (or update n_points) a series row.  Returns the row id.
    pub async fn upsert_series(
        &self,
        name: String,
        precision: String,
        x_value: String,
        args_json: String,
        noise_json: Option<String>,
        n_points: u64,
        sum_json: Option<String>,
    ) -> Result<i64> {
        let Some(conn) = &self.conn else {
            return Ok(-1);
        };
        conn.call(move |c| {
            let tx = c.transaction()?;
            tx.execute(
                "INSERT INTO series (name,precision,x_value,args_json,noise_json,n_points,sum_json)
                 VALUES (?1,?2,?3,?4,?5,?6,?7)
                 ON CONFLICT(name,precision,x_value,args_json,noise_json)
                 DO UPDATE SET
                     n_points = MAX(excluded.n_points, series.n_points),
                     sum_json = COALESCE(excluded.sum_json, series.sum_json)",
                params![
                    name,
                    precision,
                    x_value,
                    args_json,
                    noise_json.clone().unwrap_or_default(),
                    n_points,
                    sum_json
                ],
            )?;
            let id: i64 = tx.query_row(
                "SELECT id FROM series \
                 WHERE name=?1 AND precision=?2 AND x_value=?3 \
                   AND args_json=?4 AND noise_json=?5",
                params![
                    name,
                    precision,
                    x_value,
                    args_json,
                    noise_json.unwrap_or_default()
                ],
                |r| r.get(0),
            )?;
            tx.commit()?;
            Ok(id)
        })
        .await
        .map_err(Into::into)
    }

    /// Load series blob data.
    pub async fn get_series_data(&self, series_id: i64) -> Result<Option<CachedSeriesData>> {
        let Some(conn) = &self.conn else {
            return Ok(None);
        };
        conn.call(move |c| {
            let res = c.query_row(
                "SELECT sn_kind,sn_len,sn_m0,sn_m1,sn_m2,sn_m3,
                        an_kind,an_len,an_m0,an_m1,an_m2,an_m3,
                        dev_kind,dev_len,dev_m0,dev_m1,dev_m2,dev_m3
                 FROM series_data WHERE series_id=?1",
                params![series_id],
                |r| {
                    let load = |base: usize| -> rusqlite::Result<RawArrBlobs> {
                        Ok(RawArrBlobs {
                            kind: r.get(base)?,
                            len: r.get(base + 1)?,
                            m: [
                                r.get::<_, Vec<u8>>(base + 2).unwrap_or_default(),
                                r.get::<_, Vec<u8>>(base + 3).unwrap_or_default(),
                                r.get::<_, Vec<u8>>(base + 4).unwrap_or_default(),
                                r.get::<_, Vec<u8>>(base + 5).unwrap_or_default(),
                            ],
                            e: Default::default(),
                        })
                    };
                    Ok(CachedSeriesData {
                        result: CachedResultData {
                            values: load(0)?,
                            an: load(6)?,
                            deviations: load(12)?,
                        },
                    })
                },
            );
            match res {
                Ok(d) => Ok(Some(d)),
                Err(rusqlite::Error::QueryReturnedNoRows) => Ok(None),
                Err(e) => Err(tokio_rusqlite::Error::Rusqlite(e)),
            }
        })
        .await
        .map_err(Into::into)
    }

    /// Store (replace) series blob data.
    pub async fn insert_series_data(&self, series_id: i64, data: CachedSeriesData) -> Result<()> {
        let Some(conn) = &self.conn else {
            return Ok(());
        };
        conn.call(move |c| {
            let d = &data;
            c.execute(
                "INSERT OR REPLACE INTO series_data
                 (series_id,
                  sn_kind,sn_len,sn_m0,sn_m1,sn_m2,sn_m3,
                  an_kind,an_len,an_m0,an_m1,an_m2,an_m3,
                  dev_kind,dev_len,dev_m0,dev_m1,dev_m2,dev_m3)
                 VALUES (?1,?2,?3,?4,?5,?6,?7,?8,?9,?10,?11,?12,?13,?14,?15,?16,?17,?18,?19)",
                params![
                    series_id,
                    d.result.values.kind,
                    d.result.values.len,
                    d.result.values.m[0],
                    d.result.values.m[1],
                    d.result.values.m[2],
                    d.result.values.m[3],
                    d.result.an.kind,
                    d.result.an.len,
                    d.result.an.m[0],
                    d.result.an.m[1],
                    d.result.an.m[2],
                    d.result.an.m[3],
                    d.result.deviations.kind,
                    d.result.deviations.len,
                    d.result.deviations.m[0],
                    d.result.deviations.m[1],
                    d.result.deviations.m[2],
                    d.result.deviations.m[3],
                ],
            )
            .map_err(tokio_rusqlite::Error::Rusqlite)?;
            Ok(())
        })
        .await
        .map_err(Into::into)
    }

    // -----------------------------------------------------------------------
    // Accelerations
    // -----------------------------------------------------------------------

    /// Returns `(id, n_points)` if a matching `(accel, filter)` row exists.
    pub async fn accel_exists(
        &self,
        series_id: i64,
        accel_name: String,
        m_value: Option<i64>,
        args_json: String,
        filter_type: Option<String>,
        filter_args_json: Option<String>,
    ) -> Result<Option<(i64, u64)>> {
        let Some(conn) = &self.conn else {
            return Ok(None);
        };
        conn.call(move |c| {
            let res = c.query_row(
                "SELECT id, n_points FROM accelerations \
                 WHERE series_id=?1 AND accel_name=?2 \
                   AND m_value=?3 AND args_json=?4 \
                   AND filter_type=?5 \
                   AND filter_args_json=?6",
                params![
                    series_id,
                    accel_name,
                    m_value.unwrap_or(-1),
                    args_json,
                    filter_type.unwrap_or_default(),
                    filter_args_json.unwrap_or_default()
                ],
                |r| Ok((r.get::<_, i64>(0)?, r.get::<_, i64>(1)? as u64)),
            );
            match res {
                Ok(v) => Ok(Some(v)),
                Err(rusqlite::Error::QueryReturnedNoRows) => Ok(None),
                Err(e) => Err(tokio_rusqlite::Error::Rusqlite(e)),
            }
        })
        .await
        .map_err(Into::into)
    }

    /// Insert (or update n_points) an `(accel, filter)` row.  Returns the row id.
    pub async fn upsert_accel(
        &self,
        series_id: i64,
        accel_name: String,
        m_value: Option<i64>,
        args_json: String,
        filter_type: Option<String>,
        filter_args_json: Option<String>,
        n_points: u64,
    ) -> Result<i64> {
        let Some(conn) = &self.conn else {
            return Ok(-1);
        };
        conn.call(move |c| {
            let tx = c.transaction()?;
            tx.execute(
                "INSERT INTO accelerations
                 (series_id,accel_name,m_value,args_json,filter_type,filter_args_json,n_points)
                 VALUES (?1,?2,?3,?4,?5,?6,?7)
                 ON CONFLICT(series_id,accel_name,m_value,args_json,filter_type,filter_args_json)
                 DO UPDATE SET n_points = MAX(excluded.n_points, accelerations.n_points)",
                params![
                    series_id,
                    accel_name,
                    m_value.unwrap_or(-1),
                    args_json,
                    filter_type.clone().unwrap_or_default(),
                    filter_args_json.clone().unwrap_or_default(),
                    n_points
                ],
            )?;
            let id: i64 = tx.query_row(
                "SELECT id FROM accelerations \
                 WHERE series_id=?1 AND accel_name=?2 \
                   AND m_value=?3 AND args_json=?4 \
                   AND filter_type=?5 \
                   AND filter_args_json=?6",
                params![
                    series_id,
                    accel_name,
                    m_value.unwrap_or(-1),
                    args_json,
                    filter_type.unwrap_or_default(),
                    filter_args_json.unwrap_or_default()
                ],
                |r| r.get(0),
            )?;
            tx.commit()?;
            Ok(id)
        })
        .await
        .map_err(Into::into)
    }

    /// Load accel blob data.
    pub async fn get_accel_data(&self, accel_id: i64) -> Result<Option<CachedAccelData>> {
        let Some(conn) = &self.conn else {
            return Ok(None);
        };
        conn.call(move |c| {
            let res = c.query_row(
                "SELECT val_kind,val_len,val_m0,val_m1,val_m2,val_m3,
                        an_kind,an_len,an_m0,an_m1,an_m2,an_m3,
                        dev_kind,dev_len,dev_m0,dev_m1,dev_m2,dev_m3,start_n
                 FROM accel_data WHERE accel_id=?1",
                params![accel_id],
                |r| {
                    let load = |base: usize| -> rusqlite::Result<RawArrBlobs> {
                        Ok(RawArrBlobs {
                            kind: r.get(base)?,
                            len: r.get(base + 1)?,
                            m: [
                                r.get::<_, Vec<u8>>(base + 2).unwrap_or_default(),
                                r.get::<_, Vec<u8>>(base + 3).unwrap_or_default(),
                                r.get::<_, Vec<u8>>(base + 4).unwrap_or_default(),
                                r.get::<_, Vec<u8>>(base + 5).unwrap_or_default(),
                            ],
                            e: Default::default(),
                        })
                    };
                    Ok(CachedAccelData {
                        result: CachedResultData {
                            values: load(0)?,
                            an: load(6)?,
                            deviations: load(12)?,
                        },
                        start_offset: r.get::<_, u64>(18).unwrap_or(0),
                    })
                },
            );
            match res {
                Ok(d) => Ok(Some(d)),
                Err(rusqlite::Error::QueryReturnedNoRows) => Ok(None),
                Err(e) => Err(tokio_rusqlite::Error::Rusqlite(e)),
            }
        })
        .await
        .map_err(Into::into)
    }

    /// Store (replace) accel blob data.
    pub async fn insert_accel_data(&self, accel_id: i64, data: CachedAccelData) -> Result<()> {
        let Some(conn) = &self.conn else {
            return Ok(());
        };
        conn.call(move |c| {
            c.execute(
                "INSERT OR REPLACE INTO accel_data
                 (accel_id,
                  val_kind,val_len,val_m0,val_m1,val_m2,val_m3,
                  an_kind,an_len,an_m0,an_m1,an_m2,an_m3,
                  dev_kind,dev_len,dev_m0,dev_m1,dev_m2,dev_m3,start_n)
                 VALUES (?1,?2,?3,?4,?5,?6,?7,?8,?9,?10,?11,?12,?13,?14,?15,?16,?17,?18,?19,?20)",
                params![
                    accel_id,
                    data.result.values.kind,
                    data.result.values.len,
                    data.result.values.m[0],
                    data.result.values.m[1],
                    data.result.values.m[2],
                    data.result.values.m[3],
                    data.result.an.kind,
                    data.result.an.len,
                    data.result.an.m[0],
                    data.result.an.m[1],
                    data.result.an.m[2],
                    data.result.an.m[3],
                    data.result.deviations.kind,
                    data.result.deviations.len,
                    data.result.deviations.m[0],
                    data.result.deviations.m[1],
                    data.result.deviations.m[2],
                    data.result.deviations.m[3],
                    data.start_offset
                ],
            )
            .map_err(tokio_rusqlite::Error::Rusqlite)?;
            Ok(())
        })
        .await
        .map_err(Into::into)
    }

    // -----------------------------------------------------------------------
    // Events
    // -----------------------------------------------------------------------

    /// Insert events for an accel row (replaces all existing events for that accel).
    pub async fn insert_events(&self, accel_id: i64, events: Vec<CachedEvent>) -> Result<()> {
        let Some(conn) = &self.conn else {
            return Ok(());
        };
        conn.call(move |c| {
            let tx = c.transaction()?;
            tx.execute("DELETE FROM events WHERE accel_id=?1", params![accel_id])?;
            for ev in &events {
                tx.execute(
                    "INSERT INTO events (accel_id,n,name,description) VALUES (?1,?2,?3,?4)",
                    params![accel_id, ev.n as i64, ev.name, ev.description],
                )?;
            }
            tx.commit()?;
            Ok(())
        })
        .await
        .map_err(Into::into)
    }

    /// Load events for an accel row.
    pub async fn get_events(&self, accel_id: i64) -> Result<Vec<CachedEvent>> {
        let Some(conn) = &self.conn else {
            return Ok(Vec::new());
        };
        conn.call(move |c| {
            let mut stmt =
                c.prepare("SELECT n, name, description FROM events WHERE accel_id=?1 ORDER BY n")?;
            let rows = stmt.query_map(params![accel_id], |r| {
                Ok(CachedEvent {
                    n: r.get::<_, i64>(0)? as u64,
                    name: r.get(1)?,
                    description: r.get::<_, Option<String>>(2)?.unwrap_or_default(),
                })
            })?;
            rows.collect::<rusqlite::Result<Vec<_>>>()
                .map_err(tokio_rusqlite::Error::Rusqlite)
        })
        .await
        .map_err(Into::into)
    }

    // -----------------------------------------------------------------------
    // Misc
    // -----------------------------------------------------------------------

    pub async fn clear_all(&self) -> Result<()> {
        let Some(conn) = &self.conn else {
            return Ok(());
        };
        conn.call(|c| {
            c.execute_batch(
                "DELETE FROM events;
                 DELETE FROM accel_data;
                 DELETE FROM accelerations;
                 DELETE FROM series_data;
                 DELETE FROM series;",
            )
            .map_err(tokio_rusqlite::Error::Rusqlite)
        })
        .await
        .map_err(Into::into)
    }
}
