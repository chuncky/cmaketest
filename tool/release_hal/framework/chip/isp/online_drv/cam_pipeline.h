#ifndef __CAM_PIPELINE_H__
#define __CAM_PIPELINE_H__

#include "cam_hal_drv_interface.h"
#include "../../../device/camera/cam_sensor.h"
#include "../camera_common_log.h"

#include "cam_list.h"
typedef enum PIPE_SMP_RATE
{
	PIPE_SMP_RATE_1_1 = 0,
	PIPE_SMP_RATE_1_2,
	PIPE_SMP_RATE_1_4,
	PIPE_SMP_RATE_1_8,
	PIPE_SMP_RATE_1_16,
	PIPE_SMP_RATE_1_32,
}PIPE_SMP_RATE;

typedef enum ISP_OUT_FMT
{
	ISP_OUT_FMT_ISPOUTYUV444 = 0,
	ISP_OUT_FMT_SPI_YUV_BYPASSISP,
}ISP_OUT_FMT;

struct crop_param {
    uint_16 start_x;
    uint_16 start_y;
    uint_16 end_x;
    uint_16 end_y;
};

struct isp_param {
    uint_8 isp_inited;
	uint_16 isp_input_width;
	uint_16 isp_input_height;
	uint_16 isp_output_width;
	uint_16 isp_output_height;
	struct crop_param isp_crop; //CZM: only spi sensor support isp crop
	uint_8 isp_aec;  //0x00:not use isp aec   0x01:use isp aec
	uint_8 isp_i2c_control;//0x00:camera control i2c    0x01:isp control i2c
	uint16_t snr_mbus_code;
};

enum isp_dma_state {
	ISPDMA_IDLE	= 0,
	ISPDMA_ACTIVE,
	ISPDMA_HW_NEXT_FRAME_NO_STREAM,
	ISPDMA_HW_NO_STREAM,
};

/*size:isp output size-->crop-->subsample-->scaler-->dma size*/
struct pipeline_param {
	uint_8 pipeline_id;  //0x0:preview   0x1:video    0x2:capture    
	uint_8 pipeline_enable;  //0x0:not enable   0x1:enable
	uint_8 shadow_mode;  //0x0:direct mode   0x1:shadow mode
	uint_8 data_range;  //0x0:no data range(yuv 0-255)   0x1:data range(Y¡ä range of [16..235] and U/V range of [16..240])
	//less 0x100:not use zoom    smooth_zoom =0x100  0x115  0x132  0x152  0x174  0x200  0x230  0x264  0x303  0x349  0x400      zoom = smooth_zoom /0x100
	uint_16 streamon_zoom;//for first stream on used
	uint_16 cur_zoom;//for smooth zoom
	uint_16 target_zoom;//for smooth zoom
	struct crop_param pipeline_crop;
	uint_8 subsample;  //0x0:1-1   0x1 1-2  0x2:1-4  0x3:1-8  0x4:1-16  0x5 1-32
	uint_8 jpeg_mode;  //0x0:not jpeg_mode   0x1:jpeg_mode
	uint_8 output_format;
	uint_16 dma_stride_y;
	uint_16 pipeline_outw;/*dma_w*/
	uint_16 pipeline_outh;/*dma_h*/
	enum isp_dma_state	dma_state;
	HANDLE pipe_stop_semaphore; //for stop down
};

typedef struct tuning_set_param{
	uint_32 tuning_set_value;
	uint_32 read_flag;
}TUNING_PARAM;

struct tuning_param{
	TUNING_PARAM brightness;
	TUNING_PARAM saturation;
	TUNING_PARAM contrast;
	TUNING_PARAM step;
};

