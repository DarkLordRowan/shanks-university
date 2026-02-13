import matplotlib.pyplot as plt
import numpy as np
from obj_in_pyshanks import series, methods, suffixes
import sys
import argparse
import pandas as pd

sys.path.append("../pyshanks")

import pyshanks as ps

def get_info_from_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser("test_algos")
    parser.add_argument("suffix", help = f"Suffix to use: {suffixes}")
    parser.add_argument("series", help = f"Series to test on {series}")
    parser.add_argument("x", help = "x to use")
    parser.add_argument("n", help = "n to use")
    parser.add_argument("m", help = "m to use")
    return parser.parse_args()

def test_plot(suf : str, ser : str, x : str, n : int, m : int):

    figure, ax = plt.subplots(figsize = (21, 19))

    series_ = getattr(ps, ser + suf)
    type_   = getattr(ps, suf)

    data, s_sum = series_(n + 4 * m, type_(float(x)))
    
    ax.hlines(float(s_sum.__repr__()), 1, n+1, color = "red", linestyle = "--")

    for method in methods:

        method_ = getattr(ps, method+suf)

        accelerated = []
        mistakes = []
        a = type_(0)
        for i in range(1, n+1):
            try:
                a = method_(i, m, data)
            except OverflowError:
                mistakes.append([i, float(a.__repr__())])
                print("error occured in " + method + suf)
            accelerated.append(a)

        ax.plot(range(1,n+1), [float(x.__repr__()) for x in accelerated], label = method + suf)
        ax.scatter([x[0] for x in mistakes], [x[1] for x in mistakes], label = method + suf, marker = "x")
    
    plt.legend()
    plt.tight_layout(pad = 1.5)
    plt.show()


if __name__ == "__main__":

    args = get_info_from_args()
    test_plot(args.suffix, args.series, args.x, int(args.n), int(args.m))
