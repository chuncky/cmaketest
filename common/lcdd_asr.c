#include "lcd_predefine.h"
#include "panel_drv.h"
#include "lcd_drv.h"
#include "lcd_fns.h"
#include "backlight_drv.h"
#include "lcdd_asr.h"
#include "panel_list.h"

#define MAX_ROTBQ_NUM 2
#define MAX_LAYER_RECTS 10
#define MAX_IMG_FRAME 5
#define LCD_TASK_SIZE					1024
#define LCD_TASK_PRIORITY				200
#define LCD_MESSAGE_Q_MAX	64
#define LCD_MESSAGE_Q_SIZE	16
#define LCD_EVNET_CLOSE	0x1


enum media_clk_module_id{
	MODULE_CAMERA = 0,
	MODULE_DISPLAY,
	MODULE_VIDEO,
	MODULE_MAX,
};


enum{
	LCD_STATUS_POWEROFF,
	LCD_STATUS_POWERON,
	LCD_STATUS_HWON,
	LCD_STATUS_SLEEP,
	LCD_STATUS_LIMIT
};

typedef enum {
	LCD_MSG_TYPE_UPDATE,
	LCD_MSG_TYPE_STOP,
	LCD_MSG_TYPE_FILL,
	LCD_MSG_TYPE_LIMIT
}LCD_MSG_TYPE;


struct asrlcdd_message_info
{
	unsigned int msg_type;
	unsigned int bg_color;
	unsigned int roi_start;
	unsigned int roi_size;
		
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

struct s_lcd_ROTBQ{
	struct s_lcd_ROT_buffer buffers[MAX_ROTBQ_NUM];
	int used_count;
	int free_count;
	int count;
};

UINT8 g_lcd_status = LCD_STATUS_POWEROFF;
UINT8 g_lcd_task_exit = 1;


struct s_lcd_ctx *g_plcd = NULL;
struct panel_spec *g_ppanel = NULL;
ASRLCDD_CB_FUNC g_lcd_cbs[ASRLCDD_CB_TYPE_MAX] = {NULL};
void *g_lcd_cb_params[ASRLCDD_CB_TYPE_MAX] = {NULL};

struct s_lcd_ROTBQ g_lcd_ROTBQ = {NULL};
struct s_img_layer_setting g_lcd_img_setting = {0};
struct s_osd_layer_setting g_lcd_osd_setting = {0};


static OSMutexRef g_lcd_mutex_ref = NULL;
static OSAMsgQRef g_lcd_msg_queue_ref = NULL;
static OSAFlagRef g_lcd_flag_ref = NULL;
static OSTaskRef g_lcd_task_ref = NULL;

static OSATimerRef g_lcd_timer_ref = NULL;
BOOL g_lcd_timer_started = FALSE;
char *g_last_ROT_buffer = NULL;
static int g_release_bq = 0;

__align(4) uint8_t g_lcd_task_stack[LCD_TASK_SIZE];

volatile int g_flip_pending = 0;

#ifdef ASRLCD_DOUBLE_FB
unsigned char g_asrhw_framebuffer[240 * 320 * 2];
uint8 g_lcd_hwfb_mutex_id = 0;
#endif

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

#if 0
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

	if(left_size < bq_size){
		LCDLOGE("Info: create_ROT_BufQueue: Buffer is too small!\r\n");
		return -1;
	}

	if(g_lcd_ROTBQ.count != 0){
		LCDLOGE("ERROR: create_ROT_BufQueue: Current ROT BQ is not NULL!\r\n");
		return -1;
	}
	g_lcd_ROTBQ.used_count = 0;

	while(left_size >= bq_size)
	{
		g_lcd_ROTBQ.buffers[g_lcd_ROTBQ.count].addr = pBQaddr;
		g_lcd_ROTBQ.buffers[g_lcd_ROTBQ.count].in_used = 0;
		g_lcd_ROTBQ.count++;
		g_lcd_ROTBQ.free_count++;
		pBQaddr += bq_size;
		left_size -= bq_size;
	};

