#include <ui_os_api.h>
#include "ui_mem_api.h"
#include "../cam_fe.h"
#include "../cam_pipeline.h"
#include "intc_api.h"
#include "cam_pipeline_reg.h"

#include "../../offline_drv/jpeg_drv.h"
#include "plat_basic_api.h"
#include "../../cam-hal/camera_jpeg_encoder.h"
#include "task_cnf.h"

struct cam_isp_pipeline cam_pipeline_cfg;
struct isp_buf_mgr cam_buf_mgr;
unsigned int cam_drv_inited = 0;
static OS_HISR	cam_hisr_ref;

INT32 Os_Activate_HISR(VOID** hisr);
void Os_Create_HISR(VOID** hisr, char* name, VOID (*hisr_entry)(VOID), unsigned char priority);

#define BITS_PER_LONG 32
static unsigned long __ffs(unsigned long word)
{
	int num = 0;

#if BITS_PER_LONG == 64
	if ((word & 0xffffffff) == 0) {
		num += 32;
		word >>= 32;
	}
#endif
	if ((word & 0xffff) == 0) {
		num += 16;
		word >>= 16;
	}
	if ((word & 0xff) == 0) {
		num += 8;
		word >>= 8;
	}
	if ((word & 0xf) == 0) {
		num += 4;
		word >>= 4;
	}
	if ((word & 0x3) == 0) {
		num += 2;
		word >>= 2;
	}
	if ((word & 0x1) == 0)
		num += 1;
	return num;
}

struct isp_buf_mgr * get_isp_buf_mgr(void)
{
	return &cam_buf_mgr;
}

struct cam_isp_pipeline *get_isp_pipe_mgr(void)
{
	return &cam_pipeline_cfg;
}

struct pipeline_param *get_pipe_mgr_cfg(uint_8 pipe_id,struct cam_isp_pipeline * pipe_mgr)
{
	if(pipe_id == PIPELINE_ID_PREV)
		return &pipe_mgr->pipe0_cfg;
	else if(pipe_id == PIPELINE_ID_VIDEO)
		return &pipe_mgr->pipe1_cfg;
	else
		return &pipe_mgr->pipe2_cfg;
}

int all_port_disable(struct cam_isp_pipeline * pipe_mgr)
{
	if(pipe_mgr->pipe0_cfg.pipeline_enable == 0 && pipe_mgr->pipe1_cfg.pipeline_enable == 0 && pipe_mgr->pipe2_cfg.pipeline_enable == 0)
		return 1;
	else
		return 0;
}

int cam_init_bufq(struct isp_buf_mgr *buf_mgr,struct isp_buf_request *buf_request)
{
	int i=0;
	int num_buf = buf_request->num_buf;
	uint_8 pipeline_id = buf_request->pipe_id;
	struct isp_buf_queue *buf_queue = &buf_mgr->bufque[pipeline_id];
	if(num_buf<=0 || num_buf>ISP_QBUF_MAX_NUM){
		CAMLOGE("cam_init_bufq.err num_buf,pipeline_id :%d,num_buf:%d",pipeline_id,num_buf);
		return -1;
	}

	if(pipeline_id>PIPELINE_ID_CAP){
		CAMLOGE("cam_init_bufq.err pipeid");
		return -1;
	}
	CAMLOGV("cam_init_bufq.pipeline_id :%d,num_buf:%d,msgQRef:%d",pipeline_id,num_buf,buf_request->msgQRef);
	UOS_TakeMutex(buf_queue->queue_lock);
	buf_queue->pipe_id = pipeline_id;
	buf_queue->req_buf_num = num_buf;
	for(i=0;i<ISP_QBUF_MAX_NUM;i++)
	{
		memset(&buf_mgr->bufque[pipeline_id].bufinfo[i], 0, sizeof(struct isp_qbuf_buffer_info));
		if(i<num_buf){
			buf_mgr->bufque[pipeline_id].bufinfo[i].buf_idx = i;
			buf_mgr->bufque[pipeline_id].bufinfo[i].parqueue = buf_queue;
			buf_mgr->bufque[pipeline_id].bufinfo[i].buf_status = ISP_STATS_BUFSTS_INITED;
			buf_mgr->bufque[pipeline_id].bufinfo[i].buf_flag = ISP_STATS_BUFSTS_INITED;
			INIT_LIST_HEAD(&buf_mgr->bufque[pipeline_id].bufinfo[i].hook);
		}
	}

	INIT_LIST_HEAD(&buf_queue->idle_buf);
	INIT_LIST_HEAD(&buf_queue->busy_buf);
	buf_queue->busy_buf_cnt = 0;
	buf_queue->idle_buf_cnt= 0;
	buf_queue->msgQRef = buf_request->msgQRef;
	UOS_ReleaseMutex(buf_queue->queue_lock);
	return 0;
}

int cam_pipe_setzoom(uint_8 pipe_id_mask, uint_16 zoomValue)
{
	int i = 0;
	struct cam_isp_pipeline * pipe_mgr = get_isp_pipe_mgr();
	unsigned long	map = pipe_id_mask;
	uint_8 bit = 0, pipe_id = 0;

	for (i = 0; i < PIPELINE_ID_MAX; i++) {
		if(map == 0)
		    break;
		bit = __ffs(map);
		if(bit >=PIPELINE_ID_MAX)
			break;
		map &= ~(1 << bit);
		pipe_id = bit;

		set_pipeline_target_zoomratio(pipe_mgr, pipe_id, zoomValue);
	}

	return 0;
}

int cam_buf_enqueue(struct isp_buf_mgr *buf_mgr,struct isp_buf_info *info)
{
	uint_8 pipe_id = info->pipe_id;
	int32_t buf_idx = info->buf_idx;
	struct isp_qbuf_buffer_info* bufinfo = NULL;
	struct isp_buf_queue *buf_queue = &buf_mgr->bufque[pipe_id];

	if(pipe_id>PIPELINE_ID_CAP){
		CAMLOGE("cam_buf_enqueue.err pipeid");
		return -1;
	}
	if(buf_idx>buf_queue->req_buf_num || buf_idx<0){
		CAMLOGE("cam_buf_enqueue.err buf_idx");
		return -1;
	}

	CAMLOGV("cam_buf_enqueue.pipeline_id :%d,buf_idx:%d.", pipe_id, buf_idx);
	UOS_TakeMutex(buf_queue->queue_lock);
	bufinfo = &buf_mgr->bufque[pipe_id].bufinfo[buf_idx];
	bufinfo->buf = info->buffer;
	bufinfo->buf_status = ISP_STATS_BUFSTS_IDLE;

	bufinfo->buf_idx = buf_idx;

#ifdef ISP_USE_HARDWARE_TIMER
	//two buffers running no need list.
#else
	list_add_tail(&bufinfo->hook, &buf_queue->idle_buf);
	buf_queue->idle_buf_cnt++;
#endif
	bufinfo->buf_flag = ISP_STATS_BUFSTS_IDLE;
	UOS_ReleaseMutex(buf_queue->queue_lock);
	return 0;
}

int cam_buf_dequeue(struct isp_buf_mgr *buf_mgr,struct isp_buf_info *info)
{
	uint_8 pipe_id = info->pipe_id;
	uint_8 buf_idx = info->buf_idx;
	struct isp_qbuf_buffer_info* bufinfo = NULL;
	struct isp_buf_queue *buf_queue = &buf_mgr->bufque[pipe_id];
	bufinfo = &buf_mgr->bufque[pipe_id].bufinfo[buf_idx];

	if(pipe_id>PIPELINE_ID_CAP){
		CAMLOGE("cam_buf_dequeue.err pipeid");
		return -1;
	}
	if(buf_idx>buf_queue->req_buf_num /*|| buf_idx<0*/){
		CAMLOGE("cam_buf_dequeue.err buf_idx");
		return -1;
	}
	CAMLOGV("cam_buf_dequeue.pipeline_id :%d,buf_idx:%d.",pipe_id,buf_idx);

	UOS_TakeMutex(buf_queue->queue_lock);
	/*fill info for hal*/
	info->metadata = bufinfo->metadata;
#ifdef ISP_USE_HARDWARE_TIMER
	//nothing to do.
#else
	/*clear bufinfo in queue*/
	memset(&bufinfo->buf , 0, sizeof(struct isp_qbuf_buffer));
	memset(&bufinfo->metadata , 0, sizeof(struct isp_buf_eof_metadata));
	bufinfo->buf_idx = -1;
#endif
	bufinfo->buf_status = ISP_STATS_BUFSTS_UNPAPRED;
	bufinfo->buf_flag = ISP_STATS_BUFSTS_INITED;
	UOS_ReleaseMutex(buf_queue->queue_lock);
	return 0;
}

