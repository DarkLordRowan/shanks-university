//! SQLite cache for series and acceleration results.
//!
//! This module provides persistent storage for computed results,
//! allowing fast retrieval without recomputation.

use anyhow::Result;
use rusqlite::{params, Connection};
use std::path::Path;

/// Helper to convert f64 slice to bytes
fn f64_to_bytes(data: &[f64]) -> &[u8] {
    unsafe { std::slice::from_raw_parts(data.as_ptr() as *const u8, data.len() * 8) }
}

/// Helper to convert i64 slice to bytes
fn i64_to_bytes(data: &[i64]) -> &[u8] {
    unsafe { std::slice::from_raw_parts(data.as_ptr() as *const u8, data.len() * 8) }
}

/// Helper to parse f64 vec from bytes carefully (alignment safe)
fn bytes_to_f64(data: &[u8]) -> Vec<f64> {
    if data.is_empty() { return Vec::new(); }
    let count = data.len() / 8;
    let mut vec = Vec::with_capacity(count);
    unsafe {
        std::ptr::copy_nonoverlapping(data.as_ptr(), vec.as_mut_ptr() as *mut u8, count * 8);
        vec.set_len(count);
    }
    vec
}

/// Helper to parse i64 vec from bytes carefully (alignment safe)
fn bytes_to_i64(data: &[u8]) -> Vec<i64> {
    if data.is_empty() { return Vec::new(); }
    let count = data.len() / 8;
    let mut vec = Vec::with_capacity(count);
    unsafe {
        std::ptr::copy_nonoverlapping(data.as_ptr(), vec.as_mut_ptr() as *mut u8, count * 8);
        vec.set_len(count);
    }
    vec
}

/// SQLite cache for series and acceleration results.
pub struct Cache {
    conn: Option<Connection>,
}

impl Cache {
    /// Open or create the cache database.
    pub fn new(path: &Path) -> Result<Self> {
        let conn = Connection::open(path)?;
        Ok(Self { conn: Some(conn) })
    }

    /// Create a disabled cache that does nothing.
    pub fn disabled() -> Self {
        Self { conn: None }
    }

    /// Check if the cache is enabled.
    pub fn is_enabled(&self) -> bool {
        self.conn.is_some()
    }

