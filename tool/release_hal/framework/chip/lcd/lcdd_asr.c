#include "lcd_predefine.h"
#include "panel_drv.h"
#include "lcd_drv.h"
#include "lcd_fns.h"
#include "ui_os_api.h"
#include "ui_mem_api.h"
#include "lowpower_api.h"
#include "task_cnf.h"
#include "backlight_drv.h"
#include "lcdd_asr.h"
#include "../../chip/isp/offline_drv/cam_offline.h"
#include "../../chip/clock/media-clk/media_clk.h"
#include "cache_api.h"
#include "mci_lcd.h"
#include "../../device/lcd/panel_list.h"
#include "lcdd_esd.h"

//#define ASRLCD_DOUBLE_FB

#define MAX_ROTBQ_NUM 2
#define MAX_LAYER_RECTS 10
#define MAX_IMG_FRAME 5

enum{
	LCD_STATUS_POWEROFF = 0 ,
	LCD_STATUS_HWON = 1,
	LCD_STATUS_POWERON = 2,
	LCD_STATUS_SUB_POWERON = 4,
	LCD_STATUS_SLEEP = 8,
	LCD_STATUS_SUB_SLEEP = 0x10,
	LCD_STATUS_ASSERTON = 0x20,
};

enum{
	LCD_MSG_TYPE_UPDATE,
	LCD_MSG_TYPE_STOP,
	LCD_MSG_TYPE_FILL,
	LCD_MSG_TYPE_SUB_FILL,
	LCD_MSG_TYPE_SYNC,
	LCD_MSG_TYPE_LIMIT
};

struct asrlcdd_refresh_layout{
	struct asrlcdd_roi_info osd_roi;
	struct asrlcdd_roi_info img_roi;
};

struct s_lcd_ROT_buffer{
	void* addr;
	int in_used;
};

struct s_osd_layer_setting{
	uint8_t dirty;
	uint8_t layer_en;
	uint8_t format;
	struct asrlcdd_framebuffer_info frame_buffer;
	struct asrlcdd_roi_info src_rect;
	struct asrlcdd_roi_info dest_rect;
};

struct s_img_layer_setting{
	uint8_t dirty;
	uint8_t layer_en;
	uint8_t format;
	unsigned int planes;
	unsigned int addr[3];
	unsigned int stride[3];
	struct asrlcdd_roi_info src_rect;
	struct asrlcdd_roi_info dest_rect;
};

struct s_wb_layer_setting{
	uint8_t layer_en;
	uint8_t format;
	unsigned int addr;
	unsigned int width;
	unsigned int height;
	unsigned int stride;
};

struct s_lcd_ROTBQ{
	struct s_lcd_ROT_buffer buffers[MAX_ROTBQ_NUM];
	int used_count;
	int free_count;
	int count;
};

int g_lcd_status = LCD_STATUS_POWEROFF;
struct s_lcd_ctx *g_plcd = NULL;
struct panel_spec *g_ppanel = NULL;
#ifdef LCD_DUAL_PANEL_SUPPORT
struct panel_spec *g_psubpanel = NULL;
#endif
ASRLCDD_CB_FUNC g_lcd_cbs[ASRLCDD_CB_TYPE_MAX] = {NULL};
void *g_lcd_cb_params[ASRLCDD_CB_TYPE_MAX] = {NULL};
u8 g_lcd_flag_id = INVALID_FLAG_ID;
u8 g_lcd_msgq_id = 0;
HANDLE g_lcd_task = NULL;
#ifdef GUI_LCD_MUTEX
volatile int g_gui_lcd_mutex_id = INVALID_MUTEX_ID;
#endif
int g_lcd_mutex_id = INVALID_MUTEX_ID;
int g_lcd_thread_exit = 1;
char *g_last_ROT_buffer = NULL;
struct s_lcd_ROTBQ g_lcd_ROTBQ = {NULL};
struct s_img_layer_setting g_lcd_img_setting = {0};
struct s_osd_layer_setting g_lcd_osd_setting = {0};
u8 g_lcd_timer_started = 0;
static int g_release_bq = 0;

#ifdef LCD_DUAL_PANEL_SUPPORT
struct s_img_layer_setting g_sublcd_img_setting = {0};
struct s_osd_layer_setting g_sublcd_osd_setting = {0};
#endif

//static u8 g_lcd_Blit16_timerid = INVALID_TIMER_ID;

volatile int g_flip_pending = 0;
#ifdef ASRLCD_DOUBLE_FB
unsigned char g_asrhw_framebuffer[240 * 320 * 2];
u8 g_lcd_hwfb_mutex_id = INVALID_MUTEX_ID;
#endif
int g_frame_count = 0;
int g_lcd_camerastatus = 0;/*0 -normal, 1-pause*/
int g_lcd_camerastatus_pending = 0;/*0 -not pending, 1-pending*/
u8	g_lcd_sync_flag = INVALID_FLAG_ID;
static int g_cam_init = 0;
unsigned int g_alpha_val = 0x80;
struct s_lcd_esd* g_esd_ctx = NULL;

int g_refreshed = 0;
int g_backlight_on = 0;
int g_backlight_level = 0x0;
#ifdef LCD_DUAL_PANEL_SUPPORT
int g_subrefreshed = 0;
int g_subbacklight_on = 0;
int g_subbacklight_level = 0x0;
#endif

struct asrlcdd_screen_info g_ui_info;
unsigned char* g_fstn_data = NULL;

#if (defined(LCD_ROTATION_90) || defined(LCD_ROTATION_270) || defined(LCD_ROTATION_180))
unsigned char g_landscape_buffer[240 * 320 * 4];
#endif

static int lcd_do_rotation(struct cam_offline_buf *src_buffer, struct cam_offline_buf *dst_buf,
	unsigned int rotation, char *dest_buffer);
static unsigned int switch_lcdformat_to_camformat(unsigned int lcdformat);


#if 0
static void dump_framewindow(struct asrlcdd_framebuffer_window *frame_window)
{
	LCDLOGD("Info: dump_framewindow ++\r\n");
	LCDLOGD("Info:  frame_info.pbuffer = 0x%x\r\n", frame_window->frame_info.pbuffer);
	LCDLOGD("Info:  frame_info.height = %d\r\n", frame_window->frame_info.height);
	LCDLOGD("Info:  frame_info.stride = %d\r\n", frame_window->frame_info.stride);
	LCDLOGD("Info:  frame_info.bitdepth = %d\r\n", frame_window->frame_info.bitdepth);
	LCDLOGD("Info:  roi_info.startX = %d\r\n", frame_window->roi_info.startX);
	LCDLOGD("Info:  roi_info.startY = %d\r\n", frame_window->roi_info.startY);
	LCDLOGD("Info:  roi_info.width = %d\r\n", frame_window->roi_info.width);
	LCDLOGD("Info:  roi_info.height = %d\r\n", frame_window->roi_info.height);
}
#endif

#if 0
static void dump_camerabuffer(struct asrlcdd_camera_buffer *pbuffer)
{
	LCDLOGD("Info: dump_camerabuffer ++\r\n");
	LCDLOGD("Info:  width = %d\r\n", pbuffer->width);
	LCDLOGD("Info:  height = %d\r\n", pbuffer->height);
	LCDLOGD("Info:	format = %d\r\n", pbuffer->format);
	LCDLOGD("Info:	planes = %d\r\n", pbuffer->planes);
	LCDLOGD("Info:	addr[0] = 0x%x\r\n", pbuffer->addr[0]);
	LCDLOGD("Info:	addr[1] = 0x%x\r\n", pbuffer->addr[1]);
	LCDLOGD("Info:	addr[2] = 0x%x\r\n", pbuffer->addr[2]);
	LCDLOGD("Info:  stride[0] = %d\r\n", pbuffer->stride[0]);
	LCDLOGD("Info:  stride[1] = %d\r\n", pbuffer->stride[1]);
	LCDLOGD("Info:  stride[2] = %d\r\n", pbuffer->stride[2]);
}
#endif

#if 1
static void dump_img_setting(struct s_img_layer_setting *setting)
{
	LCDLOGD("Info: dump_img_setting ++\r\n");
	LCDLOGD("Info:  dirty = %d\r\n",setting->dirty);
	LCDLOGD("Info:  layer_en = %d\r\n",setting->layer_en);
	LCDLOGD("Info:  format = %d\r\n",setting->format);
	LCDLOGD("Info:  planes = %d\r\n",setting->planes);
	LCDLOGD("Info:  addr = 0x%x, 0x%x, 0x%x\r\n",setting->addr[0],
		setting->addr[1], setting->addr[2]);
	LCDLOGD("Info:  stride = %d, %d, %d\r\n",setting->stride[0],
		setting->stride[1], setting->stride[2]);
	LCDLOGD("Info:  src_rect = (%d, %d, %d, %d)\r\n",setting->src_rect.startX,
		setting->src_rect.startY, setting->src_rect.width, setting->src_rect.height);
	LCDLOGD("Info:  dest_rect = (%d, %d, %d, %d)\r\n",setting->dest_rect.startX,
		setting->dest_rect.startY, setting->dest_rect.width, setting->dest_rect.height);
}
#endif

#if 1
static void dump_osd_setting(struct s_osd_layer_setting *setting)
{
	LCDLOGD("Info: dump_osd_setting ++\r\n");
	LCDLOGD("Info:  dirty = %d\r\n",setting->dirty);
	LCDLOGD("Info:  layer_en = %d\r\n",setting->layer_en);
	LCDLOGD("Info:  format = %d\r\n",setting->format);
	LCDLOGD("Info:  addr = 0x%x\r\n",setting->frame_buffer.pbuffer);
	LCDLOGD("Info:  stride = %d\r\n",setting->frame_buffer.stride);
	LCDLOGD("Info:  height = %d\r\n",setting->frame_buffer.height);
	LCDLOGD("Info:  bitdepth = %d\r\n",setting->frame_buffer.bitdepth);
	LCDLOGD("Info:  src_rect = (%d, %d, %d, %d)\r\n",setting->src_rect.startX,
		setting->src_rect.startY, setting->src_rect.width, setting->src_rect.height);
	LCDLOGD("Info:  dest_rect = (%d, %d, %d, %d)\r\n", setting->dest_rect.startX,
		setting->dest_rect.startY, setting->dest_rect.width, setting->dest_rect.height);
}
#endif

#if 1
static void dump_wb_setting(struct s_wb_layer_setting *setting)
{
	LCDLOGD("Info: dump_wb_setting ++\r\n");
	LCDLOGD("Info:  layer_en = %d\r\n",setting->layer_en);
	LCDLOGD("Info:  format = %d\r\n",setting->format);
	LCDLOGD("Info:  addr = 0x%x\r\n",setting->addr);
	LCDLOGD("Info:  width = %d\r\n",setting->width);
	LCDLOGD("Info:  height = %d\r\n",setting->height);
	LCDLOGD("Info:  stride = %d\r\n",setting->stride);
}
#endif

static int merge_roi(struct asrlcdd_roi_info *src_roi_1, struct asrlcdd_roi_info *src_roi_2,
	struct asrlcdd_roi_info *dest_roi)
{
	if((NULL == src_roi_1) || (NULL == src_roi_2) ||(NULL == dest_roi)){
		LCDLOGE("ERR: merge_roi: Invalid param ++\r\n");
		return -1;
	}
#if 0
	LCDLOGD("merge_roi: before (%d, %d, %d, %d), (%d, %d, %d, %d)\r\n", src_roi_1->startX, src_roi_1->startY,
		src_roi_1->width, src_roi_1->height, src_roi_2->startX, src_roi_2->startY,
		src_roi_2->width, src_roi_2->height);
#endif
	if(src_roi_1->startX < src_roi_2->startX)
		dest_roi->startX = src_roi_1->startX;
	else
		dest_roi->startX = src_roi_2->startX;

	if(src_roi_1->startY < src_roi_2->startY)
		dest_roi->startY = src_roi_1->startY;
	else
		dest_roi->startY = src_roi_2->startY;

	if(src_roi_1->startX + src_roi_1->width > src_roi_2->startX + src_roi_2->width)
		dest_roi->width = src_roi_1->startX + src_roi_1->width - dest_roi->startX;
	else
		dest_roi->width = src_roi_2->startX + src_roi_2->width - dest_roi->startX;

	if(src_roi_1->startY + src_roi_1->height> src_roi_2->startY + src_roi_2->height)
		dest_roi->height = src_roi_1->startY + src_roi_1->height - dest_roi->startY;
	else
		dest_roi->height = src_roi_2->startY + src_roi_2->height - dest_roi->startY;
#if 0
	LCDLOGD("merge_roi: after:(%d, %d, %d, %d)\r\n", dest_roi->startX, dest_roi->startY, dest_roi->width,
		dest_roi->height);
#endif
	return 0;
}

static int create_ROT_BufQueue(void* pBuffer, int size)
{
	struct panel_spec *panel = g_ppanel;
	int left_size = size;
	char *pBQaddr = pBuffer;
	int bq_size = 0;
	int i;

	if(NULL == pBuffer){
		LCDLOGE("ERROR: create_ROT_BufQueue: Invalid param!\r\n");
		return -1;
	}

	if(g_lcd_status == LCD_STATUS_POWEROFF){
		LCDLOGE("Info: create_ROT_BufQueue: Invalid lcd status!\r\n");
		return -1;
	}

	if(NULL == panel){
		LCDLOGE("Info: create_ROT_BufQueue: Invalid panel!\r\n");
		return -1;
	}

	/*ROT buffer is YUV420, so byte/pixel = 1.5, 2 buffers*/
	bq_size = panel->height * panel->width * 3/2;
//	left_size = bq_size;
//	if(left_size < bq_size * 2){
	if(left_size < bq_size){
		LCDLOGE("Info: create_ROT_BufQueue: Buffer is too small!\r\n");
		return -1;
	}

	if(g_lcd_ROTBQ.count != 0){
		LCDLOGE("ERROR: create_ROT_BufQueue: Current ROT BQ is not NULL!\r\n");
		return -1;
	}
	g_lcd_ROTBQ.used_count = 0;

	while(left_size >= bq_size){
		g_lcd_ROTBQ.buffers[g_lcd_ROTBQ.count].addr = pBQaddr;
		g_lcd_ROTBQ.buffers[g_lcd_ROTBQ.count].in_used = 0;
		g_lcd_ROTBQ.count++;
		g_lcd_ROTBQ.free_count++;
		pBQaddr += bq_size;
		left_size -= bq_size;
	};

	LCDLOGI("Info: create_ROT_BufQueue: Create %d ROT BQs and %d buffer left!\r\n",
		g_lcd_ROTBQ.count, left_size);

	for(i = 0; i< g_lcd_ROTBQ.count; i++){
		LCDLOGD("DBG: buffer_queue[%d] = 0x%x\r\n", i, g_lcd_ROTBQ.buffers[i].addr);
	}
	return 0;
}

static void release_ROT_BufQueue(void)
{
	int i;

	LCDLOGW("Info: release_ROT_BufQueue: release %d ROT BQs!\r\n", g_lcd_ROTBQ.count);

	for(i=0; i<MAX_ROTBQ_NUM; i++){
		g_lcd_ROTBQ.buffers[i].addr = 0;
		g_lcd_ROTBQ.buffers[i].in_used = 0;
	}
	g_lcd_ROTBQ.count = 0;
	g_lcd_ROTBQ.used_count = 0;
	g_lcd_ROTBQ.free_count = 0;
}

static int dequeue_specify_ROT_BQs(void* pbuffer)
{
	int i, id = -1;

	LCDLOGI("INFO: dequeue_ROT_BQs: +++\r\n");

	if(NULL == pbuffer){
		LCDLOGE("ERROR: dequeue_ROT_BQs: Invalid param\r\n");
		return -1;
	}

	if(0 == g_lcd_ROTBQ.count){
		LCDLOGE("ERROR: dequeue_ROT_BQs: ROT BQ is not created!!\r\n");
		return -1;
	}

	if(g_lcd_ROTBQ.free_count == 0){
		LCDLOGD("Warning: dequeue_ROT_BQs: BQ is all in used!!\r\n");
		return -1;
	}

	for(i=0; i<MAX_ROTBQ_NUM; i++){
		if(pbuffer == g_lcd_ROTBQ.buffers[i].addr){
			id = i;
			break;
		}
	}


	if(-1 == id){
		LCDLOGW("Warning: dequeue_ROT_BQs: not found empty buffer!!\r\n");
		return -1;
	}

	if(g_lcd_ROTBQ.buffers[id].in_used == 1){
		LCDLOGW("Warning: dequeue_ROT_BQs: specify Buffer is in used! Can't be dequeue!!!\r\n");
		return -1;
	}

	g_lcd_ROTBQ.buffers[id].in_used = 1;
	g_lcd_ROTBQ.free_count--;
	g_lcd_ROTBQ.used_count++;

	LCDLOGI("INFO: dequeue_ROT_BQs (0x%x)--- \r\n", g_lcd_ROTBQ.buffers[id].addr);
	return 0;
}


static int dequeue_ROT_BQs(void** ppbuffer)
{
	int i, id = -1;

	LCDLOGI("INFO: dequeue_ROT_BQs: +++\r\n");

	if(0 == g_lcd_ROTBQ.count){
		LCDLOGE("ERROR: dequeue_ROT_BQs: ROT BQ is not created!!\r\n");
		return -1;
	}

	if(g_lcd_ROTBQ.free_count == 0){
		LCDLOGD("Warning: dequeue_ROT_BQs: BQ is all in used!!\r\n");
		return -1;
	}

	for(i=0; i<MAX_ROTBQ_NUM; i++){
		if(0 == g_lcd_ROTBQ.buffers[i].in_used){
			id = i;
			break;
		}
	}

	if(-1 == id){
		LCDLOGW("Warning: dequeue_ROT_BQs: not found empty buffer!!\r\n");
		return -1;
	}

	g_lcd_ROTBQ.buffers[id].in_used = 1;
	*ppbuffer = g_lcd_ROTBQ.buffers[id].addr;
	g_lcd_ROTBQ.free_count--;
	g_lcd_ROTBQ.used_count++;

	LCDLOGI("INFO: dequeue_ROT_BQs (0x%x)--- \r\n", g_lcd_ROTBQ.buffers[id].addr);
	return 0;
}

static int enqueue_ROT_BQs(void* pbuffer)
{
	int i, id = -1;

	if(0 == g_lcd_ROTBQ.count){
		LCDLOGE("ERROR: enqueue_ROT_BQs: ROT BQ is not created!!\r\n");
		return -1;
	}

	if(0 == g_lcd_ROTBQ.used_count){
		LCDLOGE("ERROR: enqueue_ROT_BQs: BQ is empty!!\r\n");
		return -1;
	}

	for(i=0; i<MAX_ROTBQ_NUM; i++){
		if(pbuffer == g_lcd_ROTBQ.buffers[i].addr){
			id = i;
			break;
		}
	}

	if(-1 == id){
		LCDLOGW("Warning: enqueue_ROT_BQs: not found this buffer!!\r\n");
		return -1;
	}

	if(0 == g_lcd_ROTBQ.buffers[i].in_used){
		LCDLOGW("Warning: enqueue_ROT_BQs: the buffer is not in_used!!\r\n");
		return -1;
	}
	g_lcd_ROTBQ.buffers[id].in_used = 0;
	g_lcd_ROTBQ.free_count++;
	g_lcd_ROTBQ.used_count--;
	return 0;
}

static void config_lcd_layers(struct s_img_layer_setting *img_setting,
	struct s_osd_layer_setting *osd_setting, struct asrlcdd_roi_info *dest_roi,
	struct s_lcd_ctx *lcd)
{
	struct s_layer_setting *layer = NULL;
	unsigned int addr = 0;
	unsigned int size = 0;

