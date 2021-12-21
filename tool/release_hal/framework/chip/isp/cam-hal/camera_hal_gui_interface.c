
#include "camera_hal_gui_interface.h"

//int Crane_CamHalDevOpen(int sensor_id)
UINT32 MCI_CamPowerUp (INT32  camera_id, void (*cb)(int32))
{
	int error = CRANE_CAMERA_ERROR_NONE;

	error = CCOpenCamera(camera_id);
	return error;
}

//int Crane_CamHalDevClose(int sensor_id)
UINT32  MCI_CamPowerDown (VOID)
{
	int error = CRANE_CAMERA_ERROR_NONE;

	error = CCCloseCamera();
	return error;
}

//int Crane_CamHalStartPreview(CAM_PREVIEW_STRUCT *preview_data)
UINT32  MCI_CamPreviewOpen(void *preview_data)
{
    int error = CRANE_CAMERA_ERROR_NONE;
	CCStartPreviewStruct previewStruct;

	if (!preview_data) {
		CAMLOGE("start preview with invalid parameter!");
		return CRANE_CAMERA_ERROR_BADARGUMENT;
	}

	memset(&previewStruct, 0, sizeof(CCStartPreviewStruct));
	previewStruct.image_width = ((CAM_PREVIEW_STRUCT *)preview_data)->image_width;
	previewStruct.image_height = ((CAM_PREVIEW_STRUCT *)preview_data)->image_height;
	previewStruct.factor = ((CAM_PREVIEW_STRUCT *)preview_data)->factor;
    error = CCStartPreview(&previewStruct);
    return error;
}

UINT32  MCI_CamSetATA(int camera_id, uint_32 ata_on, INT32 file_handle, void *capture_data)
{
	int error = CRANE_CAMERA_ERROR_NONE;
	CCCaputreStruct captureStruct;

	if (!capture_data) {
		CAMLOGE("start capture with invalid parameter!");
		return CRANE_CAMERA_ERROR_BADARGUMENT;
	}

	memset(&captureStruct, 0, sizeof(CCCaputreStruct));
	captureStruct.image_width = ((CAM_CAPTURE_STRUCT *)capture_data)->image_width;
	captureStruct.image_height = ((CAM_CAPTURE_STRUCT *)capture_data)->image_height;
	captureStruct.media_mode = ((CAM_CAPTURE_STRUCT *)capture_data)->media_mode;

	error = CCATATakePicture(camera_id,ata_on, file_handle, &captureStruct);
	return error;
}

//int Crane_CamHalStopPreview()
UINT32  MCI_CamPreviewClose (VOID)
{
    int error = CRANE_CAMERA_ERROR_NONE;

    error = CCStopPreview();
    return error;
}

UINT32  MCI_CamPreviewPause (VOID)
{
    int error = CRANE_CAMERA_ERROR_NONE;

    error = CCPausePreview();
    return error;
}

UINT32  MCI_CamPreviewResume (VOID)
{
    int error = CRANE_CAMERA_ERROR_NONE;

    error = CCResumePreview();
    return error;
}

UINT32  MCI_CamPreviewSetAlpha (UINT32 value)
{
    int error = CRANE_CAMERA_ERROR_NONE;

    error = CCSetAlphaPreview(value);
    return error;
}


//int Crane_CamHalCapture(CAM_CAPTURE_STRUCT *capture_data)
UINT32  MCI_CamCapture (void *capture_data)
{
    int error = CRANE_CAMERA_ERROR_NONE;
	CCCaputreStruct captureStruct;

	if (!capture_data) {
		CAMLOGE("start capture with invalid parameter!");
		return CRANE_CAMERA_ERROR_BADARGUMENT;
	}

	memset(&captureStruct, 0, sizeof(CCCaputreStruct));
	captureStruct.image_width = ((CAM_CAPTURE_STRUCT *)capture_data)->image_width;
	captureStruct.image_height = ((CAM_CAPTURE_STRUCT *)capture_data)->image_height;
	captureStruct.media_mode = ((CAM_CAPTURE_STRUCT *)capture_data)->media_mode;
    error = CCTakePicture(&captureStruct);
    return error;
}