	LCDLOGI("Info: create_ROT_BufQueue: Create %d ROT BQs and %d buffer left!\r\n",
		g_lcd_ROTBQ.count, left_size);

	for(i = 0; i< g_lcd_ROTBQ.count; i++)
	{
		LCDLOGD("DBG: buffer_queue[%d] = 0x%x\r\n", i, g_lcd_ROTBQ.buffers[i].addr);
	}
	return 0;
}

static void release_ROT_BufQueue(void)
{
	int i;

	LCDLOGI("Info: release_ROT_BufQueue: release %d ROT BQs!\r\n", g_lcd_ROTBQ.count);

	for(i=0; i<MAX_ROTBQ_NUM; i++){
		g_lcd_ROTBQ.buffers[i].addr = 0;
		g_lcd_ROTBQ.buffers[i].in_used = 0;
	}
	g_lcd_ROTBQ.count = 0;
	g_lcd_ROTBQ.used_count = 0;
	g_lcd_ROTBQ.free_count = 0;
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
		LCDLOGW("Warning: dequeue_ROT_BQs: BQ is all in used!!\r\n");
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

static int refresh_lcd(struct s_img_layer_setting *img_setting,
	struct s_osd_layer_setting *osd_setting, struct asrlcdd_roi_info *dest_roi)
{
	struct s_layer_setting *layer = NULL;
	struct s_lcd_ctx *lcd = g_plcd;
	struct panel_spec *panel = g_ppanel;
	unsigned int addr = 0;
	int ret;
	LCDLOGD("refresh_lcd: image layer %d, osd layer %d\r\n",img_setting->layer_en,osd_setting->layer_en);

	
	if(g_lcd_status == LCD_STATUS_SLEEP){
		LCDLOGI("INFO: need to sleep, not refresh this frame!\n");
		return 0;
	}

	g_flip_pending = 1;
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
			layer->src_buffer[1].stride = img_setting->stride[1]; 
		}
		if(img_setting->planes == 3){
			layer->src_buffer[2].addr = img_setting->addr[2];
			layer->src_buffer[2].width = img_setting->src_rect.width;
			layer->src_buffer[2].height = img_setting->src_rect.height;
			layer->src_buffer[2].stride = img_setting->stride[2];
		}
	
		layer->dest_point.x = dest_roi->startX - img_setting->dest_rect.startX;
		layer->dest_point.y = dest_roi->startY - img_setting->dest_rect.startY;
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
			layer->layer_alpha = 0x80;
		else
			layer->layer_alpha = 0xFF;

		addr = (uint32_t)osd_setting->frame_buffer.pbuffer;
		addr += osd_setting->src_rect.startY * osd_setting->frame_buffer.stride +
			osd_setting->src_rect.startX * osd_setting->frame_buffer.bitdepth/8;

		layer->src_buffer[0].addr = addr;
		layer->src_buffer[0].width = osd_setting->src_rect.width;
		layer->src_buffer[0].height = osd_setting->src_rect.height;
		layer->src_buffer[0].stride = osd_setting->frame_buffer.stride;
		if(1 == img_setting->layer_en)
		{
			layer->dest_point.x = dest_roi->startX;
			layer->dest_point.y = dest_roi->startY;
		}
		else{
			layer->dest_point.x = dest_roi->startX - osd_setting->dest_rect.startX;
			layer->dest_point.y = dest_roi->startY - osd_setting->dest_rect.startY;
		}

		layer->ck_setting.ck_en = 0;
		layer->cbsh_setting.cbsh_en = 0;
		layer->mirror_en = 0;
		lcd_enable_layer(lcd, LCD_LAYER_OSD1);
	} else {
		lcd_disable_layer(lcd, LCD_LAYER_OSD1);
	}

	lcd_disable_layer(lcd, LCD_LAYER_OSD2);
	lcd_update_output_setting(lcd, dest_roi->width, dest_roi->height);

	LCDLOGD("DBG: refresh_lcd: refresh frame!\r\n");
	ret = panel_before_refresh(panel, dest_roi->startX, dest_roi->startY, dest_roi->height, dest_roi->width);
	if(0 != ret){
		LCDLOGE("ERR: refresh_lcd: panel_before_refresh fail\r\n");
	}
