#include "cam_pipeline.h"
#include "..\..\clock\media-clk\media_clk.h"
#include "cam_fe.h"
#include "pipeline\cam_pipeline_reg.h"


int camdrv_set_param(CamDrvParaMode param_mode, void *pValue, uint_8 pipeIdMask)
{
	int ret = 0;

	struct cam_isp_pipeline *pipe_mgr = get_isp_pipe_mgr();
	SENSOR_CTRL_T *sensor_ctrl = NULL;
	uint_32 sensor_out_format = SENSOR_FMT_YUV422;

	sensor_ctrl = &(pipe_mgr->sensor_ctrl);
	sensor_out_format = sensor_ctrl->sensor_data->res[sensor_ctrl->sensor_data->cur_res_index].sensor_out_format;
	CAMLOGI("set param format = %s param_mode =%d",(sensor_out_format == SENSOR_FMT_YUV422)?"YUV":"RAW",param_mode); 
	if(sensor_out_format == SENSOR_FMT_YUV422){
		if (NULL == sensor_ctrl->sensor_data->ops)
			return -1;
		switch (param_mode) {
			case CAMDRV_PARAM_ZOOM_FACTOR:
				// YUV sensor do not support zoom
				break;
			case CAMDRV_PARAM_CONTRAST:
				if (NULL != sensor_ctrl->sensor_data->ops->set_contrast)
					ret = sensor_ctrl->sensor_data->ops->set_contrast(sensor_ctrl, *((uint_16*)pValue));
				break;
			case CAMDRV_PARAM_BRIGHTNESS:
				if (NULL != sensor_ctrl->sensor_data->ops->set_brightness)
					ret = sensor_ctrl->sensor_data->ops->set_brightness(sensor_ctrl, *((uint_16*)pValue));

				break;
			case CAMDRV_PARAM_SATURATION:
				if (NULL != sensor_ctrl->sensor_data->ops->set_saturation)
					ret = sensor_ctrl->sensor_data->ops->set_saturation(sensor_ctrl, *((uint_16*)pValue));
				break;
			case CAMDRV_PARAM_WB:
				if (NULL != sensor_ctrl->sensor_data->ops->set_wb)
					ret = sensor_ctrl->sensor_data->ops->set_wb(sensor_ctrl, *((uint_16*)pValue));
				break;
			case CAMDRV_PARAM_EXPOSURE:
				break;
			case CAMDRV_PARAM_EFFECT:
				if (NULL != sensor_ctrl->sensor_data->ops->set_effect)
					ret = sensor_ctrl->sensor_data->ops->set_effect(sensor_ctrl, *((uint_16*)pValue));
				break;
			case CAMDRV_PARAM_NIGHT_MODE:
				//do need set to drv,only need by hal
				break;
			case CAMDRV_PARAM_FLASH_MODE:
				//do need set to drv,only need by hal
				break;
			case CAMDRV_PARAM_BANDING:
				if (NULL != sensor_ctrl->sensor_data->ops->set_banding)
					ret = sensor_ctrl->sensor_data->ops->set_banding(sensor_ctrl, *((uint_16*)pValue));
				break;
			case CAMDRV_PARAM_QUALITY:
				//do need set to drv,only need by hal
				break;
			default:
				CAMLOGE("invalid param mode!");
				ret = -1;
				break;
		}

	} else {
		switch (param_mode) {
			case CAMDRV_PARAM_ZOOM_FACTOR:
				ret = cam_pipe_setzoom(pipeIdMask, *((uint_16*)pValue));
				break;
			case CAMDRV_PARAM_CONTRAST:
				ret = isp_ctl_set_contrast( *((uint_16*)pValue), &pipe_mgr->default_param);
				break;
			case CAMDRV_PARAM_BRIGHTNESS:
				ret = isp_ctl_set_brightness( *((uint_16*)pValue), &pipe_mgr->default_param);
				break;
			case CAMDRV_PARAM_SATURATION:
				ret = isp_ctl_set_saturation( *((uint_16*)pValue), &pipe_mgr->default_param);
				break;
			case CAMDRV_PARAM_WB:
				ret = isp_ctl_set_wb( *((uint_16*)pValue));
				break;
			case CAMDRV_PARAM_EXPOSURE:
				break;
			case CAMDRV_PARAM_EFFECT:
				ret = isp_ctl_set_effect( *((uint_16*)pValue));
				break;
			case CAMDRV_PARAM_NIGHT_MODE:
				//do need set to drv,only need by hal
				break;
			case CAMDRV_PARAM_FLASH_MODE:
				//do need set to drv,only need by hal
				break;
			case CAMDRV_PARAM_BANDING:
				if (sensor_ctrl == NULL) {
					CAMLOGE("get sensor_ctrl failed!"); 
					return -1;
				}
				ret = isp_ctl_set_banding( *((uint_16*)pValue),sensor_ctrl);
				break;
			case CAMDRV_PARAM_QUALITY:
				//do need set to drv,only need by hal
				break;
			default:
				CAMLOGE("invalid param mode!");
				ret = -1;
				break;
		}

	}

	if (ret < 0) {
		CAMLOGE("set param mode %d,vaule=%d failed!", param_mode, *((int *)pValue));
	}
	return ret;
}