int cam_buf_flush(struct isp_buf_mgr *buf_mgr ,uint_8 pipe_id_mask)
{
	//flush buf.make sure hw can not use this buf anymore.
	unsigned long	map = pipe_id_mask;
	uint_8 bit=0;
	int i=0;
	int ret=0;
	struct cam_isp_pipeline * pipe_mgr = get_isp_pipe_mgr();
	//struct pipeline_param *pipe_mgr_cfg = NULL;
	uint_8 pipe_id=0;

	struct isp_buf_queue *buf_queue = NULL;
	//1, stop stream
	//cam_pipe_stream_off(pipe_id_mask, 1);  // we had streamoff before flush buffer

	//2,flush bufqueue
	for (i = 0; i < PIPELINE_ID_MAX; i++) {
		if(map == 0)
		    break;
		bit = __ffs(map);
		if(bit >=PIPELINE_ID_MAX)
			break;
		map &= ~(1 << bit);
		pipe_id = bit;
		buf_queue = &buf_mgr->bufque[pipe_id];
		CAMLOGV("cam_buf_flush.pipe_id :%d!",pipe_id);
		UOS_TakeMutex(buf_queue->queue_lock);
		for(i=0;i<ISP_QBUF_MAX_NUM;i++)
		{
			memset(&buf_mgr->bufque[pipe_id].bufinfo[i], 0, sizeof(struct isp_qbuf_buffer_info));
			buf_mgr->bufque[pipe_id].bufinfo[i].buf_idx = -1;
		}
		buf_queue->req_buf_num = 0;
		INIT_LIST_HEAD(&buf_queue->idle_buf);
		INIT_LIST_HEAD(&buf_queue->busy_buf);
		buf_queue->busy_buf_cnt = 0;
		buf_queue->idle_buf_cnt= 0;
		//buf_queue->msgQRef = NULL;
		UOS_ReleaseMutex(buf_queue->queue_lock);
	}
	return ret;
}

struct isp_qbuf_buffer_info  *isp_get_buffer_from_bufque(struct isp_buf_mgr *buf_mgr,uint32_t pipe_id)
{
	int i = 0;
	struct isp_qbuf_buffer_info* bufinfo = NULL;
	struct isp_buf_queue *buf_queue = &buf_mgr->bufque[pipe_id];

	for (i = 0; i < buf_queue->req_buf_num; i++) {
		bufinfo = &buf_queue->bufinfo[i];
		if (bufinfo->buf_flag == ISP_STATS_BUFSTS_IDLE) {
			bufinfo->buf_flag = ISP_STATS_BUFSTS_PREPARED;
			return bufinfo;
		}
	}

	return NULL;
}

int isp_put_buffer_to_bufque(struct isp_qbuf_buffer_info* bufinfo)
{
	if (!bufinfo) {
		CAMLOGE("invalid paramter!");
		return -1;
	}

	bufinfo->buf_flag = ISP_STATS_BUFSTS_IDLE;
	return 0;
}

struct isp_qbuf_buffer_info  *isp_get_idle_buffer(struct isp_buf_mgr *buf_mgr,uint32_t pipe_id)
{
	struct isp_qbuf_buffer_info* bufinfo = NULL;
	struct isp_buf_queue *buf_queue = &buf_mgr->bufque[pipe_id];

	UOS_TakeMutex(buf_queue->queue_lock);

	if (buf_queue->idle_buf_cnt) {
		bufinfo = list_first_entry(&buf_queue->idle_buf,struct isp_qbuf_buffer_info, hook);
		list_del_init(&bufinfo->hook);
		buf_queue->idle_buf_cnt--;
	}

	UOS_ReleaseMutex(buf_queue->queue_lock);
	return bufinfo;
}

int isp_put_idle_buffer(struct isp_buf_mgr *buf_mgr,uint32_t pipe_id,
				struct isp_qbuf_buffer_info* bufinfo)
{
	struct isp_buf_queue *buf_queue = &buf_mgr->bufque[pipe_id];

	if (bufinfo == NULL || !list_empty(&bufinfo->hook)) {
		CAMLOGV("isp_put_idle_buffer: error! return!.pipe_id:%d", pipe_id);
		return -1;
	}

	UOS_TakeMutex(buf_queue->queue_lock);
	list_add_tail(&bufinfo->hook, &buf_queue->idle_buf);
	buf_queue->idle_buf_cnt++;
	UOS_ReleaseMutex(buf_queue->queue_lock);
	return 0;
}


struct isp_qbuf_buffer_info *isp_get_idle_buffer_by_index(struct isp_buf_mgr *buf_mgr,uint32_t pipe_id,uint32_t buf_idx)
{
	int reqbufnum = buf_mgr->bufque[pipe_id].req_buf_num;
	struct isp_qbuf_buffer_info* bufinfo = NULL;
	struct isp_buf_queue *buf_queue = &buf_mgr->bufque[pipe_id];
	struct list_head *list_i;

	CAMLOGV("isp_get_idle_buffer_by_index.index:%d,reqbufnum:%d,pipe_id:%d",buf_idx,reqbufnum,pipe_id);
	if(buf_idx > reqbufnum /*||buf_idx<0*/)
	{
		return NULL;
	}

	UOS_TakeMutex(buf_queue->queue_lock);
#if 0
	list_for_each_entry(bufinfo, &buf_queue->idle_buf, hook) {
		if (buf_idx <= 0)
			break;
		buf_idx--;
	}
#else
	list_for_each(list_i, &buf_queue->idle_buf){
		bufinfo = list_entry(list_i,struct isp_qbuf_buffer_info,hook);
		if (buf_idx <= 0)
			break;
		buf_idx--;
	}
#endif

	UOS_ReleaseMutex(buf_queue->queue_lock);
	return bufinfo;
}

struct isp_qbuf_buffer_info  *isp_get_busy_buffer(struct isp_buf_mgr *buf_mgr,uint32_t pipe_id)
{
	struct isp_qbuf_buffer_info* bufinfo = NULL;
	struct isp_buf_queue *buf_queue = &buf_mgr->bufque[pipe_id];

#ifndef ISP_USE_HARDWARE_TIMER
	UOS_TakeMutex(buf_queue->queue_lock);
#endif

	if (buf_queue->busy_buf_cnt) {
		bufinfo = list_first_entry(&buf_queue->busy_buf,
			struct isp_qbuf_buffer_info, hook);
		list_del_init(&bufinfo->hook);
		buf_queue->busy_buf_cnt--;
	}

#ifndef ISP_USE_HARDWARE_TIMER
	UOS_ReleaseMutex(buf_queue->queue_lock);
#endif

	return bufinfo;
}

//bufqueType, 0:idle, 1:busy.
int isp_get_buffer_count(struct isp_buf_queue *buf_queue, int bufqueType)
{
//	unsigned long flags;
	int bufCnt = 0;

	UOS_TakeMutex(buf_queue->queue_lock);

	if (bufqueType)
		bufCnt = buf_queue->busy_buf_cnt;
	else
		bufCnt = buf_queue->idle_buf_cnt;

	UOS_ReleaseMutex(buf_queue->queue_lock);

	return bufCnt;
}

struct isp_qbuf_buffer_info *isp_find_busy_buffer(struct isp_buf_mgr *buf_mgr,uint32_t pipe_id,
	unsigned char pos, int *pBufCnt)
{
	struct isp_qbuf_buffer_info* bufinfo = NULL;
	struct isp_buf_queue *buf_queue = &buf_mgr->bufque[pipe_id];
	struct list_head *list_i;

#ifndef ISP_USE_HARDWARE_TIMER
	UOS_TakeMutex(buf_queue->queue_lock);
#endif

	*pBufCnt = buf_queue->busy_buf_cnt;
	if (pos >= buf_queue->busy_buf_cnt) {
	#ifndef ISP_USE_HARDWARE_TIMER
		UOS_ReleaseMutex(buf_queue->queue_lock);
	#endif
		return NULL;
	}
#if 0
	list_for_each_entry(bufinfo, &buf_queue->busy_buf, hook) {
		if (pos <= 0)
			break;
		pos--;
	}
#else
	list_for_each(list_i, &buf_queue->busy_buf){
		bufinfo = list_entry(list_i,struct isp_qbuf_buffer_info,hook);
		if (pos <= 0)
			break;
		pos--;
	}
#endif

#ifndef ISP_USE_HARDWARE_TIMER
	UOS_ReleaseMutex(buf_queue->queue_lock);
#endif
	return bufinfo;
}