#ifdef ASRLCD_DOUBLE_FB
	OSAMutexLock(g_lcd_hwfb_mutex_id,OSA_SUSPEND);
#endif
	ret = lcd_refresh(lcd);
	if(0 != ret){
		LCDLOGE("ERR: refresh_lcd: lcd_refresh fail\r\n");
	}

#ifdef ASRLCD_DOUBLE_FB
	OSAMutexUnlock(g_lcd_hwfb_mutex_id);
#endif

	ret = lcd_after_refresh(lcd);
	if(0 != ret){
		LCDLOGE("ERR: refresh_lcd: lcd_after_refresh fail\r\n");
	}
	
	ret = panel_after_refresh(panel);
	if(0 != ret){
		LCDLOGE("ERR: refresh_lcd: panel_after_refresh fail\r\n");
	}
	g_flip_pending = 0;

	return 0;
}

void lcd_timer_handler(void)
{
ASSERT(0);
#if 0
	struct asrlcdd_message_info lcd_msg;
	OSA_STATUS osa_status;

	g_lcd_timer_started = 0;
	
	if(0 == g_lcd_osd_setting.dirty)		
		return;

	if(g_lcd_status != LCD_STATUS_POWERON)
		return;


	memset(&lcd_msg, 0, sizeof(lcd_msg));
	
	lcd_msg.msg_type= LCD_MSG_TYPE_UPDATE;
	osa_status = OSAMsgQSend(g_lcd_msg_queue_ref, sizeof(lcd_msg), (UINT8*)&lcd_msg, OS_NO_SUSPEND);
	LCD_ASSERT(osa_status == OS_SUCCESS);
#endif
}