    /// Initialize the database schema.
    pub fn initialize_schema(&self) -> Result<()> {
        let conn = match &self.conn {
            Some(c) => c,
            None => return Ok(()),
        };
        conn.execute_batch(
            r#"
            -- Series metadata and parameters
            CREATE TABLE IF NOT EXISTS series (
                id INTEGER PRIMARY KEY,
                name TEXT NOT NULL,
                precision TEXT NOT NULL,
                x_value TEXT,
                arguments JSON,
                noise_config JSON,
                profiling JSON,
                created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
                sum TEXT
            );
            CREATE UNIQUE INDEX IF NOT EXISTS idx_series_unique ON series(name, precision, x_value, arguments, IFNULL(noise_config, ''));

            -- Drop legacy row-by-row tables
            DROP TABLE IF EXISTS series_points;
            DROP TABLE IF EXISTS accel_points;

            -- Series binary data
            CREATE TABLE IF NOT EXISTS series_data (
                series_id INTEGER PRIMARY KEY,
                sn_type INTEGER, sn_len INTEGER,
                sn_m0 BLOB, sn_e0 BLOB, sn_m1 BLOB, sn_e1 BLOB, sn_m2 BLOB, sn_e2 BLOB, sn_m3 BLOB, sn_e3 BLOB,
                an_type INTEGER, an_len INTEGER,
                an_m0 BLOB, an_e0 BLOB, an_m1 BLOB, an_e1 BLOB, an_m2 BLOB, an_e2 BLOB, an_m3 BLOB, an_e3 BLOB,
                dev_type INTEGER, dev_len INTEGER,
                dev_m0 BLOB, dev_e0 BLOB, dev_m1 BLOB, dev_e1 BLOB, dev_m2 BLOB, dev_e2 BLOB, dev_m3 BLOB, dev_e3 BLOB,
                FOREIGN KEY (series_id) REFERENCES series(id)
            );

            -- Acceleration results
            CREATE TABLE IF NOT EXISTS accelerations (
                id INTEGER PRIMARY KEY,
                series_id INTEGER NOT NULL,
                accel_name TEXT NOT NULL,
                m_value INTEGER,
                additional_args JSON,
                profiling JSON,
                created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
                FOREIGN KEY (series_id) REFERENCES series(id)
            );

            -- Accelerated binary data
            CREATE TABLE IF NOT EXISTS accel_data (
                accel_id INTEGER PRIMARY KEY,
                val_type INTEGER, val_len INTEGER,
                val_m0 BLOB, val_e0 BLOB, val_m1 BLOB, val_e1 BLOB, val_m2 BLOB, val_e2 BLOB, val_m3 BLOB, val_e3 BLOB,
                dev_type INTEGER, dev_len INTEGER,
                dev_m0 BLOB, dev_e0 BLOB, dev_m1 BLOB, dev_e1 BLOB, dev_m2 BLOB, dev_e2 BLOB, dev_m3 BLOB, dev_e3 BLOB,
                FOREIGN KEY (accel_id) REFERENCES accelerations(id)
            );

            -- Events and errors during computation
            CREATE TABLE IF NOT EXISTS events (
                id INTEGER PRIMARY KEY,
                series_id INTEGER,
                accel_id INTEGER,
                n INTEGER,
                event_type TEXT,
                name TEXT,
                description TEXT,
                FOREIGN KEY (series_id) REFERENCES series(id),
                FOREIGN KEY (accel_id) REFERENCES accelerations(id)
            );

            -- Filtered estimates for divergent tails
            CREATE TABLE IF NOT EXISTS filtered_estimates (
                id INTEGER PRIMARY KEY,
                accel_id INTEGER NOT NULL,
                event_name TEXT NOT NULL,
                filter TEXT NOT NULL,
                limit_points JSON NOT NULL,
                start_n INTEGER NOT NULL,
                length INTEGER NOT NULL,
                FOREIGN KEY (accel_id) REFERENCES accelerations(id)
            );

            -- Indexes for common queries
            CREATE INDEX IF NOT EXISTS idx_series_name ON series(name);
            CREATE INDEX IF NOT EXISTS idx_series_precision ON series(precision);
            CREATE INDEX IF NOT EXISTS idx_accel_name ON accelerations(accel_name);
            CREATE INDEX IF NOT EXISTS idx_accel_series ON accelerations(series_id);

            -- Migrations / Schema updates
            ALTER TABLE series_data ADD COLUMN dev_type INTEGER DEFAULT 0;
            ALTER TABLE series_data ADD COLUMN dev_m0 BLOB;
            ALTER TABLE series_data ADD COLUMN dev_e0 BLOB;
            ALTER TABLE series_data ADD COLUMN dev_m1 BLOB;
            ALTER TABLE series_data ADD COLUMN dev_e1 BLOB;
            ALTER TABLE series_data ADD COLUMN dev_m2 BLOB;
            ALTER TABLE series_data ADD COLUMN dev_e2 BLOB;
            ALTER TABLE series_data ADD COLUMN dev_m3 BLOB;
            ALTER TABLE series_data ADD COLUMN dev_e3 BLOB;

            ALTER TABLE accel_data ADD COLUMN dev_type INTEGER DEFAULT 0;
            ALTER TABLE accel_data ADD COLUMN dev_m0 BLOB;
            ALTER TABLE accel_data ADD COLUMN dev_e0 BLOB;
            ALTER TABLE accel_data ADD COLUMN dev_m1 BLOB;
            ALTER TABLE accel_data ADD COLUMN dev_e1 BLOB;
            ALTER TABLE accel_data ADD COLUMN dev_m2 BLOB;
            ALTER TABLE accel_data ADD COLUMN dev_e2 BLOB;
            ALTER TABLE accel_data ADD COLUMN dev_m3 BLOB;
            ALTER TABLE accel_data ADD COLUMN dev_e3 BLOB;
            "#,
        ).ok(); // Ignore errors if columns already exist
        
        Ok(())
    }

