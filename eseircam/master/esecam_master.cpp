
#include "esecam_master.h"

void esecam_cb (const char* szCameraName,
				int eventID,
				int eventDataSize,
				void* pEventData,
                unsigned char* pFrame,
				void*  pUserData)
{
	EseCamMaster * master = (EseCamMaster)(pUserData);
	if (eventID == NEW_FRAME || eventID == NEW_FRAME_SYNCHRO_ERROR)
	{
		if (pFrame) {

		} else {
			printf("Esecam callback: wrong frame ptr\n");
		}
	} else {
		printf("Esecam callback Event = %d, size = %d\n", eventID, eventDataSize);
	}
}

EseCamMaster::EseCamMaster()
{
	d_is_alive.store(false);
	d_uds = std::make_unique<UdsUniComm>(EYEMETER_ROLE_CAM);
}

EseCamMaster::~EseCamMaster()
{
	stop();
}

void EseCamMaster::stop()
{
	d_is_alive.store(false);
	d_uds->stop();
	if (d_comm_thread.joinable())
		d_comm_thread.join();
}

int EseCamMaster::setup()
{
	if (d_uds->start() < 0) return -1;

	d_cam_name = esecam_getname();
	if (d_cam_name == "") return -1;

	d_comm_thread = std::thread(&EseCamMaster::comm_process, this);
}

void EseCamMaster::comm_process()
{
	while (d_is_alive.load()) {
		UdsUniPack pack;
		if (d_uds->wait_recv(pack) == 0) {
			switch (pack.msg.title) {
				case USDUNI_TITLE_STREAM_START:
					break;
				case USDUNI_TITLE_STREAM_STOP:
					USB_StopVideoStream(d_cam_name.c_str());
					break;
				case UDSUNI_TITLE_FRAME_FREE:
					break;
				default:
					break;
			}
		}
	}
}
