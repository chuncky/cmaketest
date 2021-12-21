#ifndef _CAM_HAL_GUI_INTERFACE_H_
#define _CAM_HAL_GUI_INTERFACE_H_

#include "gui/cpu_share.h"
#include "gui/media/med_api.h"
#include "camera_hal_core.h"


//UINT32 MCI_CamPowerUp (INT32  vediomode, void (*cb)(int32));
//UINT32  MCI_CamPowerDown (VOID);
//UINT32  MCI_CamPreviewOpen(void *preview_data);
//UINT32  MCI_CamPreviewClose (VOID);
//UINT32  MCI_CamCapture (void *capture_data);
//INT32  MCI_CamSavePhoto (INT32 file_handle);

//int Crane_CamHalDevOpen(int sensor_id);
//int Crane_CamHalDevClose(int sensor_id);
//int Crane_CamHalStartPreview(CAM_PREVIEW_STRUCT *preview_data);
//int Crane_CamHalStopPreview();
//int Crane_CamHalCapture(CAM_CAPTURE_STRUCT *capture_data);
//int Crane_CamHalSavePhoto(int file_handle);
int Crane_CamHalSetParamter(void);
//int Crane_CamHalStartVideoRecord(uint_32 width, uint_32 height, uint_32 format);
#ifdef BUILD_CAMERA_HAL
int Crane_CamHalStartVideoRecord(CamRecordParmeters *pVideoRecordParam, uint_32 format);
int Crane_CamHalStopVideoRecord(void);
int Crane_CamSetPara(int32 effectCode, int32 value);
#else
#define Crane_CamHalStartVideoRecord(x,y) 1003
#define Crane_CamHalStopVideoRecord() 1003
#define Crane_CamSetPara(x,y) 1003
#endif

int Crane_CamHalPauseVideoRecord(void);
int Crane_CamHalResumeVideoRecord(void);

#endif