int isp_put_busy_buffer(struct isp_buf_mgr *buf_mgr,uint32_t pipe_id,
				struct isp_qbuf_buffer_info* bufinfo)
{
	struct isp_buf_queue *buf_queue = &buf_mgr->bufque[pipe_id];

	if (bufinfo == NULL || !list_empty(&bufinfo->hook))
		return -1;

	//no need to mutext because this function is only called in interrupt handler.
#ifndef ISP_USE_HARDWARE_TIMER	
	UOS_TakeMutex(buf_queue->queue_lock);
#endif

	list_add_tail(&bufinfo->hook, &buf_queue->busy_buf);
	buf_queue->busy_buf_cnt++;

#ifndef ISP_USE_HARDWARE_TIMER	
	UOS_ReleaseMutex(buf_queue->queue_lock);
#endif

	return 0;
}

int isp_streamon_fill_buf(struct cam_isp_pipeline * pipe_mgr,int  pipe_id)
{
	struct isp_buf_mgr *buf_mgr = pipe_mgr->buf_mgr;
	struct isp_qbuf_buffer_info *buf_info = NULL;

#ifdef ISP_USE_HARDWARE_TIMER
	buf_info = isp_get_buffer_from_bufque(buf_mgr, pipe_id);
#else
	buf_info = isp_get_idle_buffer(buf_mgr,pipe_id);
#endif
	
	if(buf_info == NULL){
		return -1;
	}

	buf_info->buf_status = ISP_STATS_BUFSTS_PREPARED;
	if (pipe_mgr->online_isp_drop_frm_num > 0) {
		buf_info->buf_status = ISP_STATS_BUFSTS_SKIPED;
	}

	CAMLOGV("isp_streamon_fill_buf:pipe_id:%d,buf_idx:%d!",pipe_id,buf_info->buf_idx);
	isp_put_busy_buffer(buf_mgr,pipe_id,buf_info);
	cam_set_addr(pipe_id,buf_info->buf);

	return 0;
}

/*
 * This function contains the following steps:
 * 1.stream off preview(disable p0 port);
 * 2.stream on capture(enable p2 port).
 * Why: Because we need to reuse the preview buffers and reduce the bandwidth of ddr.
 */
int cam_pipe_streamon_capture_off_preview(void)
{
	struct cam_isp_pipeline * pipe_mgr = get_isp_pipe_mgr();
	struct pipeline_param *pipe_mgr_cfg = NULL, *pPreviewPipeCfg = NULL;
	int ret=0;

	pipe_mgr_cfg = get_pipe_mgr_cfg(PIPELINE_ID_CAP, pipe_mgr);
	cam_set_pipe_outfmt(pipe_mgr_cfg);
	cam_set_pipe_jpeg_mode(pipe_mgr_cfg);
	cam_set_pitch_dma_size(pipe_mgr_cfg);

	ret = isp_streamon_fill_buf(pipe_mgr, PIPELINE_ID_CAP);
	if(ret<0){
		return -1;
	}

	pPreviewPipeCfg = get_pipe_mgr_cfg(PIPELINE_ID_PREV, pipe_mgr);

	cam_set_pipe_shadow_mode(pipe_mgr_cfg->pipeline_id, pipe_mgr_cfg->shadow_mode);
	cam_set_pipe_shadow_mode(pPreviewPipeCfg->pipeline_id, pPreviewPipeCfg->shadow_mode);

	pipe_mgr_cfg->pipeline_enable = 1;

	/*Do our best to reduce the gap between disable p0 and enable p2.*/
	if (pPreviewPipeCfg->pipeline_enable) {
		cam_set_pipe_hdl(pPreviewPipeCfg, 0);
		pPreviewPipeCfg->pipeline_enable = 0;
	}

	cam_set_pipe_shadow_ready(PIPELINE_ID_PREV);
	cam_set_pipe_hdl(pipe_mgr_cfg, 1);
	cam_set_pipe_shadow_ready(PIPELINE_ID_CAP);

	pipe_mgr_cfg->dma_state = ISPDMA_ACTIVE;

	return ret;	
}

int cam_pipe_prepare_online_preview_pipe(void)
{
	struct cam_isp_pipeline * pipe_mgr = get_isp_pipe_mgr();
	struct pipeline_param *pipe_mgr_cfg = NULL;
	int ret=0;

	pipe_mgr_cfg = get_pipe_mgr_cfg(PIPELINE_ID_PREV, pipe_mgr);

	set_pipeline_zoom_cfg(pipe_mgr, PIPELINE_ID_PREV, pipe_mgr_cfg->streamon_zoom);
	cam_set_pipe_outfmt(pipe_mgr_cfg);
	cam_set_pipe_jpeg_mode(pipe_mgr_cfg);
	cam_set_pitch_dma_size(pipe_mgr_cfg);
	cam_config_data_range(pipe_mgr_cfg);

	cam_set_pipe_shadow_mode(pipe_mgr_cfg->pipeline_id, pipe_mgr_cfg->shadow_mode);

	pipe_mgr_cfg->pipeline_enable = 1;

	cam_set_pipe_hdl(pipe_mgr_cfg, 1);

	ret = isp_streamon_fill_buf(pipe_mgr, PIPELINE_ID_PREV);

	pipe_mgr_cfg->pipe_stop_semaphore = INVALID_SEMAPHORE_ID;
	pipe_mgr_cfg->pipe_stop_semaphore = UOS_CreateSemaphore(0);

	return ret;	
}

int cam_pipe_prepare_online_capture_pipe(void)
{
	struct cam_isp_pipeline * pipe_mgr = get_isp_pipe_mgr();
	struct pipeline_param *pipe_mgr_cfg = NULL;
	int ret=0;

	pipe_mgr_cfg = get_pipe_mgr_cfg(PIPELINE_ID_CAP, pipe_mgr);

	//online jpeg stride must be 128 align
	if((pipe_mgr_cfg->dma_stride_y % 128) != 0) {
		pipe_mgr_cfg->dma_stride_y = (pipe_mgr_cfg->dma_stride_y / 128 + 1) * 128;
	}

	ret = isp_streamon_fill_buf(pipe_mgr, PIPELINE_ID_CAP);
	cam_set_pipe_outfmt(pipe_mgr_cfg);
	cam_set_pipe_jpeg_mode(pipe_mgr_cfg);
	cam_set_pitch_dma_size(pipe_mgr_cfg);

	cam_set_pipe_shadow_mode(pipe_mgr_cfg->pipeline_id, pipe_mgr_cfg->shadow_mode);

	pipe_mgr_cfg->pipeline_enable = 1;

	cam_set_pipe_hdl(pipe_mgr_cfg, 1);

	return ret;	
}

int cam_pipe_streamon_online_capture(struct isp_qbuf_buffer *buffer)
{
	struct cam_isp_pipeline * pipe_mgr = get_isp_pipe_mgr();
	struct pipeline_param *pipe_mgr_cfg = get_pipe_mgr_cfg(PIPELINE_ID_CAP, pipe_mgr);
	int jpeg_size = 0;
	uint32_t buffer_addr = buffer->planes[0].addr;
	uint32_t buffer_size = 0, i;

	CAMLOGD("streamon online capture");

	for(i = 0; i < buffer->num_planes; i++)
	{
		buffer_size += buffer->planes[i].length;
	}

	//do jpeg encode with starting preview
	jpeg_size = CameraOnlineJpegEncode(pipe_mgr_cfg, buffer_addr, buffer_size, 65, 1);

	return jpeg_size;
}

