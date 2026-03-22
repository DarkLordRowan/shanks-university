//! Symlog scale for plotting values with large dynamic range.
//!
//! Ported from old-vizr/src/symlog.rs

use serde::{Deserialize, Serialize};

/// A value in scientific notation: mantissa × 10^exponent.
#[derive(Clone, Copy, Debug, Default, Serialize, Deserialize)]
pub struct Scientific(pub f64, pub i32);

impl Scientific {
    /// Convert to approximate f64 value.
    /// May lose precision for very large/small values.
    pub fn approx_f64(&self) -> f64 {
        self.0 * 10f64.powi(self.1)
    }

    /// Create from f64 value.
    pub fn from_f64(val: f64) -> Self {
        if val == 0.0 {
            return Scientific(0.0, 0);
        }

        let abs_val = val.abs();
        let sign = val.signum();
        let log10 = abs_val.log10();
        let exponent = log10.floor() as i32;
        let mantissa = sign * abs_val / 10f64.powi(exponent);

        Scientific(mantissa, exponent)
    }

    /// Apply symlog transformation for plotting.
    ///
    /// Symlog provides a smooth transition between linear and logarithmic scales:
    /// - Near zero: linear (preserves sign and small differences)
    /// - Far from zero: logarithmic (handles large dynamic range)
    pub fn symlog(&self, log_linthresh: f64) -> f64 {
        let linthresh = 10f64.powf(log_linthresh);
        let mantissa = self.0;
        let exponent = self.1;

        // Handle zero
        if mantissa == 0.0 {
            return 0.0;
        }

        let sign = mantissa.signum();
        let abs_mantissa = mantissa.abs();

        // log10(|x|)
        let val_log10 = abs_mantissa.log10() + exponent as f64;

        // Compare magnitude vs threshold
        // If the value is more than 16 orders of magnitude larger than threshold,
        // the "+ 1" in symlog formula becomes irrelevant due to f64 precision limits.
        let magnitude_diff = val_log10 - log_linthresh;

        if magnitude_diff > 16.0 {
            // Huge numbers: log approximation
            // Formula: log10(|x|) - log10(L)
            // This preserves precision for massive numbers (e.g. 1e100) avoiding overflow.
            sign * magnitude_diff
        } else {
            // Small/transition numbers: exact math
            // Formula: log10(1 + |x|/L)
            // Near threshold, the "+ 1" creates the smooth curve.
            sign * (1.0 + self.approx_f64().abs() / linthresh).log10()
        }
    }

    /// Format as a human-readable string.
    pub fn format(&self) -> String {
        format_value(self.approx_f64())
    }
}

impl From<f64> for Scientific {
    fn from(val: f64) -> Self {
        Self::from_f64(val)
    }
}

impl From<Scientific> for f64 {
    fn from(val: Scientific) -> Self {
        val.approx_f64()
    }
}

/// Format a value for display, avoiding scientific notation for small exponents.
pub fn format_value(val: f64) -> String {
    if val == 0.0 {
        return "0".to_string();
    }
    if !val.is_finite() {
        return format!("{val}");
    }

    let abs_val = val.abs();
    let log10 = abs_val.log10();
    let exponent = log10.floor() as i32;

    // Avoid scientific notation for 10^-8 to 10^12
    if exponent >= -7 && exponent <= 7 {
        // High precision for f64 to avoid truncation
        let s = format!("{:.18}", val);
        if s.contains('.') {
            let trimmed = s.trim_end_matches('0').trim_end_matches('.');
            if trimmed.is_empty() || trimmed == "-" {
                "0".to_string()
            } else {
                trimmed.to_string()
            }
        } else {
            s
        }
    } else {
        // Use scientific notation with high precision
        let s = format!("{:.15e}", val);
        if let Some(e_pos) = s.find('e') {
            let (mantissa, exp) = s.split_at(e_pos);
            let trimmed_mantissa = mantissa.trim_end_matches('0').trim_end_matches('.');
            format!("{}{}", trimmed_mantissa, exp)
        } else {
            s
        }
    }
}

/// Convert symlog-transformed value back to a formatted string.
///
/// This is used for axis labels on symlog_scaled plots.
pub fn symlog_formatter(val: f64, log_linthresh: f64) -> String {
    if val == 0.0 {
        return "0".to_string();
    }

    // Use the EXACT inverse: |x| = L * (10^|y| - 1)
    let linthresh = 10f64.powf(log_linthresh);
    let true_abs_x = linthresh * (10f64.powf(val.abs()) - 1.0);
    let true_x = true_abs_x * val.signum();

    format_value(true_x)
}

/// Format a value for grid/axis labels with reduced precision to avoid epsilon floating errors.
pub fn format_grid_value(val: f64) -> String {
    if val == 0.0 {
        return "0".to_string();
    }
    if !val.is_finite() {
        return format!("{val}");
    }

    let abs_val = val.abs();
    let log10 = abs_val.log10();
    let exponent = log10.floor() as i32;

    if exponent >= -5 && exponent <= 5 {
        let s = format!("{:.5}", val);
        if s.contains('.') {
            let trimmed = s.trim_end_matches('0').trim_end_matches('.');
            if trimmed.is_empty() || trimmed == "-" {
                "0".to_string()
            } else {
                trimmed.to_string()
            }
        } else {
            s
        }
    } else {
        let s = format!("{:.3e}", val);
        if let Some(e_pos) = s.find('e') {
            let (mantissa, exp) = s.split_at(e_pos);
            let mem = mantissa.trim_end_matches('0').trim_end_matches('.');
            // Clean up e format (e.g. e-05 to e-5)
            let exp_clean = if exp.starts_with("e-0") {
                format!("e-{}", &exp[3..])
            } else if exp.starts_with("e0") {
                format!("e{}", &exp[2..])
            } else if exp.starts_with("e+0") {
                format!("e{}", &exp[3..])
            } else if exp.starts_with("e+") {
                format!("e{}", &exp[2..])
            } else {
                exp.to_string()
            };
            format!("{}{}", mem, exp_clean)
        } else {
            s
        }
    }
}

/// Convert symlog-transformed value back to a formatted string for grid display.
pub fn symlog_grid_formatter(val: f64, log_linthresh: f64) -> String {
    if val == 0.0 {
        return "0".to_string();
    }

    // Use the EXACT inverse: |x| = L * (10^|y| - 1)
    let linthresh = 10f64.powf(log_linthresh);
    let true_abs_x = linthresh * (10f64.powf(val.abs()) - 1.0);
    let true_x = true_abs_x * val.signum();

    format_grid_value(true_x)
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_scientific_from_f64() {
        let s = Scientific::from_f64(123.456);
        assert!((s.0 - 1.23456).abs() < 0.0001);
        assert_eq!(s.1, 2);
    }

    #[test]
    fn test_scientific_approx_f64() {
        let s = Scientific(1.23, 5);
        assert!((s.approx_f64() - 123000.0).abs() < 0.1);
    }

    #[test]
    fn test_symlog_zero() {
        let s = Scientific(0.0, 0);
        assert_eq!(s.symlog(-50.0), 0.0);
    }

    #[test]
    fn test_symlog_symmetry() {
        let pos = Scientific(1.5, 10);
        let neg = Scientific(-1.5, 10);
        assert!((pos.symlog(-50.0) + neg.symlog(-50.0)).abs() < 1e-10);
    }

    #[test]
    fn test_format() {
        assert_eq!(Scientific(0.0, 0).format(), "0");
        assert!(Scientific(1.5, 10).format().contains("e10"));
    }
}
