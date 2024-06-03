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
#include <vector>

struct ShmemBlock
{
	unsigned int id;
	unsigned int size;
	void * ptr;
};

/**
 * Creates shm buffer. Returns pointers for writting data.
 */
class ShmemBlockAllocator
{
public:

	ShmemBlockAllocator(unsigned int unit_size, unsigned int n_units, std::string name);
	~ShmemBlockAllocator();

	int setup();

	int block_alloc(ShmemBlock & block);
	int block_free(unsigned int block_id);
	unsigned int check_free();

	void show(unsigned int n_bytes) {
		for (unsigned int i = 0; i < n_bytes; i++) {
			printf("%x ", *((unsigned char*)(d_mmap_ptr) + i));
		}
		printf("\n");
	}

protected:

	int d_handle;
	volatile void * d_mmap_ptr;

	unsigned int d_unit_size;
	unsigned int d_n_units;
	std::string d_name;
	std::vector<ShmemBlock> d_blocks;

};

/**
 * Read shmem.
 */
class ShmemBlockReader
{
public:

	ShmemBlockReader(unsigned int unit_size, unsigned int n_units, std::string name);
	~ShmemBlockReader();

	int setup();

	int get_block(ShmemBlock & block);

protected:

	int d_handle;
	volatile void * d_mmap_ptr;

	unsigned int d_unit_size;
	unsigned int d_n_units;
	std::string d_name;

};

#endif //_SHMEM_ALLOC_H_
