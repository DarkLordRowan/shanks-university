//! SQLite cache for series and acceleration results.
//!
//! This module provides persistent storage for computed results,
//! allowing fast retrieval without recomputation.

use anyhow::Result;
use rusqlite::{Connection, params};
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
                created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
                UNIQUE(name, precision, x_value, arguments)
            );

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
    ) -> Result<i64> {
        let tx = self.conn.transaction()?;
        
        let result = tx.execute(
            "INSERT OR IGNORE INTO series (name, precision, x_value, arguments, noise_config)
             VALUES (?1, ?2, ?3, ?4, ?5)",
            params![name, precision, x_value, arguments, noise_config],
        );

        let id = if result? == 0 {
            // Row already exists, get the ID
            tx.query_row(
                "SELECT id FROM series WHERE name = ?1 AND precision = ?2 AND x_value = ?3 AND arguments = ?4",
                params![name, precision, x_value, arguments],
                |row| row.get(0),
            )?
        } else {
            tx.last_insert_rowid()
        };

        tx.commit()?;
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
                params![series_id, n, sn_real, sn_imag, sn_exp, an_real, an_imag, an_exp, deviation],
            )?;
        }

        tx.commit()?;
        Ok(())
    }

    /// Insert an acceleration and return its ID.
    pub fn insert_acceleration(
        &mut self,
        series_id: i64,
        accel_name: &str,
        m_value: Option<i64>,
        additional_args: &str,
    ) -> Result<i64> {
        let tx = self.conn.transaction()?;
        
        tx.execute(
            "INSERT INTO accelerations (series_id, accel_name, m_value, additional_args)
             VALUES (?1, ?2, ?3, ?4)",
            params![series_id, accel_name, m_value, additional_args],
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

    /// Check if a series exists in the cache.
    pub fn series_exists(
        &self,
        name: &str,
        precision: &str,
        x_value: &str,
        arguments: &str,
    ) -> Result<Option<i64>> {
        let result = self.conn.query_row(
            "SELECT id FROM series WHERE name = ?1 AND precision = ?2 AND x_value = ?3 AND arguments = ?4",
            params![name, precision, x_value, arguments],
            |row| row.get(0),
        );

        match result {
            Ok(id) => Ok(Some(id)),
            Err(rusqlite::Error::QueryReturnedNoRows) => Ok(None),
            Err(e) => Err(e.into()),
        }
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
        let mut stmt = self.conn.prepare(
            "SELECT DISTINCT precision FROM series WHERE name = ?1"
        )?;
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
        let series_count: i64 = self.conn.query_row(
            "SELECT COUNT(*) FROM series", [], |row| row.get(0)
        )?;
        let accel_count: i64 = self.conn.query_row(
            "SELECT COUNT(*) FROM accelerations", [], |row| row.get(0)
        )?;
        let points_count: i64 = self.conn.query_row(
            "SELECT COUNT(*) FROM series_points", [], |row| row.get(0)
        )?;

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
