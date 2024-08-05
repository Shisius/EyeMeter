#include "eye_shmem.h"

MeasResultShmemReader::MeasResultShmemReader() : ShmemReader(MEAS_RESULT_SHBUF_SIZE, MEAS_RESULT_SHBUF_NAME)
{
	;
}

int MeasResultShmemReader::get_pupils(SharedPupilImage & left, SharedPupilImage & right)
{
	left.width = SHARED_PUPIL_IMAGE_WIDTH;
	left.height = SHARED_PUPIL_IMAGE_HEIGHT;
	left.size = SHARED_PUPIL_IMAGE_SIZE;
	left.data_ptr = (void*)(d_mmap_ptr);

	right.width = SHARED_PUPIL_IMAGE_WIDTH;
	right.height = SHARED_PUPIL_IMAGE_HEIGHT;
	right.size = SHARED_PUPIL_IMAGE_SIZE;
	right.data_ptr = (void*)((char*)(d_mmap_ptr) + SHARED_PUPIL_IMAGE_SIZE);
	return 0;
}

int MeasResultShmemReader::get_skew(std::vector<EyeSkewCoords> & left, std::vector<EyeSkewCoords> & right, unsigned int max_n_points)
{
	// left
	left.clear();
	for (unsigned int i = 0; i < max_n_points; i++) {
		EyeSkewCoords skew;
		memcpy(&skew, (EyeSkewCoords*)((char*)(d_mmap_ptr) + 2*SHARED_PUPIL_IMAGE_SIZE) + i, sizeof(EyeSkewCoords));
		if (skew.x == MEAS_RESULT_ANGLE_INVAL || skew.y == MEAS_RESULT_ANGLE_INVAL) {
			break;
		} else {
			left.push_back(skew);
		}
	}
	// right
	right.clear();
	for (unsigned int i = 0; i < max_n_points; i++) {
		EyeSkewCoords skew;
		memcpy(&skew, (EyeSkewCoords*)((char*)(d_mmap_ptr) + 2*SHARED_PUPIL_IMAGE_SIZE) + i + max_n_points, sizeof(EyeSkewCoords));
		if (skew.x == MEAS_RESULT_ANGLE_INVAL || skew.y == MEAS_RESULT_ANGLE_INVAL) {
			break;
		} else {
			right.push_back(skew);
		}
	}
	return 0;
}