	if(1 == img_setting->layer_en){
		layer = &lcd->layer_setting[LCD_LAYER_IMG];
		layer->format = img_setting->format;
		layer->alpha_sel = LCD_ALPHA_LAYER;
		layer->layer_alpha = 0xFF;
		layer->src_buffer[0].addr = img_setting->addr[0];
		layer->src_buffer[0].width = img_setting->src_rect.width;
		layer->src_buffer[0].height = img_setting->src_rect.height;
		layer->src_buffer[0].stride = img_setting->stride[0];
		if(img_setting->planes >= 2){
			layer->src_buffer[1].addr = img_setting->addr[1];
			layer->src_buffer[1].width =img_setting->src_rect.width;
			layer->src_buffer[1].height = img_setting->src_rect.height;
			layer->src_buffer[1].stride = img_setting->stride[1]; /*FIX ME*/
		}
		if(img_setting->planes == 3){
			layer->src_buffer[2].addr = img_setting->addr[2];
			layer->src_buffer[2].width = img_setting->src_rect.width;
			layer->src_buffer[2].height = img_setting->src_rect.height;
			layer->src_buffer[2].stride = img_setting->stride[2];/*FIX ME*/
		}

		layer->dest_point.x = img_setting->dest_rect.startX - dest_roi->startX;
		layer->dest_point.y = img_setting->dest_rect.startY -  dest_roi->startY;
		layer->ck_setting.ck_en = 0;
		layer->cbsh_setting.cbsh_en = 0;
		layer->mirror_en = 0;
		lcd_enable_layer(lcd, LCD_LAYER_IMG);
	} else {
		lcd_disable_layer(lcd, LCD_LAYER_IMG);
	}

	if(1 == osd_setting->layer_en){
		layer = &lcd->layer_setting[LCD_LAYER_OSD1];
		layer->format = osd_setting->format;
		layer->alpha_sel = LCD_ALPHA_LAYER;
		if(1 == img_setting->layer_en)
			layer->layer_alpha = g_alpha_val;
		else
			layer->layer_alpha = 0xFF;

		addr = (uint32_t)osd_setting->frame_buffer.pbuffer;
		addr += osd_setting->src_rect.startY * osd_setting->frame_buffer.stride +
			osd_setting->src_rect.startX * osd_setting->frame_buffer.bitdepth/8;

		layer->src_buffer[0].addr = addr;
		layer->src_buffer[0].width = osd_setting->src_rect.width;
		layer->src_buffer[0].height = osd_setting->src_rect.height;
		layer->src_buffer[0].stride = osd_setting->frame_buffer.stride;
		size = osd_setting->src_rect.height * osd_setting->frame_buffer.stride;
		size = LCD_ROUND_UP(size,32);
		CacheCleanMemory( (void *)addr, size);

		if(1 == img_setting->layer_en){
#if 0
			layer->dest_point.x = dest_roi->startX;
			layer->dest_point.y = dest_roi->startY;
#else
			layer->dest_point.x = osd_setting->dest_rect.startX - dest_roi->startX;
			layer->dest_point.y = osd_setting->dest_rect.startY - dest_roi->startY;
#endif
		}
		else{
			layer->dest_point.x = osd_setting->dest_rect.startX - dest_roi->startX;
			layer->dest_point.y = osd_setting->dest_rect.startY - dest_roi->startY;
		}
		layer->ck_setting.ck_en = 0;
		layer->cbsh_setting.cbsh_en = 0;
		layer->mirror_en = 0;
		lcd_enable_layer(lcd, LCD_LAYER_OSD1);
	} else {
		lcd_disable_layer(lcd, LCD_LAYER_OSD1);
	}

	lcd_disable_layer(lcd, LCD_LAYER_OSD2);
}

static int rotate_osd_layer(struct s_osd_layer_setting *osd_setting, char* dest_buf, struct panel_spec *panel, int rotation)
{
	struct cam_offline_buf src_buf;
	struct cam_offline_buf dst_buf;
	unsigned int addr = 0;
	int temp;
	uint32_t size;
	int ret;

	if(0 == osd_setting->layer_en)
		return 0;

	if((rotation != ROT_270_DEG) && (rotation != ROT_180_DEG) && (rotation != ROT_90_DEG)){
		LCDLOGE("ERROR:rotate_osd_layer: Invalid rotation (%d)!\r\n", rotation);
		return -1;
	}

	src_buf.num_planes = 1; /*OSD layer use 1 plane only*/
	src_buf.width = osd_setting->src_rect.width ;
	src_buf.height = osd_setting->src_rect.height;
	src_buf.width = (src_buf.width + 1) & 0xFFFFFFFE; /*2 alignment*/
	src_buf.height = (src_buf.height + 1) & 0xFFFFFFFE; /*2 alignment*/
	src_buf.stride = (osd_setting->frame_buffer.stride / (osd_setting->frame_buffer.bitdepth/8));
	src_buf.pixelformat = (SCALER_INPUT_FMT)switch_lcdformat_to_camformat(osd_setting->format);
	addr = (uint32_t)osd_setting->frame_buffer.pbuffer;
	addr += osd_setting->src_rect.startY * osd_setting->frame_buffer.stride +
		osd_setting->src_rect.startX * osd_setting->frame_buffer.bitdepth/8;
	src_buf.plane_addr[0] = addr;

#ifdef GUI_LCD_MUTEX
	if(g_gui_lcd_mutex_id != INVALID_MUTEX_ID)
		UOS_TakeMutex(g_gui_lcd_mutex_id);
#endif
	size = src_buf.stride * src_buf.height * osd_setting->frame_buffer.bitdepth/8;
	size = LCD_ROUND_UP(size,32);
	CacheCleanMemory( (void *)addr, size);
	ret = lcd_do_rotation(&src_buf, &dst_buf, rotation, dest_buf);

#ifdef GUI_LCD_MUTEX
	if(g_gui_lcd_mutex_id != INVALID_MUTEX_ID)
		UOS_ReleaseMutex(g_gui_lcd_mutex_id);
#endif

	if(0 != ret){
		LCDLOGE("ERROR:rotate_osd_layer: rotation fail!\r\n");
		return -1;
	}

	osd_setting->frame_buffer.pbuffer = dest_buf;
	osd_setting->frame_buffer.height = dst_buf.height;
	osd_setting->frame_buffer.stride = dst_buf.stride * (osd_setting->frame_buffer.bitdepth/8);
	osd_setting->src_rect.startX = 0;
	osd_setting->src_rect.startY = 0;
	osd_setting->src_rect.width = dst_buf.width;
	osd_setting->src_rect.height = dst_buf.height;

	if(ROT_270_DEG == rotation){
		temp = osd_setting->dest_rect.startX;
		osd_setting->dest_rect.startX = osd_setting->dest_rect.startY;
		osd_setting->dest_rect.startY = panel->height - temp - osd_setting->src_rect.height;
	} else if(ROT_180_DEG == rotation){
		temp = osd_setting->dest_rect.startX;
		osd_setting->dest_rect.startX = panel->width - temp - osd_setting->src_rect.width;
		temp = osd_setting->dest_rect.startY;
		osd_setting->dest_rect.startY = panel->height - temp - osd_setting->src_rect.height;
	} else if(ROT_90_DEG == rotation){
		temp = osd_setting->dest_rect.startY;
		osd_setting->dest_rect.startY = osd_setting->dest_rect.startX;
		osd_setting->dest_rect.startX = panel->width - temp - osd_setting->src_rect.width;
	}
	if((ROT_270_DEG == rotation) || (ROT_90_DEG == rotation)) {
		temp = osd_setting->dest_rect.width;
		osd_setting->dest_rect.width = osd_setting->dest_rect.height;
		osd_setting->dest_rect.height = temp;
	}

	return 0;
}

static void prepare_fstn_data(struct s_osd_layer_setting *osd_setting, unsigned char* pfstn_data)
{
	int i, j, z;
	struct panel_spec *panel = g_ppanel;
	unsigned int bitdepth = osd_setting->frame_buffer.bitdepth;
	unsigned char* psrc = (unsigned char*)osd_setting->frame_buffer.pbuffer;
	unsigned char* pdst = pfstn_data;
	unsigned char r,g,b;
	unsigned char y;
	unsigned short rgb565;
	unsigned char fstn = 0;

	LCDLOGD("INFO: prepare_fstn_data++ (%d * %d)\r\n", panel->width, panel->height);

	if(NULL == pfstn_data){
		LCDLOGW("Warning: prepare_fstn_data, invalid param!\r\n");
		return;
	}
	pdst = pfstn_data;
	for(i = 0; i < panel->height; i=i+8){
		for(j = 0; j < panel->width; j++){
			fstn = 0;
			for(z = 0; z < 8; z++){
				psrc = (unsigned char*)osd_setting->frame_buffer.pbuffer +
					osd_setting->frame_buffer.stride * (i + z) + j * bitdepth / 8;
				if(bitdepth == 16){/*RGB565*/
					rgb565 = *psrc++;
					rgb565 = (*psrc++ << 8) | rgb565;
					r = (rgb565 >> 11) & 0x1F;
					g = (rgb565 >> 5) & 0x3F;
					b = rgb565 & 0x1F;
				} else {/*RGBA8888*/
					b = *psrc++;
					g = *psrc++;
					r = *psrc++;
					psrc++;
				}
				y = (uint8_t)((77 * r + 150 * g + 29 * b) >> 8);
				if(y < 128){
					fstn |= (1 << z);
				}
			}
			if(j % 2 == 0){
				pdst++;
				*pdst = fstn;
				pdst--;
			} else {
				*pdst = fstn;
				pdst+=2;
			}
		}
	}
	LCDLOGD("INFO: prepare_fstn_data--\r\n");
}

static int refresh_lcd_rgb(struct s_img_layer_setting *img_setting,
	struct s_osd_layer_setting *osd_setting, struct asrlcdd_roi_info *dest_roi, int lcd_id)
{
	struct s_lcd_ctx *lcd = g_plcd;
	struct panel_spec *panel = g_ppanel;
	int ret;

#if (defined(GUI_LCD_MUTEX) && !defined(LCD_ROTATION_90) && !defined(LCD_ROTATION_270) && !defined(LCD_ROTATION_180))
	if(g_gui_lcd_mutex_id != INVALID_MUTEX_ID)
		UOS_TakeMutex(g_gui_lcd_mutex_id);
#endif
	if((LCD_ID_MAIN == lcd_id) && (LCD_STATUS_SLEEP == (g_lcd_status & LCD_STATUS_SLEEP))){
		LCDLOGI("INFO: need to sleep, not refresh this frame!\n");
#if (defined(GUI_LCD_MUTEX) && !defined(LCD_ROTATION_90) && !defined(LCD_ROTATION_270) && !defined(LCD_ROTATION_180))
		if(g_gui_lcd_mutex_id != INVALID_MUTEX_ID)
			UOS_ReleaseMutex(g_gui_lcd_mutex_id);
#endif
		return 0;
	}

	if((LCD_ID_MAIN != lcd_id) && (LCD_STATUS_SUB_SLEEP == (g_lcd_status & LCD_STATUS_SUB_SLEEP))){
		LCDLOGI("INFO: sub panel need to sleep, not refresh this frame!\n");
#if (defined(GUI_LCD_MUTEX) && !defined(LCD_ROTATION_90) && !defined(LCD_ROTATION_270) && !defined(LCD_ROTATION_180))
		if(g_gui_lcd_mutex_id != INVALID_MUTEX_ID)
			UOS_ReleaseMutex(g_gui_lcd_mutex_id);
#endif
		return 0;
	}

	panel = g_ppanel;
#ifdef LCD_DUAL_PANEL_SUPPORT
	if(LCD_ID_SUB == lcd_id)
		panel = g_psubpanel;
#endif

	if((NULL != g_esd_ctx) && (INVALID_MUTEX_ID != g_esd_ctx->esd_mutex)){
		UOS_TakeMutex(g_esd_ctx->esd_mutex);
	}

	g_flip_pending = 1;
	lcd->wb_setting.wb_en = 0;
	config_lcd_layers(img_setting, osd_setting ,dest_roi, lcd);
	lcd_output_setting(lcd, panel);
	lcd_update_output_setting(lcd, dest_roi->width, dest_roi->height);

//		LCDLOGD("DBG: lcd_thread_handler: refresh frame!\r\n");
	ret = panel_before_refresh(panel, dest_roi->startX, dest_roi->startY, dest_roi->height, dest_roi->width);
	if(0 != ret){
		LCDLOGE("ERR: refresh_lcd_rgb: panel_before_refresh fail\r\n");
	}
#ifdef ASRLCD_DOUBLE_FB
	UOS_TakeMutex(g_lcd_hwfb_mutex_id);
#endif
	ret = lcd_refresh(lcd);
	if(0 != ret){
		LCDLOGE("ERR: refresh_lcd_rgb: lcd_refresh fail\r\n");
	}
#ifdef ASRLCD_DOUBLE_FB
	UOS_ReleaseMutex(g_lcd_hwfb_mutex_id);
#endif
	ret = lcd_after_refresh(lcd);
	if(0 != ret){
		LCDLOGE("ERR: refresh_lcd_rgb: lcd_after_refresh fail\r\n");
	}
	
	ret = panel_after_refresh(panel);
	if(0 != ret){
		LCDLOGE("ERR: refresh_lcd_rgb: panel_after_refresh fail\r\n");
	}
	g_flip_pending = 0;

	if((NULL != g_esd_ctx) && (INVALID_MUTEX_ID != g_esd_ctx->esd_mutex)){
		UOS_ReleaseMutex(g_esd_ctx->esd_mutex);
	}

#if (defined(GUI_LCD_MUTEX) && !defined(LCD_ROTATION_90) && !defined(LCD_ROTATION_270) && !defined(LCD_ROTATION_180))
	if(g_gui_lcd_mutex_id != INVALID_MUTEX_ID)
		UOS_ReleaseMutex(g_gui_lcd_mutex_id);
#endif
	return 0;
}

static int refresh_lcd_fstn(struct s_img_layer_setting *img_setting,
	struct s_osd_layer_setting *osd_setting, struct asrlcdd_roi_info *dest_roi, int lcd_id)
{
	struct s_lcd_ctx *lcd = g_plcd;
	struct panel_spec *panel;
	struct s_img_layer_setting fstn_img_setting;
	struct s_osd_layer_setting fstn_osd_setting;
	struct asrlcdd_roi_info fstn_dest_roi;
	int ret;
	int i;

	LCDLOGD("INFO:refresh_lcd_fstn++\r\n");

#if (defined(GUI_LCD_MUTEX) && !defined(LCD_ROTATION_90) && !defined(LCD_ROTATION_270) && !defined(LCD_ROTATION_180))
	if(g_gui_lcd_mutex_id != INVALID_MUTEX_ID)
		UOS_TakeMutex(g_gui_lcd_mutex_id);
#endif
	if((LCD_ID_MAIN == lcd_id) && (LCD_STATUS_SLEEP == (g_lcd_status & LCD_STATUS_SLEEP))){
		LCDLOGI("INFO: need to sleep, not refresh this frame!\n");
#ifdef GUI_LCD_MUTEX
		if(g_gui_lcd_mutex_id != INVALID_MUTEX_ID)
			UOS_ReleaseMutex(g_gui_lcd_mutex_id);
#endif
		return 0;
	}

	if((LCD_ID_MAIN != lcd_id) && (LCD_STATUS_SUB_SLEEP == (g_lcd_status & LCD_STATUS_SUB_SLEEP))){
		LCDLOGI("INFO: sub panel need to sleep, not refresh this frame!\n");
#if (defined(GUI_LCD_MUTEX) && !defined(LCD_ROTATION_90) && !defined(LCD_ROTATION_270) && !defined(LCD_ROTATION_180))
		if(g_gui_lcd_mutex_id != INVALID_MUTEX_ID)
			UOS_ReleaseMutex(g_gui_lcd_mutex_id);
#endif
		return 0;
	}

	if(NULL == g_fstn_data){
		LCDLOGE("ERROR: refresh_lcd_fstn: no fstn memory!!\n");
		return 0;
	}

	panel = g_ppanel;
#ifdef LCD_DUAL_PANEL_SUPPORT
	if(LCD_ID_SUB == lcd_id)
		panel = g_psubpanel;
#endif

	prepare_fstn_data(osd_setting, g_fstn_data);
#if (defined(GUI_LCD_MUTEX) && !defined(LCD_ROTATION_90) && !defined(LCD_ROTATION_270) && !defined(LCD_ROTATION_180))
	if(g_gui_lcd_mutex_id != INVALID_MUTEX_ID)
		UOS_ReleaseMutex(g_gui_lcd_mutex_id);
#endif

	fstn_img_setting.layer_en = 0;

	fstn_osd_setting.layer_en = 1;
	fstn_osd_setting.format = LCD_FORMAT_RGB565;
	fstn_osd_setting.src_rect.startX = 0;
	fstn_osd_setting.src_rect.startY = 0;
	fstn_osd_setting.src_rect.width = panel->width / 2;
	fstn_osd_setting.src_rect.height = 1;
	fstn_osd_setting.dest_rect.startX = 0;
	fstn_osd_setting.dest_rect.startY = 0;
	fstn_osd_setting.dest_rect.width = panel->width / 2;
	fstn_osd_setting.dest_rect.height = 1;
	fstn_osd_setting.frame_buffer.pbuffer = g_fstn_data;
	fstn_osd_setting.frame_buffer.stride = panel->width;
	fstn_osd_setting.frame_buffer.bitdepth = 16;
	fstn_osd_setting.frame_buffer.height = 1;

	fstn_dest_roi.startX = 0;
	fstn_dest_roi.startY = 0;
	fstn_dest_roi.width = panel->width / 2;
	fstn_dest_roi.height = 1;

	g_flip_pending = 1;
	lcd->wb_setting.wb_en = 0;

	for(i= 0; i<panel->height; i+=8){
		fstn_osd_setting.frame_buffer.pbuffer = g_fstn_data + panel->width * i/8;

		config_lcd_layers(&fstn_img_setting, &fstn_osd_setting ,&fstn_dest_roi, lcd);
		lcd_update_output_setting(lcd, fstn_dest_roi.width, fstn_dest_roi.height);

		ret = panel_before_refresh(panel, 0, i, fstn_dest_roi.height, fstn_dest_roi.width);
		if(0 != ret){
			LCDLOGE("ERR: refresh_lcd_fstn: panel_before_refresh fail\r\n");
		}

#ifdef ASRLCD_DOUBLE_FB
		UOS_TakeMutex(g_lcd_hwfb_mutex_id);
#endif
		ret = lcd_refresh(lcd);
		if(0 != ret){
			LCDLOGE("ERR: refresh_lcd_fstn: lcd_refresh fail\r\n");
		}
#ifdef ASRLCD_DOUBLE_FB
		UOS_ReleaseMutex(g_lcd_hwfb_mutex_id);
#endif
		ret = lcd_after_refresh(lcd);
		if(0 != ret){
			LCDLOGE("ERR: refresh_lcd_fstn: lcd_after_refresh fail\r\n");
		}
		ret = panel_after_refresh(panel);
		if(0 != ret){
			LCDLOGE("ERR: refresh_lcd_fstn: panel_after_refresh fail\r\n");
		}
	}
	g_flip_pending = 0;
	LCDLOGD("INFO:refresh_lcd_fstn--\r\n");

	return 0;
}

static int refresh_lcd(struct s_img_layer_setting *img_setting,
	struct s_osd_layer_setting *osd_setting, int lcd_id)
{
	struct panel_spec *panel = NULL;
	struct asrlcdd_roi_info dest_roi;
	int ret = 0;

	LCDLOGD("DBG: refresh_lcd ++ (%d)!\n", lcd_id);

	if(lcd_id >= MAX_PANEL_COUNT){
		LCDLOGE("ERR: refresh_lcd: Invalid lcd_id (%d)!\n", lcd_id);
		return -1;
	}

	panel = g_ppanel;
#ifdef LCD_DUAL_PANEL_SUPPORT
	if(LCD_ID_SUB == lcd_id)
		panel = g_psubpanel;
#endif

#ifdef LCD_ROTATION_90
	ret = rotate_osd_layer(osd_setting, (char*)g_landscape_buffer, panel, ROT_90_DEG);
#endif

#ifdef LCD_ROTATION_270
	ret = rotate_osd_layer(osd_setting, (char*)g_landscape_buffer, panel, ROT_270_DEG);
#endif

#ifdef LCD_ROTATION_180
	ret = rotate_osd_layer(osd_setting, (char*)g_landscape_buffer, panel, ROT_180_DEG);
#endif

	if(ret != 0){
		LCDLOGE("ERR:refresh_lcd: rotate osd layer fail!\r\n");
		return -1;
	}

