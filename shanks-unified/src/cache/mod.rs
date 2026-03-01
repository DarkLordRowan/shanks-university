//! Async SQLite cache for series and acceleration results.
//!
//! Uses `tokio-rusqlite` — the connection lives on a dedicated OS thread,
//! accessed entirely via `.call(|conn| …).await`. Callers never block.
//!
//! All methods take `&self` and are `async`.  `Cache` is `Clone + Send + Sync`
//! (the inner `tokio_rusqlite::Connection` is already `Arc`-backed).

use anyhow::Result;
use rusqlite::params;
use std::path::Path;
use tokio_rusqlite::Connection;

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
            // WAL for low-latency concurrent reads
            c.execute_batch("PRAGMA journal_mode=WAL; PRAGMA synchronous=NORMAL;")?;

            c.execute_batch(
                r#"
                CREATE TABLE IF NOT EXISTS series (
                    id          INTEGER PRIMARY KEY,
                    name        TEXT    NOT NULL,
                    precision   TEXT    NOT NULL,
                    x_value     TEXT,
                    arguments   JSON,
                    noise_config JSON,
                    profiling   JSON,
                    created_at  TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
                    sum         TEXT
                );
                CREATE UNIQUE INDEX IF NOT EXISTS idx_series_unique
                    ON series(name, precision, x_value, arguments, IFNULL(noise_config,''));

                DROP TABLE IF EXISTS series_points;
                DROP TABLE IF EXISTS accel_points;

                CREATE TABLE IF NOT EXISTS series_data (
                    series_id   INTEGER PRIMARY KEY,
                    sn_type     INTEGER, sn_len   INTEGER,
                    sn_m0 BLOB, sn_e0 BLOB, sn_m1 BLOB, sn_e1 BLOB,
                    sn_m2 BLOB, sn_e2 BLOB, sn_m3 BLOB, sn_e3 BLOB,
                    an_type     INTEGER, an_len   INTEGER,
                    an_m0 BLOB, an_e0 BLOB, an_m1 BLOB, an_e1 BLOB,
                    an_m2 BLOB, an_e2 BLOB, an_m3 BLOB, an_e3 BLOB,
                    dev_type    INTEGER DEFAULT 0, dev_len INTEGER,
                    dev_m0 BLOB, dev_e0 BLOB, dev_m1 BLOB, dev_e1 BLOB,
                    dev_m2 BLOB, dev_e2 BLOB, dev_m3 BLOB, dev_e3 BLOB,
                    FOREIGN KEY (series_id) REFERENCES series(id)
                );

                CREATE TABLE IF NOT EXISTS accelerations (
                    id           INTEGER PRIMARY KEY,
                    series_id    INTEGER NOT NULL,
                    accel_name   TEXT    NOT NULL,
                    m_value      INTEGER,
                    additional_args JSON,
                    profiling    JSON,
                    created_at   TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
                    FOREIGN KEY (series_id) REFERENCES series(id)
                );

                CREATE TABLE IF NOT EXISTS accel_data (
                    accel_id    INTEGER PRIMARY KEY,
                    val_type    INTEGER, val_len  INTEGER,
                    val_m0 BLOB, val_e0 BLOB, val_m1 BLOB, val_e1 BLOB,
                    val_m2 BLOB, val_e2 BLOB, val_m3 BLOB, val_e3 BLOB,
                    dev_type    INTEGER DEFAULT 0, dev_len INTEGER,
                    dev_m0 BLOB, dev_e0 BLOB, dev_m1 BLOB, dev_e1 BLOB,
                    dev_m2 BLOB, dev_e2 BLOB, dev_m3 BLOB, dev_e3 BLOB,
                    FOREIGN KEY (accel_id) REFERENCES accelerations(id)
                );

                CREATE TABLE IF NOT EXISTS events (
                    id          INTEGER PRIMARY KEY,
                    series_id   INTEGER,
                    accel_id    INTEGER,
                    n           INTEGER,
                    event_type  TEXT,
                    name        TEXT,
                    description TEXT,
                    FOREIGN KEY (series_id) REFERENCES series(id),
                    FOREIGN KEY (accel_id)  REFERENCES accelerations(id)
                );

                CREATE TABLE IF NOT EXISTS filtered_estimates (
                    id          INTEGER PRIMARY KEY,
                    accel_id    INTEGER NOT NULL,
                    event_name  TEXT    NOT NULL,
                    filter      TEXT    NOT NULL,
                    limit_points JSON  NOT NULL,
                    start_n     INTEGER NOT NULL,
                    length      INTEGER NOT NULL,
                    FOREIGN KEY (accel_id) REFERENCES accelerations(id)
                );

                CREATE INDEX IF NOT EXISTS idx_series_name      ON series(name);
                CREATE INDEX IF NOT EXISTS idx_series_precision  ON series(precision);
                CREATE INDEX IF NOT EXISTS idx_accel_name        ON accelerations(accel_name);
                CREATE INDEX IF NOT EXISTS idx_accel_series      ON accelerations(series_id);
                "#,
            )?;
            Ok(())
        })
        .await?;

        Ok(Self { conn: Some(conn) })
    }

    /// Create a no-op cache that silently discards all writes and returns `None` on reads.
    pub fn disabled() -> Self {
        Self { conn: None }
    }

    // -----------------------------------------------------------------------
    // Helpers
    // -----------------------------------------------------------------------

    /// Returns `true` if caching is enabled.
    #[inline]
    pub fn is_enabled(&self) -> bool {
        self.conn.is_some()
    }

    // -----------------------------------------------------------------------
    // Series
    // -----------------------------------------------------------------------

    /// Insert (or retrieve if already exists) a series row.  Returns the row id.
    pub async fn upsert_series(
        &self,
        name: String,
        precision: String,
        x_value: String,
        arguments: String,
        noise_config: Option<String>,
        sum_val: Option<String>,
    ) -> Result<i64> {
        let Some(conn) = &self.conn else {
            return Ok(0);
        };

        conn.call(move |c| {
            let tx = c.transaction()?;

            let rows = tx.execute(
                "INSERT OR IGNORE INTO series
                 (name, precision, x_value, arguments, noise_config, sum)
                 VALUES (?1,?2,?3,?4,?5,?6)",
                params![name, precision, x_value, arguments, noise_config, sum_val],
            )?;

            let id: i64 = if rows == 0 {
                // Already exists — retrieve id
                match &noise_config {
                    Some(nc) => tx.query_row(
                        "SELECT id FROM series \
                         WHERE name=?1 AND precision=?2 AND x_value=?3 \
                         AND arguments=?4 AND noise_config=?5",
                        params![name, precision, x_value, arguments, nc],
                        |r| r.get(0),
                    )?,
                    None => tx.query_row(
                        "SELECT id FROM series \
                         WHERE name=?1 AND precision=?2 AND x_value=?3 \
                         AND arguments=?4 AND noise_config IS NULL",
                        params![name, precision, x_value, arguments],
                        |r| r.get(0),
                    )?,
                }
            } else {
                tx.last_insert_rowid()
            };

            // Backfill sum if the row pre-dated it
            if let Some(s) = sum_val {
                tx.execute(
                    "UPDATE series SET sum=?1 WHERE id=?2 AND sum IS NULL",
                    params![s, id],
                )?;
            }

            tx.commit()?;
            Ok(id)
        })
        .await
        .map_err(Into::into)
    }

    /// Check whether a series is cached.  Returns `Some(id)` if found.
    pub async fn series_exists(
        &self,
        name: String,
        precision: String,
        x_value: String,
        arguments: String,
        noise_config: Option<String>,
    ) -> Result<Option<i64>> {
        let Some(conn) = &self.conn else {
            return Ok(None);
        };

        conn.call(move |c| {
            let res = match &noise_config {
                Some(nc) => c.query_row(
                    "SELECT id FROM series \
                     WHERE name=?1 AND precision=?2 AND x_value=?3 \
                     AND arguments=?4 AND noise_config=?5",
                    params![name, precision, x_value, arguments, nc],
                    |r| r.get(0),
                ),
                None => c.query_row(
                    "SELECT id FROM series \
                     WHERE name=?1 AND precision=?2 AND x_value=?3 \
                     AND arguments=?4 AND noise_config IS NULL",
                    params![name, precision, x_value, arguments],
                    |r| r.get(0),
                ),
            };
            match res {
                Ok(id) => Ok(Some(id)),
                Err(rusqlite::Error::QueryReturnedNoRows) => Ok(None),
                Err(e) => Err(tokio_rusqlite::Error::Rusqlite(e)),
            }
        })
        .await
        .map_err(Into::into)
    }

    /// Store raw binary series data (sn / an / deviations), keyed by series_id.
    pub async fn insert_series_data(&self, series_id: i64, data: SeriesData) -> Result<()> {
        let Some(conn) = &self.conn else {
            return Ok(());
        };

        conn.call(move |c| {
            c.execute(
                "INSERT OR REPLACE INTO series_data (
                    series_id,
                    sn_type,  sn_len,
                    sn_m0,  sn_e0,  sn_m1,  sn_e1,  sn_m2,  sn_e2,  sn_m3,  sn_e3,
                    an_type,  an_len,
                    an_m0,  an_e0,  an_m1,  an_e1,  an_m2,  an_e2,  an_m3,  an_e3,
                    dev_type, dev_len,
                    dev_m0, dev_e0, dev_m1, dev_e1, dev_m2, dev_e2, dev_m3, dev_e3
                 ) VALUES (
                    ?1,
                    ?2,?3,   ?4,?5,?6,?7,?8,?9,?10,?11,
                    ?12,?13, ?14,?15,?16,?17,?18,?19,?20,?21,
                    ?22,?23, ?24,?25,?26,?27,?28,?29,?30,?31
                 )",
                params![
                    series_id,
                    data.sn.kind, data.sn.len,
                    data.sn.m[0], data.sn.e[0], data.sn.m[1], data.sn.e[1],
                    data.sn.m[2], data.sn.e[2], data.sn.m[3], data.sn.e[3],
                    data.an.kind, data.an.len,
                    data.an.m[0], data.an.e[0], data.an.m[1], data.an.e[1],
                    data.an.m[2], data.an.e[2], data.an.m[3], data.an.e[3],
                    data.dev.kind, data.dev.len,
                    data.dev.m[0], data.dev.e[0], data.dev.m[1], data.dev.e[1],
                    data.dev.m[2], data.dev.e[2], data.dev.m[3], data.dev.e[3],
                ],
            )?;
            Ok(())
        })
        .await
        .map_err(Into::into)
    }

    /// Load raw series data.
    pub async fn get_series_data(&self, series_id: i64) -> Result<Option<(String, SeriesData)>> {
        let Some(conn) = &self.conn else {
            return Ok(None);
        };

        conn.call(move |c| {
            // Also pull sum alongside the blobs
            let sum: Option<String> = c
                .query_row(
                    "SELECT sum FROM series WHERE id=?1",
                    params![series_id],
                    |r| r.get(0),
                )
                .unwrap_or(None);

            let result = c.query_row(
                "SELECT
                    sn_type,  sn_len,
                    sn_m0,  sn_e0,  sn_m1,  sn_e1,  sn_m2,  sn_e2,  sn_m3,  sn_e3,
                    an_type,  an_len,
                    an_m0,  an_e0,  an_m1,  an_e1,  an_m2,  an_e2,  an_m3,  an_e3,
                    dev_type, dev_len,
                    dev_m0, dev_e0, dev_m1, dev_e1, dev_m2, dev_e2, dev_m3, dev_e3
                 FROM series_data WHERE series_id=?1",
                params![series_id],
                |row| {
                    let load_arr = |base: usize| -> rusqlite::Result<RawArrBlobs> {
                        Ok(RawArrBlobs {
                            kind: row.get(base)?,
                            len:  row.get(base + 1)?,
                            m: [row.get(base+2)?, row.get(base+4)?, row.get(base+6)?, row.get(base+8)?],
                            e: [row.get(base+3)?, row.get(base+5)?, row.get(base+7)?, row.get(base+9)?],
                        })
                    };
                    Ok(SeriesData {
                        sn:  load_arr(0)?,
                        an:  load_arr(10)?,
                        dev: load_arr(20)?,
                    })
                },
            );
            match result {
                Ok(data)  => Ok(Some((sum.unwrap_or_default(), data))),
                Err(rusqlite::Error::QueryReturnedNoRows) => Ok(None),
                Err(e) => Err(tokio_rusqlite::Error::Rusqlite(e)),
            }
        })
        .await
        .map_err(Into::into)
    }

    // -----------------------------------------------------------------------
    // Accelerations
    // -----------------------------------------------------------------------

    /// Insert (or retrieve) an acceleration row.
    pub async fn upsert_acceleration(
        &self,
        series_id: i64,
        accel_name: String,
        m_value: Option<i64>,
        additional_args: String,
    ) -> Result<i64> {
        let Some(conn) = &self.conn else {
            return Ok(0);
        };

        conn.call(move |c| {
            let tx = c.transaction()?;
            tx.execute(
                "INSERT OR IGNORE INTO accelerations \
                 (series_id, accel_name, m_value, additional_args) \
                 VALUES (?1,?2,?3,?4)",
                params![series_id, accel_name, m_value, additional_args],
            )?;
            let id: i64 = tx.query_row(
                "SELECT id FROM accelerations \
                 WHERE series_id=?1 AND accel_name=?2 \
                 AND (m_value=?3 OR (m_value IS NULL AND ?3 IS NULL)) \
                 AND additional_args=?4",
                params![series_id, accel_name, m_value, additional_args],
                |r| r.get(0),
            )?;
            tx.commit()?;
            Ok(id)
        })
        .await
        .map_err(Into::into)
    }

    /// Check whether an acceleration is cached.
    pub async fn acceleration_exists(
        &self,
        series_id: i64,
        accel_name: String,
        m_value: Option<i64>,
        additional_args: String,
    ) -> Result<Option<i64>> {
        let Some(conn) = &self.conn else {
            return Ok(None);
        };

        conn.call(move |c| {
            let res = c.query_row(
                "SELECT id FROM accelerations \
                 WHERE series_id=?1 AND accel_name=?2 \
                 AND (m_value=?3 OR (m_value IS NULL AND ?3 IS NULL)) \
                 AND additional_args=?4",
                params![series_id, accel_name, m_value, additional_args],
                |r| r.get(0),
            );
            match res {
                Ok(id) => Ok(Some(id)),
                Err(rusqlite::Error::QueryReturnedNoRows) => Ok(None),
                Err(e) => Err(tokio_rusqlite::Error::Rusqlite(e)),
            }
        })
        .await
        .map_err(Into::into)
    }

    /// Store binary accel data.
    pub async fn insert_accel_data(&self, accel_id: i64, data: AccelData) -> Result<()> {
        let Some(conn) = &self.conn else {
            return Ok(());
        };

        conn.call(move |c| {
            c.execute(
                "INSERT OR REPLACE INTO accel_data (
                    accel_id,
                    val_type,  val_len,
                    val_m0, val_e0, val_m1, val_e1, val_m2, val_e2, val_m3, val_e3,
                    dev_type,  dev_len,
                    dev_m0, dev_e0, dev_m1, dev_e1, dev_m2, dev_e2, dev_m3, dev_e3
                 ) VALUES (
                    ?1,
                    ?2,?3,  ?4,?5,?6,?7,?8,?9,?10,?11,
                    ?12,?13, ?14,?15,?16,?17,?18,?19,?20,?21
                 )",
                params![
                    accel_id,
                    data.val.kind, data.val.len,
                    data.val.m[0], data.val.e[0], data.val.m[1], data.val.e[1],
                    data.val.m[2], data.val.e[2], data.val.m[3], data.val.e[3],
                    data.dev.kind, data.dev.len,
                    data.dev.m[0], data.dev.e[0], data.dev.m[1], data.dev.e[1],
                    data.dev.m[2], data.dev.e[2], data.dev.m[3], data.dev.e[3],
                ],
            )?;
            Ok(())
        })
        .await
        .map_err(Into::into)
    }

    /// Load binary accel data.
    pub async fn get_accel_data(&self, accel_id: i64) -> Result<Option<AccelData>> {
        let Some(conn) = &self.conn else {
            return Ok(None);
        };

        conn.call(move |c| {
            let result = c.query_row(
                "SELECT
                    val_type,  val_len,
                    val_m0, val_e0, val_m1, val_e1, val_m2, val_e2, val_m3, val_e3,
                    dev_type,  dev_len,
                    dev_m0, dev_e0, dev_m1, dev_e1, dev_m2, dev_e2, dev_m3, dev_e3
                 FROM accel_data WHERE accel_id=?1",
                params![accel_id],
                |row| {
                    let load = |base: usize| -> rusqlite::Result<RawArrBlobs> {
                        Ok(RawArrBlobs {
                            kind: row.get(base)?,
                            len:  row.get(base + 1)?,
                            m: [row.get(base+2)?, row.get(base+4)?, row.get(base+6)?, row.get(base+8)?],
                            e: [row.get(base+3)?, row.get(base+5)?, row.get(base+7)?, row.get(base+9)?],
                        })
                    };
                    Ok(AccelData { val: load(0)?, dev: load(10)? })
                },
            );
            match result {
                Ok(d)  => Ok(Some(d)),
                Err(rusqlite::Error::QueryReturnedNoRows) => Ok(None),
                Err(e) => Err(tokio_rusqlite::Error::Rusqlite(e)),
            }
        })
        .await
        .map_err(Into::into)
    }

    // -----------------------------------------------------------------------
    // Events / filtered estimates
    // -----------------------------------------------------------------------

    pub async fn insert_events(&self, accel_id: i64, events: Vec<CachedEvent>) -> Result<()> {
        let Some(conn) = &self.conn else {
            return Ok(());
        };
        conn.call(move |c| {
            let tx = c.transaction()?;
            for ev in &events {
                tx.execute(
                    "INSERT INTO events (accel_id, n, name, description) VALUES (?1,?2,?3,?4)",
                    params![accel_id, ev.n, ev.name, ev.description],
                )?;
            }
            tx.commit()?;
            Ok(())
        })
        .await
        .map_err(Into::into)
    }

    pub async fn insert_filtered_estimates(
        &self,
        accel_id: i64,
        estimates: Vec<CachedEstimate>,
    ) -> Result<()> {
        let Some(conn) = &self.conn else {
            return Ok(());
        };
        conn.call(move |c| {
            let tx = c.transaction()?;
            for est in &estimates {
                tx.execute(
                    "INSERT INTO filtered_estimates \
                     (accel_id, event_name, filter, limit_points, start_n, length) \
                     VALUES (?1,?2,?3,?4,?5,?6)",
                    params![
                        accel_id, est.event_name, est.filter,
                        est.limit_points_json, est.start_n, est.length
                    ],
                )?;
            }
            tx.commit()?;
            Ok(())
        })
        .await
        .map_err(Into::into)
    }

    pub async fn get_events(&self, accel_id: i64) -> Result<Vec<CachedEvent>> {
        let Some(conn) = &self.conn else {
            return Ok(Vec::new());
        };
        conn.call(move |c| {
            let mut stmt = c.prepare(
                "SELECT n, name, description FROM events WHERE accel_id=?1 ORDER BY n",
            )?;
            let rows = stmt.query_map(params![accel_id], |r| {
                Ok(CachedEvent {
                    n:           r.get::<_, i64>(0)? as u64,
                    name:        r.get(1)?,
                    description: r.get::<_, Option<String>>(2)?.unwrap_or_default(),
                })
            })?;
            rows.collect::<rusqlite::Result<Vec<_>>>().map_err(tokio_rusqlite::Error::Rusqlite)
        })
        .await
        .map_err(Into::into)
    }

    pub async fn get_filtered_estimates(&self, accel_id: i64) -> Result<Vec<CachedEstimate>> {
        let Some(conn) = &self.conn else {
            return Ok(Vec::new());
        };
        conn.call(move |c| {
            let mut stmt = c.prepare(
                "SELECT event_name, filter, limit_points, start_n, length \
                 FROM filtered_estimates WHERE accel_id=?1 ORDER BY start_n",
            )?;
            let rows = stmt.query_map(params![accel_id], |r| {
                Ok(CachedEstimate {
                    event_name:       r.get(0)?,
                    filter:           r.get(1)?,
                    limit_points_json: r.get(2)?,
                    start_n:          r.get::<_, i64>(3)? as u64,
                    length:           r.get::<_, i64>(4)? as u64,
                })
            })?;
            rows.collect::<rusqlite::Result<Vec<_>>>().map_err(tokio_rusqlite::Error::Rusqlite)
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
                "DELETE FROM filtered_estimates;
                 DELETE FROM events;
                 DELETE FROM accel_data;
                 DELETE FROM accelerations;
                 DELETE FROM series_data;
                 DELETE FROM series;",
            ).map_err(tokio_rusqlite::Error::Rusqlite)
        })
        .await
        .map_err(Into::into)
    }

    pub async fn stats(&self) -> Result<CacheStats> {
        let Some(conn) = &self.conn else {
            return Ok(CacheStats::default());
        };
        conn.call(|c| {
            let series_count: i64 =
                c.query_row("SELECT COUNT(*) FROM series", [], |r| r.get(0))?;
            let accel_count: i64 =
                c.query_row("SELECT COUNT(*) FROM accelerations", [], |r| r.get(0))?;
            Ok(CacheStats { series_count, accel_count })
        })
        .await
        .map_err(Into::into)
    }

    /// Needed to call schema init separately (for construction in sync contexts).
    /// Prefer `Cache::new` which handles this automatically.
    pub async fn initialize_schema(&self) -> Result<()> {
        // Schema is always applied in new(); this is a no-op kept for API compatibility.
        Ok(())
    }
}