//int Crane_CamHalSavePhoto(int file_handle)
INT32  MCI_CamSavePhoto (INT32 file_handle)
{
    int error = CRANE_CAMERA_ERROR_NONE;

    error = CCSavePicture(file_handle);
    return error;
}

int MCI_VideoCaptureStart(CamRecordParmeters *pVideoRecordParam)
{
    int error = CRANE_CAMERA_ERROR_NONE;

    error = CCStartVideoCall(pVideoRecordParam);
    return error;
}

int MCI_VideoCaptureStop(void)
{
    int error = CRANE_CAMERA_ERROR_NONE;

    error = CCStopVideoCall();
    return error;
}

//int Crane_CamHalStartVideoRecord(uint_32 width, uint_32 height, uint_32 format)
int Crane_CamHalStartVideoRecord(CamRecordParmeters *pVideoRecordParam, uint_32 format)
{
    int error = CRANE_CAMERA_ERROR_NONE;

    CCStartRecord(pVideoRecordParam, format);
    return error;
}

int Crane_CamHalPauseVideoRecord(void)
{
	int error = CRANE_CAMERA_ERROR_NONE;

	error = CCPauseRecord();
	return error;
}

int Crane_CamHalResumeVideoRecord(void)
{
	int error = CRANE_CAMERA_ERROR_NONE;

	error = CCResumeRecord();
	return error;
}

int Crane_CamHalStopVideoRecord(void)
{
    int error = CRANE_CAMERA_ERROR_NONE;

    CCStopRecord();
    return error;
}

#ifdef CAM_FULLSIZE_STREAM
int Crane_CamHalStartFullSizeStream(CamRecordParmeters *pVideoRecordParam, uint_32 format)
{
	int error = CRANE_CAMERA_ERROR_NONE;

	CCStartFullSizeStream(pVideoRecordParam, format);
	return error;
}

int Crane_CamHalPauseFullSizeStream(void)
{
	int error = CRANE_CAMERA_ERROR_NONE;

	error = CCPauseFullSizeStream();
	return error;
}

int Crane_CamHalResumeFullSizeStream(void)
{
	int error = CRANE_CAMERA_ERROR_NONE;

	error = CCResumeFullSizeStream();
	return error;
}

int Crane_CamHalStopFullSizeStream(void)
{
	int error = CRANE_CAMERA_ERROR_NONE;

	CCStopFullSizeStream();
	return error;
}
#endif