int cam_pipe_streamoff_online_capture(void)
{
	struct cam_isp_pipeline * pipe_mgr = get_isp_pipe_mgr();
	struct pipeline_param *cap_pipe_mgr_cfg = get_pipe_mgr_cfg(PIPELINE_ID_CAP, pipe_mgr);
	struct pipeline_param *preview_pipe_mgr_cfg = get_pipe_mgr_cfg(PIPELINE_ID_PREV, pipe_mgr);
	OSA_STATUS status = OS_SUCCESS;

	CAMLOGD("streamoff online capture");

	cam_set_pipe_hdl(preview_pipe_mgr_cfg, 0);
	cam_set_pipe_shadow_ready(PIPELINE_ID_PREV);
	preview_pipe_mgr_cfg->pipeline_enable = 0;

	cam_set_pipe_hdl(cap_pipe_mgr_cfg, 0);
	cam_set_pipe_shadow_ready(PIPELINE_ID_CAP);
	cap_pipe_mgr_cfg->pipeline_enable = 0;

	if(INVALID_SEMAPHORE_ID != preview_pipe_mgr_cfg->pipe_stop_semaphore) {
		//timeout unit is tick(5ms),we wait max time is 67msx2,2 fps.
		status = UOS_WaitForSemaphore(preview_pipe_mgr_cfg->pipe_stop_semaphore, 27);
		if (status != OS_SUCCESS) {
			CAMLOGE("wait for %d pipe stop done error %d!", PIPELINE_ID_PREV, status);
		} else
			CAMLOGD("wait for %d pipe stop done!", PIPELINE_ID_PREV);
		UOS_DeleteSemaphore(preview_pipe_mgr_cfg->pipe_stop_semaphore);
		preview_pipe_mgr_cfg->pipe_stop_semaphore = INVALID_SEMAPHORE_ID;
	}

	return 0;
}

/*
 * For Multiple output pipelines, no need to worry about
 * concurrent stream cfg/on/off/flush, because all stream opt must hold
 * pipeline stream_cfg_lock first, so only one thread will
 * reach this switch case
*/
int cam_pipe_stream_on(uint_8 pipe_id_mask)
{
	unsigned long	map = pipe_id_mask;
	uint_8 bit=0;
	int i=0;
	int ret=0;
	struct cam_isp_pipeline * pipe_mgr = get_isp_pipe_mgr();
	struct pipeline_param *pipe_mgr_cfg = NULL;
	SENSOR_CTRL_T *sensor_ctrl = &pipe_mgr->sensor_ctrl;
	uint_8 pipe_id=0;

	for (i = 0; i < PIPELINE_ID_MAX; i++) {
		if(map == 0)
		    break;
		bit = __ffs(map);
		if(bit >=PIPELINE_ID_MAX)
			break;
		map &= ~(1 << bit);
		pipe_id = bit;
		pipe_mgr_cfg = get_pipe_mgr_cfg(pipe_id,pipe_mgr);
        if(pipe_mgr->isp_cfg.isp_inited == 0){
            pipe_mgr->isp_cfg.isp_inited = 1;
		    fe_isp_precess_config(&pipe_mgr->isp_cfg,sensor_ctrl);
        }

#ifdef CAM_FULLSIZE_STREAM
		pipe_mgr_cfg->pipe_stop_semaphore = INVALID_SEMAPHORE_ID;
		pipe_mgr_cfg->pipe_stop_semaphore = UOS_CreateSemaphore(0);
#else
		if (pipe_id!= PIPELINE_ID_CAP) {
			pipe_mgr_cfg->pipe_stop_semaphore = INVALID_SEMAPHORE_ID;
			pipe_mgr_cfg->pipe_stop_semaphore = UOS_CreateSemaphore(0);
		}
#endif
		set_pipeline_zoom_cfg(pipe_mgr,pipe_id,pipe_mgr_cfg->streamon_zoom);
		set_pipeline_cur_zoomratio(pipe_mgr,pipe_id,pipe_mgr_cfg->streamon_zoom);
		set_pipeline_target_zoomratio(pipe_mgr,pipe_id,pipe_mgr_cfg->streamon_zoom);
		//cam_set_pipe_crop(pipe_mgr_cfg);
		//cam_set_pipe_subsmp(pipe_mgr_cfg);
		//cam_set_scaler(pipe_mgr_cfg);
		cam_set_pipe_outfmt(pipe_mgr_cfg);
		cam_set_pipe_jpeg_mode(pipe_mgr_cfg);
		cam_set_pitch_dma_size(pipe_mgr_cfg);
		cam_config_data_range(pipe_mgr_cfg);
#ifdef CAM_FULLSIZE_STREAM
		cam_set_pipe_shadow_mode(pipe_mgr_cfg->pipeline_id, pipe_mgr_cfg->shadow_mode);
		cam_set_pipe_hdl(pipe_mgr_cfg,1);
#else
		if (pipe_id == PIPELINE_ID_CAP) {
			//p0 and p1 buffer and shadow ready set by isp sof
			ret = isp_streamon_fill_buf(pipe_mgr,pipe_id);
			if(ret<0){
				return -1;
			}
		}
		cam_set_pipe_shadow_mode(pipe_mgr_cfg->pipeline_id, pipe_mgr_cfg->shadow_mode);
		cam_set_pipe_hdl(pipe_mgr_cfg,1);
		if (pipe_id == PIPELINE_ID_CAP)
			cam_set_pipe_shadow_ready(pipe_id);
#endif
		pipe_mgr_cfg->pipeline_enable = 1;

		pipe_mgr_cfg->dma_state = ISPDMA_ACTIVE;

		if(sensor_ctrl->stream_cnt == 0){
			ret = sensor_ctrl->ops->stream_on(sensor_ctrl);
			if (ret < 0)
				return ret;
		}

		//cam_set_pipe_shadow_mode(pipe_mgr_cfg->pipeline_id, pipe_mgr_cfg->shadow_mode );
	}

	CAMLOGI("cam_pipe_stream_on success");
	return ret;
}

int cam_pipe_stream_off(uint_8 pipe_id_mask, uint_32 keep_sensor)
{
	unsigned long	map = pipe_id_mask;
	uint_8 bit=0;
	int i=0, index = 0;
	int ret=0;
	//int wait_cnt = 0;
	struct cam_isp_pipeline * pipe_mgr = get_isp_pipe_mgr();
	struct pipeline_param *pipe_mgr_cfg = NULL;
	SENSOR_CTRL_T *sensor_ctrl = &pipe_mgr->sensor_ctrl;
	uint_8 pipe_id=0;
	OSA_STATUS status = OS_SUCCESS;
	ACC_TIMER_STATUS time_status = ACC_INACTIVE;

	for (i = 0; i < PIPELINE_ID_MAX; i++) {
		if(map == 0)
		    break;
		bit = __ffs(map);
		if(bit >=PIPELINE_ID_MAX)
			break;
		map &= ~(1 << bit);
		pipe_id = bit;
		pipe_mgr_cfg = get_pipe_mgr_cfg(pipe_id,pipe_mgr);

		if (pipe_mgr_cfg->pipeline_enable) {
			cam_set_pipe_hdl(pipe_mgr_cfg,0);
			cam_set_pipe_shadow_ready(pipe_id);
			pipe_mgr_cfg->pipeline_enable = 0;
			/*wait hw is idle*/
#ifdef CAM_FULLSIZE_STREAM
			if (INVALID_SEMAPHORE_ID != pipe_mgr_cfg->pipe_stop_semaphore) {
				//timeout unit is tick(5ms),we wait max time is 2 frames time.
				status = UOS_WaitForSemaphore(pipe_mgr_cfg->pipe_stop_semaphore, pipe_mgr->timeout_value * 2);
				if (status != OS_SUCCESS) {
					CAMLOGE("wait for %d pipe stop done error %d!", pipe_id, status);
				} else
					CAMLOGV("wait for %d pipe stop done!", pipe_id);
				UOS_DeleteSemaphore(pipe_mgr_cfg->pipe_stop_semaphore);
				pipe_mgr_cfg->pipe_stop_semaphore = INVALID_SEMAPHORE_ID;
			}
#else
			if (pipe_id != PIPELINE_ID_CAP && (INVALID_SEMAPHORE_ID != pipe_mgr_cfg->pipe_stop_semaphore)) {
				//timeout unit is tick(5ms),we wait max time is 2 frames time.
				status = UOS_WaitForSemaphore(pipe_mgr_cfg->pipe_stop_semaphore, pipe_mgr->timeout_value * 2);
				if (status != OS_SUCCESS) {
					CAMLOGE("wait for %d pipe stop done error %d!", pipe_id, status);
				} else
					CAMLOGV("wait for %d pipe stop done!", pipe_id);
				UOS_DeleteSemaphore(pipe_mgr_cfg->pipe_stop_semaphore);
				pipe_mgr_cfg->pipe_stop_semaphore = INVALID_SEMAPHORE_ID;
			}
#endif
		}
	}

#if 0	
	/*wait hw is idle*/
	map = pipe_id_mask;
	pipe_id=0;
	for (i = 0; i < PIPELINE_ID_MAX; i++) {
		if(map == 0)
		    break;
		bit = __ffs(map);
		if(bit >=PIPELINE_ID_MAX)
			break;
		map &= ~(1 << bit);
		pipe_id = bit;
		
		while (pipe_mgr_cfg->dma_state != ISPDMA_HW_NO_STREAM) {
			/*1 tick=5ms*/
			OSATaskSleep(1);
			if (wait_cnt++ > 80) {
				CAMLOGE("err wait HW_NO_STREAM,:pipe_id%d",pipe_id);
				pipe_mgr_cfg->dma_state = ISPDMA_HW_NO_STREAM;
				break;
			}
		}
	}
#endif

	if(!keep_sensor && all_port_disable(pipe_mgr)) {
		sensor_ctrl->ops->stream_off(sensor_ctrl);

#ifdef ISP_USE_HARDWARE_TIMER
		//stream off sensor means isp sof are not coming.
		if (pipe_mgr->timer_id > 0) {
			while (1) {
				time_status = GetTimerStatus(pipe_mgr->timer_id);
				if (ACC_INACTIVE == time_status) {
					break;
				}
	
				if (index > 11) {
					CAMLOGW("the acc time %d excuses callback timeout!", pipe_mgr->timer_id);
					break;
				}
				CAMLOGV("wait the acc timer %d to excue callback!", pipe_mgr->timer_id);
				UOS_Sleep(3); //15ms
				index++;
			}
	
			AccTimerDelete(pipe_mgr->timer_id);
			pipe_mgr->timer_id = 0;
		}
#endif
		
	}
	CAMLOGI("camframe stream numbers = %d ", pipe_mgr->frame_num);
	return ret;
}

