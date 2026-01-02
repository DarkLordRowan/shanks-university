import numpy as np

# Thresholds for linear region near zero
# (symlog)
LINTHRESH = 1e-50
LOG_LINTHRESH = -50.0


class Scientific:
    """
    A custom arbitrary-precision floating point number representation.
    Not to lose precision when loading from Parquet.
    """

    def __init__(self, mantissa: float, exponent: int):
        self.mantissa = mantissa
        self.exponent = exponent

    @classmethod
    def from_str(cls, s: str) -> "Scientific":
        """
        Parses a scientific string (e.g., "1.23e-100") into a Scientific object.
        """
        if not s:
            return cls(0.0, 0)
        s = s.lower()
        if "e" in s:
            parts = s.split("e")
            mantissa = float(parts[0])
            exponent = int(parts[1])
            return cls(mantissa, exponent)
        else:
            return cls(float(s), 0)

    def approx_f64(self) -> float:
        """
        Converts to a standard Python float (64-bit).
        Warning: May overflow/underflow if the exponent is wild. Obviously.
        """
        return self.mantissa * (10.0**self.exponent)

    def symlog(self) -> float:
        """
        Computes the Symmetric Logarithmic value.
        Useful for plotting data that spans many orders of magnitude and includes zero/negative values.
        """
        if self.mantissa == 0.0:
            return 0.0

        sign = np.sign(self.mantissa)
        abs_mantissa = abs(self.mantissa)

        val_log10 = np.log10(abs_mantissa) + self.exponent

        magnitude_diff = val_log10 - LOG_LINTHRESH

        if magnitude_diff > 16.0:
            return sign * magnitude_diff
        else:
            return sign * np.log10(1.0 + abs(self.approx_f64()) / LINTHRESH)

    def format(self) -> str:
        """
        Formats the number nicely for display.
        """
        if self.mantissa == 0.0:
            return "0"

        sign_str = "-" if self.mantissa < 0.0 else ""
        abs_mantissa = abs(self.mantissa)

        if self.exponent < -2 or self.exponent > 3:
            return f"{sign_str}{abs_mantissa:.1f}e{self.exponent:.0f}"
        else:
            real_val = abs_mantissa * (10.0**self.exponent)
            # Remove trailing zeros and decimal point if not needed
            return f"{sign_str}{real_val:.6f}".rstrip("0").rstrip(".")


def vectorized_approx_f64(mantissa: np.ndarray, exponent: np.ndarray) -> np.ndarray:
    """
    Vectorized version of approx_f64 using numpy arrays.
    """
    return mantissa * (10.0**exponent)


def vectorized_symlog(mantissa: np.ndarray, exponent: np.ndarray) -> np.ndarray:
    """
    Vectorized Symmetric Log transformation for arrays of mantissas/exponents.
    High performance version for plotting large datasets.
    """
    # Handle zeros
    result = np.zeros_like(mantissa)
    non_zero = mantissa != 0

    if not np.any(non_zero):
        return result

    m = mantissa[non_zero]
    e = exponent[non_zero]

    sign = np.sign(m)
    abs_mantissa = np.abs(m)

    # log10(|x|)
    val_log10 = np.log10(abs_mantissa) + e

    magnitude_diff = val_log10 - LOG_LINTHRESH

    # Mask for huge numbers
    huge_mask = magnitude_diff > 16.0
    small_mask = ~huge_mask

    # Prepare output for non-zero elements
    out_vals = np.zeros_like(m)

    # Calculate for huge numbers
    if np.any(huge_mask):
        out_vals[huge_mask] = sign[huge_mask] * magnitude_diff[huge_mask]

    # Calculate for small numbers
    if np.any(small_mask):
        approx_val = m[small_mask] * (10.0 ** e[small_mask])
        out_vals[small_mask] = sign[small_mask] * np.log10(
            1.0 + np.abs(approx_val) / LINTHRESH
        )

    result[non_zero] = out_vals

    return result


def symlog_formatter(val: float) -> str:
    """
    Formats a value on the SymLog axis for humans to read.
    Reverses the SymLog math to show actual values.
    """
    if val == 0.0:
        return "0"
    elif abs(val + LOG_LINTHRESH) < 0.00001:
        return "1"

    sign_str = "-" if val < 0.0 else ""
    abs_plot_y = abs(val)

    target_log10 = LOG_LINTHRESH + abs_plot_y

    exponent = np.floor(target_log10)
    fractional = target_log10 - exponent
    mantissa = 10.0**fractional

    if exponent < -2.0 or exponent > 3.0:
        return f"{sign_str}{mantissa:.1f}e{exponent:.0f}"
    else:
        real_val = mantissa * (10.0 ** int(exponent))
        return f"{sign_str}{real_val:.4f}".rstrip("0").rstrip(".")