	if((1 == img_setting->layer_en) && (1 == osd_setting->layer_en)){
#if 0
		LCDLOGE("merge_roi: before (%d, %d, %d, %d), (%d, %d, %d, %d)\r\n", img_setting->dest_rect.startX, img_setting->dest_rect.startY,
			img_setting->dest_rect.width, img_setting->dest_rect.height,osd_setting->dest_rect.startX, osd_setting->dest_rect.startY,
			osd_setting->dest_rect.width, osd_setting->dest_rect.height);
#endif

		ret = merge_roi(&(img_setting->dest_rect), &(osd_setting->dest_rect), &dest_roi);
#if 0
		LCDLOGE("merge_roi: after:(%d, %d, %d, %d)\r\n", dest_roi.startX, dest_roi.startY, dest_roi.width,
			dest_roi.height);
#endif
	} else if(1 == img_setting->layer_en){
		dest_roi.startX = img_setting->dest_rect.startY;
		dest_roi.startY = img_setting->dest_rect.startX;
		dest_roi.width = img_setting->dest_rect.width;
		dest_roi.height = img_setting->dest_rect.height;
	} else if(1 == osd_setting->layer_en){
		dest_roi.startX = osd_setting->dest_rect.startX;
		dest_roi.startY = osd_setting->dest_rect.startY;
		dest_roi.width = osd_setting->dest_rect.width;
		dest_roi.height = osd_setting->dest_rect.height;
	} else {
		//refresh bg color
		dest_roi.startX = osd_setting->dest_rect.startX;
		dest_roi.startY = osd_setting->dest_rect.startY;
		dest_roi.width = osd_setting->dest_rect.width;
		dest_roi.height = osd_setting->dest_rect.height;
	}

	LCDLOGD("refresh_lcd:dest_roi: (%d, %d, %d, %d)\r\n", dest_roi.startX, dest_roi.startY, dest_roi.width,
		dest_roi.height);

	if(panel->type == LCD_TYPE_FSTN)
		ret = refresh_lcd_fstn(img_setting, osd_setting, &dest_roi, lcd_id);
	else
		ret =  refresh_lcd_rgb(img_setting, osd_setting, &dest_roi, lcd_id);

	if(ret != 0){
		LCDLOGE("ERR:refresh_lcd: refresh fail!\r\n");
		return -1;
	}

	/*Patch for delay backlight*/
	if(LCD_ID_MAIN == lcd_id){
		LCDLOGD("DBG:refresh_lcd: g_backlight_on = %d, g_backlight_level = %d\r\n", g_backlight_on, g_backlight_level);
		if((g_backlight_on == 0) && (g_backlight_level != 0) && (g_refreshed == 0)){
			backlight_set_brightness(g_backlight_level);
			g_backlight_on = 1;
			g_refreshed = 1;
		}
#ifdef LCD_DUAL_PANEL_SUPPORT
	} else {
		LCDLOGD("DBG:refresh_lcd: g_subbacklight_on = %d, g_subbacklight_level = %d\r\n", g_subbacklight_on, g_subbacklight_level);
		if((g_subbacklight_on == 0) && (g_subbacklight_level != 0) && (g_subrefreshed == 0)){
			sub_backlight_set_brightness(g_subbacklight_level);
			g_subbacklight_on = 1;
			g_subrefreshed = 1;
		}

#endif
	}
#if 0 /*Patch for delay backlight*/
	{
		extern UINT8 g_mciLcdBrightnessLevel; // do not use the dimming value		
		extern BOOL g_mciLcdBackLightOn;
#ifndef CONFIG_RELEASE_WITH_NO_ASR_UI
		extern bool g_delay_backlight_enable;
#endif
		if((g_mciLcdBackLightOn == FALSE)
#ifndef CONFIG_RELEASE_WITH_NO_ASR_UI
                    && (g_delay_backlight_enable == TRUE)
#endif
		  )
		{
			backlight_set_brightness(g_mciLcdBrightnessLevel);
			g_mciLcdBackLightOn = TRUE;	
			raw_uart_log("delay backlight flag:[%d,%d]\n", FALSE, TRUE);
		}
	}
#endif
	return ret;
}

static int wb_lcd(struct s_img_layer_setting *img_setting,
	struct s_osd_layer_setting *osd_setting, struct s_wb_layer_setting *wb_setting)
{
	struct s_wb_setting *wb = NULL;
	struct s_lcd_ctx *lcd = g_plcd;
	struct panel_spec *panel = g_ppanel;
	struct asrlcdd_roi_info dest_roi;
	int ret;
#ifdef GUI_LCD_MUTEX
	if(g_gui_lcd_mutex_id != INVALID_MUTEX_ID)
		UOS_TakeMutex(g_gui_lcd_mutex_id);
#endif
	if(LCD_STATUS_SLEEP == (g_lcd_status & LCD_STATUS_SLEEP)){
		LCDLOGI("INFO: need to sleep, not refresh this frame!\n");
		return 0;
	}

	g_flip_pending = 1;

	wb = &(lcd->wb_setting);
	wb->wb_en = 1;
	wb->format = wb_setting->format;
	wb->dest_buffer.addr = wb_setting->addr;
	wb->dest_buffer.height = wb_setting->height;
	wb->dest_buffer.width = wb_setting->width;
	wb->dest_buffer.stride = wb_setting->stride;

	dest_roi.startX = 0;
	dest_roi.startY = 0;
	dest_roi.width = wb_setting->width;
	dest_roi.height = wb_setting->height;

	config_lcd_layers(img_setting, osd_setting ,&dest_roi, lcd);
	lcd_update_output_setting(lcd, dest_roi.width, dest_roi.height);

//		LCDLOGD("DBG: lcd_thread_handler: refresh frame!\r\n");
	ret = panel_before_wb(panel);
	if(0 != ret){
		LCDLOGE("ERR: lcd_thread_handler: panel_before_refresh fail\r\n");
	}
#ifdef ASRLCD_DOUBLE_FB
	UOS_TakeMutex(g_lcd_hwfb_mutex_id);
#endif
	ret = lcd_refresh(lcd);
	if(0 != ret){
		LCDLOGE("ERR: lcd_thread_handler: lcd_refresh fail\r\n");
	}
#ifdef ASRLCD_DOUBLE_FB
	UOS_ReleaseMutex(g_lcd_hwfb_mutex_id);
#endif
	ret = lcd_after_refresh(lcd);
	if(0 != ret){
		LCDLOGE("ERR: lcd_thread_handler: lcd_after_refresh fail\r\n");
	}
	
	ret = panel_after_refresh(panel);
	if(0 != ret){
		LCDLOGE("ERR: lcd_thread_handler: panel_after_refresh fail\r\n");
	}
	g_flip_pending = 0;

#ifdef GUI_LCD_MUTEX
	if(g_gui_lcd_mutex_id != INVALID_MUTEX_ID)
		UOS_ReleaseMutex(g_gui_lcd_mutex_id);
#endif
	return 0;
}

#if 0
void lcd_timer_handler(void* argv)
{
	unsigned int event[4];

	LCDLOGI("INFO: lcd_timer_handler +++\n");

	/*for debug camera hang*/
	if (!strcmp(argv, "close_camera")){
		event[1] = 0xcd;
		//LCDLOGW("INFO: lcd_timer_handler msg from close camera (%d,%d,%d,%d) %s\n",g_release_bq, g_lcd_osd_setting.dirty,g_lcd_status,g_lcd_img_setting.layer_en,argv);
	} else{
		event[1] = 0;
	}

	g_lcd_timer_started = 0;

	if((0 == g_lcd_osd_setting.dirty) && (g_release_bq != 2))
		return;

	if((LCD_STATUS_POWERON != (g_lcd_status & LCD_STATUS_POWERON)) && (g_release_bq != 2))
		return;

	event[0] = LCD_MSG_TYPE_UPDATE;
	event[2] = LCD_ID_MAIN;
 	UOS_SendMsg((void*)event, g_lcd_msgq_id, UOS_SEND_EVT);

	LCDLOGI("INFO: lcd_timer_handler ---\n");
}
#endif

void lcd_thread_handler(void* argv)
{
//	struct asrlcdd_roi_info dest_roi;
	struct s_lcd_ctx *lcd = NULL;
	unsigned int event[4];
	int ret;
	u8 ret1;
	u32 msgs_in_queue;
	u8 i;
	int skip_flag;
	struct s_img_layer_setting img_setting;
	struct s_osd_layer_setting osd_setting;
//	struct panel_spec *panel = NULL;
	int lcd_id = LCD_ID_MAIN;

	skip_flag = 0;

	LCDLOGI("INFO: lcd_thread_handler +++\n");
	while(0 == g_lcd_thread_exit){

		if (0 == skip_flag) {
       		UOS_WaitMsg(event, g_lcd_msgq_id, OSA_SUSPEND);
		} else {
			// event has got already
			skip_flag = 0;
		}

		LCDLOGD("DBG: lcd_thread_handler: Get evet (%d)\n", event[0]);
		if(1 == g_lcd_thread_exit){
			LCDLOGI("Info: lcd_thread_handler: need exit thread!\n");
			break;
		}

		lcd = g_plcd;

		switch(event[0]){
		case LCD_MSG_TYPE_STOP:
			g_lcd_thread_exit = 1;
			LCDLOGI("INFO: lcd_thread_handler: LCD_MSG_TYPE_STOP\n");
			break;
		case LCD_MSG_TYPE_FILL:
		case LCD_MSG_TYPE_SUB_FILL:
			lcd->bg_color = event[1];
	//		dest_roi.startX = event[2] & 0xFFFF;
	//		dest_roi.startY = (event[2] >> 16) & 0xFFFF;
	//		dest_roi.width = event[3] & 0xFFFF;
	//		dest_roi.height = (event[3] >> 16) & 0xFFFF;
			img_setting.layer_en = 0;
			osd_setting.layer_en = 0;
			osd_setting.dest_rect.startX = event[2] & 0xFFFF;
			osd_setting.dest_rect.startY = (event[2] >> 16) & 0xFFFF;
			osd_setting.dest_rect.width = event[3] & 0xFFFF;
			osd_setting.dest_rect.height = (event[3] >> 16) & 0xFFFF;
//			refresh_lcd(&img_setting, &osd_setting, &dest_roi);
			if(LCD_MSG_TYPE_FILL == event[0])
				refresh_lcd(&img_setting, &osd_setting, LCD_ID_MAIN);
#ifdef LCD_DUAL_PANEL_SUPPORT
			else
				refresh_lcd(&img_setting, &osd_setting, LCD_ID_SUB);
#endif
			break;
		case LCD_MSG_TYPE_SYNC:
			ret = UOS_SetFlag(g_lcd_sync_flag, 0x1, OSA_FLAG_OR);
			if (ret != 0) {
				LCDLOGE("ERROR: lcd_thread_handler: flag set error\n");
			}
			break;
		case LCD_MSG_TYPE_UPDATE:
//			panel = g_ppanel;
			lcd_id = LCD_ID_MAIN;
#ifdef LCD_DUAL_PANEL_SUPPORT
			if(event[2] != LCD_ID_MAIN){
//				panel = g_psubpanel;
				lcd_id = LCD_ID_SUB;
			}
#endif
			// merge n UPDATE message to 1 UPDATE message
			msgs_in_queue = UOS_MsgQEnqueued(g_lcd_msgq_id);
			for (i = 0; i < msgs_in_queue; i++) {
				UOS_WaitMsg(event, g_lcd_msgq_id, OSA_NO_SUSPEND);
				if (LCD_MSG_TYPE_UPDATE != event[0]) {
					skip_flag = 1;
					break;
				}
			}
			
			UOS_TakeMutex(g_lcd_mutex_id);
			if(lcd_id == LCD_ID_MAIN){
				memcpy(&img_setting, &g_lcd_img_setting, sizeof(struct s_img_layer_setting));
				memcpy(&osd_setting, &g_lcd_osd_setting, sizeof(struct s_osd_layer_setting));
				g_lcd_img_setting.dirty = 0;
				g_lcd_osd_setting.dirty = 0;
#ifdef LCD_DUAL_PANEL_SUPPORT
			} else {
				memcpy(&img_setting, &g_sublcd_img_setting, sizeof(struct s_img_layer_setting));
				memcpy(&osd_setting, &g_sublcd_osd_setting, sizeof(struct s_osd_layer_setting));
				g_sublcd_img_setting.dirty = 0;
				g_sublcd_osd_setting.dirty = 0;
#endif
			}
			UOS_ReleaseMutex(g_lcd_mutex_id);
			LCDLOGD("DBG: lcd_thread_handler: Get osd mutex (%d, %d, %d, %d), (%d, %d, %d, %d)\r\n",
				osd_setting.src_rect.startX, osd_setting.src_rect.startY, 
				osd_setting.src_rect.width, osd_setting.src_rect.height,
				osd_setting.dest_rect.startX, osd_setting.dest_rect.startY, 
				osd_setting.dest_rect.width, osd_setting.dest_rect.height);
			LCDLOGD("DBG: lcd_thread_handler: Get img mutex (%d, %d, %d, %d), (%d, %d, %d, %d)\r\n",
				img_setting.src_rect.startX, img_setting.src_rect.startY,
				img_setting.src_rect.width, img_setting.src_rect.height,
				img_setting.dest_rect.startX, img_setting.dest_rect.startY,
				img_setting.dest_rect.width, img_setting.dest_rect.height);

#if 0
			if((1 == img_setting.layer_en) && (1 == osd_setting.layer_en)){
				//ret = merge_roi(&(img_setting.dest_rect), &(osd_setting.dest_rect), &dest_roi);
				dest_roi.startX = 0;
				dest_roi.startY = 0;
				dest_roi.width  = panel->width;
				dest_roi.height = panel->height;
				osd_setting.src_rect.startX = 0;
				osd_setting.src_rect.startY = 0;
				osd_setting.src_rect.width = panel->width;
				osd_setting.src_rect.height = panel->height;
				refresh_lcd(&img_setting, &osd_setting, &dest_roi);
			} else if(1 == img_setting.layer_en){
				refresh_lcd(&img_setting, &osd_setting, &img_setting.dest_rect);
			} else if(1 == osd_setting.layer_en){
				refresh_lcd(&img_setting, &osd_setting, &osd_setting.dest_rect);
			} else {
				LCDLOGE("ERROR: lcd_thread_handler: No layer enabled!!\n");
				continue;
			}
#else
			refresh_lcd(&img_setting, &osd_setting, lcd_id);
#endif

			if(img_setting.layer_en == 1){
				/*for debug camera hang*/
				if(event[1] == 0xcd){
					LCDLOGE("ERROR: lcd_thread_handler: img layer should disable,(%d,%d)\n",g_lcd_ROTBQ.count,g_release_bq);
				}

				/*enqueue rot buffer when camera status is normal*/
				if(g_lcd_camerastatus == 0 && g_lcd_camerastatus_pending == 0){
					ret = enqueue_ROT_BQs((void*)(img_setting.addr[0]));
					if(0 != ret){
						LCDLOGE("ERR: lcd_thread_handler:enqueue buffer fail\r\n");
					}
				}
			}else {
				/*for debug camera hang*/
				if(event[1] == 0xcd){
					LCDLOGW("INFO: lcd_thread_handler: (%d,%d)\n",g_lcd_ROTBQ.count,g_release_bq);
				}

				if((0 != g_lcd_ROTBQ.count) && (g_release_bq == 2)){
					release_ROT_BufQueue();
					if(g_lcd_cbs[ASRLCDD_CB_TYPE_BUFDONE] != NULL){
						LCDLOGW("INFO: lcd_thread_handler: cb +++\n");
						g_lcd_cbs[ASRLCDD_CB_TYPE_BUFDONE](g_lcd_cb_params[ASRLCDD_CB_TYPE_BUFDONE], NULL);
						LCDLOGW("INFO: lcd_thread_handler: cb ---\n");
					}
					g_last_ROT_buffer = NULL;
					g_release_bq = 0;
				}
			}
			break;

		default:
			LCDLOGE("ERROR: lcd_thread_handler: Invalid msg type (%d)\n", event[0]);
			continue;
    	}
		LCDLOGD("DBG: lcd_thread_handler: evet (%d) finish!\n", event[0]);
	}

	ret1 = UOS_SetFlag(g_lcd_flag_id, 0x1, OSA_FLAG_OR);
	if(0 != ret1){
		LCDLOGE("ERROR: lcd_thread_handler: flag set error\n");
	}
	LCDLOGI("INFO: lcd_thread_handler ---\n");
}

static int create_asrlcd_task()
{
	LCDLOGI("INFO: create_asrlcd_task+++\n");

	if(NULL != g_lcd_task){
		LCDLOGI("%s: task has been createdd!\n", __func__);
		return 0;
	}
#ifdef GUI_LCD_MUTEX	
	g_gui_lcd_mutex_id = UOS_NewMutex("gui_lcd");
#endif
#ifdef ASRLCD_DOUBLE_FB
	g_lcd_hwfb_mutex_id = UOS_NewMutex("lcd_fb");
#endif
	g_lcd_mutex_id = UOS_NewMutex("lcd");
	g_lcd_thread_exit = 0;
	g_lcd_msgq_id = UOS_NewMessageQueue("lcdTsk", TASK_DEFAULT_MSGQ_SIZE);
	g_lcd_flag_id = UOS_CreateFlag();
	g_lcd_task= UOS_CreateTask(lcd_thread_handler, NULL, TASK_WITHOUT_MSGQ, LCD_TASK_SIZE, LCD_TASK_PRIORITY, "uiLcdThd");

	if(g_lcd_sync_flag == INVALID_FLAG_ID){
		g_lcd_sync_flag = UOS_CreateFlag();
	}
	LCDLOGI("INFO: create_asrlcd_task---\n");
	return 0;
}

static int delete_asrlcd_task()
{
    uint_32   actual_flags = 0;
	u8 ret;
	unsigned int event[4];

	LCDLOGI("INFO: delete_asrlcd_task +++\n");
	if(NULL == g_lcd_task){
		LCDLOGI("%s: task has been deleted!\n", __func__);
		return 0;
	}

	g_lcd_thread_exit = 1;
	event[0] = LCD_MSG_TYPE_STOP;
	UOS_SendMsg((void*)event, g_lcd_msgq_id, UOS_SEND_EVT);

    while(1) {
		ret = UOS_WaitFlag(g_lcd_flag_id, 1, OSA_FLAG_OR_CLEAR, &actual_flags, OS_SUSPEND);
		if (ret == OS_SUCCESS && (actual_flags & 1)){
			LCDLOGI("INFO: ASRLCDD_Close: lcd thread exited!\r\n");
			break;
		}else{
			LCDLOGW("Warning: ASRLCDD_Close: Wait for lcd thread exit (0x%x)!\r\n", actual_flags);
		}
    }
#ifdef GUI_LCD_MUTEX
	UOS_FreeMutex(g_gui_lcd_mutex_id);
	g_gui_lcd_mutex_id = INVALID_MUTEX_ID;
#endif
#ifdef ASRLCD_DOUBLE_FB
	UOS_FreeMutex(g_lcd_hwfb_mutex_id);
#endif
	UOS_FreeMutex(g_lcd_mutex_id);
	UOS_FreeMessageQueue(g_lcd_msgq_id);
	UOS_DeleteFlag(g_lcd_flag_id);
	g_lcd_flag_id = INVALID_FLAG_ID;
	UOS_DeleteFlag(g_lcd_sync_flag);
	g_lcd_sync_flag = INVALID_FLAG_ID;
	UOS_DeleteTask((TASK_HANDLE *)g_lcd_task);
	g_lcd_task = NULL;

	LCDLOGI("INFO: delete_asrlcd_task ---\n");
	return 0;
}

static unsigned int switch_camformat_to_lcdformat(unsigned int camformat)
{
	unsigned int lcdformat;
	switch(camformat){
	case SCALER_INPUT_FMT_YUV422_YUYV:
		lcdformat = ASRLCDD_FORMAT_YUV422_PACKED;
		break;
	case SCALER_INPUT_FMT_YUV420_I420:
		lcdformat = ASRLCDD_FORMAT_YUV420_PLANAR;
		break;
	case SCALER_INPUT_FMT_YUV420_NV12:
		lcdformat = ASRLCDD_FORMAT_YUV420_SEMI;
		break;
	case SCALER_INPUT_FMT_ARGB32:
		lcdformat = ASRLCDD_FORMAT_RGBA888;
		break;
	default:
		LCDLOGE("ERROR: switch_camformat_to_lcdformat: Invalid format!\r\n");
		lcdformat = ASRLCDD_FORMAT_LIMIT;
	}
	return lcdformat;
}

