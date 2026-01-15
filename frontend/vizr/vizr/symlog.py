"""
Author: Sobolev Y. A.
Description: Provides scientific number handling and symmetric logarithmic scaling for visualization.
"""

import numpy as np

# we define a very small threshold for the linear region around zero.
# in a standard logarithmic scale, log(0) is undefined and log(negative) is complex.
# symmetric log (symlog) solves this by being linear around zero and logarithmic elsewhere.
# LINTHRESH is the range (-LINTHRESH, LINTHRESH) where the plot is linear.
# LOG_LINTHRESH is simply log10(LINTHRESH) precomputed for speed.
LINTHRESH = 1e-50
LOG_LINTHRESH = -50.0


class Scientific:
    """
    A custom arbitrary-precision floating point number representation.
    Not to lose precision when loading from Parquet.
    """

    def __init__(self, mantissa: float, exponent: int):
        """
        Initializes a Scientific number.

        Inputs:
            mantissa (float): The significant digits of the number.
            exponent (int): The power of 10 to multiply the mantissa by.
        """
        self.mantissa = mantissa
        self.exponent = exponent

    @classmethod
    def from_str(cls, s: str) -> "Scientific":
        """
        Parses a scientific string (e.g., "1.23e-100") into a Scientific object.
        We expect strings because standard JSON or Parquet floats often lack the bits
        needed for exponents like -10000.

        Inputs:
            s (str): The string representation of the number (e.g., "1.2e-5").

        Outputs:
            Scientific: The parsed Scientific object.
        """
        if not s:
            return cls(0.0, 0)
        
        # standardize the string to handle 'e' or 'E'.
        s = s.lower()
        if "e" in s:
            parts = s.split("e")
            mantissa = float(parts[0])
            exponent = int(parts[1])
            return cls(mantissa, exponent)
        else:
            # if there is no exponent part, it is just a regular number with exponent 0.
            return cls(float(s), 0)

    def approx_f64(self) -> float:
        """
        Converts to a standard Python float (64-bit).
        Warning: May overflow/underflow if the exponent is wild. Obviously.
        This is mostly used when we know the value is within displayable range
        or when we accept the loss of precision for a pixel coordinate.

        Outputs:
            float: The 64-bit floating point approximation.
        """
        return self.mantissa * (10.0**self.exponent)

    def symlog(self) -> float:
        """
        Computes the Symmetric Logarithmic value.
        Useful for plotting data that spans many orders of magnitude and includes zero/negative values.
        
        The transformation function is roughly:
        y = sign(x) * (log10(|x|) - log10(threshold)) for |x| > threshold
        y = sign(x) * linear_interpolation(...) for |x| <= threshold

        Outputs:
            float: The symmetric logarithm of the value.
        """
        if self.mantissa == 0.0:
            return 0.0

        sign = np.sign(self.mantissa)
        abs_mantissa = abs(self.mantissa)

        # calculate the log10 magnitude using the separate exponent to avoid underflow
        # before taking the log.
        # log10(m * 10^e) = log10(m) + e
        val_log10 = np.log10(abs_mantissa) + self.exponent

        # how far are we from the linear threshold?
        magnitude_diff = val_log10 - LOG_LINTHRESH

        # if we are significantly larger than the threshold, we use the log behavior.
        # we use 16.0 as a somewhat arbitrary cutoff where we switch to pure log behavior.
        if magnitude_diff > 16.0:
            return sign * magnitude_diff
        else:
            # in the transition region or near zero, we use a function that behaves linearly
            # to avoid the singularity at zero.
            return sign * np.log10(1.0 + abs(self.approx_f64()) / LINTHRESH)

    def format(self) -> str:
        """
        Formats the number nicely for display.
        We want short strings for tooltips, so we strip unnecessary zeros.

        Outputs:
            str: A string representation suitable for UI display.
        """
        if self.mantissa == 0.0:
            return "0"

        sign_str = "-" if self.mantissa < 0.0 else ""
        abs_mantissa = abs(self.mantissa)

        # if the number is very large or very small, force scientific notation.
        if self.exponent < -2 or self.exponent > 3:
            return f"{sign_str}{abs_mantissa:.1f}e{self.exponent:.0f}"
        else:
            # otherwise, try to show it as a normal decimal number.
            real_val = abs_mantissa * (10.0**self.exponent)
            # remove trailing zeros and decimal point if not needed
            return f"{sign_str}{real_val:.6f}".rstrip("0").rstrip(".")


