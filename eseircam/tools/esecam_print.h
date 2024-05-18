#ifndef _ESECAM_PRINT_H_
#define _ESECAM_PRINT_H_

#include "eseusbcam.h"
#include "iostream"
#include <string>

inline int esecam_list()
{
	DEVICE_DATA DeviceData;

	int result = USB_GetCameraList(&DeviceData);

	if ( result != 1 ) printf("Error: %s\n", USB_GetLastError());

	printf("Num devices = %d\n", DeviceData.numDevices);
	for (unsigned int i = 0; i < DeviceData.numDevices; i++) {
		printf("cam %d : %s\n", i, DeviceData.deviceList[i].DeviceName);
	}

	return result;
}
inline std::string esecam_getname()
{
	DEVICE_DATA DeviceData;

	int result = USB_GetCameraList(&DeviceData);

	if ( result != 1 || DeviceData.numDevices <= 0) return std::string("");

	return std::string(DeviceData.deviceList[0].DeviceName);
}
inline int esecam_print_caps()
{
	PUSB_CAMERA_CAPABILITES caps = NULL;
	std::string name = esecam_getname();

	int result = USB_GetCameraCapabilites(name.c_str(), &caps);

	if (caps) {
		printf("Capabilities: \n \
				CameraSerial: %d\n \
				CameraSWVersion: %d\n \
				CameraFWVersion: %d\n \
				BasicFunction: %d\n \
				FeatureHi: %d\n \
				FeatureLo: %d\n \
				NumFmt: %d, CurFmt: %d, BitPerPixel: %d\n \
				", caps->CameraSerial, caps->CameraSWVersion, caps->CameraFWVersion, 
				*(eseusbcam_unsigned_long_t*)(&(caps->BasicFunction)), *(eseusbcam_unsigned_long_t*)(&caps->FeatureHi),
				*(eseusbcam_unsigned_long_t*)(&caps->FeatureLo), caps->CameraFormats.numFormats, caps->CameraFormats.CurrFormat, 
				caps->CameraFormats.formatsList[caps->CameraFormats.CurrFormat].bitCountBitMap);
	}

	if (result != 1) printf("USB_GetCameraCapabilites failed with %d\n", result);
	return result;
}

#endif // _ESECAM_PRINT_H_