static unsigned int switch_lcdformat_to_camformat(unsigned int lcdformat)
{
	unsigned int camformat;
	switch(lcdformat){
	case ASRLCDD_FORMAT_YUV422_PACKED:
		camformat = SCALER_INPUT_FMT_YUV422_YUYV;
		break;
	case ASRLCDD_FORMAT_YUV420_PLANAR:
		camformat = SCALER_INPUT_FMT_YUV420_I420;
		break;
	case ASRLCDD_FORMAT_YUV420_SEMI:
		camformat = SCALER_INPUT_FMT_YUV420_NV12;
		break;
	case ASRLCDD_FORMAT_RGBA888:
		camformat = SCALER_INPUT_FMT_ARGB32;
		break;
	default:
		LCDLOGE("ERROR: switch_lcdformat_to_camformat: Invalid format!\r\n");
		camformat = SCALER_INPUT_FMT_MAX;
	}
	return camformat;
}

static void switch_cambuffer_to_lcdbuffer(struct cam_offline_buf *cam_buffer,
	struct asrlcdd_camera_buffer *lcd_buffer)
{
	lcd_buffer->planes = cam_buffer->num_planes;
	lcd_buffer->addr[0] = cam_buffer->plane_addr[0];
	lcd_buffer->addr[1] = cam_buffer->plane_addr[1];
	lcd_buffer->addr[2] = cam_buffer->plane_addr[2];
	
	lcd_buffer->stride[0] = cam_buffer->plane[0].pitch;
	lcd_buffer->stride[1] = cam_buffer->plane[1].pitch;
	lcd_buffer->stride[2] = cam_buffer->plane[2].pitch;
	
	lcd_buffer->format = switch_camformat_to_lcdformat(cam_buffer->pixelformat);
	lcd_buffer->width = cam_buffer->width;
	lcd_buffer->height = cam_buffer->height;
}

static void switch_lcdbuffer_to_cambuffer(struct cam_offline_buf *cam_buffer,
	struct asrlcdd_camera_buffer *lcd_buffer)
{
	cam_buffer->num_planes = lcd_buffer->planes;
	cam_buffer->width = lcd_buffer->width;
	cam_buffer->height = lcd_buffer->height;
	cam_buffer->stride= lcd_buffer->width; /* Fix Me, suppose stride == width */
	cam_buffer->pixelformat = (SCALER_INPUT_FMT)switch_lcdformat_to_camformat(lcd_buffer->format);
	cam_buffer->plane_addr[0] = lcd_buffer->addr[0];
	if(cam_buffer->num_planes > 1)
		cam_buffer->plane_addr[1] = lcd_buffer->addr[1];
	if(cam_buffer->num_planes > 2)
		cam_buffer->plane_addr[2] = lcd_buffer->addr[2];
	LCDLOGD("DBG: (%d) buf_src.addr = 0x%x, 0x%x, 0x%x\r\n", cam_buffer->num_planes, cam_buffer->plane_addr[0],
		cam_buffer->plane_addr[1], cam_buffer->plane_addr[2]);
}

static void switch_img_buf_des_to_setting(struct asrlcdd_src_buf_des *img_buffer,
	struct s_img_layer_setting *img_setting)
{
	if(NULL != img_buffer){
		img_setting->layer_en = 1;
		img_setting->format = img_buffer->format;
		img_setting->planes = img_buffer->planes;
		img_setting->addr[0] = img_buffer->buffer[0].addr;
		img_setting->stride[0] = img_buffer->buffer[0].stride;
		if(img_setting->planes > 1){
			img_setting->addr[1] = img_buffer->buffer[1].addr;
			img_setting->stride[1] = img_buffer->buffer[1].stride;
		}
		if(img_setting->planes > 2){
			img_setting->addr[2] = img_buffer->buffer[2].addr;
			img_setting->stride[2] = img_buffer->buffer[2].stride;
		}
		img_setting->src_rect.startX = img_buffer->src_roi.startX;
		img_setting->src_rect.startY = img_buffer->src_roi.startY;
		img_setting->src_rect.width= img_buffer->src_roi.width;
		img_setting->src_rect.height= img_buffer->src_roi.height;

		img_setting->dest_rect.startX = img_buffer->dest_point.startX;
		img_setting->dest_rect.startY = img_buffer->dest_point.startY;
		img_setting->dest_rect.width= img_buffer->src_roi.width;
		img_setting->dest_rect.height= img_buffer->src_roi.height;
	} else {
		img_setting->layer_en = 0;
	}
	dump_img_setting(img_setting);
}

static void switch_osd_buf_des_to_setting(struct asrlcdd_src_buf_des *osd_buffer,
	struct s_osd_layer_setting *osd_setting)
{
	if(NULL != osd_buffer){
		osd_setting->layer_en = 1;
		osd_setting->format = osd_buffer->format;
		osd_setting->frame_buffer.pbuffer = (void*)osd_buffer->buffer[0].addr;
		osd_setting->frame_buffer.stride = osd_buffer->buffer[0].stride;
		osd_setting->frame_buffer.height = osd_buffer->src_roi.height;
		osd_setting->frame_buffer.bitdepth = get_format_bpp(osd_setting->format);
		osd_setting->src_rect.startX = osd_buffer->src_roi.startX;
		osd_setting->src_rect.startY = osd_buffer->src_roi.startY;
		osd_setting->src_rect.width= osd_buffer->src_roi.width;
		osd_setting->src_rect.height= osd_buffer->src_roi.height;
		osd_setting->dest_rect.startX = osd_buffer->dest_point.startX;
		osd_setting->dest_rect.startY = osd_buffer->dest_point.startY;
		osd_setting->dest_rect.width= osd_buffer->src_roi.width;
		osd_setting->dest_rect.height= osd_buffer->src_roi.height;
	} else {
		osd_setting->layer_en = 0;
	}
	dump_osd_setting(osd_setting);
}

static void switch_wb_buf_des_to_setting(struct asrlcdd_dest_buf_des *wb_buffer,
	struct s_wb_layer_setting *wb_setting)
{
	if(NULL != wb_buffer){
		wb_setting->layer_en = 1;
		wb_setting->format = wb_buffer->format;
		wb_setting->addr = wb_buffer->buffer.addr;
		wb_setting->stride = wb_buffer->buffer.stride;
		wb_setting->width= wb_buffer->width;
		wb_setting->height= wb_buffer->height;
	} else {
		wb_setting->layer_en = 0;
	}
	dump_wb_setting(wb_setting);
}

static int lcd_do_rotation(struct cam_offline_buf *src_buffer, struct cam_offline_buf *dst_buf, 
	unsigned int rotation, char *dest_buffer)
{
	int ret;
	int divh = 1, divs = 1;
	unsigned int lcd_format;

	LCDLOGI("DBG: lcd_do_rotation +++ (%d), (%d, %d)!\r\n", rotation, src_buffer->width, src_buffer->height);

	lcd_format = switch_camformat_to_lcdformat(src_buffer->pixelformat);
	divs = get_uv_stride_dividor(lcd_format);
	divh = get_uv_height_dividor(lcd_format);
	if(((0 == divs) || (0 == divh)) && (src_buffer->num_planes > 1)){
		LCDLOGE("ERROR: lcd_do_rotation: Invalid param (divs:%d, divh:%d)\r\n", divs, divh);
		return -1;
	}

	dst_buf->num_planes = src_buffer->num_planes;
	if((ROT_90_DEG == rotation) || (ROT_270_DEG == rotation)){
		dst_buf->width = src_buffer->height;
		dst_buf->height = src_buffer->width;
	} else {
		dst_buf->width = src_buffer->width;
		dst_buf->height = src_buffer->height;
	}
	dst_buf->stride= dst_buf->width; /* Fix Me, suppose stride == width */
	dst_buf->pixelformat = src_buffer->pixelformat;
	dst_buf->plane_addr[0] = (unsigned int)dest_buffer;
	if(dst_buf->num_planes > 1) {
		dst_buf->plane_addr[1] = (unsigned int)(dest_buffer + dst_buf->width * dst_buf->height);
		if(dst_buf->num_planes > 2){
			dst_buf->plane_addr[2] = (unsigned int)(dst_buf->plane_addr[1] + (dst_buf->width / divs)
				* (dst_buf->height / divh));
		} else {
			dst_buf->plane_addr[2] = 0;
		}
	} else {
		dst_buf->plane_addr[1] = 0;
		dst_buf->plane_addr[2] = 0;
	}
	LCDLOGD("DBG: (%d) buf_dst.addr = 0x%x, 0x%x, 0x%x\r\n", dst_buf->num_planes, dst_buf->plane_addr[0],
		dst_buf->plane_addr[1], dst_buf->plane_addr[2]);

	ret = cam_offline_rotation(src_buffer, dst_buf, (enum offline_rot)rotation, (int)MODULE_DISPLAY);
	if(0 != ret){
		LCDLOGE("ERROR:lcd_do_rotation: rotation fail!\r\n");
		return -1;
	}
	LCDLOGI("Info: lcd_do_rotation --- (%d) (%d, %d)!\r\n", rotation, dst_buf->width, dst_buf->height);
	return 0;
}

static int lcd_do_scaling(struct cam_offline_buf *src_buffer, struct cam_offline_buf *dst_buf, 
	unsigned short dst_width, unsigned short dst_height, char *dest_buffer)
{
	int ret = 0;
	int divh = 1, divs = 1;
	unsigned int lcd_format;

	LCDLOGI("Info: lcd_do_scaling +++ (w:%d, h: %d)!\r\n", dst_width, dst_height);

	lcd_format = switch_camformat_to_lcdformat(src_buffer->pixelformat);
	divs = get_uv_stride_dividor(lcd_format);
	divh = get_uv_height_dividor(lcd_format);
	if(((0 == divs) || (0 == divh)) && (src_buffer->num_planes > 1)){
		LCDLOGE("ERROR: lcd_do_scaling: Invalid param (divs:%d, divh:%d)\r\n", divs, divh);
		return -1;
	}

	dst_buf->num_planes = src_buffer->num_planes;
	dst_buf->width = dst_width;
	dst_buf->height = dst_height;
	dst_buf->stride= dst_buf->width; /* Fix Me, suppose stride == width */
	dst_buf->pixelformat = src_buffer->pixelformat;
	dst_buf->plane_addr[0] = (unsigned int)dest_buffer;
	if(dst_buf->num_planes > 1) {
		dst_buf->plane_addr[1] = (unsigned int)(dst_buf->plane_addr[0] + dst_buf->width * dst_buf->height);
		if(dst_buf->num_planes > 2){
			dst_buf->plane_addr[2] = (unsigned int)(dst_buf->plane_addr[1] + (dst_buf->width / divs)
				* (dst_buf->height / divh));
		} else {
			dst_buf->plane_addr[2] = 0;
		}
	} else {
		dst_buf->plane_addr[1] = 0;
		dst_buf->plane_addr[2] = 0;
	}
	LCDLOGD("DBG: (%d) buf_dst.addr = 0x%x, 0x%x, 0x%x\r\n", dst_buf->num_planes, dst_buf->plane_addr[0],
		dst_buf->plane_addr[1], dst_buf->plane_addr[2]);
	
	ret = cam_offline_scaler_zoom(src_buffer, dst_buf, 0x100, MODULE_DISPLAY);
	if(0 != ret){
		LCDLOGE("ERROR:lcd_do_scaling: scaling fail!\r\n");
		return -1;
	}
	LCDLOGI("Info: lcd_do_scaling --- (w:%d, h: %d)!\r\n", dst_width, dst_height);
	return 0;
}

static int lcd_do_memcpy(struct asrlcdd_camera_buffer *psrc_buffer, 
	struct asrlcdd_camera_buffer *pdst_buffer, char *dest_buffer)
{
	char *addr = NULL;
	unsigned int size = 0, size1 = 0;
	int divh = 1, divs = 1;

	LCDLOGI("Info: lcd_do_memcpy +++!\r\n");

	divs = get_uv_stride_dividor(psrc_buffer->format);
	divh = get_uv_height_dividor(psrc_buffer->format);
	if(((0 == divs) || (0 == divh)) && (psrc_buffer->planes> 1)){
		LCDLOGE("ERROR: lcd_do_memcpy: Invalid param (divs:%d, divh:%d)\r\n", divs, divh);
		return -1;
	}

	addr = dest_buffer;
	size = psrc_buffer->height * psrc_buffer->stride[0];
	size1 = LCD_ROUND_UP(size,32);
	CacheInvalidateMemory((void*)psrc_buffer->addr[0], size1);
	memcpy((void*)addr, (void*)(psrc_buffer->addr[0]), size);
	CacheCleanMemory((void*)addr, size1);
	LCDLOGD("DBG: copy buffer from	0x%x to 0x%x\r\n", (int)psrc_buffer->addr[0], (int)addr);
	pdst_buffer->addr[0] = (unsigned int)addr;
	pdst_buffer->stride[0] = psrc_buffer->stride[0];

	if(psrc_buffer->planes > 1){
		addr += size;
		size = (psrc_buffer->height / divh) * psrc_buffer->stride[1];
		size1 = LCD_ROUND_UP(size,32);
		CacheInvalidateMemory((void*)psrc_buffer->addr[1], size1);
		memcpy((void*)addr, (void*)(psrc_buffer->addr[1]), size);
		CacheCleanMemory((void*)addr, size1);
		LCDLOGD("DBG: copy buffer from	0x%x to 0x%x\r\n", (int)psrc_buffer->addr[1], (int)addr);
		pdst_buffer->addr[1] = (unsigned int)addr;
		pdst_buffer->stride[1] = psrc_buffer->stride[1];

		if(psrc_buffer->planes > 2){
			addr += size;
			size = (psrc_buffer->height / divh) * psrc_buffer->stride[2];
			size1 = LCD_ROUND_UP(size,32);
			CacheInvalidateMemory((void*)psrc_buffer->addr[2], size1);
			memcpy((void*)addr, (void*)(psrc_buffer->addr[2]), size);
			CacheCleanMemory((void*)addr, size);
			LCDLOGD("DBG: copy buffer from	0x%x to 0x%x\r\n", (int)psrc_buffer->addr[2], (int)addr);
			pdst_buffer->addr[2] = (unsigned int)addr;
			pdst_buffer->stride[2] = psrc_buffer->stride[2];
		} else {
			pdst_buffer->addr[2] = 0;
			pdst_buffer->stride[2] = 0;
		}
	} else {
		pdst_buffer->addr[1] = 0;
		pdst_buffer->stride[1] = 0;
		pdst_buffer->addr[2] = 0;
		pdst_buffer->stride[2] = 0;
	}
	pdst_buffer->format = psrc_buffer->format;
	pdst_buffer->height = psrc_buffer->height;
	pdst_buffer->width = psrc_buffer->width;
	pdst_buffer->planes = psrc_buffer->planes;
	return 0;
}

static int lcd_do_rotation_scaling(struct asrlcdd_camera_buffer *psrc_buffer, 
	struct asrlcdd_camera_buffer *pdst_buffer, unsigned int rotation, unsigned short dst_width,
	unsigned short dst_height)
{
	char* dest_buffer = NULL;
	char* temp_buffer = NULL;
	int ret = -1;
	int count = 0;
	struct cam_offline_buf buf_src;
	struct cam_offline_buf buf_dst;
	struct cam_offline_buf buf_temp;
	unsigned int s_mode = 0;
	unsigned int temp_width, temp_height;
	
	LCDLOGI("Info: lcd_do_rotation_scaling +++ r(%d), s(%d, %d), d(%d, %d)!\r\n", rotation, 
		psrc_buffer->width, psrc_buffer->height, dst_width, dst_height);

	ret = dequeue_ROT_BQs((void**)&dest_buffer);
	count = 0;
	while(0 != ret){
		count++;
		if(count >= 100){
			LCDLOGE("ERROR: lcd_do_rotation_scaling: can't get rot bqs!\r\n");
			return -1;
		}				
		UOS_Sleep(5);
		ret = dequeue_ROT_BQs((void**)&dest_buffer);
	}
	LCDLOGD("DBG: Got dequeue buffer 0x%x\r\n", (int)dest_buffer);

	if((ROT_90_DEG == rotation) || (ROT_270_DEG == rotation)){
		temp_width = psrc_buffer->height;
		temp_height = psrc_buffer->width;
	} else {
		temp_width = psrc_buffer->width;
		temp_height = psrc_buffer->height;
	}

	if((temp_width == dst_width) && (temp_height == dst_height)){
		s_mode = 0; /*no scaling*/
	} else if(temp_width * temp_height > dst_width * dst_height){
		s_mode = 1; /*scaling down*/
	} else {
		s_mode = 2; /*scaling up*/
	}

	if((ROT_0_DEG != rotation) && (0 != s_mode)){
		ret = dequeue_ROT_BQs((void**)&temp_buffer);
		count = 0;
		while(0 != ret){
			count++;
			if(count >= 100){
				LCDLOGE("ERROR: lcd_do_rotation_scaling: can't get rot bqs!\r\n");
				ret = enqueue_ROT_BQs(dest_buffer);
				return -1;
			}				
			UOS_Sleep(5);
			ret = dequeue_ROT_BQs((void**)&temp_buffer);
		}
		LCDLOGD("DBG: Got dequeue temp buffer 0x%x\r\n", (int)temp_buffer);
	}

	switch_lcdbuffer_to_cambuffer(&buf_src, psrc_buffer);

	if(ROT_0_DEG == rotation){ /*no rotation*/
		if(0 == s_mode){ /*no rotation, no scaling, memory copy only*/
			LCDLOGD("DBG: no rotation, no scaling, only copy\r\n");
			ret = lcd_do_memcpy(psrc_buffer, pdst_buffer, dest_buffer);
			if(0 != ret){
				LCDLOGE("ERROR:lcd_do_rotation_scaling: memcpy fail!\r\n");
				ret = enqueue_ROT_BQs(dest_buffer);
				return -1;
			}
		} else { /*no rotation, scaling only*/
			LCDLOGD("DBG: no rotation, scaling only\r\n");
			ret = lcd_do_scaling(&buf_src, &buf_dst, dst_width, dst_height, dest_buffer);
			if(0 != ret){
				LCDLOGE("ERROR:lcd_do_rotation_scaling: scaling fail!\r\n");
				ret = enqueue_ROT_BQs(dest_buffer);
				return -1;
			}
			switch_cambuffer_to_lcdbuffer(&buf_dst, pdst_buffer);
		}
	} else { /*need rotation*/
		//LCDLOGD("DBG: s_mode = %d\r\n", s_mode);
		if(0 == s_mode){/*rotation only + no scaling*/
			LCDLOGD("DBG: no scaling, rotation only\r\n");
			ret = lcd_do_rotation(&buf_src, &buf_dst, rotation, dest_buffer);
			if(0 != ret){
				LCDLOGE("ERROR:lcd_do_rotation_scaling: rotation fail!\r\n");
				ret = enqueue_ROT_BQs(dest_buffer);
				return -1;
			}
		} else if(2 == s_mode){
			/*rotation + scaling up*/
			LCDLOGD("DBG: rotation, scaling up\r\n");
			ret = lcd_do_rotation(&buf_src, &buf_temp, rotation, temp_buffer);
			if(0 != ret){
				LCDLOGE("ERROR:lcd_do_rotation_scaling: rotation fail!\r\n");
				ret = enqueue_ROT_BQs(dest_buffer);
				ret = enqueue_ROT_BQs(temp_buffer);
				return -1;
			}

			ret = lcd_do_scaling(&buf_temp, &buf_dst, dst_width, dst_height, dest_buffer);
			if(0 != ret){
				LCDLOGE("ERROR:lcd_do_rotation_scaling: scaling fail!\r\n");
				ret = enqueue_ROT_BQs(dest_buffer);
				ret = enqueue_ROT_BQs(temp_buffer);
				return -1;
			}
			ret = enqueue_ROT_BQs(temp_buffer);
		}else {
			/*rotation + scaling down*/
			LCDLOGD("DBG: scaling down, rotation\r\n");
			if((ROT_90_DEG == rotation) || (ROT_270_DEG == rotation))
				ret = lcd_do_scaling(&buf_src, &buf_temp, dst_height, dst_width, temp_buffer);
			else /*ROT_180_DEG*/
				ret = lcd_do_scaling(&buf_src, &buf_temp, dst_width, dst_height, temp_buffer);
			if(0 != ret){
				LCDLOGE("ERROR:lcd_do_rotation_scaling: rotation fail!\r\n");
				ret = enqueue_ROT_BQs(dest_buffer);
				ret = enqueue_ROT_BQs(temp_buffer);
				return -1;
			}

			ret = lcd_do_rotation(&buf_temp, &buf_dst, rotation, dest_buffer);
			if(0 != ret){
				LCDLOGE("ERROR:lcd_do_rotation_scaling: rotation fail!\r\n");
				ret = enqueue_ROT_BQs(dest_buffer);
				ret = enqueue_ROT_BQs(temp_buffer);
				return -1;
			}
			ret = enqueue_ROT_BQs(temp_buffer);
		}
		switch_cambuffer_to_lcdbuffer(&buf_dst, pdst_buffer);
	}
	g_last_ROT_buffer = dest_buffer;
	//dump_camerabuffer(pdst_buffer);

	LCDLOGI("Info: lcd_do_rotation_scaling ---!\r\n");
	return 0;
}