void lcd_thread_handler(void* argv)
{
ASSERT(0);
#if 0
	struct asrlcdd_roi_info dest_roi;
	struct s_lcd_ctx *lcd = NULL;
	UINT8 recvMsg[LCD_MESSAGE_Q_SIZE];
	struct asrlcdd_message_info *lcd_msg;
	int ret;
	OSA_STATUS  osaStatus; 
	struct s_img_layer_setting img_setting;
	struct s_osd_layer_setting osd_setting;
	UINT32 msgs_in_queue;
	UINT8 i;
	int skip_flag;

	
	skip_flag = 0;

	LCDLOGI("INFO: lcd_thread_handler +++\n");
	while(0 == g_lcd_task_exit){
		
		if (0 == skip_flag) 
		{
	       	osaStatus = OSAMsgQRecv(g_lcd_msg_queue_ref,recvMsg, LCD_MESSAGE_Q_SIZE, OSA_SUSPEND);
			LCD_ASSERT(osaStatus == OS_SUCCESS);
		}else
		{
			
			// event has got already
			skip_flag = 0;
		}
		if(1 == g_lcd_task_exit){
			LCDLOGI("Info: lcd_thread_handler: need exit thread!\n");
			break;
		}

		lcd_msg = (struct asrlcdd_message_info *)recvMsg;
		lcd = g_plcd;
		LCDLOGI("INFO: lcd_thread_handler event %d +++\n",lcd_msg->msg_type);
		switch(lcd_msg->msg_type){
		case LCD_MSG_TYPE_FILL:
			lcd->bg_color = lcd_msg->bg_color;
			dest_roi.startX = lcd_msg->roi_start& 0xFFFF;
			dest_roi.startY = (lcd_msg->roi_start >> 16) & 0xFFFF;
			dest_roi.width = lcd_msg->roi_size& 0xFFFF;
			dest_roi.height = (lcd_msg->roi_size >> 16) & 0xFFFF;
			img_setting.layer_en = 0;
			osd_setting.layer_en = 0;
			refresh_lcd(&img_setting, &osd_setting, &dest_roi);
			break;
		case LCD_MSG_TYPE_UPDATE:
		    osaStatus = OSAMsgQPoll(g_lcd_msg_queue_ref, &msgs_in_queue);
		    ASSERT(OS_SUCCESS == osaStatus);
			for (i = 0; i < msgs_in_queue; i++)
			{
				osaStatus = OSAMsgQRecv(g_lcd_msg_queue_ref,recvMsg, LCD_MESSAGE_Q_SIZE, OSA_NO_SUSPEND);
				if (LCD_MSG_TYPE_FILL == ((struct asrlcdd_message_info *)recvMsg)->msg_type) {
					skip_flag = 1;
					break;
				}
			}
			OSAMutexLock(g_lcd_mutex_ref, OSA_SUSPEND);

			memcpy(&img_setting, &g_lcd_img_setting, sizeof(struct s_img_layer_setting));
			memcpy(&osd_setting, &g_lcd_osd_setting, sizeof(struct s_osd_layer_setting));
			g_lcd_img_setting.dirty = 0;
			g_lcd_osd_setting.dirty = 0;
			OSAMutexUnlock(g_lcd_mutex_ref);
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
			

			if((1 == img_setting.layer_en) && (1 == osd_setting.layer_en)){
				
				dest_roi.startX = 0;
				dest_roi.startY = 0;
				dest_roi.width  = 240;
				dest_roi.height = 320;
				osd_setting.src_rect.startX = 0;
				osd_setting.src_rect.startY = 0;
				osd_setting.src_rect.width = 240;
				osd_setting.src_rect.height = 320;
				refresh_lcd(&img_setting, &osd_setting, &dest_roi);
			} else if(1 == img_setting.layer_en){
				refresh_lcd(&img_setting, &osd_setting, & img_setting.dest_rect);
			} else if(1 == osd_setting.layer_en){
				refresh_lcd(&img_setting, &osd_setting, &osd_setting.dest_rect);
			} else {
				LCDLOGE("ERROR: lcd_thread_handler: No layer enabled!!\n");
				continue;
			}

			if(img_setting.layer_en == 1){
				ret = enqueue_ROT_BQs((void*)(img_setting.addr[0]));
				if(0 != ret){
					LCDLOGE("ERR: lcd_thread_handler:enqueue buffer fail\r\n");
				}
			}else {
				if((0 != g_lcd_ROTBQ.count) && (g_release_bq == 2)){
					release_ROT_BufQueue();
					if(g_lcd_cbs[ASRLCDD_CB_TYPE_BUFDONE] != NULL){
						g_lcd_cbs[ASRLCDD_CB_TYPE_BUFDONE](g_lcd_cb_params[ASRLCDD_CB_TYPE_BUFDONE], NULL);
					}
					g_last_ROT_buffer = NULL;
					g_release_bq = 0;
				}
			}
			break;
		default:
			LCDLOGE("ERROR: lcd_thread_handler: Invalid msg type (%d)\n", lcd_msg->msg_type);
			continue;
    	}
		LCDLOGD("DBG: lcd_thread_handler: evet (%d) finish!\n", lcd_msg->msg_type);
	}
	
	osaStatus = OSAFlagSet(g_lcd_flag_ref, LCD_EVNET_CLOSE, OSA_FLAG_OR);
	LCD_ASSERT(osaStatus == OS_SUCCESS);
	
	LCDLOGI("INFO: lcd_thread_handler ---\n");
#endif
}


static void ASRLCC_FillRect16_Dummy()
{
	struct asrlcdd_roi_info roi;
	struct panel_spec *panel = g_ppanel;

	roi.startX = 0;
    roi.startY = 0;
    roi.width = panel->width;
    roi.height = panel->height;

    ASRLCDD_FillRect16(&roi, 0);

}

