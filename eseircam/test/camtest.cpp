#include "esecam_print.h"
#include <fstream>
#include "udsunicomm.h"
#include "shmem_alloc.h"
#include "esecam_master.h"

void esecam_cb (const char* szCameraName,
				int eventID,
				int eventDataSize,
				void* pEventData,
                unsigned char* pFrame,
				void*  pUserData)
{
	printf("Capture! Event = %d, size = %d\n", eventID, eventDataSize);
	if (eventID == NEW_FRAME || eventID == NEW_FRAME_SYNCHRO_ERROR)
	{
		// USB_StopVideoStream(szCameraName);
		if (pFrame) {
			//std::ofstream ofs("frame.bin", std::ofstream::out | std::ofstream::binary | std::ofstream::app);
			RET_SERV_DATA_HEADER rRetHeader = *((RET_SERV_DATA_HEADER *)(pFrame));
			printf("header size = %d, type = %d\n", rRetHeader.ulNextDataSize, rRetHeader.ulServDataType);
			//ofs.write((const char *)pFrame, rRetHeader.ulNextDataSize);
			//ofs.rdbuf()->pubsync();
    		//ofs.close();
			if (rRetHeader.ulServDataType == 3) {
				RET_SERV_DATA_ITK4_0 rRetSrv2 = *((RET_SERV_DATA_ITK4_0 *)(pFrame));
				unsigned int width = rRetSrv2.StreamServData.usWidth;
				unsigned int height = rRetSrv2.StreamServData.usHeight;
				printf("Width = %d, Height = %d, bpp =%d\n", width, height, rRetSrv2.StreamServData.chBitPerPixel);
				for (unsigned i = 0; i < 100; i++) {
					printf("%x ", *((pFrame) + i + sizeof(RET_SERV_DATA_ITK4_0)));
				}
				printf("\n");
				memcpy(pUserData, pFrame+sizeof(RET_SERV_DATA_ITK4_0), 1936*1216*rRetSrv2.StreamServData.chBitPerPixel/8);
				printf("time = %ld\n",  std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count());

			} else printf("Serv data type is not 3!\n");
		} else printf("Error: wrong pFrame ptr\n");
	}
}

int main(int argc, char ** argv)
{
	UdsUniComm uuc(1);
	UdsUniPack up;
	// up.fetch_data();
	ShmemBlockAllocator sba(1936*1216*8/8, 1, FRAME_SHBUF_NAME);
	ShmemBlock sblk;
	if (sba.setup() != 0) {printf("Shmem setup failed\n"); return 1;}
	if (sba.block_alloc(sblk) != 0) {printf("Shmem block failed\n"); return 1;}
	sba.show(100);

	std::string cam_name;
	printf("list result = %d\n", esecam_list());
	std::cin.get();

	cam_name = esecam_getname();

	esecam_print_caps();

	esecam_print_features();

	USB_SetFormat(cam_name.c_str(), 0);

	std::cin.get();
	// char frame[1936*1216];
	VIDEO_STREAM_PARAM_EX oStreamParam = {};
	oStreamParam.ppReturnedParams = nullptr;
	oStreamParam.pBuffBitmap = nullptr; // sblk.ptr;
	oStreamParam.pDirectBuffer = nullptr;
	oStreamParam.pEvent = nullptr;
	printf("start result = %d\n", USB_VideoOnCallbackStart(cam_name.c_str(), &oStreamParam, esecam_cb, sblk.ptr));

	//char s[5] = {'a','b','c','d','e'};
	//memcpy((void*)(sblk.ptr), s, 5);

	std::cin.get();
	USB_StopVideoStream(cam_name.c_str());
	sba.show(100);

	std::string filename = "frame";
	if (argc > 1) {
		filename += std::string(argv[1]);
	}
	filename += ".bin";
	std::ofstream ofs(filename.c_str(), std::ofstream::out | std::ofstream::binary);
	ofs.write((char*)(sblk.ptr), sblk.size);
	ofs.close();
	// for (unsigned i = 0; i < 100; i++) {
	// 	printf("%x ", *((char*)(oStreamParam.pBuffBitmap) + i));
	// }
	printf("\n");
	std::cin.get();
	printf("finished\n");

	return 0;
}
 