static int update_camera_rotation(int org_rot, int add_rot)
{
	int new_rot = ROT_0_DEG;
	switch(org_rot){
	case ROT_0_DEG:
		if(add_rot == ROT_90_DEG)
			new_rot = ROT_90_DEG;
		else if(add_rot == ROT_180_DEG)
			new_rot = ROT_180_DEG;
		else if(add_rot == ROT_270_DEG)
			new_rot = ROT_270_DEG;
		break;
	case ROT_90_DEG:
		if(add_rot == ROT_90_DEG)
			new_rot = ROT_0_DEG;
		else if(add_rot == ROT_180_DEG)
			new_rot = ROT_270_DEG;
		else if(add_rot == ROT_270_DEG)
			new_rot = ROT_180_DEG;
		break;
	case ROT_180_DEG:
		if(add_rot == ROT_90_DEG)
			new_rot = ROT_270_DEG;
		else if(add_rot == ROT_180_DEG)
			new_rot = ROT_0_DEG;
		else if(add_rot == ROT_270_DEG)
			new_rot = ROT_90_DEG;
		break;
	case ROT_270_DEG:
		if(add_rot == ROT_90_DEG)
			new_rot = ROT_180_DEG;
		else if(add_rot == ROT_180_DEG)
			new_rot = ROT_90_DEG;
		else if(add_rot == ROT_270_DEG)
			new_rot = ROT_0_DEG;
		break;
	}
	return new_rot;
}

static int lcd_enter_d2_cb(void)
{
//	LCDLOGI("Info: lcd_enter_d2_cb +++!\r\n");
#if 0
	if(g_lcd_status != LCD_STATUS_SLEEP){
		LCDLOGE("Err: lcd_enter_d2_cb: Invalid lcd status(%d)!\r\n", g_lcd_status);
		return -1;
	}

	if(g_lcd_status != LCD_STATUS_POWEROFF){
//#ifdef USE_MEDIACLK_INTERFACE
//		lcd_poweroff();
//#else
		lcd_ass_poweroff();
//#endif
		g_lcd_status = LCD_STATUS_POWEROFF; 	
	}
#endif	
//	LCDLOGI("Info: lcd_enter_d2_cb ---!\r\n");
	return 0;
}

static int lcd_enter_c1_cb(void)
{
//	LCDLOGI("Info: lcd_enter_c1_cb +++!\r\n");

	if(g_flip_pending == 1)
		return 0;
	else
		return 1;
//	LCDLOGI("Info: lcd_enter_c1_cb ---!\r\n");
}


static int lcd_exit_d2_cb(BOOL ExitFromD2)
{
//	LCDLOGI("Info: lcd_exit_d2_cb +++!\r\n");
#if 0
	if(g_lcd_status != LCD_STATUS_POWEROFF){
		LCDLOGE("Err: lcd_exit_d2_cb: Invalid lcd status(%d)!\r\n", g_lcd_status);
		return -1;	
	}
	if(NULL == g_plcd || NULL == g_ppanel){
		LCDLOGE("Err: lcd_exit_d2_cb: Invalid g_ppanel (0x%x), g_plcd(0x%x)!\r\n", g_ppanel, g_plcd);
		return -1;	
	}

//#ifdef USE_MEDIACLK_INTERFACE
//	lcd_poweron();
//#else
	lcd_ass_poweron();
//#endif
	
	lcd_reset(g_plcd);
#ifdef LCD_INTERRUPT_MODE
	panel_reset(g_ppanel);
#else
	panel_reset(g_ppanel);
#endif
	g_lcd_status = LCD_STATUS_SLEEP;
#else
	if(ExitFromD2)
		g_lcd_status = LCD_STATUS_POWEROFF;
#endif
//	LCDLOGI("Info: lcd_exit_d2_cb ---!\r\n");
	return 0;
}



static int lcd_exit_c1_cb(void)
{
//	LCDLOGI("Info: lcd_exit_c1_cb !\r\n");

	return 0;
}

int ASRLCDD_Open(int panel_is_ready)
{
	struct panel_spec *panel = NULL;
#ifdef LCD_DUAL_PANEL_SUPPORT
	struct panel_spec *sub_panel = NULL;
#endif
	struct s_lcd_ctx *lcd = NULL;
	int ret = -1;

	LCDLOGE("Info: ASRLCDD_Open (ready: %d) +++\r\n", panel_is_ready);
	uiSetSuspendFlag(LP_ID_LCD, 0);

#if 0
	if (g_lcd_Blit16_timerid == INVALID_TIMER_ID)
		g_lcd_Blit16_timerid = UOS_get_FunctionTimer();
#endif

	ret = create_asrlcd_task();
	if(0 != ret){
		LCDLOGW("Warning: ASRLCDD_Open: Create task fail\r\n");
	}

	if((g_lcd_status != LCD_STATUS_POWEROFF) && (g_lcd_status != LCD_STATUS_HWON)){
		LCDLOGW("Info: ASRLCDD_Open: asrlcd has been opened!\r\n");
		return 0;
	}

#ifdef USE_MEDIACLK_INTERFACE
	lcd_poweron();
#else
	lcd_ass_poweron();
#endif

#ifdef LCD_DUAL_PANEL_SUPPORT
	g_lcd_status = LCD_STATUS_SLEEP | LCD_STATUS_SUB_SLEEP;
#else
	g_lcd_status = LCD_STATUS_SLEEP;
#endif

#ifdef LCD_INTERRUPT_MODE
	lcd = (struct s_lcd_ctx*)lcd_init(LCD_WORK_MODE_INTERRUPT);
#else
	lcd = (struct s_lcd_ctx*)lcd_init(LCD_WORK_MODE_POLLING);
#endif
	if(NULL == lcd){
		LCDLOGE("ERR: ASRLCDD_Open: lcd_init fail\r\n");
		ASRLCDD_Close();
		return -1;
	}
	g_plcd = lcd;

	if(NULL == g_ppanel){
#ifdef LCD_INTERRUPT_MODE
		panel = find_panel(LCD_SCLK_FREQ, LCD_WORK_MODE_INTERRUPT, panel_is_ready, LCD_ID_MAIN);
#else
		panel = find_panel(LCD_SCLK_FREQ, LCD_WORK_MODE_ASS_POLLING, panel_is_ready, LCD_ID_MAIN);
#endif
		if(NULL == panel){
			LCDLOGE("ERR: ASRLCDD_Open: find_panel fail\r\n");
			ASRLCDD_Close();
			return -1;
		}
		g_ppanel = panel;

#ifdef LCD_DUAL_PANEL_SUPPORT
#ifdef LCD_INTERRUPT_MODE
		sub_panel = find_panel(LCD_SCLK_FREQ, LCD_WORK_MODE_INTERRUPT, panel_is_ready, LCD_ID_SUB);
#else
		sub_panel = find_panel(LCD_SCLK_FREQ, LCD_WORK_MODE_ASS_POLLING, panel_is_ready, LCD_ID_SUB);
#endif
		if(NULL == sub_panel){
			LCDLOGE("ERR: ASRLCDD_Open: find_panel fail\r\n");
			ASRLCDD_Close();
			return -1;
		}
		g_psubpanel = sub_panel;
#endif
	} else {
		panel = g_ppanel;
#ifdef LCD_DUAL_PANEL_SUPPORT
		sub_panel = g_psubpanel;
#endif
	}

	#ifdef LCD_INTERRUPT_MODE
		ret = panel_init(panel, LCD_SCLK_FREQ, LCD_WORK_MODE_INTERRUPT,panel_is_ready, LCD_ID_MAIN);
	#else
		ret = panel_init(panel, LCD_SCLK_FREQ, LCD_WORK_MODE_POLLING,panel_is_ready, LCD_ID_MAIN);
	#endif
	if(0 != ret){
		LCDLOGE("ERR: ASRLCDD_Open: panel_init fail\r\n");
		ASRLCDD_Close();
		return -1;
	}

#ifdef LCD_DUAL_PANEL_SUPPORT
#ifdef LCD_INTERRUPT_MODE
	ret = panel_init(sub_panel, LCD_SCLK_FREQ, LCD_WORK_MODE_INTERRUPT,panel_is_ready, LCD_ID_SUB);
#else
	ret = panel_init(sub_panel, LCD_SCLK_FREQ, LCD_WORK_MODE_POLLING,panel_is_ready, LCD_ID_SUB);
#endif
	if(0 != ret){
		LCDLOGE("ERR: ASRLCDD_Open: panel_init fail\r\n");
		ASRLCDD_Close();
		return -1;
	}
#endif

	lcd->bg_color = 0xFF;
	lcd->alpha_mode = LCD_ALPHA_MODE_NORMAL;
	lcd->gamma_en = 0;
	lcd->wb_setting.wb_en = 0;

#if 0
	ret = lcd_output_setting(lcd, panel);
	if(0 != ret){
		LCDLOGE("ERR: lcd_output_setting fail\r\n");
		ASRLCDD_Close();
		return -1;
	}
#endif
	if((panel->type == LCD_TYPE_FSTN) && (NULL == g_fstn_data)){
		g_fstn_data = (unsigned char*)UOS_MALLOC(panel->height / 8 * panel->width);
		if(NULL == g_fstn_data){
			LCDLOGE("ERR: ASRLCDD_Open: Can't malloc fstn memory\r\n");
			ASRLCDD_Close();
			return -1;
		} else {
			LCDLOGD("DBG: ASRLCDD_Open: malloc fstn memory OK! (0x%x)\r\n", g_fstn_data);
		}
	}

	uiD2CallbackRegister(LP_ID_LCD, lcd_enter_d2_cb, lcd_exit_d2_cb);
	uiC1CallbackRegister(LP_ID_LCD,lcd_enter_c1_cb,lcd_exit_c1_cb);

	g_esd_ctx = lcd_esd_init(lcd, panel);
	if(NULL == g_esd_ctx){
		LCDLOGE("ERROR: lcd_esd_init fail, can't do esd check!\r\n");
	} else {
		ret = lcd_esd_start(g_esd_ctx);
		if(0 != ret){
			LCDLOGE("ERROR: lcd_esd_start fail, can't do esd check!\r\n");
		}
	}

	g_ui_info.width = panel->width;
	g_ui_info.height = panel->height;

	LCDLOGI("Info: ASRLCDD_Open ---\r\n");

	/*
	 * scaling and rotation supplied by camera offline module
	 * available before open camera
	 */
	cam_hw_version_init();
	cam_offline_init();
	g_cam_init = 1;

	return 0;
}

int ASRLCDD_Close(void)
{
	int i = 0;

	LCDLOGE("INFO: ASRLCDD_Close +++!\r\n");

	uiSetSuspendFlag(LP_ID_LCD, 1);
	uiD2CallbackunRegister(LP_ID_LCD);

	if(NULL != g_esd_ctx){
		lcd_esd_stop(g_esd_ctx);
		lcd_esd_uninit(g_esd_ctx);
	}

	delete_asrlcd_task();

	if(NULL != g_plcd){
		lcd_uninit(g_plcd);
		g_plcd = NULL;
	}
	if(NULL != g_ppanel){
		panel_uninit(g_ppanel, LCD_ID_MAIN);
		g_ppanel = NULL;
	}
#ifdef LCD_DUAL_PANEL_SUPPORT
	if(NULL != g_psubpanel){
		panel_uninit(g_psubpanel, LCD_ID_SUB);
		g_psubpanel = NULL;
	}
#endif
	if(g_lcd_status != LCD_STATUS_POWEROFF){
#ifdef USE_MEDIACLK_INTERFACE
		lcd_poweroff();
#else
		lcd_ass_poweroff();
#endif

		g_lcd_status = LCD_STATUS_POWEROFF;		
	}
	for(i=0;i<ASRLCDD_CB_TYPE_MAX;i++){
		g_lcd_cbs[i] = NULL;
		g_lcd_cb_params[i] = NULL;
	}

	if(NULL != g_fstn_data){
		UOS_FREE(g_fstn_data);
		g_fstn_data = NULL;
	}

	LCDLOGI("INFO: ASRLCDD_Close ---\n");

	/* 
	 * scaling and rotation supplied by camera offline module 
	 * available before open camera
	 */
	if(g_cam_init == 1){
		cam_offline_deinit();
		g_cam_init = 0;
	}

	return 0;
}

int ASRLCDD_Sleep(void)
{
	int ret  = -1;
	struct panel_spec *panel = g_ppanel;
	int timeout = 1000;

	LCDLOGE("Info: ASRLCDD_Sleep ++\r\n");

	if(NULL == panel){
		LCDLOGE("ERR: ASRLCDD_Sleep: panel has not been inited!\r\n");
		return -1;
	}

	if(LCD_STATUS_SLEEP == (g_lcd_status & LCD_STATUS_SLEEP)){
		LCDLOGE("ERR: ASRLCDD_Sleep: panel has been in sleep!\r\n");
		return 0;
	} else if(g_lcd_status == LCD_STATUS_POWEROFF){
		LCDLOGE("ERR: ASRLCDD_Sleep: Invalid lcd status!\r\n");
		return -1;
	}

	if(NULL != g_esd_ctx){
		lcd_esd_stop(g_esd_ctx);
	}

	g_lcd_status &= (LCD_STATUS_SUB_POWERON | LCD_STATUS_SUB_SLEEP);
	g_lcd_status |= LCD_STATUS_SLEEP;

	while((g_flip_pending == 1) && (timeout > 0)){
		UOS_Sleep(MS_TO_TICKS(2));
		timeout--;
	}

	ret = panel_sleep(panel);
	if(-1 == ret){
		LCDLOGE("ERR: ASRLCDD_Sleep: panel_sleep fail\r\n");
		return ret;
	}
	if(g_lcd_status == LCD_STATUS_SUB_SLEEP | LCD_STATUS_SLEEP)
		uiSetSuspendFlag(LP_ID_LCD, 1);
	return 0;
}

int ASRLCDD_WakeUp(void)
{
	int ret  = -1;
	struct panel_spec *panel = g_ppanel;

	LCDLOGE("Info: ASRLCDD_WakeUp (0x%x) ++\r\n", g_lcd_status);

	if(NULL == panel){
		LCDLOGE("ERR: ASRLCDD_WakeUp: panel has not been inited!\r\n");
		return -1;
	}

	if(g_lcd_status != LCD_STATUS_ASSERTON)
		uiSetSuspendFlag(LP_ID_LCD, 0);

	if(LCD_STATUS_POWERON == ( g_lcd_status & LCD_STATUS_POWERON)){
		LCDLOGW("Warning: ASRLCDD_WakeUp: panel has been in power on!\r\n");
		return 0;
	} else if(g_lcd_status == LCD_STATUS_POWEROFF){
		LCDLOGI("INFO: ASRLCDD_WakeUp: Wakeup from D2!\r\n");

	#ifdef USE_MEDIACLK_INTERFACE
		lcd_poweron();
	#else
		lcd_ass_poweron();
	#endif
		
		lcd_reset(g_plcd);
#ifdef LCD_INTERRUPT_MODE
		panel_reset(g_ppanel);
#else
		panel_reset(g_ppanel);
#endif
		g_lcd_status &= (LCD_STATUS_SUB_POWERON | LCD_STATUS_SUB_SLEEP);
		g_lcd_status |= LCD_STATUS_SLEEP;
	}

	ret = panel_wakeup(panel);
	if(-1 == ret){
		LCDLOGE("ERR: ASRLCDD_WakeUp: panel_wakeup fail\r\n");
		return ret;
	}
	g_lcd_status &= (LCD_STATUS_SUB_POWERON | LCD_STATUS_SUB_SLEEP);
	g_lcd_status |= LCD_STATUS_POWERON;

	if(NULL != g_esd_ctx){
		lcd_esd_start(g_esd_ctx);
	}

	return 0;
}