int camdrv_isp_power_on(void)
{
	CAMLOGV("camdrv_isp_power_on in!");

	cam_phase_init();/*todo: this function would be called in hal start step.now just call in hear temp.*/
	media_clk_dump(LEVEL_DUMP_CLOCK_ONLY);
	media_power_on(MODULE_CAMERA);
	media_clk_enable(MEDIA_CLK_LCD_AHB|MEDIA_CLK_LCD_CI|MEDIA_CLK_PHY|MEDIA_CLK_ISP2X|MEDIA_CLK_INTERFACE|MEDIA_CLK_OFFLINE|MEDIA_CLK_OFFLINE_AXI, MODULE_CAMERA);
	media_clk_reset(MEDIA_CLK_ISP2X);
	media_clk_dump(LEVEL_DUMP_CLOCK_ONLY);
	cam_irq_init();
	CAMLOGV("camdrv_isp_power_on out");
	return 0;
}

int camdrv_isp_power_off(void)
{
	struct cam_isp_pipeline *pipe_mgr = get_isp_pipe_mgr();

	CAMLOGV("camdrv_isp_power_off in");
	pipe_mgr->isp_cfg.isp_inited = 0;
	media_clk_dump(LEVEL_DUMP_CLOCK_ONLY);
	media_clk_disable(MEDIA_CLK_LCD_AHB|MEDIA_CLK_LCD_CI|MEDIA_CLK_PHY|MEDIA_CLK_ISP2X|MEDIA_CLK_INTERFACE|MEDIA_CLK_OFFLINE|MEDIA_CLK_OFFLINE_AXI, MODULE_CAMERA);
	media_power_off(MODULE_CAMERA);
	media_clk_dump(LEVEL_DUMP_CLOCK_ONLY);
	cam_irq_deinit();
	cam_phase_deinit();
	CAMLOGV("camdrv_isp_power_off out");

	return 0;
}

int camdrv_camera_reg_reset(void) 
{
	set_pipeline_reg_reset();
	return 0;
}

void camdrv_raw_dump(uint_t raw_on)
{
	cam_raw_dump(raw_on);
}

int camdrv_init_bufq(struct isp_buf_request *buf_request)
{
	struct isp_buf_mgr *buf_mgr = (struct isp_buf_mgr *)get_isp_buf_mgr();
	if(buf_mgr == NULL)
		return -1;
	return cam_init_bufq(buf_mgr,buf_request);
}

int camdrv_buf_enqueue(struct isp_buf_info *info)
{
	struct isp_buf_mgr *buf_mgr =  (struct isp_buf_mgr *)get_isp_buf_mgr();
	if(buf_mgr == NULL)
		return -1;
	return cam_buf_enqueue(buf_mgr,info);
}

int camdrv_buf_dequeue(struct isp_buf_info *info)
{
	struct isp_buf_mgr *buf_mgr =  (struct isp_buf_mgr *)get_isp_buf_mgr();
	if(buf_mgr == NULL)
		return -1;
	return cam_buf_dequeue(buf_mgr,info);
}

int camdrv_buf_flush(uint_8 pipe_id_mask)
{
	struct isp_buf_mgr *buf_mgr =  (struct isp_buf_mgr *)get_isp_buf_mgr();
	struct cam_isp_pipeline *pipe_mgr = get_isp_pipe_mgr();
	int ret = 0;
	if(buf_mgr == NULL)
		return -1;
	if(pipe_id_mask > (BIT(0)+BIT(1)+BIT(2)))
		return -1;
	if(pipe_mgr == NULL)
		return -1;
	UOS_TakeMutex(pipe_mgr->stream_cfg_lock);
	ret = cam_buf_flush(buf_mgr,pipe_id_mask);
	UOS_ReleaseMutex(pipe_mgr->stream_cfg_lock);
	return ret;
}


