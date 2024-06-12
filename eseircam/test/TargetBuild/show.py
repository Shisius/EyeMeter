import numpy as np
from matplotlib import pyplot as plt
import sys

fname = "frame"
if len(sys.argv) > 1:
    fname += sys.argv[1]
fname += ".bin"

f = open(fname, 'rb')
s=f.read()
f.close()
a = np.ndarray([1216,1936], dtype = np.uint8, buffer=s)
plt.imshow(a)
plt.show()