int ASRLCDD_Blit16(struct asrlcdd_framebuffer_window *frame_window,
	uint16_t startX, uint16_t startY)
{
	struct asrlcdd_framebuffer_info *src_frame_buffer, *dest_frame_buffer;
	struct asrlcdd_roi_info temp_roi1, temp_roi2, temp_roi3;
	unsigned int event[4];
#ifdef ASRLCD_DOUBLE_FB
	int i;
	uint32_t addr_src;
	uint32_t addr_dest;
	unsigned int size = 0;
#endif

	LCDLOGD("Info: ASRLCDD_Blit16 +++ (%d, %d, %d, %d) (%d, %d)\r\n", frame_window->roi_info.startX, 
		frame_window->roi_info.startY, frame_window->roi_info.width, frame_window->roi_info.height, startX, startY);

	if(NULL == frame_window){
		LCDLOGE("ERR: ASRLCDD_Blit16: Invalid param\r\n");
		return -1;
	}

	if(LCD_STATUS_POWERON != (g_lcd_status & LCD_STATUS_POWERON)){
		LCDLOGE("ERR: ASRLCDD_Blit16: Invalid lcd status!\r\n");
		return -1;
	}
//	dump_framewindow(frame_window);

	UOS_TakeMutex(g_lcd_mutex_id);
//	LCDLOGD("DBG: ASRLCDD_Blit16 get mutex\r\n");

	src_frame_buffer = &(frame_window->frame_info);
	dest_frame_buffer =  &(g_lcd_osd_setting.frame_buffer);

	if(g_lcd_osd_setting.dirty){
		if((dest_frame_buffer->bitdepth != src_frame_buffer->bitdepth) ||
			(dest_frame_buffer->stride != src_frame_buffer->stride)){
			LCDLOGE("ERR: ASRLCDD_Blit16 Not the same frame setting!!!!!\r\n");
			UOS_ReleaseMutex(g_lcd_mutex_id);
			return -1;
		}
	}

#ifdef ASRLCD_DOUBLE_FB
	for(i=0; i<frame_window->roi_info.height;i++){
		addr_src = (uint32_t)src_frame_buffer->pbuffer +
			(frame_window->roi_info.startY + i) * src_frame_buffer->stride +
			frame_window->roi_info.startX * src_frame_buffer->bitdepth/8;
		addr_dest = (uint32_t)&g_asrhw_framebuffer[0] +
			(frame_window->roi_info.startY + i) * dest_frame_buffer->stride +
			frame_window->roi_info.startX * dest_frame_buffer->bitdepth/8;
		UOS_TakeMutex(g_lcd_hwfb_mutex_id);
		memcpy((void*)addr_dest, (void*)addr_src, frame_window->roi_info.width * src_frame_buffer->bitdepth/8);
		UOS_ReleaseMutex(g_lcd_hwfb_mutex_id);
		size = frame_window->roi_info.width * src_frame_buffer->bitdepth/8;
		size = LCD_ROUND_UP(size,32);
		CacheCleanMemory((void*)addr_dest, size);
	}
#endif

	if(g_lcd_osd_setting.dirty){
		LCDLOGD("DBG: ASRLCDD_Blit16 osd layer is dirty\r\n");

		temp_roi2.startX = startX;
		temp_roi2.startY = startY;
		temp_roi2.height = frame_window->roi_info.height;
		temp_roi2.width = frame_window->roi_info.width;

		temp_roi1.startX = g_lcd_osd_setting.src_rect.startX;
		temp_roi1.startY = g_lcd_osd_setting.src_rect.startY;
		temp_roi1.width = g_lcd_osd_setting.src_rect.width;
		temp_roi1.height = g_lcd_osd_setting.src_rect.height;

		temp_roi3.startX = g_lcd_osd_setting.dest_rect.startX;
		temp_roi3.startY = g_lcd_osd_setting.dest_rect.startY;
		temp_roi3.width = g_lcd_osd_setting.dest_rect.width;
		temp_roi3.height = g_lcd_osd_setting.dest_rect.height;

		merge_roi(&temp_roi1, &frame_window->roi_info,
			&g_lcd_osd_setting.src_rect);
		merge_roi(&temp_roi2, &temp_roi3, &g_lcd_osd_setting.dest_rect);
	} else {
		LCDLOGD("DBG: ASRLCDD_Blit16 osd layer is not dirty!\r\n");
#ifdef ASRLCD_DOUBLE_FB
		dest_frame_buffer->pbuffer = &g_asrhw_framebuffer[0];
#else
		dest_frame_buffer->pbuffer = src_frame_buffer->pbuffer;
#endif
		dest_frame_buffer->bitdepth = src_frame_buffer->bitdepth;
		dest_frame_buffer->stride = src_frame_buffer->stride;
		dest_frame_buffer->height = src_frame_buffer->height;

		switch(frame_window->frame_info.bitdepth){
		case 16:
			g_lcd_osd_setting.format = LCD_FORMAT_RGB565;
			break;
		case 32:
			g_lcd_osd_setting.format = LCD_FORMAT_RGBA888;
			break;
		default:
			LCDLOGE("ERR: ASRLCDD_Blit16: Invalid bitdepth (%d)!\r\n", frame_window->frame_info.bitdepth);
			g_lcd_osd_setting.format = LCD_FORMAT_RGB565; //use RGB565 as default
		}

		g_lcd_osd_setting.src_rect.startX = frame_window->roi_info.startX;
		g_lcd_osd_setting.src_rect.startY = frame_window->roi_info.startY;
		g_lcd_osd_setting.src_rect.height = frame_window->roi_info.height;
		g_lcd_osd_setting.src_rect.width = frame_window->roi_info.width;
		g_lcd_osd_setting.dest_rect.startX = startX;
		g_lcd_osd_setting.dest_rect.startY = startY;
		g_lcd_osd_setting.dest_rect.height = frame_window->roi_info.height;
		g_lcd_osd_setting.dest_rect.width = frame_window->roi_info.width;
	}

	g_lcd_osd_setting.layer_en = 1;
	g_lcd_osd_setting.dirty = 1;

	LCDLOGI("Info: ASRLCDD_Blit16 ---(%d, %d, %d, %d), (%d, %d, %d, %d)\r\n",
		g_lcd_osd_setting.src_rect.startX, g_lcd_osd_setting.src_rect.startY,
		g_lcd_osd_setting.src_rect.width, g_lcd_osd_setting.src_rect.height,
		g_lcd_osd_setting.dest_rect.startX, g_lcd_osd_setting.dest_rect.startY,
		g_lcd_osd_setting.dest_rect.width, g_lcd_osd_setting.dest_rect.height);

#if 0
	if((0 == g_lcd_timer_started) && ((0 == g_lcd_img_setting.layer_en) ||( 1 == g_lcd_camerastatus))){
		UOS_StartFunctionTimer_single(g_lcd_Blit16_timerid, MS_TO_TICKS(30), lcd_timer_handler, NULL, "lcd_timer_blit16");
		g_lcd_timer_started = 1;
	}
#else
	if((0 == g_lcd_img_setting.layer_en) ||( 1 == g_lcd_camerastatus)){
		event[0] = LCD_MSG_TYPE_UPDATE;
		event[2] = LCD_ID_MAIN;
		UOS_SendMsg((void*)event, g_lcd_msgq_id, UOS_SEND_EVT);
	}
#endif
	UOS_ReleaseMutex(g_lcd_mutex_id);

//	dump_osd_setting(&g_lcd_osd_setting);

	return 0;
}

int ASRLCDD_Blit16_sync(struct asrlcdd_framebuffer_window *frame_window,
	uint16_t startX, uint16_t startY)
{
	struct asrlcdd_framebuffer_info *src_frame_buffer, *dest_frame_buffer;
	struct asrlcdd_roi_info temp_roi1, temp_roi2, temp_roi3;
//	struct asrlcdd_roi_info dest_roi;
#ifdef ASRLCD_DOUBLE_FB
	int i;
	uint32_t addr_src;
	uint32_t addr_dest;
	unsigned int size = 0;
#endif

	struct s_osd_layer_setting osd_setting;
	struct s_img_layer_setting img_setting;

	if(NULL == frame_window){
		LCDLOGE("ERR: ASRLCDD_Blit16_sync: Invalid param\r\n");
		return -1;
	}

	LCDLOGI("Info: ASRLCDD_Blit16_sync +++ (%d, %d, %d, %d)\r\n", frame_window->roi_info.startX, 
		frame_window->roi_info.startY, frame_window->roi_info.width, frame_window->roi_info.height);

	if(LCD_STATUS_POWERON != (g_lcd_status & LCD_STATUS_POWERON)){
		LCDLOGE("ERR: ASRLCDD_Blit16_sync: Invalid lcd status!\r\n");
		return -1;
	}

//	dump_framewindow(frame_window);

	UOS_TakeMutex(g_lcd_mutex_id);
//	LCDLOGD("DBG: ASRLCDD_Blit16 get mutex\r\n");

	src_frame_buffer = &(frame_window->frame_info);
	dest_frame_buffer =  &(g_lcd_osd_setting.frame_buffer);         //  yuhuizhang dest 

	if(g_lcd_osd_setting.dirty){
		if((dest_frame_buffer->bitdepth != src_frame_buffer->bitdepth) ||
			(dest_frame_buffer->stride != src_frame_buffer->stride)){
			LCDLOGE("ERR: ASRLCDD_Blit16_sync Not the same frame setting!!!!!\r\n");
			UOS_ReleaseMutex(g_lcd_mutex_id);
			return -1;
		}
	}

	if(frame_window->frame_info.pbuffer == NULL){ 
		if(g_lcd_osd_setting.dirty  || g_lcd_img_setting.dirty){
			//g_lcd_osd_setting.layer_en = 1;
			LCDLOGI("Info: ASRLCDD_Blit16_sync ---(%d, %d, %d, %d), (%d, %d, %d, %d)\r\n",
				g_lcd_osd_setting.src_rect.startX, g_lcd_osd_setting.src_rect.startY,
				g_lcd_osd_setting.src_rect.width, g_lcd_osd_setting.src_rect.height,
				g_lcd_osd_setting.dest_rect.startX, g_lcd_osd_setting.dest_rect.startY,
				g_lcd_osd_setting.dest_rect.width, g_lcd_osd_setting.dest_rect.height);

			memcpy(&osd_setting, &g_lcd_osd_setting, sizeof(struct s_osd_layer_setting));
			memcpy(&img_setting, &g_lcd_img_setting, sizeof(struct s_img_layer_setting));
			g_lcd_osd_setting.dirty = 0;
			g_lcd_img_setting.dirty = 0;
			//osd_setting.layer_en = 1;
			//img_setting.layer_en = 0;
			UOS_ReleaseMutex(g_lcd_mutex_id);

#if 0
			if((1 == img_setting.layer_en) && (1 == osd_setting.layer_en)){
				merge_roi(&(img_setting.dest_rect), &(osd_setting.dest_rect), &dest_roi);
				refresh_lcd(&img_setting, &osd_setting, &dest_roi);
			} else if(1 == img_setting.layer_en){
				refresh_lcd(&img_setting, &osd_setting, &img_setting.dest_rect);
			} else if(1 == osd_setting.layer_en){
				refresh_lcd(&img_setting, &osd_setting, &osd_setting.dest_rect);
			} else {
				LCDLOGE("ERROR: lcd_thread_handler: No layer enabled!!\n");
			}
#else
			refresh_lcd(&img_setting, &osd_setting, LCD_ID_MAIN);
#endif
			return 0;
		} 
	}

#ifdef ASRLCD_DOUBLE_FB
	for(i=0; i<frame_window->roi_info.height;i++){
		addr_src = (uint32_t)src_frame_buffer->pbuffer +
			(frame_window->roi_info.startY + i) * src_frame_buffer->stride +
			frame_window->roi_info.startX * src_frame_buffer->bitdepth/8;
		addr_dest = (uint32_t)&g_asrhw_framebuffer[0] +
			(frame_window->roi_info.startY + i) * dest_frame_buffer->stride +
			frame_window->roi_info.startX * dest_frame_buffer->bitdepth/8;
		UOS_TakeMutex(g_lcd_hwfb_mutex_id)
		memcpy((void*)addr_dest, (void*)addr_src, frame_window->roi_info.width * src_frame_buffer->bitdepth/8);
		UOS_ReleaseMutex(g_lcd_hwfb_mutex_id);
		size = frame_window->roi_info.width * src_frame_buffer->bitdepth/8;
		size = LCD_ROUND_UP(size,32);
		CacheCleanMemory((void*)addr_dest, size);
	}
#endif

	if(g_lcd_osd_setting.dirty){
		LCDLOGD("DBG: ASRLCDD_Blit16_sync osd layer is dirty\r\n");

		temp_roi2.startX = startX;
		temp_roi2.startY = startY;
		temp_roi2.height = frame_window->roi_info.height;
		temp_roi2.width = frame_window->roi_info.width;

		temp_roi1.startX = g_lcd_osd_setting.src_rect.startX;
		temp_roi1.startY = g_lcd_osd_setting.src_rect.startY;
		temp_roi1.width = g_lcd_osd_setting.src_rect.width;
		temp_roi1.height = g_lcd_osd_setting.src_rect.height;

		temp_roi3.startX = g_lcd_osd_setting.dest_rect.startX;
		temp_roi3.startY = g_lcd_osd_setting.dest_rect.startY;
		temp_roi3.width = g_lcd_osd_setting.dest_rect.width;
		temp_roi3.height = g_lcd_osd_setting.dest_rect.height;

		merge_roi(&temp_roi1, &frame_window->roi_info,
			&g_lcd_osd_setting.src_rect);
		merge_roi(&temp_roi2, &temp_roi3, &g_lcd_osd_setting.dest_rect);
	} else {
		LCDLOGD("DBG: ASRLCDD_Blit16_sync osd layer is not dirty!\r\n");
#ifdef ASRLCD_DOUBLE_FB
		dest_frame_buffer->pbuffer = &g_asrhw_framebuffer[0];
#else
		dest_frame_buffer->pbuffer = src_frame_buffer->pbuffer;
#endif
		dest_frame_buffer->bitdepth = src_frame_buffer->bitdepth;
		dest_frame_buffer->stride = src_frame_buffer->stride;
		dest_frame_buffer->height = src_frame_buffer->height;

		switch(frame_window->frame_info.bitdepth){
		case 16:
			g_lcd_osd_setting.format = LCD_FORMAT_RGB565;
			break;
		case 32:
			g_lcd_osd_setting.format = LCD_FORMAT_RGBA888;
			break;
		default:
			LCDLOGE("ERR: ASRLCDD_Blit16_sync: Invalid bitdepth (%d)!\r\n", frame_window->frame_info.bitdepth);
			g_lcd_osd_setting.format = LCD_FORMAT_RGB565; //use RGB565 as default
		}

		g_lcd_osd_setting.src_rect.startX = frame_window->roi_info.startX;
		g_lcd_osd_setting.src_rect.startY = frame_window->roi_info.startY;
		g_lcd_osd_setting.src_rect.height = frame_window->roi_info.height;
		g_lcd_osd_setting.src_rect.width = frame_window->roi_info.width;
		g_lcd_osd_setting.dest_rect.startX = startX;
		g_lcd_osd_setting.dest_rect.startY = startY;
		g_lcd_osd_setting.dest_rect.height = frame_window->roi_info.height;
		g_lcd_osd_setting.dest_rect.width = frame_window->roi_info.width;
	}

	//g_lcd_osd_setting.layer_en = 1;
	g_lcd_osd_setting.dirty = 1;

	LCDLOGI("Info: ASRLCDD_Blit16_sync ---(%d, %d, %d, %d), (%d, %d, %d, %d)\r\n",
		g_lcd_osd_setting.src_rect.startX, g_lcd_osd_setting.src_rect.startY,
		g_lcd_osd_setting.src_rect.width, g_lcd_osd_setting.src_rect.height,
		g_lcd_osd_setting.dest_rect.startX, g_lcd_osd_setting.dest_rect.startY,
		g_lcd_osd_setting.dest_rect.width, g_lcd_osd_setting.dest_rect.height);
	
	memcpy(&osd_setting, &g_lcd_osd_setting, sizeof(struct s_osd_layer_setting));
	g_lcd_osd_setting.dirty = 0;
	osd_setting.layer_en = 1;  // yuhuizhang
	img_setting.layer_en = 0;  // yuhuizhang
	UOS_ReleaseMutex(g_lcd_mutex_id);

//	refresh_lcd(&img_setting, &osd_setting, &g_lcd_osd_setting.dest_rect);
	refresh_lcd(&img_setting, &osd_setting, LCD_ID_MAIN);

	return 0;
		//} 
}

int ASRLCDD_FillRect16(struct asrlcdd_roi_info *roi_info, unsigned int bgColor)
{
	unsigned int event[4];

	LCDLOGI("Info: ASRLCDD_FillRect16 +++\r\n");

	if(NULL == roi_info){
		LCDLOGE("ERR: ASRLCDD_FillRect16: Invalid param\r\n");
		return -1;
	}

	if(LCD_STATUS_POWERON != (g_lcd_status & LCD_STATUS_POWERON)){
		LCDLOGE("ERR: ASRLCDD_FillRect16: Invalid lcd status!\r\n");
		return -1;
	}

	event[0] = LCD_MSG_TYPE_FILL;
	event[1] = bgColor;
	event[2] = (roi_info->startY << 16) | (roi_info->startX);
	event[3] = (roi_info->height << 16) | (roi_info->width);

	UOS_SendMsg((void*)event, g_lcd_msgq_id, UOS_SEND_EVT);
	LCDLOGI("Info: ASRLCDD_FillRect16 ---\r\n");
	return 0;
}

int ASRLCDD_SetBuffers(void* pBuffer, int size, ASRLCDD_CB_FUNC cb, void* param)
{
	int ret;

	LCDLOGW("Info: ASRLCDD_SetBuffers +++\r\n");

	if(NULL == pBuffer){
		LCDLOGE("ERROR: ASRLCDD_SetBuffers: Invalid param\r\n");
		return -1;
	}

	ret = create_ROT_BufQueue(pBuffer, size);
	if(0 != ret){
		LCDLOGE("ERROR: ASRLCDD_SetBuffers: create buffer queue fail\r\n");
		return -1;
	}

	if(NULL != cb){
		g_lcd_cbs[ASRLCDD_CB_TYPE_BUFDONE] = cb;
		g_lcd_cb_params[ASRLCDD_CB_TYPE_BUFDONE] = param;
	}
	g_release_bq = 1;

	LCDLOGW("Info: ASRLCDD_SetBuffers ---\r\n");

	return 0;
}

/* 
 * 0x00 =< val =< 0xFF 
 */
void ASRLCDD_alpha_Blending(unsigned int val)
{
	g_alpha_val = val;
}

/*img layer not support crop!*/
int ASRLCDD_CameraMerge(struct asrlcdd_camera_buffer *pbuffer, unsigned int rotation, struct asrlcdd_roi_info *dest_roi)
{
	struct panel_spec *panel = g_ppanel;
	unsigned int dest_height, dest_width;
	int ret;
	struct asrlcdd_camera_buffer pdest_buffer;
	static int frame_count = 0;
	unsigned int event[4];
	int startX, startY;

	LCDLOGI("Info: ASRLCDD_CameraMerge (%d) (%d)+++\r\n", rotation, frame_count);
	
	//txchk_cur_task_stack_print();
	if(NULL == panel){
		LCDLOGE("ERR: ASRLCDD_CameraMerge: Invalid panel!\r\n");
		return -1;
	}

	if((LCD_STATUS_POWERON != (g_lcd_status & LCD_STATUS_POWERON)) && (NULL != pbuffer)){
		LCDLOGE("ERR: ASRLCDD_CameraMerge: Invalid lcd status!\r\n");
		return -1;
	}

	if(g_lcd_camerastatus == 1){
		LCDLOGE("ERR: ASRLCDD_CameraMerge: Camera is in pause status, can't merge!\r\n");
		return -1;
	}

	if(NULL == pbuffer){
		LCDLOGE("Info: ASRLCDD_CameraMerge: close camera\r\n");
		UOS_TakeMutex(g_lcd_mutex_id);
		LCDLOGD("DBG: ASRLCDD_CameraMerge get mutex\r\n");
		g_lcd_img_setting.layer_en = 0;
		g_lcd_img_setting.dirty = 1;		
		g_lcd_osd_setting.dirty = 1;
		g_lcd_osd_setting.layer_en = 1;
		g_lcd_osd_setting.src_rect.startX = 0;
		g_lcd_osd_setting.src_rect.startY = 0;
		g_lcd_osd_setting.dest_rect.startX = 0;
		g_lcd_osd_setting.dest_rect.startY= 0;
		g_lcd_osd_setting.src_rect.width = g_ui_info.width;
		g_lcd_osd_setting.src_rect.height = g_ui_info.height;
		g_lcd_osd_setting.dest_rect.width = g_ui_info.width;
		g_lcd_osd_setting.dest_rect.height = g_ui_info.height;

		if (g_release_bq == 1)
			g_release_bq = 2;
		
		UOS_ReleaseMutex(g_lcd_mutex_id);

		if((0 != g_lcd_ROTBQ.count) && (g_release_bq == 2)){
			release_ROT_BufQueue();

			if(g_lcd_cbs[ASRLCDD_CB_TYPE_BUFDONE] != NULL){
				LCDLOGW("INFO: lcd_thread_handler: cb +++\n");
				g_lcd_cbs[ASRLCDD_CB_TYPE_BUFDONE](g_lcd_cb_params[ASRLCDD_CB_TYPE_BUFDONE], NULL);
				LCDLOGW("INFO: lcd_thread_handler: cb ---\n");
			}

			g_last_ROT_buffer = NULL;
			g_release_bq = 0;
		}

	} else {
		LCDLOGD("Info: ASRLCDD_CameraMerge: show camera\r\n");
#ifdef LCD_ROTATION_90
		rotation = update_camera_rotation(rotation, ROT_90_DEG);
#endif
#ifdef LCD_ROTATION_180
		rotation = update_camera_rotation(rotation, ROT_180_DEG);
#endif
#ifdef LCD_ROTATION_270
		rotation = update_camera_rotation(rotation, ROT_270_DEG);
#endif
		LCDLOGD("DBG: ASRLCDD_CameraMerge (changed_r: %d)\r\n", rotation);

		if(NULL != dest_roi){
			LCDLOGI("Info: ASRLCDD_CameraMerge: input dest_roi (%d,%d, %d, %d)\r\n",
				dest_roi->startX, dest_roi->startY, dest_roi->width, dest_roi->height);
		} 
//		dump_camerabuffer(pbuffer);
		if(NULL == dest_roi){
			LCDLOGI("Info: ASRLCDD_CameraMerge: dest_roi is NULL!\r\n");
			if((ROT_90_DEG == rotation) || (ROT_270_DEG == rotation)){
				dest_width = pbuffer->height;
				dest_height = pbuffer->width;
			} else {
				dest_height = pbuffer->height;
				dest_width = pbuffer->width;
			}
		} else {
#if (defined(LCD_ROTATION_90) || defined(LCD_ROTATION_270))
			dest_width = dest_roi->height;
			dest_height = dest_roi->width;
#else
			dest_width = dest_roi->width;
			dest_height = dest_roi->height;
#endif
		}
		if((dest_height > panel->height) || (dest_width > panel->width)){
			LCDLOGE("ERROR: ASRLCDD_CameraMerge: roi is too large!\r\n");
			return -1;
		}
		if(1 == g_lcd_img_setting.dirty){
			if((g_lcd_img_setting.format != pbuffer->format) ||
				(g_lcd_img_setting.planes != pbuffer->planes) ||
				(g_lcd_img_setting.src_rect.width != dest_width)){
				LCDLOGE("ERR: ASRLCDD_CameraMerge Not the same camera config!!!!!\r\n");
				return -1;
			}
		}

		ret = lcd_do_rotation_scaling(pbuffer, &pdest_buffer, rotation, dest_width, dest_height);
		if(0 != ret){
			LCDLOGE("ERROR: ASRLCDD_CameraMerge: rotation/scaling fail!\r\n");
			return -1;
		}

		if(0 == g_lcd_img_setting.layer_en){
//			UOS_StopFunctionTimer(g_lcd_Blit16_timerid);
		}

		UOS_TakeMutex(g_lcd_mutex_id);
		LCDLOGD("DBG: ASRLCDD_CameraMerge get mutex\r\n");
		g_lcd_img_setting.layer_en = 1;
		g_lcd_img_setting.format = pdest_buffer.format;
		g_lcd_img_setting.planes = pdest_buffer.planes;
		g_lcd_img_setting.addr[0] = pdest_buffer.addr[0];
		g_lcd_img_setting.addr[1] = pdest_buffer.addr[1];
		g_lcd_img_setting.addr[2] = pdest_buffer.addr[2];
		g_lcd_img_setting.stride[0] = pdest_buffer.stride[0];
		g_lcd_img_setting.stride[1] = pdest_buffer.stride[1];
		g_lcd_img_setting.stride[2] = pdest_buffer.stride[2];
		g_lcd_img_setting.src_rect.startX = 0;
		g_lcd_img_setting.src_rect.startY = 0;
		g_lcd_img_setting.src_rect.width = pdest_buffer.width;
		g_lcd_img_setting.src_rect.height = pdest_buffer.height;
		if(NULL != dest_roi){
			startX = dest_roi->startX;
			startY = dest_roi->startY;
		} else {
			startX = 0;
			startX = 0;
		}
#if defined(LCD_ROTATION_270)
		g_lcd_img_setting.dest_rect.startX = startY;
		g_lcd_img_setting.dest_rect.startY = panel->height - startX - pdest_buffer.height;
#elif defined(LCD_ROTATION_180)
		g_lcd_img_setting.dest_rect.startX = panel->width - startX - pdest_buffer.width;
		g_lcd_img_setting.dest_rect.startY = panel->height - startY - pdest_buffer.height;
		LCDLOGE("Jessica: %d, %d, %d, %d\r\n", panel->height, startY, pdest_buffer.height, g_lcd_img_setting.dest_rect.startY);
#elif defined(LCD_ROTATION_90)
		g_lcd_img_setting.dest_rect.startY = startX;
		g_lcd_img_setting.dest_rect.startX = panel->width - startY - pdest_buffer.width;
#else
		g_lcd_img_setting.dest_rect.startX = startX;
		g_lcd_img_setting.dest_rect.startY = startY;
#endif

		g_lcd_img_setting.dest_rect.width = pdest_buffer.width;
		g_lcd_img_setting.dest_rect.height = pdest_buffer.height;
		g_lcd_img_setting.dirty = 1;
		UOS_ReleaseMutex(g_lcd_mutex_id);
		
		event[0] = LCD_MSG_TYPE_UPDATE;
		event[2] = LCD_ID_MAIN;
		UOS_SendMsg((void*)event, g_lcd_msgq_id, UOS_SEND_EVT);
	}

	LCDLOGI("Info: ASRLCDD_CameraMerge (%d) (%d)---\r\n", rotation, frame_count);
	frame_count++;
	return 0;
}

