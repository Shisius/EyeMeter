#ifndef _ESECAM_MASTER_H_
#define _ESECAM_MASTER_H_

#include <memory>
#include "cam_types.h"
#include "esecam_print.h"
#include "shmem_alloc.h"
#include "udsunicomm.h"

class EseCamMaster
{
public:

	EseCamMaster();
	~EseCamMaster();

	int setup();
	void stop();

	void frame_ready_event(SharedFrame & frame, unsigned char* frame_ptr);

protected:

	PUSB_CAMERA_CAPABILITES d_caps = nullptr;
	std::string d_cam_name;
	StreamSettings d_stream_settings;

	std::unique_ptr<ShmemBlockAllocator> d_shmem;
	std::unique_ptr<UdsUniComm> d_uds;

	std::thread d_comm_thread;
	std::atomic<bool> d_is_alive;

	int start_stream();

	int get_frame_soft_trigger();

	void comm_process();

};

#endif // _ESECAM_MASTER_H_
