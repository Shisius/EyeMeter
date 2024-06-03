#include "shmem_alloc.h"

ShmemBlockAllocator::ShmemBlockAllocator(unsigned int unit_size, unsigned int n_units, std::string name) :
					 d_unit_size(unit_size), d_n_units(n_units), d_name(name)
{
	// Blocks
	for (unsigned int i = 0; i < d_n_units; i++) {
		ShmemBlock sb({i, unit_size, nullptr});
		d_blocks.push_back(sb);
	}
}

ShmemBlockAllocator::~ShmemBlockAllocator()
{
	if(munmap((void*)(d_mmap_ptr), d_n_units *d_unit_size)){
        perror("ShmemBlockAllocator::munmap");
    }
    if(shm_unlink(d_name.c_str())){
        perror("ShmemBlockAllocator::shm_unlink");
    }
}

int ShmemBlockAllocator::setup()
{
	// Create
	int d_handle = shm_open(d_name.c_str(), O_RDWR | O_CREAT | O_EXCL, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
    if (d_handle < 0) {
    	perror("ShmemBlockAllocator::shm_open");
    	shm_unlink(d_name.c_str());
    	usleep(10000);
    	d_handle = shm_open(d_name.c_str(), O_RDWR | O_CREAT | O_EXCL, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
    	if (d_handle < 0)
    		return -1;
    }

    // and set its size to the size of struct shmbuf
    if(ftruncate(d_handle, d_n_units *d_unit_size) == -1) { //seek??            
        perror("ShmemBlockAllocator::ftruncate");
        return -1;
    }

    // mmap
    d_mmap_ptr = mmap(nullptr, d_n_units * d_unit_size, PROT_READ | PROT_WRITE, MAP_SHARED, d_handle, 0);
	if(d_mmap_ptr == MAP_FAILED){             
        perror("ShmemBlockAllocator::mmap");
        return -1;
    }
    if(close(d_handle) == -1){ /* No longer needed */                       
        perror("ShmemBlockAllocator::close");
    }
    return 0;
}

int ShmemBlockAllocator::block_alloc(ShmemBlock & block)
{
	for (ShmemBlock & sb : d_blocks) {
		if (sb.ptr == nullptr) {
			sb.size = d_unit_size;
			sb.ptr = (void*)((char*)(d_mmap_ptr) + sb.id * d_unit_size);
			block = sb;
			return 0;
		}
	}
	return -1;
}

int ShmemBlockAllocator::block_free(unsigned int block_id)
{
	if (block_id < d_blocks.size()) {
		if (d_blocks.at(block_id).id == block_id) {
			d_blocks.at(block_id).ptr = nullptr;
			return 0;
		}
	}
	return -1;
}

unsigned int ShmemBlockAllocator::check_free()
{
	unsigned int n_free = 0;
	for (ShmemBlock & sb : d_blocks) {
		if (sb.ptr == nullptr) n_free++;
	}
	return n_free;
}

ShmemBlockReader::ShmemBlockReader(unsigned int unit_size, unsigned int n_units, std::string name) :
				  d_unit_size(unit_size), d_n_units(n_units), d_name(name)
{
	;
}

int ShmemBlockReader::setup()
{
	// Open
	int d_handle = shm_open(d_name.c_str(), O_RDWR, 0);
    if (d_handle == -1) {
      perror("ShmemBlockReader::shm_open");
      return -1;
    }
    // mmap
    d_mmap_ptr = mmap(nullptr, d_n_units *d_unit_size, PROT_READ | PROT_WRITE, MAP_SHARED, d_handle, 0);
	if(d_mmap_ptr == MAP_FAILED){             
        perror("ShmemBlockReader::mmap");
        return -1;
    }
    if(close(d_handle) == -1){ /* No longer needed */                       
        perror("ShmemBlockReader::close");
    }
    return 0;
}

ShmemBlockReader::~ShmemBlockReader()
{
	if(munmap((void*)(d_mmap_ptr), d_n_units *d_unit_size)){
        perror("ShmemBlockReader::munmap");
    }
}

int ShmemBlockReader::get_block(ShmemBlock & block)
{
	if (block.id < d_n_units) {
		block.size = d_unit_size;
		block.ptr = (void*)((char*)(d_mmap_ptr) + block.id * d_unit_size);
		return 0;
	}
	return -1;
}