int _CHG_ConvertGuiParaToDrvPara(int32 effectCode, CamDrvParaMode *pDrvParaMode, int32 value, int32 *pDrvVaule)
{
	int error = CRANE_CAMERA_ERROR_NONE;

	if (!pDrvParaMode)
		return CRANE_CAMERA_ERROR_BADARGUMENT;

	CAMLOGD("set %d paramter, value=%d!", effectCode, value);
	switch (effectCode) {
		case CAM_PARAM_ZOOM_FACTOR:
			*pDrvParaMode = CAMDRV_PARAM_ZOOM_FACTOR;
			if (value == -1)
				*pDrvVaule = 0x100;
			else if (value < 16) {
				*pDrvVaule = (value * (0x200 - 0x100) / 15) + 0x100;
			} else
				error = CRANE_CAMERA_ERROR_BADARGUMENT;
			CAMLOGD("set zoom, value=%d, pDrvValue=0x%x!", value, *pDrvVaule);
			break;
		case CAM_PARAM_CONTRAST:
			*pDrvParaMode = CAMDRV_PARAM_CONTRAST;
			if (value == 0){
				*pDrvVaule = HAL_CONTRAST_n3;
			} else if (value == 1){
				*pDrvVaule = HAL_CONTRAST_n2;
			} else if (value == 2){
				*pDrvVaule = HAL_CONTRAST_n1;
			} else if (value == 3){
				*pDrvVaule = HAL_CONTRAST_mid;
			} else if (value == 4){
				*pDrvVaule = HAL_CONTRAST_p1;
			} else if (value == 5){
				*pDrvVaule = HAL_CONTRAST_p2;
			} else if (value == 6){
				*pDrvVaule = HAL_CONTRAST_p3;
			} else {
				*pDrvVaule = HAL_CONTRAST_mid;
				error = CRANE_CAMERA_ERROR_BADARGUMENT;
			}
			CAMLOGD("set contrast, value=%d, pDrvValue=0x%x!", value, *pDrvVaule);
			break;
		case CAM_PARAM_BRIGHTNESS:
			*pDrvParaMode = CAMDRV_PARAM_BRIGHTNESS;
			if (value == 1){
				*pDrvVaule = HAL_BRIGHTNESS_1;
			} else if (value == 2){
				*pDrvVaule = HAL_BRIGHTNESS_2;
			} else if (value == 3){
				*pDrvVaule = HAL_BRIGHTNESS_3;
			} else if (value == 4){
				*pDrvVaule = HAL_BRIGHTNESS_4;
			} else if (value == 5){
				*pDrvVaule = HAL_BRIGHTNESS_mid;
			} else if (value == 6){
				*pDrvVaule = HAL_BRIGHTNESS_6;
			} else if (value == 7){
				*pDrvVaule = HAL_BRIGHTNESS_7;
			} else if (value == 8){
				*pDrvVaule = HAL_BRIGHTNESS_8;
			} else if (value == 9){
				*pDrvVaule = HAL_BRIGHTNESS_9;
			} else {
				*pDrvVaule = HAL_BRIGHTNESS_mid;
				error = CRANE_CAMERA_ERROR_BADARGUMENT;
			}
			CAMLOGD("set brightness, value=%d, pDrvValue=0x%x!", value, *pDrvVaule);
			break;
		case CAM_PARAM_SATURATION:
			*pDrvParaMode = CAMDRV_PARAM_SATURATION;
			if (value == 0){
				*pDrvVaule = HAL_SATURATION_n3;
			} else if (value == 1){
				*pDrvVaule = HAL_SATURATION_n2;
			} else if (value == 2){
				*pDrvVaule = HAL_SATURATION_n1;
			} else if (value == 3){
				*pDrvVaule = HAL_SATURATION_mid;
			} else if (value == 4){
				*pDrvVaule = HAL_SATURATION_p1;
			} else if (value == 5){
				*pDrvVaule = HAL_SATURATION_p2;
			} else if (value == 6){
				*pDrvVaule = HAL_SATURATION_p3;
			} else {
				*pDrvVaule = HAL_SATURATION_mid;
				error = CRANE_CAMERA_ERROR_BADARGUMENT;
			}
			CAMLOGD("set saturation, value=%d, pDrvValue=0x%x!", value, *pDrvVaule);
			break;
		case CAM_PARAM_WB:
			*pDrvParaMode = CAMDRV_PARAM_WB;
			if (value == 0){
				*pDrvVaule = HAL_WB_AUTO;
			} else if (value == 1){
				*pDrvVaule = HAL_WB_DAYLIGHT;
			} else if (value == 2){
				*pDrvVaule = HAL_WB_TUNGSTEN;
			} else if (value == 3){
				*pDrvVaule = HAL_WB_OFFICE;
			} else if (value == 4){
				*pDrvVaule = HAL_WB_CLOUDY;
			} else if (value == 5){
				*pDrvVaule = HAL_WB_INCANDESCENT;
			} else {
				*pDrvVaule = HAL_WB_AUTO;
				error = CRANE_CAMERA_ERROR_BADARGUMENT;
			}
			CAMLOGD("set wb, value=%d, pDrvValue=0x%x!", value, *pDrvVaule);
			break;
		case CAM_PARAM_EFFECT:
			*pDrvParaMode = CAMDRV_PARAM_EFFECT;
			if (value == 0){
				*pDrvVaule = HAL_EFFECT_NORMAL;
			} else if (value == 5){
				*pDrvVaule = HAL_EFFECT_COLORINV;
			} else if (value == 7){
				*pDrvVaule = HAL_EFFECT_BLACKBOARD;
			} else if (value == 8){
				*pDrvVaule = HAL_EFFECT_WHITEBOARD;
			} else if (value == 17){
				*pDrvVaule = HAL_EFFECT_ANTIQUE;
			} else if (value == 18){
				*pDrvVaule = HAL_EFFECT_RED;
			} else if (value == 19){
				*pDrvVaule = HAL_EFFECT_GREEN;
			} else if (value == 20){
				*pDrvVaule = HAL_EFFECT_BLUE;
			} else if (value == 21 || value == 1){
				*pDrvVaule = HAL_EFFECT_BLACKWHITE;
			} else if (value == 22){
				*pDrvVaule = HAL_EFFECT_NEGATIVE;
			} else {
				*pDrvVaule = HAL_EFFECT_NORMAL;
				error = CRANE_CAMERA_ERROR_BADARGUMENT;
			}
			CAMLOGD("set effect, value=%d, pDrvValue=0x%x!", value, *pDrvVaule);
			break;
		case CAM_PARAM_NIGHT_MODE:
			*pDrvParaMode = CAMDRV_PARAM_NIGHT_MODE;
			if (value == 0){
				*pDrvVaule = HAL_SCENE_AUTO;
			} else if (value == 1){
				*pDrvVaule = NIGHT_FPS_5;  // night mode fps [1-10]
			} else {
				*pDrvVaule = HAL_SCENE_AUTO;
			}
			CAMLOGD("set night_mode, value=%d, pDrvValue=0x%x!", value, *pDrvVaule);
			break;
		case CAM_PARAM_FLASH_MODE:
			*pDrvParaMode = CAMDRV_PARAM_FLASH_MODE;
			if (value == 0){
				*pDrvVaule = HAL_FLASH_AUTO;
			} else if (value == 1){
				*pDrvVaule = HAL_FLASH_OFF;
			} else {
				*pDrvVaule = HAL_FLASH_LEVEL_5; // flash level [1-10]
			}
			CAMLOGD("set flash_mode, level=%d, pDrvValue=0x%x!", value, *pDrvVaule);
			break;
		case CAM_PARAM_BANDING:
			*pDrvParaMode = CAMDRV_PARAM_BANDING;
			if (value == 0){
				*pDrvVaule = HAL_BANDING_50HZ;
			} else if (value == 1){
				*pDrvVaule = HAL_BANDING_60HZ;
			} else {
				*pDrvVaule = HAL_BANDING_AUTO;
				error = CRANE_CAMERA_ERROR_BADARGUMENT;
			}
			CAMLOGD("set banding_mode, value=%d, pDrvValue=0x%x!", value, *pDrvVaule);
			break;
		case CAM_PARAM_QUALITY:
			*pDrvParaMode = CAMDRV_PARAM_QUALITY;
			if (value == 0){
				*pDrvVaule = HAL_QTY_LOW;
			} else if (value == 1){
				*pDrvVaule = HAL_QTY_NORMAL;
			} else if (value == 2){
				*pDrvVaule = HAL_QTY_FINE;
			} else if (value == 3){
				*pDrvVaule = HAL_QTY_SUPER_FINE;
			} else {
				*pDrvVaule = HAL_QTY_LOW;
				error = CRANE_CAMERA_ERROR_BADARGUMENT;
			}
			CAMLOGD("set quality, value=%d, pDrvValue=0x%x!", value, *pDrvVaule);
			break;				
		default:
			CAMLOGE("camera drvier can't support this %d effectCode!", effectCode);
			error = CRANE_CAMERA_ERROR_BADARGUMENT;
			break;
	
	}
	
	return error;
}

int MCI_CamSetPara(int32 effectCode, int32 value)
{
	int error = CRANE_CAMERA_ERROR_NONE;
	CamDrvParaMode camDrvParaMode = CAMDRV_PARAM_NONE;
	int32 drvValue = 0;

	error = _CHG_ConvertGuiParaToDrvPara(effectCode, &camDrvParaMode, value, &drvValue);
	if (CRANE_CAMERA_ERROR_NONE != error){
		CAMLOGE("_CHG_ConvertGuiParaToDrvPara error %d ", error);
		return error;
	}
	error = CCSetPara(camDrvParaMode, drvValue);

	return error;
}

