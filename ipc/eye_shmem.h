#ifndef _EYE_SHMEM_H_
#define _EYE_SHMEM_H_

#include "shmem_alloc.h"
#include "comm_types.h"
#include "ai_types.h"

#define MEAS_RESULT_SHBUF_SIZE 0x40000
#define MEAS_RESULT_ANGLE_INVAL 1000.0
#define SHARED_PUPIL_IMAGE_WIDTH 256
#define SHARED_PUPIL_IMAGE_HEIGHT 256
#define SHARED_PUPIL_IMAGE_SIZE (SHARED_PUPIL_IMAGE_WIDTH*SHARED_PUPIL_IMAGE_HEIGHT)

struct SharedPupilImage
{
	unsigned short width;
	unsigned short height;
	unsigned int size;
	void * data_ptr;
};

class MeasResultShmemReader : public ShmemReader
{
public:

	MeasResultShmemReader();
	// ~MeasResultShmemReader();

	// Get pupil images. Returns 0 on success, -1 on fail.
	int get_pupils(SharedPupilImage & left, SharedPupilImage & right);

	// Get skew coords. max_n_points = n_frames in meas = n_led_pos*n_repeat
	int get_skew(std::vector<EyeSkewCoords> & left, std::vector<EyeSkewCoords> & right, unsigned int max_n_points);

protected:

};

#endif //_EYE_SHMEM_H_