static int delete_asrlcd_context()
{
    UINT32   actual_flags = 0;
	OSA_STATUS osaStatus;

	LCDLOGI("INFO: delete_asrlcd_task +++\n");
	if(NULL == g_lcd_task_ref){
		LCDLOGI("%s: task has been deleted!\n", __func__);
		return 0;
	}

	g_lcd_task_exit = 1;
	ASRLCC_FillRect16_Dummy();

  	osaStatus = OSAFlagWait(g_lcd_flag_ref, LCD_EVNET_CLOSE, OSA_FLAG_OR_CLEAR, &actual_flags, OSA_SUSPEND);		
	LCD_ASSERT(osaStatus == OS_SUCCESS);

	OSAMutexDelete(g_lcd_mutex_ref);
	OSAMsgQDelete(g_lcd_msg_queue_ref);
	OSAFlagDelete(g_lcd_flag_ref);
	OSATaskDelete(g_lcd_task_ref);

#ifdef ASRLCD_DOUBLE_FB
	OSAMutexDelete(g_lcd_hwfb_mutex_id);
#endif

	g_lcd_mutex_ref = NULL;
	g_lcd_msg_queue_ref = NULL;
	g_lcd_flag_ref = NULL;
	g_lcd_task_ref = NULL;

	LCDLOGI("INFO: delete_asrlcd_task ---\n");
	return 0;
}

#ifdef ASR_LCD_CAMERA_ENABLE
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
	cam_buffer->stride= lcd_buffer->width; 
	cam_buffer->pixelformat = (SCALER_INPUT_FMT)switch_lcdformat_to_camformat(lcd_buffer->format);
	cam_buffer->plane_addr[0] = lcd_buffer->addr[0];
	if(cam_buffer->num_planes > 1)
		cam_buffer->plane_addr[1] = lcd_buffer->addr[1];
	if(cam_buffer->num_planes > 2)
		cam_buffer->plane_addr[2] = lcd_buffer->addr[2];
	LCDLOGD("DBG: (%d) buf_src.addr = 0x%x, 0x%x, 0x%x\r\n", cam_buffer->num_planes, cam_buffer->plane_addr[0],
		cam_buffer->plane_addr[1], cam_buffer->plane_addr[2]);
}

static int lcd_do_rotation(struct cam_offline_buf *src_buffer, struct cam_offline_buf *dst_buf, 
	unsigned int rotation, char *dest_buffer)
{
	int ret;
	int divh = 1, divs = 1;
	unsigned int lcd_format;

	LCDLOGI("Info: lcd_do_rotation +++ (%d)!\r\n", rotation);

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
	dst_buf->stride= dst_buf->width;
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

	ret = cam_offline_rotation(src_buffer, dst_buf, (enum offline_rot)rotation, MODULE_DISPLAY);
	if(0 != ret){
		LCDLOGE("ERROR:lcd_do_rotation: rotation fail!\r\n");
		return -1;
	}
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
	dst_buf->stride= dst_buf->width; 
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
	return 0;
}

static int lcd_do_memcpy(struct asrlcdd_camera_buffer *psrc_buffer, 
	struct asrlcdd_camera_buffer *pdst_buffer, char *dest_buffer)
{
	char *addr = NULL;
	unsigned int size = 0;
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
	memcpy((void*)addr, (void*)(psrc_buffer->addr[0]), size);
	CacheCleanMemory((void*)addr, size);
	LCDLOGD("DBG: copy buffer from	0x%x to 0x%x\r\n", (int)psrc_buffer->addr[0], (int)addr);
	pdst_buffer->addr[0] = (unsigned int)addr;
	pdst_buffer->stride[0] = psrc_buffer->stride[0];
	
