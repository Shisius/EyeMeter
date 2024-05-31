#ifndef _ESECAM_MASTER_H_
#define _ESECAM_MASTER_H_

#include <memory>
#include "esecam_print.h"
#include "udsunicomm.h"

class EseCamMaster
{
public:

	EseCamMaster();
	~EseCamMaster();

	int setup();
	void stop();

protected:

	PUSB_CAMERA_CAPABILITES d_caps = nullptr;
	std::string d_cam_name;

	std::unique_ptr<ShmemBlockAllocator> d_shmem;
	std::unique_ptr<UdsUniComm> d_uds;

	std::thread d_comm_thread;
	std::atomic<bool> d_is_alive;

	void comm_process();

};

#endif // _ESECAM_MASTER_H_
