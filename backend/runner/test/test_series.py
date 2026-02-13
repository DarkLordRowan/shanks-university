import matplotlib.pyplot as plt
import numpy as np
from obj_in_pyshanks import series, methods, suffixes
import sys
import argparse
import pandas as pd

sys.path.append("../pyshanks")

import pyshanks as ps

def get_info_from_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser("test_series")
    parser.add_argument("suffix", help = f"Suffix to use: {suffixes}")
    parser.add_argument("series", help = f"Series to test on {series}")
    parser.add_argument("x", help = "x to use")
    parser.add_argument("n", help = "n to use")
    return parser.parse_args()

def test_plot(suf : str, ser : str, x : str, n : int):

    figure, (ax1, ax2) = plt.subplots(2, 1, figsize = (21, 19))

    series_ = getattr(ps, ser + suf)
    type_   = getattr(ps, suf)

    data, s_sum = series_(n, type_(float(x)))
   
    Sn, an = data.Sn, data.an

    plt.suptitle(f"delta = {s_sum - Sn[-1]}")
    
    ax1.hlines(float(s_sum.__repr__()), 0, n, color = "red", linestyle = "--")

    ax1.plot(range(n), [float(x.__repr__()) for x in Sn], label = ser + suf)
    ax1.legend()
    
    ax2.plot(range(n), [abs(float(x.__repr__())) for x in an], label = "an")
    ax2.set_yscale("log")
    
    
    plt.tight_layout(pad = 1.5)
    plt.show()


if __name__ == "__main__":

    args = get_info_from_args()
    test_plot(args.suffix, args.series, args.x, int(args.n))