void isp_dma_status_check(struct cam_isp_pipeline * pipe_mgr,uint_8 pipe_id)
{
	struct pipeline_param *pipe_mgr_cfg = NULL;
	if(pipe_id == PIPELINE_ID_PREV)
		pipe_mgr_cfg = &pipe_mgr->pipe0_cfg;
	else if(pipe_id == PIPELINE_ID_VIDEO)
		pipe_mgr_cfg = &pipe_mgr->pipe1_cfg;
	else{
		pipe_mgr_cfg = &pipe_mgr->pipe2_cfg;
    }
    
    if(pipe_mgr_cfg->dma_state  == ISPDMA_HW_NEXT_FRAME_NO_STREAM)
		pipe_mgr_cfg->dma_state  = ISPDMA_HW_NO_STREAM;
	return;
}


static  void isp_dma_sof_hdl(struct cam_isp_pipeline * pipe_mgr,uint_8 pipe_id)
{
	struct isp_buf_mgr * buf_mgr = pipe_mgr->buf_mgr;
	struct isp_buf_queue *buf_queue = &buf_mgr->bufque[pipe_id];
	struct isp_qbuf_buffer_info *buf_info = NULL;
	struct pipeline_param *pipe_mgr_cfg = NULL;
	int busyBufCnt = 0;

	if(pipe_id == PIPELINE_ID_PREV)
		pipe_mgr_cfg = &pipe_mgr->pipe0_cfg;
	else if(pipe_id == PIPELINE_ID_VIDEO)
		pipe_mgr_cfg = &pipe_mgr->pipe1_cfg;
	else
		pipe_mgr_cfg = &pipe_mgr->pipe2_cfg;

	/* dma eof may be lost,so we change buf status at next frame dma sof */
	buf_info = isp_find_busy_buffer(buf_mgr, pipe_id, 0, &busyBufCnt);
	if (buf_info) {
		if (ISP_STATS_BUFSTS_HWUSED == buf_info->buf_status) {
			buf_info->buf_status = ISP_STATS_BUFSTS_PREPARED;
		}
	}

	/* first check busy buf,if >1,the previous frame dma eof must be lost */
	if (busyBufCnt > 1) {
		CAMLOGV("it may lost previous eof, busycount=%d!", buf_queue->busy_buf_cnt);
		buf_info = isp_get_busy_buffer(buf_mgr, pipe_id);
		if (buf_info) {
			buf_info->buf_status = ISP_STATS_BUFSTS_PREPARED;
		}
	} else {
		#ifdef ISP_USE_HARDWARE_TIMER
			buf_info = isp_get_buffer_from_bufque(buf_mgr, pipe_id);
		#else
			buf_info = isp_get_idle_buffer(buf_mgr, pipe_id);
		#endif
			
			if (buf_info) {
				buf_info->buf_status = ISP_STATS_BUFSTS_PREPARED;
			} else {
				/* there is no buf for camera, use last buf */
				CAMLOGV("there is no buffer in idle(pipe_id:%d), busycount=%d!", pipe_id, buf_queue->busy_buf_cnt);
				/* z3 shadow in isp sof, so we don't need one buffer to be always going. */
			//	buf_info = isp_get_busy_buffer(buf_mgr, pipe_id);
			//	if (buf_info) {
			//		buf_info->buf_status = ISP_STATS_BUFSTS_HWUSED;
			//	}
			}
	}

	/* skip frm */
	if (pipe_mgr->online_isp_drop_frm_num > 0) {
		/* when use last buf at dma eof,we will drop the previous frame */
		if (buf_info && ISP_STATS_BUFSTS_HWUSED != buf_info->buf_status) {
			buf_info->buf_status = ISP_STATS_BUFSTS_SKIPED;
		}
	}

	if (buf_info && pipe_mgr_cfg->pipeline_enable) { /* buf ready */
		pipe_mgr_cfg->dma_state = ISPDMA_ACTIVE;
		isp_put_busy_buffer(buf_mgr, pipe_id, buf_info);
		cam_set_addr(pipe_id,buf_info->buf);
		cam_set_pipe_shadow_ready(pipe_id);
	} 

#if 0
	else {
        /*note ISP_SOF_IRQ would check ISPDMA_HW_NEXT_FRAME_NO_STREAM.and set to ISPDMA_HW_NO_STREAM*/
		if(pipe_mgr_cfg->dma_state  != ISPDMA_HW_NO_STREAM)
			pipe_mgr_cfg->dma_state  = ISPDMA_HW_NEXT_FRAME_NO_STREAM;
	}
#endif

	return;
}

static  void isp_dma_eof_hdl(struct cam_isp_pipeline * pipe_mgr,uint_8 pipe_id)
{
	struct isp_buf_mgr * buf_mgr = pipe_mgr->buf_mgr;
	struct isp_buf_queue *buf_queue = &buf_mgr->bufque[pipe_id];
	struct isp_qbuf_buffer_info *buf_info = NULL;
	struct pipeline_param *pipe_mgr_cfg = NULL;
	if(pipe_id == PIPELINE_ID_PREV)
		pipe_mgr_cfg = &pipe_mgr->pipe0_cfg;
	else if(pipe_id == PIPELINE_ID_VIDEO)
		pipe_mgr_cfg = &pipe_mgr->pipe1_cfg;
	else{
		pipe_mgr_cfg = &pipe_mgr->pipe2_cfg;
        pipe_mgr_cfg->dma_state  = ISPDMA_HW_NO_STREAM;
    }

	buf_info = isp_get_busy_buffer(buf_mgr,pipe_id);
	if (!buf_info) {
		CAMLOGI("ispirq:pipe_id:%d: busy buffer is NULL.idle:%d, busy:%d.",pipe_id, buf_queue->idle_buf_cnt, buf_queue->busy_buf_cnt);
	} else {
		if (ISP_STATS_BUFSTS_PREPARED == buf_info->buf_status && pipe_mgr_cfg->pipeline_enable) {
			struct isp_buf_msg *eof_msg = UOS_MALLOC(sizeof(struct isp_buf_msg));
			if(!eof_msg) {
				CAMLOGE("UOS_MALLOC for eof_msg failed!");
				return;
			}
			buf_info->metadata.frame_id = pipe_mgr->frame_num;
	//		buf_info->metadata.timestamp = OSAGetTicks();
			buf_info->metadata.timestamp = 0;
			eof_msg->msg_type = ISP_MSG_TYPE_EOF;
			eof_msg->u.data_eof.buf_idx = buf_info->buf_idx;
			eof_msg->u.data_eof.pipe_id = pipe_id;
			CAMLOGV("pipe %d send %d buffet to hal,timestamp=0x%x!",pipe_id, buf_info->buf_idx,buf_info->metadata.timestamp);
			UOS_SendMsg((void *)(eof_msg), buf_queue->msgQRef, UOS_SEND_MSG);
		} else if (ISP_STATS_BUFSTS_HWUSED == buf_info->buf_status) {
			CAMLOGV("ispirq:pipe_id:%d: pop rsv buf!\n",pipe_id);
			isp_put_busy_buffer(buf_mgr, pipe_id, buf_info);
		} else if (ISP_STATS_BUFSTS_SKIPED == buf_info->buf_status) {
		#ifdef ISP_USE_HARDWARE_TIMER
			isp_put_buffer_to_bufque(buf_info);
		#else
			isp_put_idle_buffer(buf_mgr, pipe_id, buf_info);
		#endif
		}
	}
	return;
}