// ---------------------------------------------------------------------------
// Plain data types for cache I/O (no FFI dependencies here)
// ---------------------------------------------------------------------------

/// Raw blob representation of one array (sn / an / dev / val).
/// Each array has up to 4 mantissa/exponent blob pairs (for CInterval).
#[derive(Debug, Clone, Default)]
pub struct RawArrBlobs {
    pub kind: u32,
    pub len:  i64,
    pub m: [Vec<u8>; 4],
    pub e: [Vec<u8>; 4],
}

/// Three arrays for a cached series result.
#[derive(Debug, Clone, Default)]
pub struct SeriesData {
    pub sn:  RawArrBlobs,
    pub an:  RawArrBlobs,
    pub dev: RawArrBlobs,
}

/// Two arrays for a cached acceleration result.
#[derive(Debug, Clone, Default)]
pub struct AccelData {
    pub val: RawArrBlobs,
    pub dev: RawArrBlobs,
}

/// A computation event stored in cache.
#[derive(Debug, Clone)]
pub struct CachedEvent {
    pub n:           u64,
    pub name:        String,
    pub description: String,
}

/// A filtered/smoothed estimate stored in cache.
#[derive(Debug, Clone)]
pub struct CachedEstimate {
    pub event_name:        String,
    pub filter:            String,
    /// Pre-serialised JSON of limit points (avoids a serde dep here).
    pub limit_points_json: String,
    pub start_n:  u64,
    pub length:   u64,
}

/// Cache statistics.
#[derive(Debug, Clone, Default)]
pub struct CacheStats {
    pub series_count: i64,
    pub accel_count:  i64,
}