int ASRLCDD_CameraPause(void)
{
	//char* dest_buffer = NULL;
	int ret = -1;

	LCDLOGI("Info: ASRLCDD_CameraPause+++\r\n");

	if(LCD_STATUS_POWERON != (g_lcd_status & LCD_STATUS_POWERON)){
		LCDLOGE("ERR: ASRLCDD_CameraPause: Invalid lcd status!\r\n");
		return -1;
	}

	if(NULL == g_last_ROT_buffer){
		LCDLOGE("ERR: ASRLCDD_CameraPause: Camera has not been started!\r\n");
		return -1;
	}

	if(1 == g_lcd_camerastatus){
		LCDLOGE("ERR: ASRLCDD_CameraPause: Camera has been in pause status!\r\n");
		return 0;
	}

	g_lcd_camerastatus = 1;

	ret = dequeue_specify_ROT_BQs((void*)g_last_ROT_buffer);
	LCDLOGD("DBG: dequeue_specify_ROT_BQs return  %d\r\n", ret);
	LCDLOGI("Info: ASRLCDD_CameraPause---\r\n");
	return ret;
}

int ASRLCDD_CameraContinue(void)
{
	int ret = -1;
	unsigned int event[4];
	int i;
	uint32_t actual_flags = 0;
	uint32_t expect_flag = 0;

	LCDLOGI("Info: ASRLCDD_CameraContinue+++\r\n");

	if(LCD_STATUS_POWERON != (g_lcd_status & LCD_STATUS_POWERON)){
		LCDLOGE("ERR: ASRLCDD_CameraContinue: Invalid lcd status!\r\n");
		return -1;
	}

	if(NULL == g_last_ROT_buffer){
		LCDLOGE("ERR: ASRLCDD_CameraContinue: Camera has not been started!\r\n");
		return -1;
	}

	if(1 != g_lcd_camerastatus){
		LCDLOGE("ERR: ASRLCDD_CameraContinue: Camera is not in pause status!!\r\n");
		return 0;
	}

	g_lcd_camerastatus_pending = 1;
	g_lcd_camerastatus = 0;

	while(g_lcd_timer_started == 1){
		UOS_SleepMs(10);
	}
	event[0] = LCD_MSG_TYPE_SYNC;
	UOS_SendMsg((void*)event, g_lcd_msgq_id, UOS_SEND_EVT);

	expect_flag = 1;

	while(i < 10) {
		ret = UOS_WaitFlag(g_lcd_sync_flag, expect_flag, OSA_FLAG_OR_CLEAR,
			&actual_flags, 10);
		if ((ret == 0) && (0 != (actual_flags & expect_flag))){
			LCDLOGD("DBG: ASRLCDD_CameraContinue: sync with refresh thread!\r\n");
			break;
		}else{
			LCDLOGW("Warning: ASRLCDD_CameraContinue:wait to sync (0x%x)(%d)!\r\n", actual_flags, i);
		}
		i++;
	}
	
	g_lcd_camerastatus_pending = 0;

	ret = enqueue_ROT_BQs((void*)g_last_ROT_buffer);
	LCDLOGD("DBG: enqueue_ROT_BQs return  %d\r\n", ret);
	LCDLOGI("Info: ASRLCDD_CameraContinue---\r\n");
	return 0;
}

int ASRLCDD_SetBrightness(uint32_t brightness)
{
	LCDLOGE("Info: ASRLCDD_SetBrightness ++ (%d -> %d)\r\n", g_backlight_level, brightness);

	if (g_backlight_level == brightness)
		return 0;

#if 0
	if ((LCD_STATUS_SLEEP == (g_lcd_status & LCD_STATUS_SLEEP)) || g_lcd_status == LCD_STATUS_POWEROFF) {
		if (brightness > 0)
			ASRLCDD_WakeUp();
	}

	backlight_set_brightness(brightness);

	if (brightness == 0) {
		ASRLCDD_Sleep();
	}
#else
	g_backlight_level = brightness;
	if((g_backlight_on != 0) || (g_refreshed == 1)){
		LCDLOGE("ASRLCDD_SetBrightness: real set backlight to %d, (%d, %d)\r\n", brightness, g_backlight_on, g_refreshed);
		backlight_set_brightness(brightness);
		if(brightness == 0){
			g_backlight_on = 0;
			LCDLOGD("ASRLCDD_SetBrightness: brightness is 0, set g_backlight_on to 0\r\n");
		}
	} else {
		LCDLOGE("ASRLCDD_SetBrightness: not set backlight since g_backlight_on is %d, g_refreshed is %d\r\n",
			g_backlight_on, g_refreshed);
	}
#endif
	return 0;
}

//do not delete, will be called by other modual
void ASRLCDD_LcdSetBrightness(unsigned char level)
{   
    ASRLCDD_SetBrightness(level);
}

int ASRLCDD_GetScreenInfo(struct asrlcdd_screen_info *info)
{
	struct panel_spec *panel = g_ppanel;

	LCDLOGI("Info: ASRLCDD_GetScreenInfo ++\r\n");

	if(NULL == info){
		LCDLOGE("ERR: ASRLCDD_GetScreenInfo: Invalid parameter!\r\n");
		return -1;
	}

	if(NULL == panel){
		LCDLOGE("ERR: ASRLCDD_GetScreenInfo: panel has not been inited!\r\n");
		return -1;
	}
#if (defined(LCD_ROTATION_90) || defined(LCD_ROTATION_270))
	info->width = panel->height;
	info->height = panel->width;
#else
	info->width = panel->width;
	info->height = panel->height;
#endif
	return 0;
}

BOOL ASRLCDD_LcdIsActive(void)
{
    return (LCD_STATUS_POWERON == (g_lcd_status & LCD_STATUS_POWERON)) ? TRUE : FALSE;
}

BOOL ASRLCDD_AnyLcdIsActive(void)
{
    return (0 != (g_lcd_status & (LCD_STATUS_SUB_POWERON | LCD_STATUS_POWERON))) ? TRUE : FALSE;
}

void ASRLCDD_LcdShowColor(int startX, int startY, unsigned short width, unsigned short height, LCD_BGCOLOR_INDEX_ bgIndex)
{
	unsigned char r=0;
	unsigned char g=0;
	unsigned char b=0;
	
	struct asrlcdd_roi_info roi_info;
	
	roi_info.startX = startX;
	roi_info.startY = startY;
	roi_info.width = width;
	roi_info.height = height;

	switch(bgIndex)
	{
		case BG_BLCAK_:	// black
			break;
		case BG_RED_:	// red
			r = 0x1F;
			break;
		case BG_GREEN_:	// green
			g = 0xFF;
			break;
		case BG_BLUE_:	// blue
			b = 0x1F;
			break;
		case BG_WHITE_:	// white
			r = 0x1F;
			g = 0xFF;
			b = 0x1F;
			break;
		default:
			break;
	}

	ASRLCDD_FillRect16(&roi_info, ((r<<11) | (g<<5) | b));
}

/*********Code for uboot logo mode********/
int ASRLCDD_Logo_Open(void)
{
	struct panel_spec *panel = NULL;
#ifdef LCD_DUAL_PANEL_SUPPORT
	struct panel_spec *sub_panel = NULL;
#endif
	struct s_lcd_ctx *lcd = NULL;
	int ret = -1;

	LCDLOGE("Info: ASRLCDD_Logo_Open +++\r\n");

	if(g_lcd_status != LCD_STATUS_POWEROFF){
		LCDLOGW("Info: ASRLCDD_Logo_Open: asrlcd has been opened!\r\n");
		return 0;
	}

	lcd_ass_poweron();
	g_lcd_status = LCD_STATUS_HWON;

	lcd = (struct s_lcd_ctx*)lcd_init(LCD_WORK_MODE_ASS_POLLING);
	if(NULL == lcd){
		LCDLOGE("ERR: ASRLCDD_Logo_Open: lcd_init fail\r\n");
		ASRLCDD_Logo_Close();
		return -1;
	}
	g_plcd = lcd;

	panel = find_panel(LCD_SCLK_FREQ, LCD_WORK_MODE_ASS_POLLING, 0, LCD_ID_MAIN);
	if(NULL == panel){
		LCDLOGE("ERR: ASRLCDD_Logo_Open: find_panel fail\r\n");
		ASRLCDD_Logo_Close();
		return -1;
	}
	g_ppanel = panel;

#ifdef LCD_DUAL_PANEL_SUPPORT
	sub_panel = find_panel(LCD_SCLK_FREQ, LCD_WORK_MODE_ASS_POLLING, 0, LCD_ID_SUB);
	if(NULL == sub_panel){
		LCDLOGE("ERR: ASRLCDD_Logo_Open: find_sub_panel fail\r\n");
		ASRLCDD_Logo_Close();
		return -1;
	}
	g_psubpanel = sub_panel;
#endif

	ret = panel_init(panel, LCD_SCLK_FREQ, LCD_WORK_MODE_ASS_POLLING, 0, LCD_ID_MAIN);
	if(0 != ret){
		LCDLOGE("ERR: ASRLCDD_Logo_Open: panel_init fail\r\n");
		ASRLCDD_Logo_Close();
		return -1;
	}

#ifdef LCD_DUAL_PANEL_SUPPORT
	ret = panel_init(sub_panel, LCD_SCLK_FREQ, LCD_WORK_MODE_ASS_POLLING, 0, LCD_ID_SUB);
	if(0 != ret){
		LCDLOGE("ERR: ASRLCDD_Logo_Open: sub panel_init fail\r\n");
		ASRLCDD_Logo_Close();
		return -1;
	}
#endif

	if((panel->type == LCD_TYPE_FSTN) && (NULL == g_fstn_data)){
		g_fstn_data = (unsigned char*)malloc(panel->height / 8 * panel->width);
		if(NULL == g_fstn_data){
			LCDLOGE("ERR: ASRLCDD_Logo_Open: Can't malloc fstn memory\r\n");
			ASRLCDD_Logo_Close();
			return -1;
		} else {
			LCDLOGD("DBG: ASRLCDD_Logo_Open: malloc fstn memory OK! (0x%x)\r\n", g_fstn_data);
		}
	}

	lcd->bg_color = 0xFF;
	lcd->alpha_mode = LCD_ALPHA_MODE_NORMAL;
	lcd->gamma_en = 0;
	lcd->dither_setting.dither_en = 0;
	lcd->dither_setting.dither_mode = LCD_DITHER_4X8;
	lcd->wb_setting.wb_en = 0;

#if 0
	ret = lcd_output_setting(lcd, panel);
	if(0 != ret){
		LCDLOGE("ERR: lcd_output_setting fail\r\n");
		ASRLCDD_Logo_Close();
		return -1;
	}
#endif

	mdelay(50);
	LCDLOGE("Info: ASRLCDD_Logo_Open ---\r\n");
	
	return 0;
}


int ASRLCDD_Logo_Close(void)
{
	int i = 0;

	LCDLOGE("INFO: ASRLCDD_Logo_Close +++!\r\n");

	if(NULL != g_plcd){
		lcd_uninit(g_plcd);
		g_plcd = NULL;
	}
	if(NULL != g_ppanel){
		panel_uninit(g_ppanel, LCD_ID_MAIN);
		g_ppanel = NULL;
	}
#ifdef LCD_DUAL_PANEL_SUPPORT
	if(NULL != g_psubpanel){
		panel_uninit(g_psubpanel, LCD_ID_SUB);
		g_psubpanel = NULL;
	}
#endif
	if(g_lcd_status != LCD_STATUS_POWEROFF){
		lcd_ass_poweroff();
		g_lcd_status = LCD_STATUS_POWEROFF; 	
	}
	for(i=0;i<ASRLCDD_CB_TYPE_MAX;i++){
		g_lcd_cbs[i] = NULL;
		g_lcd_cb_params[i] = NULL;
	}

	if(NULL != g_fstn_data){
		UOS_FREE(g_fstn_data);
		g_fstn_data = NULL;
	}

	LCDLOGI("INFO: ASRLCDD_Logo_Close ---\n");
	return 0;
}

int ASRLCDD_Logo_Blit16_sync(struct asrlcdd_framebuffer_window *frame_window,
	uint16_t startX, uint16_t startY)
{
	struct s_osd_layer_setting osd_setting;  // yuhuizhang
	struct s_img_layer_setting img_setting;  // yuhuizhang

	if(NULL == frame_window){
		LCDLOGE("ERR: ASRLCDD_Logo_Blit16_sync: Invalid param\r\n");
		return -1;
	}

	LCDLOGI("Info: ASRLCDD_Logo_Blit16_sync +++ (%d, %d, %d, %d)\r\n", frame_window->roi_info.startX, 
		frame_window->roi_info.startY, frame_window->roi_info.width, frame_window->roi_info.height);

	if(frame_window->frame_info.pbuffer == NULL){ 
		LCDLOGE("ERR: ASRLCDD_Logo_Blit16_sync: Invalid param\r\n");
		return -1;
	}

	if((LCD_STATUS_POWERON != (g_lcd_status & LCD_STATUS_POWERON)) &&(g_lcd_status != LCD_STATUS_HWON)){
		LCDLOGE("ERR: ASRLCDD_Logo_Blit16_sync: Invalid lcd status!\r\n");
		return -1;
	}

//	dump_framewindow(frame_window);
	osd_setting.dirty = 1;
	osd_setting.layer_en = 1;

	switch(frame_window->frame_info.bitdepth){
	case 16:
		osd_setting.format = LCD_FORMAT_RGB565;
		break;
	case 32:
		osd_setting.format = LCD_FORMAT_RGBA888;
		break;
	default:
		LCDLOGE("ERR: ASRLCDD_Logo_Blit16_sync: Invalid bitdepth (%d)!\r\n", frame_window->frame_info.bitdepth);
		osd_setting.format = LCD_FORMAT_RGB565; //use RGB565 as default
	}

	osd_setting.frame_buffer.pbuffer = frame_window->frame_info.pbuffer;
	osd_setting.frame_buffer.height = frame_window->frame_info.height;
	osd_setting.frame_buffer.stride = frame_window->frame_info.stride;
	osd_setting.frame_buffer.bitdepth = frame_window->frame_info.bitdepth;
	osd_setting.src_rect.startX = frame_window->roi_info.startX;
	osd_setting.src_rect.startY = frame_window->roi_info.startY;
	osd_setting.src_rect.height = frame_window->roi_info.height;
	osd_setting.src_rect.width = frame_window->roi_info.width;
	osd_setting.dest_rect.startX = startX;
	osd_setting.dest_rect.startY = startY;
	osd_setting.dest_rect.height = frame_window->roi_info.height;
	osd_setting.dest_rect.width = frame_window->roi_info.width;

	img_setting.layer_en = 0;

//	refresh_lcd(&img_setting, &osd_setting, &osd_setting.dest_rect);
	refresh_lcd(&img_setting, &osd_setting, LCD_ID_MAIN);

	return 0;
}

/*********Code for assert mode********/
int ASRLCDD_Assert_Open(void)
{
	struct s_lcd_ctx *lcd = NULL;
	int old_status = g_lcd_status;

	LCDLOGE("Info: ASRLCDD_Assert_Open +++\r\n");

#ifdef GUI_LCD_MUTEX
	if(g_gui_lcd_mutex_id != INVALID_MUTEX_ID){
		g_gui_lcd_mutex_id = INVALID_MUTEX_ID;
	}
#endif

	if(g_lcd_status == LCD_STATUS_ASSERTON){
		LCDLOGW("Info: ASRLCDD_Assert_Open: asrlcd has not assert on!\r\n");
		return 0;
	}

	if(g_lcd_status == LCD_STATUS_POWEROFF){
		LCDLOGW("Info: ASRLCDD_Assert_Open: asrlcd has not been opened!\r\n");
		return 0;
	}

	lcd = lcd_init(LCD_WORK_MODE_ASS_POLLING);
	if(NULL == lcd){
		LCDLOGW("Info: ASRLCDD_Assert_Open: lcd_init fail!\r\n");
	}

	panel_init(g_ppanel, LCD_SCLK_FREQ, LCD_WORK_MODE_ASS_POLLING, 1, LCD_ID_MAIN);

	g_lcd_status = LCD_STATUS_ASSERTON;
	if (old_status == LCD_STATUS_SLEEP) {
			ASRLCDD_WakeUp();
	}

	LCDLOGI("Info: ASRLCDD_Assert_Open ---\r\n");
	
	return 0;
}