    /// Insert a series and return its ID.
    pub fn insert_series(
        &mut self,
        name: &str,
        precision: &str,
        x_value: &str,
        arguments: &str,
        noise_config: Option<&str>,
        profiling: Option<&str>,
        sum_val: Option<&str>,
    ) -> Result<i64> {
        let conn = match &mut self.conn {
            Some(c) => c,
            None => return Ok(0),
        };
        let tx = conn.transaction()?;

        let result = tx.execute(
            "INSERT OR IGNORE INTO series (name, precision, x_value, arguments, noise_config, profiling, sum)
             VALUES (?1, ?2, ?3, ?4, ?5, ?6, ?7)",
            params![name, precision, x_value, arguments, noise_config, profiling, sum_val],
        );

        let id = if result? == 0 {
            // Row already exists, get the ID, ignoring profiling for uniqueness
            let existing_id: i64 = if let Some(nc) = noise_config {
                tx.query_row(
                    "SELECT id FROM series WHERE name = ?1 AND precision = ?2 AND x_value = ?3 AND arguments = ?4 AND noise_config = ?5",
                    params![name, precision, x_value, arguments, nc],
                    |row| row.get(0),
                )?
            } else {
                tx.query_row(
                    "SELECT id FROM series WHERE name = ?1 AND precision = ?2 AND x_value = ?3 AND arguments = ?4 AND noise_config IS NULL",
                    params![name, precision, x_value, arguments],
                    |row| row.get(0),
                )?
            };
            // Update profiling or sum if it was null
            if let Some(p) = profiling {
                tx.execute(
                    "UPDATE series SET profiling = ?1 WHERE id = ?2 AND profiling IS NULL",
                    params![p, existing_id],
                )?;
            }
            if let Some(s) = sum_val {
                tx.execute(
                    "UPDATE series SET sum = ?1 WHERE id = ?2 AND sum IS NULL",
                    params![s, existing_id],
                )?;
            }
            existing_id
        } else {
            tx.last_insert_rowid()
        };

        if let Err(e) = tx.commit() {
            log::error!("Failed to commit series insertion: {}", e);
            return Err(e.into());
        }
        Ok(id)
    }