void isp_set_fps(uint_32 min_fps ,uint_32 max_fps, CAM_BANDING banding)
{
	struct cam_isp_pipeline * pipe_mgr = get_isp_pipe_mgr();
	SENSOR_CTRL_T *sensor_ctrl = NULL;
	int cur_res_index = 0;
	uint_32 sensor_out_format = 0;
	SENSOR_AEC_T aec = {0x0,0x0,0x0,0x0,0x0,0x0};

	if (NULL == pipe_mgr) {
		CAMLOGE("isp_set_fps:param is NULL, return error!");
		return;
	}

	sensor_ctrl = &pipe_mgr->sensor_ctrl;
	if(! sensor_ctrl->sensor_init){
		CAMLOGE("isp_set_fps failed, isp and sensor haven't initialization!");
		return;
	}
	cur_res_index = sensor_ctrl->sensor_data->cur_res_index;
	sensor_out_format = sensor_ctrl->sensor_data->res[cur_res_index].sensor_out_format;

	if(SENSOR_FMT_YUV422 != sensor_out_format )
		isp_calc_exp(min_fps, max_fps, banding);
	else{
		aec.min_fps = min_fps;
		aec.max_fps = max_fps;
		aec.banding= banding;
		aec.aec_en = 1;
		update_sensor_aec(sensor_ctrl,aec);
	}

	pipe_mgr->min_fps = min_fps;

	if (max_fps != pipe_mgr->max_fps) {
		pipe_mgr->max_fps = max_fps;
		pipe_mgr->timeout_value = (1000 / pipe_mgr->max_fps) - ISP_HARDWARE_TIMER_DELAY;
	}

	CAMLOGI("isp minfps = %d, maxfps = %d, sof_handler timeout=%dms!", pipe_mgr->min_fps, pipe_mgr->max_fps, pipe_mgr->timeout_value);
}

void isp_update_aec(SENSOR_CTRL_T *sensor_ctrl)
{
	uint_32 exp_high = 0,  exp_low = 0, gain = 0, aec_en = 0;
	SENSOR_AEC_T aec = {0x0,0x0,0x0,0x0,0x0,0x0};
	uint_32 sensor_out_format = 0;
	int cur_res_index = 0;
	uint_32 exp_step = 0;

	if (NULL == sensor_ctrl || NULL == sensor_ctrl->sensor_data) {
		CAMLOGE("isp_update_aec sensor param is NULL, return error!");
		return;
	}

	cur_res_index = sensor_ctrl->sensor_data->cur_res_index;
	sensor_out_format = sensor_ctrl->sensor_data->res[cur_res_index].sensor_out_format;

	if(SENSOR_FMT_YUV422 != sensor_out_format ){
		isp_get_aec(&exp_high, &exp_low, &gain, &aec_en, &exp_step);

		aec.exp_high = exp_high;
		aec.exp_low = exp_low;
		aec.gain = gain;
		aec.aec_en = aec_en;

		update_sensor_aec(sensor_ctrl,aec);
	}
}

void isp_get_exp_iso(uint_32 *reciprocal_exp, uint_32 *iso)
{
	struct cam_isp_pipeline * pipe_mgr = get_isp_pipe_mgr();
	SENSOR_CTRL_T *sensor_ctrl = NULL;
	uint_32 exp_high = 0,  exp_low = 0, gain = 0, aec_en = 0;
	uint_32 sensor_out_format = 0;
	int cur_res_index = 0;
	uint_32 exp_step = 0;
	uint_32 exp = 0;

	sensor_ctrl = &pipe_mgr->sensor_ctrl;
	if (NULL == sensor_ctrl || NULL == sensor_ctrl->sensor_data) {
		CAMLOGE("isp_update_aec sensor param is NULL, return error!");
		return;
	}

	cur_res_index = sensor_ctrl->sensor_data->cur_res_index;
	sensor_out_format = sensor_ctrl->sensor_data->res[cur_res_index].sensor_out_format;

	if(SENSOR_FMT_YUV422 != sensor_out_format){
		isp_get_aec(&exp_high, &exp_low, &gain, &aec_en, &exp_step);

		exp = (exp_high<<8) | exp_low;

		*reciprocal_exp = 100 * exp_step / exp;
		if(gain == 0)
			gain = 0x1;
		*iso = gain * 16;
	}
}

#ifdef CCIC_IRQ_DEBUG
static void ccic_irq_handler(uint32_t irqs)
{
	uint32_t i;

	static const char *const err_msg[] = {
		"Reserved",
		"Reserved",
		"Reserved",
		"Reserved",
		"Reserved",
		"Reserved",
		"Reserved",
		"Reserved",
		"Reserved",
		"Reserved",
		"Reserved",
		"AXI Write Error IRQ",
		"MIPI DPHY RX CLK ULPS Active IRQ",
		"MIPI DPHY RX CLK ULPS IRQ",
		"MIPI DPHY Lane ULPS Active IRQ",
		"MIPI DPHY Lane Error Control IRQ",
		"MIPI DPHY Lane Start of Transmission Synchronization Error IRQ",
		"MIPI DPHY Lane Start of Transmission Error IRQ",
		"Reserved",
		"Reserved",
		"Reserved",
		"Reserved",
		"Reserved",
		"CSI2 Packet Error IRQ",
		"CSI2 CRC Error IRQ",
		"CSI2 ECC 2-bit (or more) Error IRQ",
		"CSI2 Parity Error IRQ",
		"CSI2 ECC Correctable Error IRQ",
		"CSI2 Lane FIFO Overrun Error IRQ",
		"CSI2 Parse Error IRQ",
		"CSI2 Generic Short Packet Valid IRQ",
		"CSI2 Generic Short Packet Error IRQ",
	};
	CAMLOGV("ccic irq status = 0x%08x", irqs);

	for (i = 0; i < 32; i++) {
		if (irqs & (1 << i)){
			CAMLOGV("### %s", err_msg[i]);
        }
	}
}
#endif

//#define CAM_IRQ_DEBUG
//#define CCIC_IRQ_DEBUG

void cam_irq_msg_thread(void* argv)
{
	struct cam_isp_pipeline * pipe_mgr = get_isp_pipe_mgr();
	uint_32 irqs, event[UOS_EVT_MBX_SIZE] = {0};

	CAMLOGI("IRQ msg thread ready work");
	do {
		memset(event,0,sizeof(event));
		UOS_WaitMsg(event, pipe_mgr->camIrqErrorMsgQRef, OS_SUSPEND);
		irqs = event[1];

		if (!irqs) {
			CAMLOGV("no IRQ error msg , continue!");
			continue;
		}

		if (irqs & PIP1_SCL_FIFO_OVERRUN_IRQ) {
			CAMLOGE("PIP1_SCL_FIFO_OVERRUN_IRQxxxx");
		}
		if (irqs & PIP0_SCL_FIFO_OVERRUN_IRQ) {
			CAMLOGE("PIP0_SCL_FIFO_OVERRUN_IRQxxxx");
		}
		if (irqs & PIP2_DMA_FIFO_OVERRUN_IRQ) {
			CAMLOGE("PIP2_DMA_FIFO_OVERRUN_IRQxxxx");
		}
		if (irqs & PIP1_DMA_FIFO_OVERRUN_IRQ) {
			CAMLOGE("PIP1_DMA_FIFO_OVERRUN_IRQxxxx");
		}
		if (irqs & PIP0_DMA_FIFO_OVERRUN_IRQ) {
			CAMLOGE("PIP0_DMA_FIFO_OVERRUN_IRQxxxx");
		}
	}while(1);

}

