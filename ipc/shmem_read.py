import numpy as np
from multiprocessing import shared_memory
from matplotlib import pyplot as plt

class ShmemReader8:

	def __init__(self, shname):
		self.shmem = shared_memory.SharedMemory(name=shname)
		self.data = np.ndarray([1216, 1936], dtype=np.uint8, buffer=self.shmem.buf)

	def __del__(self):
		self.shmem.close()

	def show(self):
		plt.imshow(self.data)
		plt.show()

class ShmemReader12:

	def __init__(self, shname, w = 1936, h = 1216):
		self.shmem = shared_memory.SharedMemory(name=shname)
		self.size = round(h * w * 3 / 2)
		self.buf = np.ndarray([size,], dtype=np.uint8, buffer=self.shmem.buf)
		self.data = np.ndarray([h, w], dtype=np.uint16)

	def __del__(self):
		self.shmem.close()

	def show(self):
		plt.imshow(self.data)
		plt.show()

	def parse(self):
                i_data = 0
                for i in range(0, self.size, 3):
                        self.data[i_data] = self.buf[i] +  ( (self.buf[i+1] & 0x0F) >> 8)
                        i_data+=1
                        self.data[i_data] = (self.buf[i+2]>>4) +  (self.buf[i+1] & 0xF0)
                        i_data+=1