int camdrv_pipe_cfg(struct isp_pipe_cfg pipe_cfg)
{
	struct cam_isp_pipeline *pipe_mgr = get_isp_pipe_mgr();
	struct pipeline_param *pipe_mgr_cfg = NULL;
	if(pipe_mgr == NULL)
		return -1;
	if(pipe_cfg.pipe_id >PIPELINE_ID_CAP)
		return -1;

	if(pipe_cfg.pipe_id == PIPELINE_ID_PREV)
		pipe_mgr_cfg = &pipe_mgr->pipe0_cfg;
	else if(pipe_cfg.pipe_id == PIPELINE_ID_VIDEO)
		pipe_mgr_cfg = &pipe_mgr->pipe1_cfg;
	else
		pipe_mgr_cfg = &pipe_mgr->pipe2_cfg;

	UOS_TakeMutex(pipe_mgr->stream_cfg_lock);
	pipe_mgr->frame_num = 0;
	pipe_mgr->min_fps = 10;
	pipe_mgr->max_fps = 20;
	pipe_mgr->timeout_value = (1000 / pipe_mgr->max_fps) - ISP_HARDWARE_TIMER_DELAY;
	pipe_mgr_cfg->pipeline_id = pipe_cfg.pipe_id;
	//pipe_mgr_cfg->pipeline_enable = pipe_cfg.pipe_enable;
	pipe_mgr_cfg->shadow_mode = 1;
	pipe_mgr_cfg->data_range = pipe_cfg.data_range;
	pipe_mgr_cfg->streamon_zoom = pipe_cfg.zoom;
	pipe_mgr_cfg->cur_zoom = 0;
	pipe_mgr_cfg->target_zoom = 0;
	pipe_mgr_cfg->subsample = 0;
	pipe_mgr_cfg->jpeg_mode = 0;
	pipe_mgr_cfg->output_format = pipe_cfg.output_format;
	pipe_mgr_cfg->dma_stride_y = pipe_cfg.pipe_outw;/*will change in cam_set_pitch_dma_size*/
	pipe_mgr_cfg->pipeline_outw = pipe_cfg.pipe_outw;
	pipe_mgr_cfg->pipeline_outh = pipe_cfg.pipe_outh;
	pipe_mgr_cfg->pipeline_crop.start_x = 0;
	pipe_mgr_cfg->pipeline_crop.start_y = 0;
	pipe_mgr_cfg->pipeline_crop.end_x= pipe_cfg.pipe_outw;
	pipe_mgr_cfg->pipeline_crop.end_y= pipe_cfg.pipe_outh;
	UOS_ReleaseMutex(pipe_mgr->stream_cfg_lock);
	return 0;
}

/*this function for set zoom after one stream is streamon*/
/*0x100 zoom 1x
  0x200 zoom 2x
*/
int camdrv_set_zoom(uint_16 zoomratio)
{
    struct cam_isp_pipeline *pipe_mgr = get_isp_pipe_mgr();
	struct pipeline_param *pipe_mgr_cfg = NULL;
    int pipe_id = 0;
    if(zoomratio > 0x400)
        zoomratio=0x400;
	UOS_TakeMutex(pipe_mgr->stream_cfg_lock);
    for(pipe_id=PIPELINE_ID_PREV;pipe_id<=PIPELINE_ID_VIDEO;pipe_id++)
    {
        if(pipe_id == PIPELINE_ID_PREV)
            pipe_mgr_cfg = &pipe_mgr->pipe0_cfg;
        else
            pipe_mgr_cfg = &pipe_mgr->pipe1_cfg;

        if(pipe_mgr_cfg->pipeline_enable){
            pipe_mgr_cfg->target_zoom = zoomratio;
        }else{
            pipe_mgr_cfg->streamon_zoom = zoomratio;
        }
    }
	UOS_ReleaseMutex(pipe_mgr->stream_cfg_lock);
    return 0;
}

int camdrv_pipe_streamon_capture_off_preview(void)
{
	int ret=0;
	struct cam_isp_pipeline *pipe_mgr = get_isp_pipe_mgr();

	if(pipe_mgr == NULL)
		return -1;

	UOS_TakeMutex(pipe_mgr->stream_cfg_lock);
	ret = cam_pipe_streamon_capture_off_preview();
	UOS_ReleaseMutex(pipe_mgr->stream_cfg_lock);
	return ret;	
}

int camdrv_pipe_streamon_online_capture(struct isp_qbuf_buffer *buffer)
{
	int ret=0;
	struct cam_isp_pipeline *pipe_mgr = get_isp_pipe_mgr();

	if(pipe_mgr == NULL)
		return -1;

	UOS_TakeMutex(pipe_mgr->stream_cfg_lock);
	ret = cam_pipe_prepare_online_preview_pipe();
	ret = cam_pipe_prepare_online_capture_pipe();
	ret = cam_pipe_streamon_online_capture(buffer);
	UOS_ReleaseMutex(pipe_mgr->stream_cfg_lock);
	return ret;	
}