void cam_irq_hisr_handler(void)
{
	struct cam_isp_pipeline * pipe_mgr = get_isp_pipe_mgr();
	uint_32 irqs, event[UOS_EVT_MBX_SIZE] = {0};
	uint_32 msgCnt = 0;

	msgCnt = UOS_MsgQEnqueued(pipe_mgr->camIrqMsgQRef);
	if(msgCnt > 0) {
		UOS_WaitMsg(event, pipe_mgr->camIrqMsgQRef, OSA_NO_SUSPEND);
		irqs = event[0];

		//CAMLOGV("hisr receive irqs=0x%x,msgcnt=%d!", irqs, msgCnt);

		if (irqs & PIP2_DMA_EOF_IRQ) {
			isp_dma_eof_hdl(pipe_mgr,PIPELINE_ID_CAP);
		}
		if (irqs & PIP1_DMA_EOF_IRQ) {
			isp_dma_eof_hdl(pipe_mgr,PIPELINE_ID_VIDEO);
		}
		if (irqs & PIP0_DMA_EOF_IRQ) {
			isp_dma_eof_hdl(pipe_mgr,PIPELINE_ID_PREV);
		}

		if (irqs & AEC_UPDATE_IRQ) {
			//CAMLOGI("AEC_UPDATE_IRQ------");
			isp_update_aec(&pipe_mgr->sensor_ctrl);
		}
	}
}

static void cam_isp_sof_timer_handler(unsigned int param)
{
	struct cam_isp_pipeline * pipe_mgr = get_isp_pipe_mgr();
	
	if (pipe_mgr->pipe1_cfg.pipeline_enable) {
		isp_dma_sof_hdl(pipe_mgr,PIPELINE_ID_VIDEO);
		set_pipeline_smooth_zoom_cfg(pipe_mgr,PIPELINE_ID_VIDEO);
	}
	
	if (pipe_mgr->pipe0_cfg.pipeline_enable) {
		isp_dma_sof_hdl(pipe_mgr,PIPELINE_ID_PREV);
		set_pipeline_smooth_zoom_cfg(pipe_mgr,PIPELINE_ID_PREV);
	}
#ifdef CAM_FULLSIZE_STREAM
	if (pipe_mgr->pipe2_cfg.pipeline_enable) {
		isp_dma_sof_hdl(pipe_mgr,PIPELINE_ID_CAP);
	}
#endif
}

//uint_32 g_halTimeTick = 0;
//uint_32 temp_index = 0;
int index_fifo = 0;
int index_SPIline = 0;
int index_SPIcrc = 0;
void cam_irq_handler(UINT32 irq_num)
{
	uint_32 irqs = 0;
	struct cam_isp_pipeline * pipe_mgr = get_isp_pipe_mgr();
//	uint_32 halTimeTick1 = 0, ret = 0;
	struct pipeline_param *pipe_mgr_cfg = NULL;
	ACC_TIMER_STATUS time_status = ACC_INACTIVE;
	u8 start_new_timer = 1; 
	uint_32 event[UOS_EVT_MBX_SIZE] = {0};

	irqs = camera_reg_read(REG_CAMERA_IRQ_STATUS);
	camera_reg_write(REG_CAMERA_IRQ_STATUS, irqs); //clear irq

	if (irqs & CSI2_EOF_IRQ) {
		//CAMLOGE("CSI2_EOF_IRQ");
	}

	/*deal with isp sof first.due to index would used by other irq.isp sof is very close with DMA sof*/
	if (irqs & ISP_SOF_IRQ) {

#ifdef ISP_USE_HARDWARE_TIMER
		if (pipe_mgr->timer_id > 0) {
			time_status = GetTimerStatus(pipe_mgr->timer_id);
			if (ACC_ACTIVE == time_status) {
				CAMLOGV("the acc timer %d has not been excued!", pipe_mgr->timer_id);
				start_new_timer= 0;
			} else if (ACC_INACTIVE == time_status) {
				AccTimerDelete(pipe_mgr->timer_id);
			}
		}

		if (start_new_timer) {
			pipe_mgr->timer_id = AccTimerStartEx(ACC_TIMER_AUTO_DELETE, pipe_mgr->timeout_value * 1000, cam_isp_sof_timer_handler, 0);
			if (pipe_mgr->timer_id <= 0) {
				CAMLOGE("start acc timer failed %d!", pipe_mgr->timer_id);
			}
		}
#endif
		pipe_mgr->frame_num++;
		if(pipe_mgr->online_isp_drop_frm_num>0)
			pipe_mgr->online_isp_drop_frm_num--;

		//we are not need this after z2 because that isp have stop done irq.
     //   isp_dma_status_check(pipe_mgr,PIPELINE_ID_PREV);
     //   isp_dma_status_check(pipe_mgr,PIPELINE_ID_VIDEO);
	}

#if 0

	if (irqs & ISP_EOF_IRQ) {
#ifdef CAM_IRQ_DEBUG
		CAMLOGI("ISP_EOF_IRQ -----");
#endif
	}

	if (irqs & ISIM_IRQ) {
		uint32_t csi_irqs = camera_reg_read(REG_CCIC_IRQ_STATUS);
		camera_reg_write(REG_CCIC_IRQ_STATUS, csi_irqs);
#ifdef CCIC_IRQ_DEBUG
		CAMLOGV("ISIM_IRQ------");
		ccic_irq_handler(csi_irqs);
#endif
	}

	if (irqs & PIP1_SHADOW_DONE_IRQ) {
#ifdef CAM_IRQ_DEBUG
		CAMLOGV("PIP1_SHADOW_DONE_IRQ------");
#endif
	}
	if (irqs & PIP0_SHADOW_DONE_IRQ) {
#ifdef CAM_IRQ_DEBUG
		CAMLOGV("PIP0_SHADOW_DONE_IRQ------");
#endif
	}
#endif

	if (irqs & PIP0_STOP_DOWN_IRQ) {
		pipe_mgr_cfg = get_pipe_mgr_cfg(PIPELINE_ID_PREV, pipe_mgr);
		if (!pipe_mgr_cfg->pipeline_enable) {
			if(INVALID_SEMAPHORE_ID != pipe_mgr_cfg->pipe_stop_semaphore)
				UOS_ReleaseSemaphore(pipe_mgr_cfg->pipe_stop_semaphore);
			else
				CAMLOGV("pipe0_cfg.pipe_stop_semaphore is invalid!");
		} else
			CAMLOGV("pipe0 stop done irq occurs which means hisr delay!");
	}

	if (irqs & PIP1_STOP_DOWN_IRQ) {
		pipe_mgr_cfg = get_pipe_mgr_cfg(PIPELINE_ID_VIDEO, pipe_mgr);
		if (!pipe_mgr_cfg->pipeline_enable) {
			if(INVALID_SEMAPHORE_ID != pipe_mgr_cfg->pipe_stop_semaphore)
				UOS_ReleaseSemaphore(pipe_mgr_cfg->pipe_stop_semaphore);
			else
				CAMLOGV("pipe1_cfg.pipe_stop_semaphore is invalid!");
		} else
			CAMLOGV("pipe1 stop done irq occurs which means hisr delay!");
	}

	if( (irqs & AEC_UPDATE_IRQ)
		||(irqs & PIP2_DMA_EOF_IRQ)
		||(irqs & PIP1_DMA_EOF_IRQ)
		||(irqs & PIP0_DMA_EOF_IRQ)) {
		//handler some IRQ in Hisr.
		event[0] = irqs;
		UOS_SendMsg((void*)event, pipe_mgr->camIrqMsgQRef, UOS_SEND_EVT);
		OS_Activate_HISR(&cam_hisr_ref);
	}

	if((irqs & PIP1_SCL_FIFO_OVERRUN_IRQ)
		||(irqs & PIP0_SCL_FIFO_OVERRUN_IRQ)
		||(irqs & PIP2_DMA_FIFO_OVERRUN_IRQ)
		||(irqs & PIP1_DMA_FIFO_OVERRUN_IRQ)
		||(irqs & PIP0_DMA_FIFO_OVERRUN_IRQ)) {
		if((index_fifo%10) == 0){
			index_fifo = 0;
			// FIFO IRQ is base on frame, we print it every 10 frames.
			event[1] = irqs;
			UOS_SendMsg((void*)event, pipe_mgr->camIrqErrorMsgQRef, UOS_SEND_EVT);
		}
		index_fifo++;
	}

	if (irqs & SPI_LINE_ERR_IRQ) {
		if((index_SPIline%10000) == 0){
			index_SPIline = 0;
			// SPI IRQ is base on line, the frequency is too high. so we print it every 10000 times.
			CAMLOGE("SPI_LINE_ERR_IRQxxxx");
		}
		index_SPIline++;
	}
	if (irqs & SPI_CRC_ERR_IRQ) {
		if((index_SPIcrc%10000) == 0){
			index_SPIcrc = 0;
			// SPI IRQ is base on line, the frequency is too high. so we print it every 10000 times.
			CAMLOGE("SPI_CRC_ERR_IRQxxxx");
		}
		index_SPIcrc++;
	}
}

