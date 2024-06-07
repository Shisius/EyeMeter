
#include "esecam_master.h"

void esecam_callback (const char* szCameraName,
						int eventID,
						int eventDataSize,
						void* pEventData,
		                unsigned char* pFrame,
						void*  pUserData)
{
	EseCamMaster * master = (EseCamMaster*)(pUserData);
	if (eventID == NEW_FRAME || eventID == NEW_FRAME_SYNCHRO_ERROR)
	{
		if (pFrame) {
			RET_SERV_DATA_HEADER rRetHeader = *((RET_SERV_DATA_HEADER *)(pFrame));
			if (rRetHeader.ulServDataType == 3) {
				RET_SERV_DATA_ITK4_0 rRetSrv2 = *((RET_SERV_DATA_ITK4_0 *)(pFrame));
				SharedFrame frame;
				frame.width = rRetSrv2.StreamServData.usWidth;
				frame.height = rRetSrv2.StreamServData.usHeight;
				frame.format = rRetSrv2.StreamServData.chBitPerPixel;
				frame.size = frame.width * frame.height * frame.format / 8;
				// printf("Width = %d, Height = %d, bpp =%d\n", frame.width, frame.height, frame.format);
				master->frame_ready_event(frame, pFrame+sizeof(RET_SERV_DATA_ITK4_0));
			} else printf("Esecam callback: wrong Serv data type!\n");
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

	d_stream_settings.cam_format = 1;

	d_uds = std::make_unique<UdsUniComm>(EYEMETER_ROLE_CAM);
}

EseCamMaster::~EseCamMaster()
{
	stop();
}

void EseCamMaster::stop()
{
	USB_StopVideoStream(d_cam_name.c_str());
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

	if (USB_GetCameraCapabilites(d_cam_name.c_str(), &d_caps) != 1) return -1;

	if (USB_SetFormat(d_cam_name.c_str(), d_stream_settings.cam_format) != 1) return -1;

	d_stream_settings.frame_width = d_caps->CameraFormats.formatsList[d_stream_settings.cam_format].MaxWidth;
	d_stream_settings.frame_height = d_caps->CameraFormats.formatsList[d_stream_settings.cam_format].MaxHeight;

	d_comm_thread = std::thread(&EseCamMaster::comm_process, this);
	return 0;
}

int EseCamMaster::start_stream()
{
	if (USB_SetFormat(d_cam_name.c_str(), d_stream_settings.cam_format) != 1) return -1;

	VIDEO_STREAM_PARAM_EX oStreamParam = {};
	oStreamParam.ppReturnedParams = nullptr;
	oStreamParam.pBuffBitmap = nullptr; 
	oStreamParam.pDirectBuffer = nullptr;
	oStreamParam.pEvent = nullptr;
	if (USB_VideoOnCallbackStart(d_cam_name.c_str(), &oStreamParam, esecam_callback, this) != 1) 
	{
		printf("EseCamMaster:: stream start failed!\n");
		return -1;
	}

	d_uds->send(UDSUNI_TITLE_STREAM_RUNNING, d_stream_settings);

	return 0;
}

int EseCamMaster::get_frame_soft_trigger()
{
	if (USB_SetCameraFeature(d_cam_name.c_str(), ONE_MULTI_FRAME_REQUEST, 1) == 1) return 0;
	return -1;
}

void EseCamMaster::comm_process()
{
	while (d_is_alive.load()) {
		UdsUniPack pack;
		if (d_uds->wait_recv(pack) == 0) {
			switch (pack.msg.title) {
				case UDSUNI_TITLE_STREAM_START:
					start_stream();
					break;
				case UDSUNI_TITLE_STREAM_STOP:
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

void EseCamMaster::frame_ready_event(SharedFrame & frame, unsigned char* frame_ptr)
{

}
