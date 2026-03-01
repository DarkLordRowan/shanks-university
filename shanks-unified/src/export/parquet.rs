//! Parquet export — stub during the compute refactor.
//! Will be rewritten once the new compute::SeriesResult / AccelResult types are stable.

use anyhow::Result;
use std::path::Path;

#[allow(dead_code)]
pub struct ExportData;

#[allow(dead_code)]
pub struct ParquetExporter;

impl ParquetExporter {
    #[allow(dead_code)]
    pub fn export(_data: ExportData, _path: &Path) -> Result<()> {
        Ok(())
    }
}
