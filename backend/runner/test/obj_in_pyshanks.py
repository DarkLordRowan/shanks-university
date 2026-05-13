import matplotlib.pyplot as plt
import numpy as np
import re
from scipy.signal import savgol_filter
import sys

sys.path.append("../pyshanks")

import pyshanks as ps

suffixes : list[str] = [
    "CF32",
    "CF64",
    "CFLong",
    "CArb",
    "CFPrec",
    "F32",
    "F64",
    "FLong",
    "Arb",
    "FPrec",
]

def exclude(x : str) -> str:
    for suf in suffixes:
        x = x.replace(suf, "")
    return x


methods : list[str] = sorted(list(set([
    exclude(x)
    for x in dir(ps)
    if not re.search(r"(.*Algorithm[U,T,V]?~?)", x) is None
])))

series : list[str] = sorted(list(set([
    re.search(r"(.*Series)", x).group(1)
    for x in dir(ps)
    if not re.search(r"(.*Series)", x) is None
])))

filters : list[str] = sorted(list(set([
    re.search(r"(.*Filter)", x).group(1)
    for x in dir(ps)
    if not re.search(r"(.*Filter)", x) is None
])))

if __name__ == "__main__":

    print("SERIES",*series, sep = "\n")
    print()
    print("METHODS",*methods, sep = "\n")
    print()
    print("FILTERS",*filters, sep = "\n")
    print()