void cam_raw_dump(uint_t raw_on)
{
	fe_isp_bypass(raw_on);
	if(raw_on){
		CAMLOGI("cam_raw_dump enable");
		if(CONFIG_CAM_HW_VERSION == CAM_HW_VERSION_3)
			camera_set_bit(REG_SUBSAMPLE_SCALER_CTRL2,PIPE2_CTRL_RAW_DUMP_A0);
		else
			camera_set_bit(REG_SUBSAMPLE_SCALER_CTRL2,PIPE2_CTRL_RAW_DUMP);
	} else {
		CAMLOGI("cam_raw_dump disable");
		if(CONFIG_CAM_HW_VERSION == CAM_HW_VERSION_3)
			camera_clr_bit(REG_SUBSAMPLE_SCALER_CTRL2,PIPE2_CTRL_RAW_DUMP_A0);
		else
			camera_clr_bit(REG_SUBSAMPLE_SCALER_CTRL2,PIPE2_CTRL_RAW_DUMP);
	}
}

int cam_irq_init(void)
{
	INTC_ReturnCode 	INTCStatus;
	uint_32 INT_NUM;
	struct cam_isp_pipeline * pipe_mgr = get_isp_pipe_mgr();
	CAMLOGV("camera_irq_init: E!");

	/* create irq_handle thread. */
	pipe_mgr->camIrqTaskRef= UOS_CreateTask(
		cam_irq_msg_thread,
		NULL,
		TASK_WITHOUT_MSGQ,
		CAMERA_IRQ_HANDLE_TASK_SIZE,
		CAMERA_IRQ_HANDLE_TASK_PRIORITY,
		"uiCamIrqHandle");

	if (pipe_mgr->camIrqTaskRef == NULL) {
		CAMLOGE("UOS_CreateTask for camera irq handle failed!");
	}

	//camera_set_bit(REG_CAMERA_IRQ_RAW_MASK, PIP1_SCL_SOF_IRQ | PIP1_SCL_EOF_IRQ | PIP0_SCL_SOF_IRQ | PIP0_SCL_EOF_IRQ | ISP_SOF_IRQ | ISP_EOF_IRQ);
	camera_set_bit(REG_CAMERA_IRQ_RAW_MASK, 0xFFFE1509 ); //enable csi2 eof, isp sof, dma eof ,aec, stop done .
	//camera_set_bit(REG_CCIC_IRQ_MASK, IRQ_CCIC_MASK_ALL);
	camera_reg_write(REG_CCIC_IRQ_MASK, 0x0); // mask all ccic IRQ

	INT_NUM = INTCGetIntVirtualNum(IRQ_CAMERA_IPE);
	CAMLOGV("cam_irq_init: %d!", INT_NUM);
	INTCStatus = INTCBind (INT_NUM , cam_irq_handler);
	INTCStatus = INTCConfigure(INT_NUM, INTC_IRQ, INTC_HIGH_LEVEL);
	INTCStatus = INTCEnable(INT_NUM);
	return INTCStatus;
}

void cam_irq_deinit(void)
{
	uint_32 INT_NUM = 0;
	struct cam_isp_pipeline * pipe_mgr = get_isp_pipe_mgr();

	INT_NUM = INTCGetIntVirtualNum(IRQ_CAMERA_IPE);
	CAMLOGV("cam_irq_deinit: %d!", INT_NUM);
	INTCDisable(INT_NUM);
	INTCUnbind(INT_NUM);

	/* destory irq handle thread. */
	if (pipe_mgr->camIrqTaskRef) {
		UOS_StopTask((TASK_HANDLE *)pipe_mgr->camIrqTaskRef);
		UOS_DeleteTask((TASK_HANDLE *)pipe_mgr->camIrqTaskRef);
		pipe_mgr->camIrqTaskRef = NULL;
	}
}

/*init isp_buf_mgr*/
int cam_init_buf_mgr(struct isp_buf_mgr *buf_mgr)
{
	int ret=0;
	int i=0;

	//buf_mgr->mgr_lock = UOS_NewMutex("cam_init_buf");
	for(i=0;i<PIPELINE_ID_MAX;i++){
		buf_mgr->bufque[i].buf_mgr = buf_mgr;
		buf_mgr->bufque[i].queue_lock = INVALID_MUTEX_ID;
		INIT_LIST_HEAD(&buf_mgr->bufque[i].idle_buf);
		INIT_LIST_HEAD(&buf_mgr->bufque[i].busy_buf);
		buf_mgr->bufque[i].busy_buf_cnt = 0;
		buf_mgr->bufque[i].idle_buf_cnt= 0;
		buf_mgr->bufque[i].req_buf_num = 0;
		buf_mgr->bufque[i].pipe_id= i;
		buf_mgr->bufque[i].msgQRef = INVALID_MSGQ_ID;
	}
	return ret;
}
		
/*
 * disableInterrupts to avoid concurrent access
 * sw init.isr init,mutex init,thread init,mem init,mutex init and so on.
 */
void cam_phase_init(void)
{
	int i = 0;

	if(cam_drv_inited == 0){
		cam_drv_inited = 1;
		memset(&cam_pipeline_cfg, 0, sizeof(struct cam_isp_pipeline));
		cam_pipeline_cfg.stream_cfg_lock = INVALID_MUTEX_ID;
		cam_pipeline_cfg.camIrqMsgQRef = INVALID_MSGQ_ID;
		cam_pipeline_cfg.camIrqErrorMsgQRef = INVALID_MSGQ_ID;
		memset(&cam_buf_mgr, 0, sizeof(struct isp_buf_mgr));
		cam_init_buf_mgr(&cam_buf_mgr);
		cam_buf_mgr.cam_isp = &cam_pipeline_cfg;
		cam_pipeline_cfg.buf_mgr = &cam_buf_mgr;

		//hisr
		OS_Create_HISR(&cam_hisr_ref, "uiCamIsr", cam_irq_hisr_handler, HISR_PRIORITY_2);
	}
	cam_pipeline_cfg.stream_cfg_lock = UOS_NewMutex("cam_stream_cfg_lock");
	cam_pipeline_cfg.camIrqMsgQRef = UOS_NewMessageQueue("camPipe", TASK_DEFAULT_MSGQ_SIZE);
	cam_pipeline_cfg.camIrqErrorMsgQRef = UOS_NewMessageQueue("camIrqmsg", TASK_DEFAULT_MSGQ_SIZE);
	for(i=0;i<PIPELINE_ID_MAX;i++){
		cam_buf_mgr.bufque[i].queue_lock = UOS_NewMutex("cam_buf_queue_lock");
	}

}

void cam_phase_deinit(void)
{
	int i = 0;

	UOS_FreeMutex(cam_pipeline_cfg.stream_cfg_lock);
	if (cam_pipeline_cfg.camIrqMsgQRef != INVALID_MSGQ_ID) {
		UOS_FreeMessageQueue(cam_pipeline_cfg.camIrqMsgQRef );
		cam_pipeline_cfg.camIrqMsgQRef = INVALID_MSGQ_ID;
	}
	if (cam_pipeline_cfg.camIrqErrorMsgQRef != INVALID_MSGQ_ID) {
		UOS_FreeMessageQueue(cam_pipeline_cfg.camIrqErrorMsgQRef );
		cam_pipeline_cfg.camIrqErrorMsgQRef = INVALID_MSGQ_ID;
	}

	for(i=0;i<PIPELINE_ID_MAX;i++){
		UOS_FreeMutex(cam_buf_mgr.bufque[i].queue_lock);
	}
}

