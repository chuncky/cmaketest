#ifndef _CAM_HAL_DRV_INTERFACE_H_
#define _CAM_HAL_DRV_INTERFACE_H_

//#include "pmic_rtc.h"
//#include "hal_timers.h"
#include "plat_types.h"

//we can have two buffers with using hard timer to config addr info.
#define ISP_USE_HARDWARE_TIMER 1
#define ISP_HARDWARE_TIMER_DELAY 4	//timer callback is called by task, so it's could be delay, unit is ms.

/////////hal interface/////////////////////////////
#define ISP_QBUF_MAX_NUM 3
#define ISP_STAT_MAX_PLANE 3
#define ISP_RESERVED_BUF_INDEX 0
typedef enum PIPELINE_ID
{
	PIPELINE_ID_PREV = 0,
	PIPELINE_ID_VIDEO,
	PIPELINE_ID_CAP,
	PIPELINE_ID_MAX,
}PIPELINE_ID;

typedef enum PIPE_OUT_FMT
{
	PIPE_OUT_FMT_YUV422_YUYV = 0,/*1 planar*/
	PIPE_OUT_FMT_YUV422_YVYU,
	PIPE_OUT_FMT_YUV422_UYVY,
	PIPE_OUT_FMT_YUV422_VYUY,
	PIPE_OUT_FMT_YUV420_I420,/*3 planar  Y, U, V */
	PIPE_OUT_FMT_YUV420_YV12,/*3 planar  Y, V, U */
	PIPE_OUT_FMT_YUV420_NV12,/*nv12  YYYYYYYY UVUV */
	PIPE_OUT_FMT_YUV420_NV21, /*NV21  YYYYYYYY VUVU */
	PIPE_OUT_FMT_YUV422_3PLANAR,/*not support in offline*/
	PIPE_OUT_FMT_JPEG,
}PIPE_OUT_FMT;

typedef enum SENSOR_ID
{
	SENSOR_ID_BACK = 0,
	SENSOR_ID_FRONT,
	SENSOR_ID_MAX,
}SENSOR_ID;

/* raw8 */
#define MEDIA_BUS_FMT_SBGGR8_1X8		0x3001
#define MEDIA_BUS_FMT_SGBRG8_1X8		0x3013
#define MEDIA_BUS_FMT_SGRBG8_1X8		0x3002
#define MEDIA_BUS_FMT_SRGGB8_1X8		0x3014
/* raw10 */
#define MEDIA_BUS_FMT_SBGGR10_1X10		0x3007
#define MEDIA_BUS_FMT_SGBRG10_1X10		0x300e
#define MEDIA_BUS_FMT_SGRBG10_1X10		0x300a
#define MEDIA_BUS_FMT_SRGGB10_1X10		0x300f
/* yuv422 */
#define MEDIA_BUS_FMT_UYVY8_2X8			0x2006
#define MEDIA_BUS_FMT_VYUY8_2X8			0x2007
#define MEDIA_BUS_FMT_YUYV8_2X8			0x2008
#define MEDIA_BUS_FMT_YVYU8_2X8			0x2009

typedef enum SENSOR_FMT
{
	SENSOR_FMT_RAW8 = 0,
	SENSOR_FMT_RAW10,
	SENSOR_FMT_RAW12,
	SENSOR_FMT_YUV422,
}SENSOR_FMT;

struct isp_buf_request {
	uint_8   msgQRef;//notify hal when eof rev
	uint_8 num_buf;
	uint_8  pipe_id;
};

struct isp_qbuf_plane {
	uint_32 addr;
	uint_32 offset;
	uint_32 length;
};

struct isp_qbuf_buffer {
	struct isp_qbuf_plane planes[ISP_STAT_MAX_PLANE];
	uint_32 num_planes;
};

typedef enum ISP_MSG_TYPE
{
	ISP_MSG_TYPE_EOF = 0,
	ISP_MSG_TYPE_SOF,
	ISP_MSG_TYPE_ERR_DROP_NO_BUF,
	/*JUST for hal to hal communication*/
	ISP_MSG_TYPE_HAL_FLUSH,
	ISP_MSG_TYPE_HAL_EQUEUE,
	ISP_MSG_TYPE_HAL_EXIT,
	ISP_MSG_TYPE_MAX,
}ISP_MSG_TYPE;

struct isp_buf_msg_data_eof{
	uint_8  pipe_id;
	uint_8  buf_idx;
};
struct isp_buf_msg_data_sof{
	uint_8  pipe_id;
	uint_8  buf_idx;
};
struct isp_buf_msg_data_err_drop_no_buf{
	uint_8  pipe_id;
	uint_32 timestamp;
	uint_32 frame_id;
};
struct isp_buf_msg {
	ISP_MSG_TYPE msg_type;
	union {
		struct isp_buf_msg_data_eof data_eof;
		struct isp_buf_msg_data_sof data_sof;
		struct isp_buf_msg_data_err_drop_no_buf data_drop_no_buf;
	} u;
};

/*for isp_buf_info dequeue normal eof buf*/
struct isp_buf_eof_metadata{
	uint_32 timestamp;
	uint_32 frame_id;
};

