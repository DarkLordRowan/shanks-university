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
        if self.0 == 0.0 {
            return "0".to_string();
        }

        let mantissa = self.0;
        let exponent = self.1;
        let sign_str = if mantissa < 0.0 { "-" } else { "" };
        let abs_mantissa = mantissa.abs();

        // Use scientific notation for very small or very large numbers
        if exponent < -2 || exponent > 3 {
            format!("{}{:.1}e{:.0}", sign_str, abs_mantissa, exponent)
        } else {
            // For numbers like 0.5, 0.01, 10.0
            let real_val = abs_mantissa * 10f64.powi(exponent);
            format!("{}{:.20}", sign_str, real_val)
        }
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

/// Convert symlog-transformed value back to a formatted string.
///
/// This is used for axis labels on symlog-scaled plots.
pub fn symlog_formatter(val: f64, log_linthresh: f64) -> String {
    if val == 0.0 {
        return "0".to_string();
    }

    let sign_str = if val < 0.0 { "-" } else { "" };

    // Use the EXACT inverse: |x| = L * (10^|y| - 1)
    let linthresh = 10f64.powf(log_linthresh);
    let true_abs_x = linthresh * (10f64.powf(val.abs()) - 1.0);

    if true_abs_x == 0.0 {
        return "0".to_string();
    }

    let target_log10 = true_abs_x.log10();
    let exponent = target_log10.floor();
    let fractional = target_log10 - exponent;
    let mantissa = 10f64.powf(fractional);

    if exponent < -2.0 || exponent > 3.0 {
        format!("{}{:.1}e{:.0}", sign_str, mantissa, exponent)
    } else {
        let rounded = (true_abs_x * 10_000.0).round() / 10_000.0;
        format!("{}{}", sign_str, rounded)
    }
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