	if(psrc_buffer->planes > 1){
		addr += size;
		size = (psrc_buffer->height / divh) * psrc_buffer->stride[1];
		memcpy((void*)addr, (void*)(psrc_buffer->addr[1]), size);
		CacheCleanMemory((void*)addr, size);
		LCDLOGD("DBG: copy buffer from	0x%x to 0x%x\r\n", (int)psrc_buffer->addr[1], (int)addr);
		pdst_buffer->addr[1] = (unsigned int)addr;
		pdst_buffer->stride[1] = psrc_buffer->stride[1];
		
		if(psrc_buffer->planes > 2){
			addr += size;
			size = (psrc_buffer->height / divh) * psrc_buffer->stride[2];
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
	
	LCDLOGI("Info: lcd_do_rotation_scaling +++ (%d, %d, %d)!\r\n", rotation, dst_width, dst_height);

	ret = dequeue_ROT_BQs((void**)&dest_buffer);
	count = 0;
	while(0 != ret){
		count++;
		if(count >= 100){
			LCDLOGE("ERROR: lcd_do_rotation_scaling: can't get rot bqs!\r\n");
			return -1;
		}				
		OSATaskSleep(5);
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
			OSATaskSleep(5);
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
			ret = lcd_do_scaling(&buf_src, &buf_temp, dst_height, dst_width, temp_buffer);
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
	dump_camerabuffer(pdst_buffer);

	LCDLOGI("Info: lcd_do_rotation ---!\r\n");
	return 0;
}
#endif

int lcd_enter_d2_cb(void)
{
	LCDLOGI("Info: lcd_enter_d2_cb +++!\r\n");
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
	LCDLOGI("Info: lcd_enter_d2_cb ---!\r\n");
	return 0;
}

static int lcd_enter_c1_cb(void)
{
	LCDLOGI("Info: lcd_enter_c1_cb +++!\r\n");

	if(g_flip_pending == 1)
		return 0;
	else
		return 1;
	LCDLOGI("Info: lcd_enter_c1_cb ---!\r\n");
}
int lcd_exit_d2_cb(BOOL ExitFromD2)
{
	LCDLOGI("Info: lcd_exit_d2_cb +++!\r\n");
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
	panel_reset(g_ppanel, LCD_SCLK_FREQ, LCD_WORK_MODE_INTERRUPT);
#else
	panel_reset(g_ppanel, LCD_SCLK_FREQ, LCD_WORK_MODE_POLLING);
#endif
	g_lcd_status = LCD_STATUS_SLEEP;
#else
    if(ExitFromD2)
		g_lcd_status = LCD_STATUS_POWEROFF;
#endif
	LCDLOGI("Info: lcd_exit_d2_cb ---!\r\n");
	return 0;
}

static int lcd_exit_c1_cb(void)
{
	LCDLOGI("Info: lcd_exit_c1_cb !\r\n");
	
	return 0;
}
int ASRLCDD_Close(void)
{
	ASSERT(0);
}

int ASRLCDD_Sleep(void)
{
	ASSERT(0);
}

int ASRLCDD_WakeUp(void)
{
	ASSERT(0);
}

int ASRLCDD_Blit16(struct asrlcdd_framebuffer_window *frame_window,
	uint16_t startX, uint16_t startY)
{
	ASSERT(0);
	return 0;
}

int ASRLCDD_Blit16_sync(struct asrlcdd_framebuffer_window *frame_window,
	uint16_t startX, uint16_t startY)
{
	return 0;
}

int ASRLCDD_FillRect16(struct asrlcdd_roi_info *roi_info, unsigned int bgColor)
{
	ASSERT(0);
	return 0;
}
int ASRLCDD_SetBrightness(uint32_t brightness)
{
	LCDLOGI("Info: ASRLCDD_SetBrightness ++ (%d)\r\n", brightness);
	backlight_set_brightness(brightness);
	return 0;
}

struct panel_spec * ASRLCDD_GetPanelInfo(void)
{
	return g_ppanel;
}


/*********Code for logo mode********/
int ASRLCDD_Logo_Open(void)
{
	struct panel_spec *panel = NULL;
	struct s_lcd_ctx *lcd = NULL;
	int ret = -1;
	unsigned int id;

	LCDLOGI("Info: ASRLCDD_Logo_Open +++\r\n");

	if(g_lcd_status != LCD_STATUS_POWEROFF){
		LCDLOGW("Info: ASRLCDD_Logo_Open: asrlcd has been opened!\r\n");
		return 0;
	}

	lcd_env_prepare();
	lcd_ass_poweron();
	g_lcd_status = LCD_STATUS_HWON;
	
	lcd = (struct s_lcd_ctx*)lcd_init1(LCD_WORK_MODE_ASS_POLLING);
	if(NULL == lcd){
		LCDLOGE("ERR: ASRLCDD_Logo_Open: lcd_init1 fail\r\n");
		ASRLCDD_Logo_Close();
		return -1;
	}
	g_plcd = lcd;
//*****************************************************
#ifndef LCD_ADAPT_PANEL		
	panel = get_panel_info();

	if(NULL == panel){
		LCDLOGE("ERR: ASRLCDD_Logo_Open: get_panel_info fail\r\n");
		ASRLCDD_Logo_Close();
		return -1;
	}
	g_ppanel = panel;
	LCDLOGI("ASRLCDD_Logo_Open step3 \r\n");

	ret = panel_init(panel, LCD_SCLK_FREQ, LCD_WORK_MODE_ASS_POLLING);

	id = panel->ops->panel_readid(panel);
	if(id != panel->panel_id){
		LCDLOGE("[READ_ID]ERR: ASRLCDD_Logo_Open expect panel_id = 0x%x, read panel_id = 0x%x\r\n",panel->panel_id,id);
		ASRLCDD_Logo_Close();
		return -1;
	}	
#else	//#ifndef LCD_ADAPT_PANEL
	get_panel_info_list();

	ret = panel_init(&g_ppanel, LCD_SCLK_FREQ, LCD_WORK_MODE_POLLING);	
	panel = g_ppanel;
#endif //#ifndef LCD_ADAPT_PANEL
//******************************************************
	
	if(0 != ret){
		LCDLOGE("ERR: ASRLCDD_Logo_Open: panel_init fail\r\n");
		ASRLCDD_Logo_Close();
		return -1;
	}

	lcd->bg_color = 0xFF;
	lcd->alpha_mode = LCD_ALPHA_MODE_NORMAL;
	lcd->gamma_en = 0;
	lcd->dither_setting.dither_en = 0;
	lcd->dither_setting.dither_mode = LCD_DITHER_4X8;
	lcd->wb_setting.wb_en = 0;

	ret = lcd_output_setting(lcd, panel);
	if(0 != ret){
		LCDLOGE("ASRLCDD_Logo_Open: lcd_output_setting fail\r\n");
		ASRLCDD_Logo_Close();
		return -1;
	}

	LCDLOGI("Info: ASRLCDD_Logo_Open ---\r\n");
	
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
		panel_uninit(g_ppanel);
		g_ppanel = NULL;
	}
	if(g_lcd_status != LCD_STATUS_POWEROFF){
		lcd_ass_poweroff();
		g_lcd_status = LCD_STATUS_POWEROFF; 	
	}
	for(i=0;i<ASRLCDD_CB_TYPE_MAX;i++){
		g_lcd_cbs[i] = NULL;
		g_lcd_cb_params[i] = NULL;
	}

	LCDLOGI("INFO: ASRLCDD_Logo_Close ---\n");
	return 0;
}

int ASRLCDD_Ass_Blit16_sync(struct asrlcdd_framebuffer_window *frame_window,
	uint16_t startX, uint16_t startY)
{
	struct s_osd_layer_setting osd_setting;  
	struct s_img_layer_setting img_setting;  

	if(NULL == frame_window){
		LCDLOGE("ERR: ASRLCDD_Ass_Blit16_sync: Invalid param\r\n");
		return -1;
	}

	LCDLOGI("Info: ASRLCDD_Ass_Blit16_sync +++ (%d, %d, %d, %d)\r\n", frame_window->roi_info.startX, 
		frame_window->roi_info.startY, frame_window->roi_info.width, frame_window->roi_info.height);

	if(frame_window->frame_info.pbuffer == NULL){ 
		LCDLOGE("ERR: ASRLCDD_Ass_Blit16_sync: Invalid param\r\n");
		return -1;
	}

	if((g_lcd_status != LCD_STATUS_POWERON) &&(g_lcd_status != LCD_STATUS_HWON)){
		LCDLOGE("ERR: ASRLCDD_Ass_Blit16_sync: Invalid lcd status!\r\n");
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
		LCDLOGE("ERR: ASRLCDD_Ass_Blit16_sync: Invalid bitdepth (%d)!\r\n", frame_window->frame_info.bitdepth);
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

	refresh_lcd(&img_setting, &osd_setting, &osd_setting.dest_rect);

	return 0;
}