/*enque,deque parameter*/
struct isp_buf_info {
	uint_8  pipe_id;
	uint_8 buf_idx;
	/*Only used for enqueue buf*/
	struct isp_qbuf_buffer buffer;
	/*Only used for dequeue buf*/
	struct isp_buf_eof_metadata metadata;
};

struct isp_pipe_cfg {
	PIPELINE_ID pipe_id; 
	uint_8 pipe_enable;  //0x0:not enable   0x1:enable
	uint_8 data_range;  //0x0:no data range(yuv 0-255)   0x1:data range(Y¡ä range of [16..235] and U/V range of [16..240])
	uint_16 zoom;  //less 0x100:not use zoom, 0x400 zoom4x 
	PIPE_OUT_FMT output_format;
	uint_16 pipe_outw;
	uint_16 pipe_outh;
};

struct isp_sensor_cfg {
	SENSOR_ID sensor_id;
	uint_16 sensor_outw;
	uint_16 sensor_outh;
	SENSOR_FMT sensor_fmt;
	uint_8  skip_frame_n; //number of sensor skip frame
};

struct isp_sensor_info {
	SENSOR_ID sensor_id;/*hal set to drv*/
	int sensor_res_idx;
	uint_16 sensor_outw;
	uint_16 sensor_outh;
	SENSOR_FMT sensor_fmt;
	uint16_t mbus_code;
	uint_8 min_fps;
	uint_8 max_fps;
};

struct isp_output_size{
	uint_16 width;
	uint_16 height;
};

typedef enum
{
	CAMDRV_PARAM_NONE=0,
	CAMDRV_PARAM_ZOOM_FACTOR,
	CAMDRV_PARAM_CONTRAST,
	CAMDRV_PARAM_BRIGHTNESS,
	CAMDRV_PARAM_WB,
	CAMDRV_PARAM_EXPOSURE,
	CAMDRV_PARAM_EFFECT,
	CAMDRV_PARAM_BANDING,
	CAMDRV_PARAM_SATURATION,
	CAMDRV_PARAM_NIGHT_MODE,
	CAMDRV_PARAM_EV_VALUE,
	CAMDRV_PARAM_FLASH,
	CAMDRV_PARAM_FLASH_MODE = CAMDRV_PARAM_FLASH,
	CAMDRV_PARAM_AE_METERING,
	CAMDRV_PARAM_AF_KEY,
	CAMDRV_PARAM_AF_METERING,
	CAMDRV_PARAM_AF_MODE,
	CAMDRV_PARAM_MANUAL_FOCUS_DIR,
	CAMDRV_PARAM_ISO,
	CAMDRV_PARAM_SCENE_MODE,
	CAMDRV_PARAM_QUALITY,
	CAMDRV_PARAM_IMAGESIZE,

	CAMDRV_NO_OF_PARAM
}CamDrvParaMode;

typedef enum{
	BANDING_50HZ	= 0,
	BANDING_60HZ	= 1,
	BANDING_AUTO	= 2,
}CAM_BANDING;

int camdrv_isp_power_on(void);
int camdrv_isp_power_off(void);

int camdrv_init_bufq(struct isp_buf_request *buf_request);
int camdrv_buf_enqueue(struct isp_buf_info *info);
int camdrv_buf_dequeue(struct isp_buf_info *info);
int camdrv_buf_flush(uint_8 pipe_id_mask);


int camdrv_pipe_cfg(struct isp_pipe_cfg pipe_cfg);
/*
	pipe_id_mask=BIT(0)(0x1),means opt preview port.
	pipe_id_mask=BIT(1)(0x2),means opt video port.
	pipe_id_mask=BIT(2)(0x4),means opt cap port.
	pipe_id_mask=BIT(0)+BIT(1),means opt prev+video at same time.
*/
int camdrv_pipe_streamon(uint_8 pipe_id_mask);
int camdrv_pipe_streamoff(uint_8 pipe_id_mask, uint_32 keep_sensor);
int camdrv_pipe_streamon_capture_off_preview(void);
int camdrv_pipe_streamon_online_capture(struct isp_qbuf_buffer *buffer);
int camdrv_pipe_streamoff_online_capture(void);


/*for sensor power/clk on*/
int camdrv_sensor_power_on(SENSOR_ID sensor_id);
/*note:isp just connect one sensor.call this function,means:sensor_cfg->sensor_id connect to isp for hal*/
int camdrv_sensor_cfg(struct isp_sensor_cfg *sensor_cfg);
/*for sensor power/clk off*/
int camdrv_sensor_power_off(SENSOR_ID sensor_id);

/*call again and agian(sensor_info->index++),until this function return -1*/
int camdrv_sensorinfo_enum(struct isp_sensor_info *sensor_info);

int camdrv_set_param(CamDrvParaMode param_mode, void *pValue, uint_8 pipeIdMask);
void camdrv_raw_dump(uint_t raw_on);
int camdrv_sensor_detect(void);
int camdrv_camera_reg_reset(void);
int camdrv_get_isp_cfg(struct isp_output_size *info);
int camdrv_set_ata(uint_32 ata_on);
int camdrv_set_fps(uint_32 min_fps ,uint_32 max_fps, CAM_BANDING banding);
void camdrv_get_exp_iso(uint_32 *reciprocal_exp, uint_32 *iso);

/////////hal interface end/////////////////////////////
#endif
