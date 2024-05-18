#ifndef _SHMEM_ALLOC_H_
#define _SHMEM_ALLOC_H_

#include <sys/mman.h>  // shm_open, shm_unlink, mmap, munmap,
#include <sys/stat.h> //  fstat, S_IRUSR, S_IWSR, S_IRGRP,| S_IWGRP
#include <fcntl.h> //O_* constants
#include <unistd.h> // ftruncate
#define _BSD_SOURCE // for ftruncate
#include <pthread.h> // pthread_mutex_t, pthread_mutexattr_t,
                     // pthread_mutexattr_init, pthread_mutexattr_setpshared,
                     // pthread_mutex_init, pthread_mutex_destroy

#include <memory>
#include <cstring> //memcpy
#include <string>

class ShmemBlockAllocator
{
public:

	ShmemBlockAllocator(unsigned int unit_size, unsigned int n_units, std::string name);
	~ShmemBlockAllocator();

	void * block_alloc();

protected:

	int d_handle;

	unsigned int d_unit_size;
	unsigned int d_n_units;

};

#endif //_SHMEM_ALLOC_H_