int camdrv_pipe_streamoff_online_capture(void)
{
	int ret=0;
	struct cam_isp_pipeline *pipe_mgr = get_isp_pipe_mgr();

	if(pipe_mgr == NULL)
		return -1;

	UOS_TakeMutex(pipe_mgr->stream_cfg_lock);
	ret = cam_pipe_streamoff_online_capture();
	UOS_ReleaseMutex(pipe_mgr->stream_cfg_lock);
	return ret;	
}

int camdrv_pipe_streamon(uint_8 pipe_id_mask)
{
	int ret=0;
	struct cam_isp_pipeline *pipe_mgr = get_isp_pipe_mgr();
	if(pipe_mgr == NULL)
		return -1;
	if(pipe_id_mask > (BIT(0)+BIT(1)+BIT(2)))
		return -1;
	UOS_TakeMutex(pipe_mgr->stream_cfg_lock);
	ret = cam_pipe_stream_on(pipe_id_mask);
	UOS_ReleaseMutex(pipe_mgr->stream_cfg_lock);
	return ret;
}

int camdrv_pipe_streamoff(uint_8 pipe_id_mask, uint_32 keep_sensor)
{
	int ret=0;
	struct cam_isp_pipeline *pipe_mgr = get_isp_pipe_mgr();
	if(pipe_mgr == NULL)
		return -1;
	if(pipe_id_mask > (BIT(0)+BIT(1)+BIT(2)))
		return -1;
	UOS_TakeMutex(pipe_mgr->stream_cfg_lock);
	ret = cam_pipe_stream_off(pipe_id_mask, keep_sensor);
	UOS_ReleaseMutex(pipe_mgr->stream_cfg_lock);
	return ret;
}

int camdrv_sensor_detect(void)
{
    return sensor_detect();
}

int camdrv_get_isp_cfg(struct isp_output_size *info)
{
	struct cam_isp_pipeline *pipe_mgr = get_isp_pipe_mgr();
	struct isp_param *isp_cfg = NULL;

	if(pipe_mgr == NULL){
		return -1;
	}

	isp_cfg = &pipe_mgr->isp_cfg;

	info->width =isp_cfg->isp_output_width;
	info->height=isp_cfg->isp_output_height;

	CAMLOGV("isp output info wxh[%d %d]", info->width, info->height);
	return 0;
}

int camdrv_sensor_power_on(SENSOR_ID sensor_id)
{
	struct cam_isp_pipeline *pipe_mgr = get_isp_pipe_mgr();
	//struct pipeline_param *pipe_mgr_cfg = NULL;
	SENSOR_CTRL_T *sensor_ctrl = NULL;
	//struct isp_param   *isp_cfg = &pipe_mgr->isp_cfg;
	int ret;

	if(pipe_mgr == NULL)
		return -1;
	if(sensor_id >= SENSOR_ID_MAX)
		return -1;

	sensor_ctrl = &pipe_mgr->sensor_ctrl;
	ret = get_sensor_ctrl(sensor_ctrl,sensor_id);
	if(ret < 0)
		return -1;

	if(sensor_ctrl->ops == NULL)
		return -1;
	sensor_ctrl->ops->power_up(sensor_ctrl);
	CAMLOGV("sensor_ctrl=0x%p,sensor_ctrl.ops=0x%p!", sensor_ctrl, sensor_ctrl->ops);
	fe_isp_reg_init(sensor_ctrl);

	return 0;
}

int camdrv_sensor_power_off(SENSOR_ID sensor_id)
{
	struct cam_isp_pipeline *pipe_mgr = get_isp_pipe_mgr();
	//struct pipeline_param *pipe_mgr_cfg = NULL;
	SENSOR_CTRL_T *sensor_ctrl = NULL;
	//struct isp_param   *isp_cfg = &pipe_mgr->isp_cfg;

	if(pipe_mgr == NULL)
		return -1;
	if(sensor_id >= SENSOR_ID_MAX)
		return -1;
	
	sensor_ctrl = &pipe_mgr->sensor_ctrl;

	if(sensor_ctrl->ops == NULL)
		return -1;
	sensor_ctrl->ops->power_down(sensor_ctrl);
	sensor_ctrl->ops->release(sensor_ctrl);

	return 0;
}

