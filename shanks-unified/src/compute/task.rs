//! Task definitions for compute orchestration.

use serde::{Deserialize, Serialize};
use std::collections::HashMap;
use uuid::Uuid;

use crate::ffi::ParamValue;

/// Parameters for a series computation.
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct SeriesParams {
    /// Series name (e.g., "Ln2Series")
    pub name: String,
    /// X value as string (scientific notation supported)
    pub x_value: String,
    /// Named parameters (e.g., "alpha" for BinSeries)
    #[serde(default)]
    pub params: HashMap<String, ParamValue>,
}

/// Parameters for an acceleration algorithm.
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct AccelParams {
    /// Algorithm name (e.g., "Shanks Transformation")
    pub name: String,
    /// Named parameters (e.g., "remainder_type", "recurrent")
    #[serde(default)]
    pub params: HashMap<String, ParamValue>,
}

/// A computation task.
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct ComputeTask {
    /// Unique task ID
    #[serde(default = "Uuid::new_v4")]
    pub id: Uuid,
    /// Precision type: "F32", "F64", "FLong", "Arb", "CF32", "CF64", "CFLong", "CArb"
    pub precision: String,
    /// Series parameters
    pub series: SeriesParams,
    /// Number of points to generate
    pub n_points: u64,
    /// Algorithms to apply
    #[serde(default)]
    pub algorithms: Vec<AccelParams>,
}

impl ComputeTask {
    /// Create a new task with default settings.
    pub fn new(series_name: impl Into<String>, n_points: u64) -> Self {
        Self {
            id: Uuid::new_v4(),
            precision: "F64".to_string(),
            series: SeriesParams {
                name: series_name.into(),
                x_value: "1.0".to_string(),
                params: HashMap::new(),
            },
            n_points,
            algorithms: Vec::new(),
        }
    }

    /// Set the precision type.
    pub fn with_precision(mut self, precision: impl Into<String>) -> Self {
        self.precision = precision.into();
        self
    }

    /// Set the x value.
    pub fn with_x(mut self, x_value: impl Into<String>) -> Self {
        self.series.x_value = x_value.into();
        self
    }

    /// Add a series parameter.
    pub fn with_param(mut self, name: impl Into<String>, value: ParamValue) -> Self {
        self.series.params.insert(name.into(), value);
        self
    }

    /// Add an algorithm to apply.
    pub fn with_algorithm(mut self, name: impl Into<String>) -> Self {
        self.algorithms.push(AccelParams {
            name: name.into(),
            params: HashMap::new(),
        });
        self
    }

    /// Add an algorithm with parameters.
    pub fn with_algorithm_params(
        mut self,
        name: impl Into<String>,
        params: HashMap<String, ParamValue>,
    ) -> Self {
        self.algorithms.push(AccelParams {
            name: name.into(),
            params,
        });
        self
    }
}