struct cam_isp_pipeline {
	struct isp_param   isp_cfg;
	struct pipeline_param pipe0_cfg;  //preview
	struct pipeline_param pipe1_cfg;  //video
	struct pipeline_param pipe2_cfg;  //capture
	uint_32 frame_num;
	int timer_id;
	uint_32 online_isp_drop_frm_num;
	struct isp_buf_mgr * buf_mgr;
	u8 stream_cfg_lock;/*lock hal opt.*/
	SENSOR_CTRL_T sensor_ctrl;
	u8   camIrqMsgQRef; //for hisr deal with interrupt
	u8   camIrqErrorMsgQRef; //for thread deal with interrupt
	HANDLE camIrqTaskRef;
	uint_32 min_fps;
	uint_32 max_fps;
	uint_32 timeout_value;
	struct tuning_param default_param;
};

enum isp_stats_buf_status {
	ISP_STATS_BUFSTS_UNUSED,
	ISP_STATS_BUFSTS_INITED,    /*buf was inited*/
	ISP_STATS_BUFSTS_IDLE,      /*buf is in idle queue*/
	ISP_STATS_BUFSTS_PREPARED,  /*buf send to drv*/
	ISP_STATS_BUFSTS_HWUSED,    /*buf is using*/
	ISP_STATS_BUFSTS_UNPAPRED,  /*buf return to hal*/
	ISP_STATS_BUFSTS_SKIPED,    /*buf need skip*/
	ISP_STATS_BUFSTS_MAX
};

struct isp_qbuf_buffer_info {
	struct isp_qbuf_buffer buf;
	struct isp_buf_eof_metadata metadata;
	uint_8 buf_status; /*isp_stats_buf_status*/
	uint_8 buf_flag;
	struct list_head	hook;/*hook to queue idle_buf and busy_buf*/
	int_32 buf_idx; /*index in buf queue*/
	struct isp_buf_queue *parqueue;/*parent queue*/
};

struct isp_buf_queue {
	struct isp_qbuf_buffer_info bufinfo[ISP_QBUF_MAX_NUM];
	u8  queue_lock; /*for ioctl opt*/
	struct isp_buf_mgr * buf_mgr;
	int req_buf_num;
	struct list_head	idle_buf;
	struct list_head	busy_buf;
	uint_8			idle_buf_cnt;
	uint_8			busy_buf_cnt;
	int  	pipe_id;/*PIPELINE_ID*/
	uint_8   msgQRef;//notify hal when eof rev
};

struct isp_buf_mgr {
	struct isp_buf_queue bufque[PIPELINE_ID_MAX];
	u8 mgr_lock;
	struct cam_isp_pipeline * cam_isp;
};

struct cam_isp_pipeline *get_isp_pipe_mgr(void);
struct isp_buf_mgr * get_isp_buf_mgr(void);

int cam_init_bufq(struct isp_buf_mgr *buf_mgr,struct isp_buf_request *buf_request);
int cam_buf_enqueue(struct isp_buf_mgr *buf_mgr,struct isp_buf_info *info);
int cam_buf_dequeue(struct isp_buf_mgr *buf_mgr,struct isp_buf_info *info);
int cam_pipe_stream_on(uint_8 pipe_id_mask);
int cam_pipe_stream_off(uint_8 pipe_id_mask, uint_32 keep_sensor);
int cam_pipe_streamon_capture_off_preview(void);
int cam_pipe_prepare_online_preview_pipe(void);
int cam_pipe_prepare_online_capture_pipe(void);
int cam_pipe_streamon_online_capture(struct isp_qbuf_buffer *buffer);
int cam_pipe_streamoff_online_capture(void);
int cam_irq_init(void);
void cam_irq_deinit(void);
int cam_pipe_setzoom(uint_8 pipe_id_mask, uint_16 zoomValue);
void cam_phase_init(void);
void cam_phase_deinit(void);
int cam_buf_flush(struct isp_buf_mgr *buf_mgr ,uint_8 pipe_id_mask);
void cam_set_pipe_shadow_ready(uint_8 pipe_id);
void cam_raw_dump(uint_t raw_on);
void isp_set_fps(uint_32 min_fps ,uint_32 max_fps, CAM_BANDING banding);
void isp_get_exp_iso(uint_32 *reciprocal_exp, uint_32 *iso);

#endif