/*note:isp just connect one sensor.call this function,means:sensor_cfg->sensor_id connect to isp for hal*/
int camdrv_sensor_cfg(struct isp_sensor_cfg *sensor_cfg)
{
	int ret = 0;
	struct cam_isp_pipeline *pipe_mgr = get_isp_pipe_mgr();
	struct isp_param *isp_cfg = NULL;
	SENSOR_CTRL_T *sensor_ctrl = NULL;
	struct isp_sensor_info sensor_info;

	/* TODO: only support one rear camera  */
	if(sensor_cfg->sensor_id > SENSOR_ID_FRONT) {
		CAMLOGE("camera id %d not support", sensor_cfg->sensor_id); 
		return -1;
	}

	isp_cfg = &(pipe_mgr->isp_cfg);
	sensor_ctrl = &(pipe_mgr->sensor_ctrl);
	if ((isp_cfg == NULL) || (sensor_ctrl == NULL)) {
		CAMLOGE("get_isp_pipe_mgr failed!"); 
		return -1;
	}

	ret = cam_sensor_ops_call(sensor_ctrl, get_fmt, &sensor_info);
	if (ret) {
		CAMLOGD("%S get_fmt failed", sensor_ctrl->sensor_data->name);
		return -1;
	}

	isp_cfg->snr_mbus_code = sensor_info.mbus_code;
	isp_cfg->isp_input_width = sensor_info.sensor_outw;
	isp_cfg->isp_input_height = sensor_info.sensor_outh;

	// only spi sensor support isp crop, so handle crop with pipeline crop function
	isp_cfg->isp_crop.start_x = 0;
	isp_cfg->isp_crop.start_y = 0;
	isp_cfg->isp_crop.end_x= isp_cfg->isp_input_width;
	if (sensor_info.sensor_fmt < SENSOR_FMT_YUV422)
		isp_cfg->isp_crop.end_y= isp_cfg->isp_input_height - EXTRA_LINES_RAW_SENSOR;
	else
		isp_cfg->isp_crop.end_y= isp_cfg->isp_input_height;
	
	isp_cfg->isp_output_width = isp_cfg->isp_crop.end_x - isp_cfg->isp_crop.start_x;
	isp_cfg->isp_output_height = isp_cfg->isp_crop.end_y - isp_cfg->isp_crop.start_y;

	UOS_TakeMutex(pipe_mgr->stream_cfg_lock);
	pipe_mgr->online_isp_drop_frm_num = sensor_cfg->skip_frame_n;//online_isp_drop_frm_num-- in ISP_SOF_IRQ

	ret = cam_sensor_ops_call(sensor_ctrl, set_fmt, sensor_info.sensor_res_idx);
	if (ret){
		CAMLOGE("%S set_fmt failed", sensor_ctrl->sensor_data->name);
		goto err_exit;
	}
//	ret = cam_sensor_ops_call(sensor_ctrl, config_ccic);
	ret = cam_ccic_config(sensor_ctrl);
	if (ret){
		CAMLOGE("%S config_ccic failed", sensor_ctrl->sensor_data->name);
		goto err_exit;
	}
err_exit:
	UOS_ReleaseMutex(pipe_mgr->stream_cfg_lock);

	return ret;
}

int camdrv_set_ata(uint_32 ata_on)
{
	int ret = 0;
	struct cam_isp_pipeline *pipe_mgr = get_isp_pipe_mgr();
	SENSOR_CTRL_T *sensor_ctrl = NULL;

	sensor_ctrl = &(pipe_mgr->sensor_ctrl);
	if (sensor_ctrl == NULL) {
		CAMLOGE("get sensor_ctrl failed!"); 
		return -1;
	}

	UOS_TakeMutex(pipe_mgr->stream_cfg_lock);

	sensor_ctrl->ata_on= ata_on;

	ret = cam_sensor_ops_call(sensor_ctrl, set_ata);
	if (ret) {
		CAMLOGD("%S set_ata failed", sensor_ctrl->sensor_data->name);
		return -1;
	}

	UOS_ReleaseMutex(pipe_mgr->stream_cfg_lock);

	return ret;
}

int camdrv_set_fps(uint_32 min_fps ,uint_32 max_fps, CAM_BANDING banding)
{
	int ret = 0;

	isp_set_fps(min_fps, max_fps, banding);

	return ret;
}

void camdrv_get_exp_iso(uint_32 *reciprocal_exp, uint_32 *iso)
{
	isp_get_exp_iso(reciprocal_exp, iso);
}

#if 0
/*call again and agian(sensor_info->index++),until this function return -1*/
int camdrv_sensorinfo_enum(struct isp_sensor_info *sensor_info)
{
	int ret;
	ret = enum_sensor_info(sensor_info);
	return ret;
}
#endif
