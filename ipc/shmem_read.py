import numpy as np
from multiprocessing import shared_memory
from matplotlib import pyplot as plt

class ShmemReader:

	def __init__(self, shname):
		self.shmem = shared_memory.SharedMemory(name=shname)
		self.data = np.ndarray([1216, 1936], dtype=np.uint8, buffer=self.shmem.buf)

	def __del__(self):
		self.shmem.close()

	def show(self):
		plt.imshow(self.data)
		plt.show()

