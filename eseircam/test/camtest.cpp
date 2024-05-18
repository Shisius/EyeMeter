#include "esecam_print.h"
#include <fstream>

void esecam_cb (const char* szCameraName,
				int eventID,
				int eventDataSize,
				void* pEventData,
                unsigned char* pFrame,
				void*  pUserData)
{
	printf("Capture! Event = %d\n", eventID);
	if (eventID == NEW_FRAME || eventID == NEW_FRAME_SYNCHRO_ERROR)
	{
		USB_StopVideoStream(szCameraName);
		if (pFrame) {
			std::ofstream ofs("frame.bin", std::ofstream::out | std::ofstream::binary | std::ofstream::app);
			RET_SERV_DATA_HEADER rRetHeader = *((RET_SERV_DATA_HEADER *)(pFrame));
			printf("header size = %d, type = %d\n", rRetHeader.ulNextDataSize, rRetHeader.ulServDataType);
			ofs.write((const char *)pFrame, rRetHeader.ulNextDataSize);
			ofs.rdbuf()->pubsync();
    		ofs.close();
			if (rRetHeader.ulServDataType == 2) {
				RET_SERV_DATA_TYPE_2 rRetSrv2 = *((RET_SERV_DATA_TYPE_2 *)(pFrame));
				unsigned int width = rRetSrv2.StreamServData.ulWidth;
				unsigned int height = rRetSrv2.StreamServData.ulHeight;
				printf("Width = %d, Height = %d\n", width, height);
			} else printf("Serv data type is not 2!\n");
		} else printf("Error: wrong pFrame ptr\n");
	}
}

int main()
{
	std::string cam_name;
	printf("list result = %d\n", esecam_list());
	std::cin.get();

	cam_name = esecam_getname();

	esecam_print_caps();

	eseusbcam_unsigned_long_t feature;
	printf("get feature result = %d\n", USB_GetCameraFeature(cam_name.c_str(), CONTRAST, &feature));
	printf("feature = %d\n", feature);

	std::cin.get();
	VIDEO_STREAM_PARAM_EX oStreamParam = {};
	oStreamParam.ppReturnedParams = nullptr;
	oStreamParam.pBuffBitmap = nullptr;
	oStreamParam.pDirectBuffer = nullptr;
	oStreamParam.pEvent = nullptr;
	printf("start result = %d\n", USB_VideoOnCallbackStart(cam_name.c_str(), &oStreamParam, esecam_cb, nullptr));

	std::cin.get();
	printf("finished\n");

	return 0;
}