    fn serialize_point_array<'a>(
        &self,
        array: &'a crate::ffi::SeriesPointArray,
    ) -> (u32, i64, Vec<(&'a [u8], &'a [u8])>) {
        let mut data = vec![(&[][..], &[][..]); 4];
        let (t, len) = match array {
            crate::ffi::SeriesPointArray::Real(r) => {
                data[0] = (f64_to_bytes(&r.mantissa), i64_to_bytes(&r.exponent));
                (0, r.len())
            }
            crate::ffi::SeriesPointArray::Complex(c) => {
                data[0] = (f64_to_bytes(&c.real.mantissa), i64_to_bytes(&c.real.exponent));
                data[1] = (f64_to_bytes(&c.imag.mantissa), i64_to_bytes(&c.imag.exponent));
                (1, c.real.len())
            }
            crate::ffi::SeriesPointArray::Interval(i) => {
                data[0] = (f64_to_bytes(&i.inf.mantissa), i64_to_bytes(&i.inf.exponent));
                data[1] = (f64_to_bytes(&i.sup.mantissa), i64_to_bytes(&i.sup.exponent));
                (2, i.inf.len())
            }
            crate::ffi::SeriesPointArray::CInterval(ci) => {
                data[0] = (
                    f64_to_bytes(&ci.real.inf.mantissa),
                    i64_to_bytes(&ci.real.inf.exponent),
                );
                data[1] = (
                    f64_to_bytes(&ci.real.sup.mantissa),
                    i64_to_bytes(&ci.real.sup.exponent),
                );
                data[2] = (
                    f64_to_bytes(&ci.imag.inf.mantissa),
                    i64_to_bytes(&ci.imag.inf.exponent),
                );
                data[3] = (
                    f64_to_bytes(&ci.imag.sup.mantissa),
                    i64_to_bytes(&ci.imag.sup.exponent),
                );
                (3, ci.real.inf.len())
            }
        };
        (t, len as i64, data)
    }

    /// Insert series binary data.
    pub fn insert_series_result(
        &mut self,
        series_id: i64,
        result: &crate::ffi::SeriesResult,
    ) -> Result<()> {
        let (sn_type, sn_len, sn_data) = self.serialize_point_array(&result.sn);
        let (an_type, an_len, an_data) = self.serialize_point_array(&result.an);
        let (dev_type, dev_len, dev_data) = self.serialize_point_array(&result.deviations);

        let conn = match &mut self.conn {
            Some(c) => c,
            None => return Ok(()),
        };

        conn.execute(
            "INSERT OR REPLACE INTO series_data (
                series_id,
                sn_type, sn_len, sn_m0, sn_e0, sn_m1, sn_e1, sn_m2, sn_e2, sn_m3, sn_e3,
                an_type, an_len, an_m0, an_e0, an_m1, an_e1, an_m2, an_e2, an_m3, an_e3,
                dev_type, dev_len, dev_m0, dev_e0, dev_m1, dev_e1, dev_m2, dev_e2, dev_m3, dev_e3
            ) VALUES (?1, ?2, ?3, ?4, ?5, ?6, ?7, ?8, ?9, ?10, ?11, ?12, ?13, ?14, ?15, ?16, ?17, ?18, ?19, ?20, ?21, ?22, ?23, ?24, ?25, ?26, ?27, ?28, ?29, ?30, ?31)",
            params![
                series_id,
                sn_type, sn_len, sn_data[0].0, sn_data[0].1, sn_data[1].0, sn_data[1].1, sn_data[2].0, sn_data[2].1, sn_data[3].0, sn_data[3].1,
                an_type, an_len, an_data[0].0, an_data[0].1, an_data[1].0, an_data[1].1, an_data[2].0, an_data[2].1, an_data[3].0, an_data[3].1,
                dev_type, dev_len, dev_data[0].0, dev_data[0].1, dev_data[1].0, dev_data[1].1, dev_data[2].0, dev_data[2].1, dev_data[3].0, dev_data[3].1
            ],
        )?;
        Ok(())
    }

    /// Insert acceleration binary data.
    pub fn insert_accel_result(
        &mut self,
        accel_id: i64,
        result: &crate::ffi::AccelResult,
    ) -> Result<()> {
        let (val_type, val_len, val_data) = self.serialize_point_array(&result.values);
        let (dev_type, dev_len, dev_data) = self.serialize_point_array(&result.deviations);

        let conn = match &mut self.conn {
            Some(c) => c,
            None => return Ok(()),
        };

        conn.execute(
            "INSERT OR REPLACE INTO accel_data (
                accel_id,
                val_type, val_len, val_m0, val_e0, val_m1, val_e1, val_m2, val_e2, val_m3, val_e3,
                dev_type, dev_len, dev_m0, dev_e0, dev_m1, dev_e1, dev_m2, dev_e2, dev_m3, dev_e3
            ) VALUES (?1, ?2, ?3, ?4, ?5, ?6, ?7, ?8, ?9, ?10, ?11, ?12, ?13, ?14, ?15, ?16, ?17, ?18, ?19, ?20, ?21)",
            params![
                accel_id,
                val_type, val_len, val_data[0].0, val_data[0].1, val_data[1].0, val_data[1].1, val_data[2].0, val_data[2].1, val_data[3].0, val_data[3].1,
                dev_type, dev_len, dev_data[0].0, dev_data[0].1, dev_data[1].0, dev_data[1].1, dev_data[2].0, dev_data[2].1, dev_data[3].0, dev_data[3].1
            ],
        )?;
        Ok(())
    }

    /// Insert an acceleration and return its ID.
    pub fn insert_acceleration(
        &mut self,
        series_id: i64,
        accel_name: &str,
        m_value: Option<i64>,
        additional_args: &str,
        profiling: Option<&str>,
    ) -> Result<i64> {
        let conn = match &mut self.conn {
            Some(c) => c,
            None => return Ok(0),
        };
        let tx = conn.transaction()?;

        tx.execute(
            "INSERT INTO accelerations (series_id, accel_name, m_value, additional_args, profiling)
             VALUES (?1, ?2, ?3, ?4, ?5)",
            params![series_id, accel_name, m_value, additional_args, profiling],
        )?;

        let id = tx.last_insert_rowid();
        tx.commit()?;
        Ok(id)
    }

    // insert_accel_points is deprecated and replaced by insert_accel_result.
    // Keeping this for now if needed by other callers.
    pub fn insert_accel_points(
        &mut self,
        _accel_id: i64,
        _points: &[(i64, String, String, i64, String, String)],
    ) -> Result<()> {
        Ok(())
    }

    /// Insert events for an acceleration.
    pub fn insert_accel_events(
        &mut self,
        accel_id: i64,
        events: &[crate::ffi::ComputeEventEntry],
    ) -> Result<()> {
        let conn = match &mut self.conn {
            Some(c) => c,
            None => return Ok(()),
        };
        let tx = conn.transaction()?;
        for e in events {
            tx.execute(
                "INSERT INTO events (accel_id, n, name, description) VALUES (?1, ?2, ?3, ?4)",
                params![accel_id, e.n, e.name, e.description],
            )?;
        }
        tx.commit()?;
        Ok(())
    }

    /// Insert filtered estimates for an acceleration.
    pub fn insert_filtered_estimates(
        &mut self,
        accel_id: i64,
        estimates: &[crate::ffi::SmoothedEstimate],
    ) -> Result<()> {
        let conn = match &mut self.conn {
            Some(c) => c,
            None => return Ok(()),
        };
        let tx = conn.transaction()?;
        for est in estimates {
            let limit_points_json = serde_json::to_string(&est.limit)?;
            tx.execute(
                "INSERT INTO filtered_estimates (accel_id, event_name, filter, limit_points, start_n, length)
                 VALUES (?1, ?2, ?3, ?4, ?5, ?6)",
                params![accel_id, est.event_name, est.filter, limit_points_json, est.start_n, est.length],
            )?;
        }
        tx.commit()?;
        Ok(())
    }

    /// Check if a series exists in the cache.
    pub fn series_exists(
        &self,
        name: &str,
        precision: &str,
        x_value: &str,
        arguments: &str,
        noise_config: Option<&str>,
    ) -> Result<Option<i64>> {
        let conn = match &self.conn {
            Some(c) => c,
            None => return Ok(None),
        };
        let result = if let Some(nc) = noise_config {
            conn.query_row(
                "SELECT id FROM series WHERE name = ?1 AND precision = ?2 AND x_value = ?3 AND arguments = ?4 AND noise_config = ?5",
                params![name, precision, x_value, arguments, nc],
                |row| row.get(0),
            )
        } else {
            conn.query_row(
                "SELECT id FROM series WHERE name = ?1 AND precision = ?2 AND x_value = ?3 AND arguments = ?4 AND noise_config IS NULL",
                params![name, precision, x_value, arguments],
                |row| row.get(0),
            )
        };

        match result {
            Ok(id) => Ok(Some(id)),
            Err(rusqlite::Error::QueryReturnedNoRows) => Ok(None),
            Err(e) => Err(e.into()),
        }
    }

    /// Check if an acceleration exists in the cache.
    pub fn acceleration_exists(
        &self,
        series_id: i64,
        accel_name: &str,
        m_value: Option<i64>,
        additional_args: &str,
    ) -> Result<Option<i64>> {
        let conn = match &self.conn {
            Some(c) => c,
            None => return Ok(None),
        };
        let result = conn.query_row(
            "SELECT id FROM accelerations WHERE series_id = ?1 AND accel_name = ?2 AND
             (m_value = ?3 OR (m_value IS NULL AND ?3 IS NULL)) AND additional_args = ?4",
            params![series_id, accel_name, m_value, additional_args],
            |row| row.get(0),
        );

        match result {
            Ok(id) => Ok(Some(id)),
            Err(rusqlite::Error::QueryReturnedNoRows) => Ok(None),
            Err(e) => Err(e.into()),
        }
    }

    pub fn get_series_result(&self, series_id: i64) -> Result<Option<crate::ffi::SeriesResult>> {
        let conn = match &self.conn {
            Some(c) => c,
            None => return Ok(None),
        };
        let sum_json: Option<String> = conn
            .query_row(
                "SELECT sum FROM series WHERE id = ?1",
                params![series_id],
                |row| row.get(0),
            )
            .unwrap_or(None);

        let sum_val = sum_json.and_then(|s| serde_json::from_str(&s).ok());

        let result = conn.query_row(
            "SELECT 
                sn_type, sn_len, sn_m0, sn_e0, sn_m1, sn_e1, sn_m2, sn_e2, sn_m3, sn_e3,
                an_type, an_len, an_m0, an_e0, an_m1, an_e1, an_m2, an_e2, an_m3, an_e3,
                dev_type, dev_len, dev_m0, dev_e0, dev_m1, dev_e1, dev_m2, dev_e2, dev_m3, dev_e3
             FROM series_data WHERE series_id = ?1",
            params![series_id],
            |row| {
                let sn_type: u32 = row.get(0)?;
                let _sn_len: i64 = row.get(1)?;
                let sn_m: Vec<Vec<u8>> = vec![row.get(2)?, row.get(4)?, row.get(6)?, row.get(8)?];
                let sn_e: Vec<Vec<u8>> = vec![row.get(3)?, row.get(5)?, row.get(7)?, row.get(9)?];

                let an_type: u32 = row.get(10)?;
                let _an_len: i64 = row.get(11)?;
                let an_m: Vec<Vec<u8>> = vec![row.get(12)?, row.get(14)?, row.get(16)?, row.get(18)?];
                let an_e: Vec<Vec<u8>> = vec![row.get(13)?, row.get(15)?, row.get(17)?, row.get(19)?];

                let dev_type: u32 = row.get(20).unwrap_or(0); // Migration fallback
                let _dev_len: i64 = row.get(21)?;
                let dev_m: Vec<Vec<u8>> = vec![
                    row.get(22).unwrap_or_default(), 
                    row.get(24).unwrap_or_default(), 
                    row.get(26).unwrap_or_default(), 
                    row.get(28).unwrap_or_default()
                ];
                let dev_e: Vec<Vec<u8>> = vec![
                    row.get(23).unwrap_or_default(), 
                    row.get(25).unwrap_or_default(), 
                    row.get(27).unwrap_or_default(), 
                    row.get(29).unwrap_or_default()
                ];

                Ok((sn_type, sn_m, sn_e, an_type, an_m, an_e, dev_type, dev_m, dev_e))
            },
        );

        let (sn_type, sn_m, sn_e, an_type, an_m, an_e, dev_type, dev_m, dev_e) = match result {
            Ok(v) => v,
            Err(rusqlite::Error::QueryReturnedNoRows) => return Ok(None),
            Err(e) => return Err(e.into()),
        };

        let sn = self.reconstruct_point_array(sn_type, sn_m, sn_e);
        let an = self.reconstruct_point_array(an_type, an_m, an_e);
        let deviations = self.reconstruct_point_array(dev_type, dev_m, dev_e);

        Ok(Some(crate::ffi::SeriesResult {
            sn,
            an,
            sum: sum_val,
            deviations,
        }))
    }

    fn reconstruct_point_array(
        &self,
        t: u32,
        m: Vec<Vec<u8>>,
        e: Vec<Vec<u8>>,
    ) -> crate::ffi::SeriesPointArray {
        match t {
            0 => crate::ffi::SeriesPointArray::Real(crate::ffi::ScientificArray {
                mantissa: bytes_to_f64(&m[0]),
                exponent: bytes_to_i64(&e[0]),
            }),
            1 => crate::ffi::SeriesPointArray::Complex(crate::ffi::ComplexArray {
                real: crate::ffi::ScientificArray {
                    mantissa: bytes_to_f64(&m[0]),
                    exponent: bytes_to_i64(&e[0]),
                },
                imag: crate::ffi::ScientificArray {
                    mantissa: bytes_to_f64(&m[1]),
                    exponent: bytes_to_i64(&e[1]),
                },
            }),
            2 => crate::ffi::SeriesPointArray::Interval(crate::ffi::IntervalArray {
                inf: crate::ffi::ScientificArray {
                    mantissa: bytes_to_f64(&m[0]),
                    exponent: bytes_to_i64(&e[0]),
                },
                sup: crate::ffi::ScientificArray {
                    mantissa: bytes_to_f64(&m[1]),
                    exponent: bytes_to_i64(&e[1]),
                },
            }),
            3 => crate::ffi::SeriesPointArray::CInterval(crate::ffi::CIntervalArray {
                real: crate::ffi::IntervalArray {
                    inf: crate::ffi::ScientificArray {
                        mantissa: bytes_to_f64(&m[0]),
                        exponent: bytes_to_i64(&e[0]),
                    },
                    sup: crate::ffi::ScientificArray {
                        mantissa: bytes_to_f64(&m[1]),
                        exponent: bytes_to_i64(&e[1]),
                    },
                },
                imag: crate::ffi::IntervalArray {
                    inf: crate::ffi::ScientificArray {
                        mantissa: bytes_to_f64(&m[2]),
                        exponent: bytes_to_i64(&e[2]),
                    },
                    sup: crate::ffi::ScientificArray {
                        mantissa: bytes_to_f64(&m[3]),
                        exponent: bytes_to_i64(&e[3]),
                    },
                },
            }),
            _ => crate::ffi::SeriesPointArray::Real(Default::default()),
        }
    }

    /// Get acceleration result from cache.
    pub fn get_accel_result(&self, accel_id: i64) -> Result<Option<crate::ffi::AccelResult>> {
        let conn = match &self.conn {
            Some(c) => c,
            None => return Ok(None),
        };
        let result = conn.query_row(
            "SELECT 
                val_type, val_len, val_m0, val_e0, val_m1, val_e1, val_m2, val_e2, val_m3, val_e3,
                dev_type, dev_len, dev_m0, dev_e0, dev_m1, dev_e1, dev_m2, dev_e2, dev_m3, dev_e3
             FROM accel_data WHERE accel_id = ?1",
            params![accel_id],
            |row| {
                let val_type: u32 = row.get(0)?;
                let val_len: i64 = row.get(1)?;
                let val_m: Vec<Vec<u8>> = vec![row.get(2)?, row.get(4)?, row.get(6)?, row.get(8)?];
                let val_e: Vec<Vec<u8>> = vec![row.get(3)?, row.get(5)?, row.get(7)?, row.get(9)?];

                let dev_type: u32 = row.get(10).unwrap_or(0);
                let _dev_len: i64 = row.get(11)?;
                let dev_m: Vec<Vec<u8>> = vec![
                    row.get(12).unwrap_or_default(), 
                    row.get(14).unwrap_or_default(), 
                    row.get(16).unwrap_or_default(), 
                    row.get(18).unwrap_or_default()
                ];
                let dev_e: Vec<Vec<u8>> = vec![
                    row.get(13).unwrap_or_default(), 
                    row.get(15).unwrap_or_default(), 
                    row.get(17).unwrap_or_default(), 
                    row.get(19).unwrap_or_default()
                ];

                Ok((val_type, val_len, val_m, val_e, dev_type, _dev_len, dev_m, dev_e))
            },
        );

        let (val_type, val_len, val_m, val_e, dev_type, _dev_len, dev_m, dev_e) = match result {
            Ok(v) => v,
            Err(rusqlite::Error::QueryReturnedNoRows) => return Ok(None),
            Err(e) => return Err(e.into()),
        };

        let values = self.reconstruct_point_array(val_type, val_m, val_e);
        let deviations = self.reconstruct_point_array(dev_type, dev_m, dev_e);

        // Reconstruct valid array (all true for cache hits)
        let valid = vec![true; val_len as usize];

        // --- Fetch events ---
        let mut stmt_events = conn.prepare(
            "SELECT n, name, description FROM events WHERE accel_id = ?1 ORDER BY n ASC"
        )?;
        let events_iter = stmt_events.query_map(params![accel_id], |row| {
            Ok(crate::ffi::ComputeEventEntry {
                n: row.get::<_, i64>(0)? as u64,
                name: row.get(1)?,
                description: row.get::<_, Option<String>>(2)?.unwrap_or_default(),
            })
        })?;
        let mut events = Vec::new();
        for e in events_iter {
            events.push(e?);
        }

        // --- Fetch filtered estimates ---
        let mut stmt_est = conn.prepare(
            "SELECT event_name, filter, limit_points, start_n, length FROM filtered_estimates WHERE accel_id = ?1 ORDER BY start_n ASC"
        )?;
        let est_iter = stmt_est.query_map(params![accel_id], |row| {
            let event_name: String = row.get(0)?;
            let filter: String = row.get(1)?;
            let limit_points_json: String = row.get(2)?;
            let start_n: i64 = row.get(3)?;
            let length: i64 = row.get(4)?;

            let limit = serde_json::from_str(&limit_points_json).unwrap_or_else(|_| vec![]);

            Ok(crate::ffi::SmoothedEstimate {
                event_name,
                filter,
                limit,
                start_n: start_n as u64,
                length: length as u64,
            })
        })?;
        let mut filtered_estimates = Vec::new();
        for est in est_iter {
            filtered_estimates.push(est?);
        }

        Ok(Some(crate::ffi::AccelResult {
            values,
            valid,
            deviations,
            events,
            errors: vec![],
            filtered_estimates,
            profiling: None,
        }))
    }

    /// Get all series names in the cache.
    pub fn list_cached_series(&self) -> Result<Vec<String>> {
        let conn = match &self.conn {
            Some(c) => c,
            None => return Ok(Vec::new()),
        };
        let mut stmt = conn.prepare("SELECT DISTINCT name FROM series")?;
        let rows = stmt.query_map([], |row| row.get(0))?;
        let mut names = Vec::new();
        for name in rows {
            names.push(name?);
        }
        Ok(names)
    }

    /// Get all precisions for a series name.
    pub fn get_precisions_for_series(&self, name: &str) -> Result<Vec<String>> {
        let conn = match &self.conn {
            Some(c) => c,
            None => return Ok(Vec::new()),
        };
        let mut stmt = conn
            .prepare("SELECT DISTINCT precision FROM series WHERE name = ?1")?;
        let rows = stmt.query_map(params![name], |row| row.get(0))?;
        let mut precisions = Vec::new();
        for precision in rows {
            precisions.push(precision?);
        }
        Ok(precisions)
    }

    /// Clear all cached data.
    pub fn clear_all(&self) -> Result<()> {
        let conn = match &self.conn {
            Some(c) => c,
            None => return Ok(()),
        };
        conn.execute("DELETE FROM accel_data", [])?;
        conn.execute("DELETE FROM accelerations", [])?;
        conn.execute("DELETE FROM events", [])?;
        conn.execute("DELETE FROM series_data", [])?;
        conn.execute("DELETE FROM series", [])?;
        Ok(())
    }

    /// Get database statistics.
    pub fn stats(&self) -> Result<CacheStats> {
        let conn = match &self.conn {
            Some(c) => c,
            None => return Ok(CacheStats { series_count: 0, accel_count: 0, points_count: 0 }),
        };
        let series_count: i64 = conn
            .query_row("SELECT COUNT(*) FROM series", [], |row| row.get(0))?;
        let accel_count: i64 =
            conn
                .query_row("SELECT COUNT(*) FROM accelerations", [], |row| row.get(0))?;
        
        Ok(CacheStats {
            series_count,
            accel_count,
            points_count: 0, // No longer counting individual points
        })
    }
}

/// Cache statistics.
#[derive(Debug, Clone)]
pub struct CacheStats {
    pub series_count: i64,
    pub accel_count: i64,
    pub points_count: i64,
}
