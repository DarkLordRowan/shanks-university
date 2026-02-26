//! SQLite cache for series and acceleration results.
//!
//! This module provides persistent storage for computed results,
//! allowing fast retrieval without recomputation.

use anyhow::Result;
use rusqlite::{params, Connection};
use std::path::Path;

/// SQLite cache for series and acceleration results.
pub struct Cache {
    conn: Connection,
}

impl Cache {
    /// Open or create the cache database.
    pub fn new(path: &Path) -> Result<Self> {
        let conn = Connection::open(path)?;
        Ok(Self { conn })
    }

    /// Initialize the database schema.
    pub fn initialize_schema(&self) -> Result<()> {
        self.conn.execute_batch(
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

            -- Partial sums Sn and terms an
            CREATE TABLE IF NOT EXISTS series_points (
                series_id INTEGER NOT NULL,
                n INTEGER NOT NULL,
                sn_real TEXT,
                sn_imag TEXT,
                sn_exp INTEGER,
                an_real TEXT,
                an_imag TEXT,
                an_exp INTEGER,
                deviation TEXT,
                PRIMARY KEY (series_id, n),
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

            -- Accelerated values
            CREATE TABLE IF NOT EXISTS accel_points (
                accel_id INTEGER NOT NULL,
                n INTEGER NOT NULL,
                value_real TEXT,
                value_imag TEXT,
                value_exp INTEGER,
                deviation TEXT,
                profiling JSON,
                PRIMARY KEY (accel_id, n),
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
            "#,
        )?;
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
        let tx = self.conn.transaction()?;

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

    /// Insert series points.
    pub fn insert_series_points(
        &mut self,
        series_id: i64,
        points: &[(i64, String, String, i64, String, String, i64, String)],
    ) -> Result<()> {
        let tx = self.conn.transaction()?;

        for (n, sn_real, sn_imag, sn_exp, an_real, an_imag, an_exp, deviation) in points {
            tx.execute(
                "INSERT OR REPLACE INTO series_points
                 (series_id, n, sn_real, sn_imag, sn_exp, an_real, an_imag, an_exp, deviation)
                 VALUES (?1, ?2, ?3, ?4, ?5, ?6, ?7, ?8, ?9)",
                params![
                    series_id, n, sn_real, sn_imag, sn_exp, an_real, an_imag, an_exp, deviation
                ],
            )?;
        }

        if let Err(e) = tx.commit() {
            log::error!("Failed to commit series points insertion: {}", e);
            return Err(e.into());
        }
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
        let tx = self.conn.transaction()?;

        tx.execute(
            "INSERT INTO accelerations (series_id, accel_name, m_value, additional_args, profiling)
             VALUES (?1, ?2, ?3, ?4, ?5)",
            params![series_id, accel_name, m_value, additional_args, profiling],
        )?;

        let id = tx.last_insert_rowid();
        tx.commit()?;
        Ok(id)
    }

    /// Insert acceleration points.
    pub fn insert_accel_points(
        &mut self,
        accel_id: i64,
        points: &[(i64, String, String, i64, String, String)],
    ) -> Result<()> {
        let tx = self.conn.transaction()?;

        for (n, value_real, value_imag, value_exp, deviation, profiling) in points {
            tx.execute(
                "INSERT OR REPLACE INTO accel_points
                 (accel_id, n, value_real, value_imag, value_exp, deviation, profiling)
                 VALUES (?1, ?2, ?3, ?4, ?5, ?6, ?7)",
                params![accel_id, n, value_real, value_imag, value_exp, deviation, profiling],
            )?;
        }

        tx.commit()?;
        Ok(())
    }

    /// Insert events for an acceleration.
    pub fn insert_accel_events(
        &mut self,
        accel_id: i64,
        events: &[crate::ffi::ComputeEventEntry],
    ) -> Result<()> {
        let tx = self.conn.transaction()?;
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
        let tx = self.conn.transaction()?;
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
        let result = if let Some(nc) = noise_config {
            self.conn.query_row(
                "SELECT id FROM series WHERE name = ?1 AND precision = ?2 AND x_value = ?3 AND arguments = ?4 AND noise_config = ?5",
                params![name, precision, x_value, arguments, nc],
                |row| row.get(0),
            )
        } else {
            self.conn.query_row(
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
        let result = self.conn.query_row(
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
        let sum_json: Option<String> = self
            .conn
            .query_row(
                "SELECT sum FROM series WHERE id = ?1",
                params![series_id],
                |row| row.get(0),
            )
            .unwrap_or(None);

        let sum_val = sum_json.and_then(|s| serde_json::from_str(&s).ok());

        let mut stmt = self.conn.prepare(
            "SELECT n, sn_real, sn_imag, sn_exp, an_real, an_imag, an_exp
             FROM series_points WHERE series_id = ?1 ORDER BY n ASC",
        )?;

        let rows = stmt.query_map(params![series_id], |row| {
            let n: i64 = row.get(0)?;
            let sn_real: Option<String> = row.get(1)?;
            let sn_imag: Option<String> = row.get(2)?;
            let sn_exp: Option<i64> = row.get(3)?;
            let an_real: Option<String> = row.get(4)?;
            let an_imag: Option<String> = row.get(5)?;
            let an_exp: Option<i64> = row.get(6)?;
            Ok((n, sn_real, sn_imag, sn_exp, an_real, an_imag, an_exp))
        })?;

        let mut sn = Vec::new();
        let mut an = Vec::new();

        for row in rows {
            let (_n, sn_real, sn_imag, sn_exp, an_real, an_imag, an_exp) = row?;

            // Build Sn point
            if let (Some(real_str), Some(exp)) = (sn_real, sn_exp) {
                if real_str.starts_with('{') {
                    if let Ok(cinterval) =
                        serde_json::from_str::<crate::ffi::CIntervalValue>(&real_str)
                    {
                        sn.push(crate::ffi::SeriesPoint::CInterval(cinterval));
                    } else if let Ok(interval) =
                        serde_json::from_str::<crate::ffi::IntervalValue>(&real_str)
                    {
                        sn.push(crate::ffi::SeriesPoint::Interval(interval));
                    }
                } else if let Ok(real_val) = real_str.parse::<f64>() {
                    let real_sci = crate::ffi::ScientificValue {
                        mantissa: real_val,
                        exponent: exp,
                    };
                    if let Some(imag_str) = sn_imag {
                        if !imag_str.is_empty() {
                            if let Ok(imag_val) = imag_str.parse::<f64>() {
                                let imag_sci = crate::ffi::ScientificValue {
                                    mantissa: imag_val,
                                    exponent: exp,
                                };
                                sn.push(crate::ffi::SeriesPoint::Complex(
                                    crate::ffi::ComplexValue {
                                        real: real_sci,
                                        imag: imag_sci,
                                    },
                                ));
                                continue;
                            }
                        }
                    }
                    sn.push(crate::ffi::SeriesPoint::Real(real_sci));
                }
            }

            // Build an point
            if let (Some(real_str), Some(exp)) = (an_real, an_exp) {
                if real_str.starts_with('{') {
                    if let Ok(cinterval) =
                        serde_json::from_str::<crate::ffi::CIntervalValue>(&real_str)
                    {
                        an.push(crate::ffi::SeriesPoint::CInterval(cinterval));
                    } else if let Ok(interval) =
                        serde_json::from_str::<crate::ffi::IntervalValue>(&real_str)
                    {
                        an.push(crate::ffi::SeriesPoint::Interval(interval));
                    }
                } else if let Ok(real_val) = real_str.parse::<f64>() {
                    let real_sci = crate::ffi::ScientificValue {
                        mantissa: real_val,
                        exponent: exp,
                    };
                    if let Some(imag_str) = an_imag {
                        if !imag_str.is_empty() {
                            if let Ok(imag_val) = imag_str.parse::<f64>() {
                                let imag_sci = crate::ffi::ScientificValue {
                                    mantissa: imag_val,
                                    exponent: exp,
                                };
                                an.push(crate::ffi::SeriesPoint::Complex(
                                    crate::ffi::ComplexValue {
                                        real: real_sci,
                                        imag: imag_sci,
                                    },
                                ));
                                continue;
                            }
                        }
                    }
                    an.push(crate::ffi::SeriesPoint::Real(real_sci));
                }
            }
        }

        if sn.is_empty() {
            return Ok(None);
        }

        Ok(Some(crate::ffi::SeriesResult {
            sn: crate::ffi::SeriesPointArray::from_vec(&sn),
            an: crate::ffi::SeriesPointArray::from_vec(&an),
            sum: sum_val,
        }))
    }

    /// Get acceleration result from cache.
    pub fn get_accel_result(&self, accel_id: i64) -> Result<Option<crate::ffi::AccelResult>> {
        let mut stmt = self.conn.prepare(
            "SELECT n, value_real, value_imag, value_exp, deviation, profiling
             FROM accel_points WHERE accel_id = ?1 ORDER BY n ASC",
        )?;

        let rows = stmt.query_map(params![accel_id], |row| {
            let n: i64 = row.get(0)?;
            let v_real: Option<String> = row.get(1)?;
            let v_imag: Option<String> = row.get(2)?;
            let v_exp: Option<i64> = row.get(3)?;
            let dev_str: Option<String> = row.get(4)?;
            let prof_json: Option<String> = row.get(5)?;
            Ok((n, v_real, v_imag, v_exp, dev_str, prof_json))
        })?;

        let mut values = Vec::new();
        let mut deviations = Vec::new();
        let mut trace_add = Vec::new();
        let mut trace_mul = Vec::new();
        let mut trace_div = Vec::new();
        let mut trace_special = Vec::new();
        let mut has_any_profiling = false;

        let mut extracted_rows = Vec::new();
        for row in rows {
            extracted_rows.push(row?);
        }

        if extracted_rows.is_empty() {
            return Ok(None);
        }

        let mut max_n = 0;
        for (n, _, _, _, _, _) in &extracted_rows {
            if *n > max_n {
                max_n = *n;
            }
        }

        values.resize((max_n) as usize, None);
        deviations.resize(
            (max_n) as usize,
            crate::ffi::ScientificValue {
                mantissa: 0.0,
                exponent: 0,
            },
        );
        trace_add.resize((max_n) as usize, 0);
        trace_mul.resize((max_n) as usize, 0);
        trace_div.resize((max_n) as usize, 0);
        trace_special.resize((max_n) as usize, 0);

        for (n, v_real, v_imag, v_exp, dev_str, prof_json) in extracted_rows {
            let idx = (n - 1) as usize;

            // value
            if let (Some(real_str), Some(exp)) = (v_real, v_exp) {
                if real_str.starts_with('{') {
                    if let Ok(cinterval) =
                        serde_json::from_str::<crate::ffi::CIntervalValue>(&real_str)
                    {
                        values[idx] = Some(crate::ffi::SeriesPoint::CInterval(cinterval));
                    } else if let Ok(interval) =
                        serde_json::from_str::<crate::ffi::IntervalValue>(&real_str)
                    {
                        values[idx] = Some(crate::ffi::SeriesPoint::Interval(interval));
                    }
                } else if let Ok(real_val) = real_str.parse::<f64>() {
                    let real_sci = crate::ffi::ScientificValue {
                        mantissa: real_val,
                        exponent: exp,
                    };
                    if let Some(imag_str) = v_imag {
                        if !imag_str.is_empty() {
                            if let Ok(imag_val) = imag_str.parse::<f64>() {
                                let imag_sci = crate::ffi::ScientificValue {
                                    mantissa: imag_val,
                                    exponent: exp,
                                };
                                values[idx] = Some(crate::ffi::SeriesPoint::Complex(
                                    crate::ffi::ComplexValue {
                                        real: real_sci,
                                        imag: imag_sci,
                                    },
                                ));
                            }
                        } else {
                            values[idx] = Some(crate::ffi::SeriesPoint::Real(real_sci));
                        }
                    } else {
                        values[idx] = Some(crate::ffi::SeriesPoint::Real(real_sci));
                    }
                }
            }

            // deviation
            if let Some(dev) = dev_str {
                if let Ok(dev_val) = dev.parse::<f64>() {
                    deviations[idx] = crate::ffi::ScientificValue::from_f64(dev_val);
                } else if dev.contains("e") {
                    let parts: Vec<&str> = dev.split('e').collect();
                    if parts.len() == 2 {
                        if let (Ok(m), Ok(e)) = (parts[0].parse::<f64>(), parts[1].parse::<i64>()) {
                            deviations[idx] = crate::ffi::ScientificValue {
                                mantissa: m,
                                exponent: e,
                            };
                        }
                    }
                }
            }

            // profiling
            if let Some(pj) = prof_json {
                if let Ok(p) = serde_json::from_str::<serde_json::Value>(&pj) {
                    trace_add[idx] = p["add"].as_u64().unwrap_or(0);
                    trace_mul[idx] = p["mul"].as_u64().unwrap_or(0);
                    trace_div[idx] = p["div"].as_u64().unwrap_or(0);
                    trace_special[idx] = p["special"].as_u64().unwrap_or(0);
                    has_any_profiling = true;
                }
            }
        }

        let profiling = if has_any_profiling {
            Some(crate::ffi::ProfilingTrace {
                add: trace_add,
                mul: trace_mul,
                div: trace_div,
                special: trace_special,
            })
        } else {
            None
        };

        let mut first_val = None;
        for v in &values {
            if let Some(val) = v {
                first_val = Some(val.clone());
                break;
            }
        }
        let fb_dummy = crate::ffi::SeriesPoint::Real(crate::ffi::ScientificValue {
            mantissa: 0.0,
            exponent: 0,
        });
        let fallback = first_val.unwrap_or(fb_dummy);

        let mut valid = Vec::with_capacity(values.len());
        let mut dense_values = Vec::with_capacity(values.len());
        for v in values {
            if let Some(val) = v {
                valid.push(true);
                dense_values.push(val);
            } else {
                valid.push(false);
                dense_values.push(fallback.clone());
            }
        }
        let values_array = crate::ffi::SeriesPointArray::from_vec(&dense_values);

        let mut dev_mantissa = Vec::with_capacity(deviations.len());
        let mut dev_exponent = Vec::with_capacity(deviations.len());
        for d in deviations {
            dev_mantissa.push(d.mantissa);
            dev_exponent.push(d.exponent);
        }
        let deviations_array = crate::ffi::ScientificArray {
            mantissa: dev_mantissa,
            exponent: dev_exponent,
        };

        // --- Fetch events ---
        let mut stmt_events = self.conn.prepare(
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
        let mut stmt_est = self.conn.prepare(
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
            values: values_array,
            valid,
            deviations: deviations_array,
            events,
            errors: vec![],
            filtered_estimates,
            profiling,
        }))
    }

    /// Get all series names in the cache.
    pub fn list_cached_series(&self) -> Result<Vec<String>> {
        let mut stmt = self.conn.prepare("SELECT DISTINCT name FROM series")?;
        let rows = stmt.query_map([], |row| row.get(0))?;
        let mut names = Vec::new();
        for name in rows {
            names.push(name?);
        }
        Ok(names)
    }

    /// Get all precisions for a series name.
    pub fn get_precisions_for_series(&self, name: &str) -> Result<Vec<String>> {
        let mut stmt = self
            .conn
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
        self.conn.execute("DELETE FROM accel_points", [])?;
        self.conn.execute("DELETE FROM accelerations", [])?;
        self.conn.execute("DELETE FROM events", [])?;
        self.conn.execute("DELETE FROM series_points", [])?;
        self.conn.execute("DELETE FROM series", [])?;
        Ok(())
    }

    /// Get database statistics.
    pub fn stats(&self) -> Result<CacheStats> {
        let series_count: i64 = self
            .conn
            .query_row("SELECT COUNT(*) FROM series", [], |row| row.get(0))?;
        let accel_count: i64 =
            self.conn
                .query_row("SELECT COUNT(*) FROM accelerations", [], |row| row.get(0))?;
        let points_count: i64 =
            self.conn
                .query_row("SELECT COUNT(*) FROM series_points", [], |row| row.get(0))?;

        Ok(CacheStats {
            series_count,
            accel_count,
            points_count,
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