def vectorized_approx_f64(mantissa: np.ndarray, exponent: np.ndarray) -> np.ndarray:
    """
    Vectorized version of approx_f64 using numpy arrays.
    Essential for plotting performance when handling thousands of points.

    Inputs:
        mantissa (np.ndarray): Array of mantissa values.
        exponent (np.ndarray): Array of exponent values.

    Outputs:
        np.ndarray: Array of approximate float64 values.
    """
    return mantissa * (10.0**exponent)


def vectorized_symlog(mantissa: np.ndarray, exponent: np.ndarray) -> np.ndarray:
    """
    Vectorized Symmetric Log transformation for arrays of mantissas/exponents.
    High performance version for plotting large datasets.
    
    This function replicates the logic of Scientific.symlog but operates on entire
    numpy arrays at once using masks.

    Inputs:
        mantissa (np.ndarray): Array of mantissas.
        exponent (np.ndarray): Array of exponents.

    Outputs:
        np.ndarray: Array of transformed values on the SymLog scale.
    """
    # handle zeros first. The result for 0 is 0.
    result = np.zeros_like(mantissa)
    non_zero = mantissa != 0

    if not np.any(non_zero):
        return result

    # we only process non-zero elements to avoid log(0) warnings.
    m = mantissa[non_zero]
    e = exponent[non_zero]

    sign = np.sign(m)
    abs_mantissa = np.abs(m)

    # log10(|x|) = log10(|m|) + e
    val_log10 = np.log10(abs_mantissa) + e

    magnitude_diff = val_log10 - LOG_LINTHRESH

    # we treat numbers much larger than the threshold differently than those close to zero.
    huge_mask = magnitude_diff > 16.0
    small_mask = ~huge_mask

    # prepare output container for the non-zero elements
    out_vals = np.zeros_like(m)

    # for huge numbers, the linear term 1.0 is negligible, so we just use the log difference.
    if np.any(huge_mask):
        out_vals[huge_mask] = sign[huge_mask] * magnitude_diff[huge_mask]

    # for small numbers, we compute the full formula to ensure smooth transition through zero.
    if np.any(small_mask):
        approx_val = m[small_mask] * (10.0 ** e[small_mask])
        out_vals[small_mask] = sign[small_mask] * np.log10(
            1.0 + np.abs(approx_val) / LINTHRESH
        )

    # scatter the computed values back into the result array
    result[non_zero] = out_vals

    return result


def symlog_formatter(val: float) -> str:
    """
    Formats a value on the SymLog axis for humans to read.
    Reverses the SymLog math to show actual values.
    
    The axis ticks are in the transformed 'symlog' space, so we need to
    invert the transformation to show the user the real number (e.g., 1e-50).

    Inputs:
        val (float): The value in SymLog space (from the plot axis).

    Outputs:
        str: The human-readable string representation of the original value.
    """
    if val == 0.0:
        return "0"
    # if we are right on the threshold, we just show 1 (scaled) or similar basic value.
    elif abs(val + LOG_LINTHRESH) < 0.00001:
        return "1"

    sign_str = "-" if val < 0.0 else ""
    abs_plot_y = abs(val)

    # invert the log transformation:
    # plot_y = log10(x) - log10(threshold)
    # log10(x) = plot_y + log10(threshold)
    target_log10 = LOG_LINTHRESH + abs_plot_y

    exponent = np.floor(target_log10)
    fractional = target_log10 - exponent
    mantissa = 10.0**fractional

    # choose format based on magnitude
    if exponent < -2.0 or exponent > 3.0:
        return f"{sign_str}{mantissa:.1f}e{exponent:.0f}"
    else:
        real_val = mantissa * (10.0 ** int(exponent))
        return f"{sign_str}{real_val:.4f}".rstrip("0").rstrip(".")
