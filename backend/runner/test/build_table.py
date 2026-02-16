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
    parser.add_argument("algo"  , help = f"Algo   to test on {methods}")
    parser.add_argument("x", help = "x to use")
    parser.add_argument("n", help = "n to use")
    parser.add_argument("m", help = "m to use")
    return parser.parse_args()

def build_table(suf : str, ser : str, algo : str, x : str, n : int, m : int):

    algo_   = getattr(ps, algo + suf)
    series_ = getattr(ps, ser + suf)
    type_   = getattr(ps, suf)

    data = [[float("nan") for i in range(n+1)] for j in range(m+1)]

    to_feed, s_sum = series_(n + 4 * m, type_(float(x)))

    for i in range(n+1):
        for j in range(m+1):
            try:
                data[j][i] = algo_(i, j, to_feed).__repr__()
            except OverflowError:
                data[j][i] = float('nan')
            except ValueError:
                data[j][i] = float('nan')
    
    res = pd.DataFrame(data = data)
    res.to_csv(f"{algo}_{ser}_{x}.csv")



if __name__ == "__main__":

    args = get_info_from_args()
    build_table(args.suffix, args.series, args.algo, args.x, int(args.n), int(args.m))
