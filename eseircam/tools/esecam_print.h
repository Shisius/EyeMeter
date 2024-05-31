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

inline void esecam_print(FEATURE_ELEMENTS_REG reg, std::string name)
{
	printf("%s MaxValue: %d\n", name.c_str(), reg.MaxValue);
	printf("%s MinValue: %d\n", name.c_str(), reg.MinValue);
	printf("%s ManualMode: %d\n", name.c_str(), reg.ManualMode);
	printf("%s AutoMode: %d\n", name.c_str(), reg.AutoMode);
}

inline void esecam_print(SHUTTER_INQ_REG reg, std::string name)
{
	printf("%s MaxValue: %d\n", name.c_str(), reg.MaxValue);
	printf("%s MinValue: %d\n", name.c_str(), reg.MinValue);
	printf("%s ManualMode: %d\n", name.c_str(), reg.ManualMode);
	printf("%s AutoMode: %d\n", name.c_str(), reg.AutoMode);
}

inline void esecam_print(IP_FORMATS_LIST iplist, unsigned int imode)
{
	printf("Mode % d: Name %s\n", imode, iplist.FormatName);
	printf("Mode % d: bitCountCamera %d\n", imode, iplist.bitCountCamera);
	printf("Mode % d: bitCountBitMap %d\n", imode, iplist.bitCountBitMap);
	printf("Mode % d: bitCountADC %d\n", imode, iplist.bitCountADC);
	printf("Mode % d: bPartial %d\n", imode, iplist.bPartial);

	printf("Mode % d: MaxWidth %d\n", imode, iplist.MaxWidth);
	printf("Mode % d: MaxHeight %d\n", imode, iplist.MaxHeight);
	printf("Mode % d: MinWidth %d\n", imode, iplist.MinWidth);
	printf("Mode % d: MinHeight %d\n", imode, iplist.MinHeight);
	printf("Mode % d: WidthFrame %d\n", imode, iplist.WidthFrame);
	printf("Mode % d: HeightFrame %d\n", imode, iplist.HeightFrame);
}

inline void esecam_print(FEATURE_HI_REG reg)
{
	printf("Alailable Trigger %d\n", reg.Trigger);
	printf("Alailable Temperature %d\n", reg.Temperature);
	printf("Alailable Focus %d\n", reg.Focus);
	printf("Alailable Iris %d\n", reg.Iris);
	printf("Alailable Gain %d\n", reg.Gain);
	printf("Alailable Shutter %d\n", reg.Shutter);
	printf("Alailable Gamma %d\n", reg.Gamma);
	printf("Alailable Saturation %d\n", reg.Saturation);
	printf("Alailable FrameExposure %d\n", reg.FrameExposure);
	printf("Alailable WhiteBalance %d\n", reg.WhiteBalance);
	printf("Alailable Sharpness %d\n", reg.Sharpness);
	printf("Alailable Contrast %d\n", reg.Contrast);
	printf("Alailable Brightness %d\n", reg.Brightness);
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
				NumFmt: %d, CurFmt: %d, BitPerPixel: %d\n", caps->CameraSerial, caps->CameraSWVersion, caps->CameraFWVersion, 
				*(eseusbcam_unsigned_long_t*)(&(caps->BasicFunction)), *(eseusbcam_unsigned_long_t*)(&caps->FeatureHi),
				*(eseusbcam_unsigned_long_t*)(&caps->FeatureLo), caps->CameraFormats.numFormats, caps->CameraFormats.CurrFormat, 
				caps->CameraFormats.formatsList[caps->CameraFormats.CurrFormat].bitCountBitMap);
		esecam_print(caps->FeatureHi);
		esecam_print(caps->Brightness, "Brightness");
		// esecam_print(caps->Contrast, "Contrast");
		esecam_print(caps->Shutter, "Shutter");

		for (unsigned imode = 0; imode < caps->CameraFormats.numFormats; imode++)
		{
			esecam_print(caps->CameraFormats.formatsList[imode], imode);
		}
	}

	if (result != 1) printf("USB_GetCameraCapabilites failed with %d\n", result);
	return result;
}

inline int esecam_print_features()
{
	std::string cam_name = esecam_getname();
	eseusbcam_unsigned_long_t feature;
	if (USB_GetCameraFeature(cam_name.c_str(), BRIGHTNESS, &feature) == 1) printf("BRIGHTNESS = %d\n", feature);
	if (USB_GetCameraFeature(cam_name.c_str(), CONTRAST, &feature) == 1) printf("CONTRAST = %d\n", feature);
	if (USB_GetCameraFeature(cam_name.c_str(), SHARPNESS, &feature) == 1) printf("SHARPNESS = %d\n", feature);
	if (USB_GetCameraFeature(cam_name.c_str(), WHITE_BALANCE, &feature) == 1) printf("WHITE_BALANCE = %d\n", feature);
	if (USB_GetCameraFeature(cam_name.c_str(), WB_RED, &feature) == 1) printf("WB_RED = %d\n", feature);
	if (USB_GetCameraFeature(cam_name.c_str(), WB_GREEN, &feature) == 1) printf("WB_GREEN = %d\n", feature);
	if (USB_GetCameraFeature(cam_name.c_str(), WB_BLUE, &feature) == 1) printf("WB_BLUE = %d\n", feature);
	if (USB_GetCameraFeature(cam_name.c_str(), LUT_TABLE, &feature) == 1) printf("LUT_TABLE = %d\n", feature);
	if (USB_GetCameraFeature(cam_name.c_str(), DST, &feature) == 1) printf("DST = %d\n", feature);
	if (USB_GetCameraFeature(cam_name.c_str(), ET, &feature) == 1) printf("ET = %d\n", feature);

	if (USB_GetCameraFeature(cam_name.c_str(), FRAMES_EXPOSURE, &feature) == 1) printf("FRAMES_EXPOSURE = %d\n", feature);
	if (USB_GetCameraFeature(cam_name.c_str(), SATURATION, &feature) == 1) printf("SATURATION = %d\n", feature);
	if (USB_GetCameraFeature(cam_name.c_str(), SHUTTER, &feature) == 1) printf("SHUTTER = %d\n", feature);
	if (USB_GetCameraFeature(cam_name.c_str(), GAIN, &feature) == 1) printf("GAIN = %d\n", feature);
	if (USB_GetCameraFeature(cam_name.c_str(), IRIS, &feature) == 1) printf("IRIS = %d\n", feature);
	if (USB_GetCameraFeature(cam_name.c_str(), FOCUS, &feature) == 1) printf("FOCUS = %d\n", feature);
	if (USB_GetCameraFeature(cam_name.c_str(), TEMPERATURE, &feature) == 1) printf("TEMPERATURE = %d\n", feature);
	if (USB_GetCameraFeature(cam_name.c_str(), TRIGGER_MODE, &feature) == 1) printf("TRIGGER_MODE = %d\n", feature);
	if (USB_GetCameraFeature(cam_name.c_str(), ZOOM, &feature) == 1) printf("ZOOM = %d\n", feature);
	if (USB_GetCameraFeature(cam_name.c_str(), PAN, &feature) == 1) printf("PAN = %d\n", feature);
	if (USB_GetCameraFeature(cam_name.c_str(), TILT, &feature) == 1) printf("TILT = %d\n", feature);
	if (USB_GetCameraFeature(cam_name.c_str(), OPTICAL_FILTER, &feature) == 1) printf("OPTICAL_FILTER = %d\n", feature);
	return 0;
}

#endif // _ESECAM_PRINT_H_
