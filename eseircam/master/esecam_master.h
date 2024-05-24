#ifndef _ESECAM_MASTER_H_
#define _ESECAM_MASTER_H_

#include "eseusbcam.h"

class EseCamMaster
{
public:

	EseCamMaster();
	~EseCamMaster();

protected:

	PUSB_CAMERA_CAPABILITES d_caps = nullptr;

};

#endif // _ESECAM_MASTER_H_
