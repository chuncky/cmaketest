#ifndef __CAM_OFFLINE_H__
#define __CAM_OFFLINE_H__

extern void txchk_cur_task_stack_print();

/*be corresponding with PIPE_OUT_FMT*/
typedef enum SCALER_INPUT_FMT
{
	SCALER_INPUT_FMT_YUV422_YUYV = 0,/*1 planar*/
	SCALER_INPUT_FMT_YUV422_YVYU,
	SCALER_INPUT_FMT_YUV422_UYVY,
	SCALER_INPUT_FMT_YUV422_VYUY,
	SCALER_INPUT_FMT_YUV420_I420,/*3 planar  Y, U, V */
	SCALER_INPUT_FMT_YUV420_YV12,/*3 planar  Y, V, U */
	SCALER_INPUT_FMT_YUV420_NV12,/*nv12  YYYYYYYY UVUV */
	SCALER_INPUT_FMT_YUV420_NV21, /*NV21  YYYYYYYY VUVU */
	SCALER_INPUT_FMT_YUV422_3PLANAR,/*not support in offline*/
	SCALER_INPUT_FMT_ARGB32,
	SCALER_INPUT_FMT_MAX
}SCALER_INPUT_FMT;

enum offline_rot{
	ROT_0_DEG = 0,
	ROT_90_DEG,
	ROT_180_DEG,
	ROT_270_DEG,
	ROT_MIR,
	ROT_FLIP,
	ROT_MAX,
};

#define VIDEO_MAX_PLANES 3
struct v4l2_plane {
	uint32_t addr;
	uint32_t sizeimage;
	uint32_t pitch;
	uint32_t bytesperline;
	uint32_t usedperline;
};

struct cam_offline_buf {
	uint32_t	num_planes;	//filled by user
	uint32_t	width;		//filled by user
	uint32_t	height;		//filled by user
	uint32_t	stride;		//filled by user
	uint32_t	bpp;
	SCALER_INPUT_FMT	pixelformat; //filled by user
	unsigned int	plane_addr[VIDEO_MAX_PLANES]; // filled by user
	struct v4l2_plane plane[VIDEO_MAX_PLANES];
};

int cam_offline_init(void);

int cam_offline_deinit(void);

int cam_offline_rotation(struct cam_offline_buf* buf_src, struct cam_offline_buf* buf_dst, 	enum offline_rot rot, int module_id);

int cam_offline_scaler_zoom(struct cam_offline_buf* buf_src, struct cam_offline_buf* buf_dst, int zoom_level, int module_id);

int cam_offline_homo_zoom(struct cam_offline_buf* buf_src, struct cam_offline_buf* buf_dst,int zoom_level, int module_id);

int cam_hw_version_init(void);

#endif