int ASRLCDD_Assert_Blit16_sync(struct asrlcdd_framebuffer_window *frame_window,
	uint16_t startX, uint16_t startY)
{
	struct s_osd_layer_setting osd_setting;
	struct s_img_layer_setting img_setting;
	struct panel_spec *panel = g_ppanel;

	if(NULL == frame_window){
		LCDLOGE("ERR: ASRLCDD_Assert_Blit16_sync: Invalid param\r\n");
		return -1;
	}

	if(panel == NULL){
		LCDLOGE("ERR: ASRLCDD_Assert_Blit16_sync: Invalid panel\r\n");
		return -1;
	}

	if((panel->type == LCD_TYPE_FSTN) && (NULL == g_fstn_data)){
		LCDLOGE("ERR: ASRLCDD_Assert_Blit16_sync: No FSTN data memory for fstn panel!\r\n");
		return -1;
	}


	LCDLOGE("Info: ASRLCDD_Assert_Blit16_sync +++ (%d, %d, %d, %d)\r\n", frame_window->roi_info.startX, 
		frame_window->roi_info.startY, frame_window->roi_info.width, frame_window->roi_info.height);

	if(frame_window->frame_info.pbuffer == NULL){ 
		LCDLOGE("ERR: ASRLCDD_Assert_Blit16_sync: Invalid param\r\n");
		return -1;
	}

#if 0
	if(g_lcd_status != LCD_STATUS_ASSERTON){
		LCDLOGE("ERR: ASRLCDD_Assert_Blit16_sync: Invalid lcd status!\r\n");
		return -1;
	}
#endif

//	dump_framewindow(frame_window);
	osd_setting.dirty = 1;
	osd_setting.layer_en = 1;

	switch(frame_window->frame_info.bitdepth){
	case 16:
		osd_setting.format = LCD_FORMAT_RGB565;
		break;
	case 32:
		osd_setting.format = LCD_FORMAT_RGBA888;
		break;
	default:
		LCDLOGE("ERR: ASRLCDD_Assert_Blit16_sync: Invalid bitdepth (%d)!\r\n", frame_window->frame_info.bitdepth);
		osd_setting.format = LCD_FORMAT_RGB565; //use RGB565 as default
	}

	osd_setting.frame_buffer.pbuffer = frame_window->frame_info.pbuffer;
	osd_setting.frame_buffer.height = frame_window->frame_info.height;
	osd_setting.frame_buffer.stride = frame_window->frame_info.stride;
	osd_setting.frame_buffer.bitdepth = frame_window->frame_info.bitdepth;
	osd_setting.src_rect.startX = frame_window->roi_info.startX;
	osd_setting.src_rect.startY = frame_window->roi_info.startY;
	osd_setting.src_rect.height = frame_window->roi_info.height;
	osd_setting.src_rect.width = frame_window->roi_info.width;
	osd_setting.dest_rect.startX = startX;
	osd_setting.dest_rect.startY = startY;
	osd_setting.dest_rect.height = frame_window->roi_info.height;
	osd_setting.dest_rect.width = frame_window->roi_info.width;

	img_setting.layer_en = 0;

//	refresh_lcd(&img_setting, &osd_setting, &osd_setting.dest_rect);
	refresh_lcd(&img_setting, &osd_setting, LCD_ID_MAIN);

	return 0;
}

int ASRLCDD_Capture(struct asrlcdd_src_buf_des *img_buffer,
	struct asrlcdd_src_buf_des *osd1_buffer,
	struct asrlcdd_dest_buf_des *dest_buffer)
{
	struct s_img_layer_setting img_setting;
	struct s_osd_layer_setting osd1_setting;
	struct s_wb_layer_setting wb_setting;
	int ret;

	LCDLOGI("Info: ASRLCDD_Capture +++\r\n");
	if((NULL == img_buffer) && (NULL == osd1_buffer)){
		LCDLOGE("ERROR: ASRLCDD_Capture: No input layers! do nothing!\r\n");
		return 0;
	}

	if(NULL == dest_buffer){
		LCDLOGE("ERROR: ASRLCDD_Capture: No requirement! do nothing!\r\n");
		return 0;
	}
	
	switch_img_buf_des_to_setting(img_buffer, &img_setting);
	switch_osd_buf_des_to_setting(osd1_buffer, &osd1_setting);
	switch_wb_buf_des_to_setting(dest_buffer, &wb_setting);

#if 1 /*work around for HW issue*/
#ifdef LCD_INTERRUPT_MODE
	lcd_hw_reset(LCD_WORK_MODE_INTERRUPT);
#else
	lcd_hw_reset(LCD_WORK_MODE_POLLING);
#endif

	lcd_reset(g_plcd);
#ifdef LCD_INTERRUPT_MODE
	panel_reset(g_ppanel);
#else
	panel_reset(g_ppanel);
#endif
#endif

	ret = wb_lcd(&img_setting, &osd1_setting, &wb_setting);
	if(0 != ret){
		LCDLOGE("ERROR: ASRLCDD_Capture: wb_lcd fail!!\r\n");
		return 0;
	}

	LCDLOGI("Info: ASRLCDD_Capture ---\r\n");
	return 0;
}

int ASRLCDD_Refresh(struct asrlcdd_src_buf_des *img_buffer,
	struct asrlcdd_src_buf_des *osd1_buffer)
{
	struct s_img_layer_setting img_setting;
	struct s_osd_layer_setting osd1_setting;
	struct asrlcdd_roi_info dest_roi, temp_roi1, temp_roi2;
	int ret;

	LCDLOGI("Info: ASRLCDD_Refresh +++\r\n");
	if((NULL == img_buffer) && (NULL == osd1_buffer)){
		LCDLOGE("ERROR: ASRLCDD_Refresh: No input layers! do nothing!\r\n");
		return 0;
	}
	
	switch_img_buf_des_to_setting(img_buffer, &img_setting);
	switch_osd_buf_des_to_setting(osd1_buffer, &osd1_setting);

	if((NULL != img_buffer) && (NULL != osd1_buffer)){
		temp_roi1.startX = img_buffer->dest_point.startX;
		temp_roi1.startY = img_buffer->dest_point.startY;
		temp_roi1.width = img_buffer->src_roi.width;
		temp_roi1.height = img_buffer->src_roi.height;

		temp_roi2.startX = osd1_buffer->dest_point.startX;
		temp_roi2.startY = osd1_buffer->dest_point.startY;
		temp_roi2.width = osd1_buffer->src_roi.width;
		temp_roi2.height = osd1_buffer->src_roi.height;

		merge_roi(&temp_roi1, &temp_roi2, &dest_roi);		
	} else if(NULL != img_buffer){
		dest_roi.startX = img_buffer->dest_point.startX;
		dest_roi.startY = img_buffer->dest_point.startY;
		dest_roi.width = img_buffer->src_roi.width;
		dest_roi.height = img_buffer->src_roi.height;
	} else {
		dest_roi.startX = osd1_buffer->dest_point.startX;
		dest_roi.startY = osd1_buffer->dest_point.startY;
		dest_roi.width = osd1_buffer->src_roi.width;
		dest_roi.height = osd1_buffer->src_roi.height;
	}

//	ret = refresh_lcd(&img_setting, &osd1_setting, &dest_roi);
	ret = refresh_lcd(&img_setting, &osd1_setting, LCD_ID_MAIN);
	if(0 != ret){
		LCDLOGE("ERROR: ASRLCDD_Refresh: refresh_lcd fail!!\r\n");
		return 0;
	}

	LCDLOGI("Info: ASRLCDD_Refresh ---\r\n");
	return 0;
}

void ASRLCDD_Read_Panel_ID(void)
{
	struct panel_spec* panel = NULL;
	int id = 0;

	panel = g_ppanel;

	if(panel == NULL)
		LCDLOGE("ERROR: ASRLCDD_Read_Panel_ID: panel == NULL\r\n");

	id = panel->ops->panel_readid(panel);

	LCDLOGE("INFO: expect id = 0x%x, really id = 0x%x !!!\r\n",panel->panel_id,id);

	if(id != panel->panel_id)
		LCDLOGE("ERROR: ASRLCDD_Read_Panel_ID: read id fail id = 0x%x !!\r\n",id);

}

void ASRLCDD_SetUIInfo(struct asrlcdd_screen_info *info)
{
	LCDLOGI("Info: ASRLCDD_SetUIInfo ++\r\n");

	if(NULL == info){
		LCDLOGE("ERR: ASRLCDD_SetUIInfo: Invalid parameter!\r\n");
		return;
	}

	g_ui_info.width = info->width;
	g_ui_info.height = info->height;
}

#ifdef LCD_DUAL_PANEL_SUPPORT
int ASRLCDD_GetSubScreenInfo(struct asrlcdd_screen_info *info)
{
	struct panel_spec *panel = g_psubpanel;

	LCDLOGD("Info: ASRLCDD_GetSubScreenInfo ++\r\n");

	if(NULL == info){
		LCDLOGE("ERR: ASRLCDD_GetSubScreenInfo: Invalid parameter!\r\n");
		return -1;
	}

	if(NULL == panel){
		LCDLOGE("ERR: ASRLCDD_GetSubScreenInfo: panel has not been inited!\r\n");
		return -1;
	}
	info->width = panel->width;
	info->height = panel->height;
	return 0;
}

int ASRLCDD_SubSleep(void)
{
	int ret  = -1;
	struct panel_spec *panel = g_psubpanel;
	int timeout = 1000;

	LCDLOGE("Info: ASRLCDD_SubSleep ++\r\n");

	if(NULL == panel){
		LCDLOGE("ERR: ASRLCDD_SubSleep: panel has not been inited!\r\n");
		return -1;
	}

	if(LCD_STATUS_SUB_SLEEP == (g_lcd_status & LCD_STATUS_SUB_SLEEP)){
		LCDLOGE("ERR: ASRLCDD_SubSleep: panel has been in sleep!\r\n");
		return 0;
	} else if(g_lcd_status == LCD_STATUS_POWEROFF){
		LCDLOGE("ERR: ASRLCDD_SubSleep: Invalid lcd status!\r\n");
		return -1;
	}

	g_lcd_status &= (LCD_STATUS_POWERON | LCD_STATUS_SLEEP);
	g_lcd_status |= LCD_STATUS_SUB_SLEEP;

	while((g_flip_pending == 1) && (timeout > 0)){
		UOS_Sleep(MS_TO_TICKS(2));
		timeout--;
	}

	ret = panel_sleep(panel);
	if(-1 == ret){
		LCDLOGE("ERR: ASRLCDD_SubSleep: panel_sleep fail\r\n");
		return ret;
	}
	if(g_lcd_status == LCD_STATUS_SUB_SLEEP | LCD_STATUS_SLEEP)
		uiSetSuspendFlag(LP_ID_LCD, 1);
	return 0;
}

int ASRLCDD_SubWakeUp(void)
{
	int ret  = -1;
	struct panel_spec *panel = g_psubpanel;

	LCDLOGE("Info: ASRLCDD_SubWakeUp (0x%x) ++\r\n", g_lcd_status);

	if(NULL == panel){
		LCDLOGE("ERR: ASRLCDD_SubWakeUp: panel has not been inited!\r\n");
		return -1;
	}

	if(g_lcd_status != LCD_STATUS_ASSERTON)
		uiSetSuspendFlag(LP_ID_LCD, 0);

	if(LCD_STATUS_SUB_POWERON == (g_lcd_status & LCD_STATUS_SUB_POWERON)){
		LCDLOGW("Warning: ASRLCDD_SubWakeUp: panel has been in power on!\r\n");
		return 0;
	} else if(g_lcd_status == LCD_STATUS_POWEROFF){
		LCDLOGI("INFO: ASRLCDD_SubWakeUp: Wakeup from D2!\r\n");

	#ifdef USE_MEDIACLK_INTERFACE
		lcd_poweron();
	#else
		lcd_ass_poweron();
	#endif
		
		lcd_reset(g_plcd);
#ifdef LCD_INTERRUPT_MODE
		panel_reset(g_psubpanel);
#else
		panel_reset(g_psubpanel);
#endif
		g_lcd_status &= (LCD_STATUS_POWERON | LCD_STATUS_SLEEP);
		g_lcd_status |= LCD_STATUS_SUB_SLEEP;
	}

	ret = panel_wakeup(panel);
	if(-1 == ret){
		LCDLOGE("ERR: ASRLCDD_SubWakeUp: panel_wakeup fail\r\n");
		return ret;
	}
	g_lcd_status &= (LCD_STATUS_POWERON | LCD_STATUS_SLEEP);
	g_lcd_status |= LCD_STATUS_SUB_POWERON;

	return 0;
}

int ASRLCDD_SubBlit16(struct asrlcdd_framebuffer_window *frame_window,
	uint16_t startX, uint16_t startY)
{
	struct asrlcdd_framebuffer_info *src_frame_buffer, *dest_frame_buffer;
	struct asrlcdd_roi_info temp_roi1, temp_roi2, temp_roi3;
	unsigned int event[4];

	LCDLOGD("Info: ASRLCDD_SubBlit16 +++ (%d, %d, %d, %d)\r\n", frame_window->roi_info.startX, 
		frame_window->roi_info.startY, frame_window->roi_info.width, frame_window->roi_info.height);

	if(NULL == frame_window){
		LCDLOGE("ERR: ASRLCDD_SubBlit16: Invalid param\r\n");
		return -1;
	}

	if(LCD_STATUS_SUB_POWERON != (g_lcd_status & LCD_STATUS_SUB_POWERON)){
		LCDLOGE("ERR: ASRLCDD_SubBlit16: Invalid lcd status!\r\n");
		return -1;
	}
//	dump_framewindow(frame_window);

	UOS_TakeMutex(g_lcd_mutex_id);
//	LCDLOGD("DBG: ASRLCDD_Blit16 get mutex\r\n");

	src_frame_buffer = &(frame_window->frame_info);
	dest_frame_buffer =  &(g_sublcd_osd_setting.frame_buffer);

	if(g_sublcd_osd_setting.dirty){
		if((dest_frame_buffer->bitdepth != src_frame_buffer->bitdepth) ||
			(dest_frame_buffer->stride != src_frame_buffer->stride)){
			LCDLOGE("ERR: ASRLCDD_SubBlit16 Not the same frame setting!!!!!\r\n");
			UOS_ReleaseMutex(g_lcd_mutex_id);
			return -1;
		}
	}


	if(g_lcd_osd_setting.dirty){
		LCDLOGD("DBG: ASRLCDD_SubBlit16 osd layer is dirty\r\n");

		temp_roi2.startX = startX;
		temp_roi2.startY = startY;
		temp_roi2.height = frame_window->roi_info.height;
		temp_roi2.width = frame_window->roi_info.width;

		temp_roi1.startX = g_sublcd_osd_setting.src_rect.startX;
		temp_roi1.startY = g_sublcd_osd_setting.src_rect.startY;
		temp_roi1.width = g_sublcd_osd_setting.src_rect.width;
		temp_roi1.height = g_sublcd_osd_setting.src_rect.height;

		temp_roi3.startX = g_sublcd_osd_setting.dest_rect.startX;
		temp_roi3.startY = g_sublcd_osd_setting.dest_rect.startY;
		temp_roi3.width = g_sublcd_osd_setting.dest_rect.width;
		temp_roi3.height = g_sublcd_osd_setting.dest_rect.height;

		merge_roi(&temp_roi1, &frame_window->roi_info,
			&g_sublcd_osd_setting.src_rect);
		merge_roi(&temp_roi2, &temp_roi3, &g_sublcd_osd_setting.dest_rect);
	} else {
		LCDLOGD("DBG: ASRLCDD_SubBlit16 osd layer is not dirty!\r\n");
		dest_frame_buffer->pbuffer = src_frame_buffer->pbuffer;
		dest_frame_buffer->bitdepth = src_frame_buffer->bitdepth;
		dest_frame_buffer->stride = src_frame_buffer->stride;
		dest_frame_buffer->height = src_frame_buffer->height;

		switch(frame_window->frame_info.bitdepth){
		case 16:
			g_sublcd_osd_setting.format = LCD_FORMAT_RGB565;
			break;
		case 32:
			g_sublcd_osd_setting.format = LCD_FORMAT_RGBA888;
			break;
		default:
			LCDLOGE("ERR: ASRLCDD_SubBlit16: Invalid bitdepth (%d)!\r\n", frame_window->frame_info.bitdepth);
			g_sublcd_osd_setting.format = LCD_FORMAT_RGB565; //use RGB565 as default
		}

		g_sublcd_osd_setting.src_rect.startX = frame_window->roi_info.startX;
		g_sublcd_osd_setting.src_rect.startY = frame_window->roi_info.startY;
		g_sublcd_osd_setting.src_rect.height = frame_window->roi_info.height;
		g_sublcd_osd_setting.src_rect.width = frame_window->roi_info.width;
		g_sublcd_osd_setting.dest_rect.startX = startX;
		g_sublcd_osd_setting.dest_rect.startY = startY;
		g_sublcd_osd_setting.dest_rect.height = frame_window->roi_info.height;
		g_sublcd_osd_setting.dest_rect.width = frame_window->roi_info.width;
	}

	g_sublcd_osd_setting.layer_en = 1;
	g_sublcd_osd_setting.dirty = 1;

	LCDLOGI("Info: ASRLCDD_SubBlit16 ---(%d, %d, %d, %d), (%d, %d, %d, %d)\r\n",
		g_sublcd_osd_setting.src_rect.startX, g_sublcd_osd_setting.src_rect.startY,
		g_sublcd_osd_setting.src_rect.width, g_sublcd_osd_setting.src_rect.height,
		g_lcd_osd_setting.dest_rect.startX, g_sublcd_osd_setting.dest_rect.startY,
		g_sublcd_osd_setting.dest_rect.width, g_sublcd_osd_setting.dest_rect.height);

#if 0
	if((0 == g_lcd_timer_started) && ((0 == g_lcd_img_setting.layer_en) ||( 1 == g_lcd_camerastatus))){
		UOS_StartFunctionTimer_single(g_lcd_Blit16_timerid, MS_TO_TICKS(30), lcd_timer_handler, NULL, "lcd_timer_blit16");
		g_lcd_timer_started = 1;
	}
#else
	event[0] = LCD_MSG_TYPE_UPDATE;
	event[2] = LCD_ID_SUB;
	UOS_SendMsg((void*)event, g_lcd_msgq_id, UOS_SEND_EVT);
#endif
		UOS_ReleaseMutex(g_lcd_mutex_id);

	return 0;
}

int ASRLCDD_SubFillRect16(struct asrlcdd_roi_info *roi_info, unsigned int bgColor)
{
	unsigned int event[4];

	LCDLOGI("Info: ASRLCDD_SubFillRect16 +++\r\n");

	if(NULL == roi_info){
		LCDLOGE("ERR: ASRLCDD_SubFillRect16: Invalid param\r\n");
		return -1;
	}

	if(LCD_STATUS_SUB_POWERON != (g_lcd_status & LCD_STATUS_SUB_POWERON)){
		LCDLOGE("ERR: ASRLCDD_SubFillRect16: Invalid lcd status!\r\n");
		return -1;
	}

	event[0] = LCD_MSG_TYPE_SUB_FILL;
	event[1] = bgColor;
	event[2] = (roi_info->startY << 16) | (roi_info->startX);
	event[3] = (roi_info->height << 16) | (roi_info->width);

	UOS_SendMsg((void*)event, g_lcd_msgq_id, UOS_SEND_EVT);
	LCDLOGI("Info: ASRLCDD_FillRect16 ---\r\n");
	return 0;
}


int ASRLCDD_SetSubBrightness(uint32_t brightness)
{
	LCDLOGE("Info: ASRLCDD_SetSubBrightness ++ (%d -> %d)\r\n", g_subbacklight_level, brightness);

	if (g_subbacklight_level == brightness)
		return 0;

#if 0
	if ((LCD_STATUS_SLEEP == (g_lcd_status & LCD_STATUS_SLEEP)) || g_lcd_status == LCD_STATUS_POWEROFF) {
		if (brightness > 0)
			ASRLCDD_WakeUp();
	}

	backlight_set_brightness(brightness);

	if (brightness == 0) {
		ASRLCDD_Sleep();
	}
#else
	g_subbacklight_level = brightness;
	if((g_subbacklight_on != 0) || (g_subrefreshed == 1)){
		LCDLOGE("ASRLCDD_SetSubBrightness: real set backlight (%d)\r\n", brightness);
		sub_backlight_set_brightness(brightness);
		if(brightness == 0){
			g_subbacklight_on = 0;
			LCDLOGE("ASRLCDD_SetSubBrightness: brightness is 0, set g_subbacklight_on to 0\r\n");
		}
	} else {
		LCDLOGE("ASRLCDD_SetSubBrightness: not set backlight since g_subbacklight_on is %d, g_subrefreshed is %d\r\n",
			g_backlight_on, g_subrefreshed);
	}
#endif
	return 0;
}

BOOL ASRLCDD_SubLcdIsActive(void)
{
    return (LCD_STATUS_SUB_POWERON == (g_lcd_status & LCD_STATUS_SUB_POWERON)) ? TRUE : FALSE;
}

#endif

