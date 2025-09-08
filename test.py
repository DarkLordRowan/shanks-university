import numpy as np
from build.pyshanks import SeriesFromArray

# make a huge numpy array of doubles
arr = np.linspace(0.0, 1.0, 1_000_000, dtype=np.float64)

s = SeriesFromArray(arr)   # wraps arr without copying
print(s(10))               # 11th element
print(s.S_n(99))           # sum of first 100 elements
print(s.get_sum())         # sum of the whole array
