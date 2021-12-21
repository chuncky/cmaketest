#ifndef __CAM_FE_H__
#define __CAM_FE_H__

#include "cam_pipeline.h"
#include "camera_common_log.h"

void fe_isp_reg_init(SENSOR_CTRL_T *sensor_ctrl);

int fe_isp_precess_config(struct isp_param* isp_cfg,SENSOR_CTRL_T *sensor_ctrl);

int spi_config(SENSOR_DATA_T *sensor_data);

int mipi_config(SENSOR_DATA_T *sensor_data);

int parallel_config(SENSOR_DATA_T * sensor_data);
int cam_ccic_config(SENSOR_CTRL_T *sensor_ctrl);
void fe_isp_bypass(uint_t bypass);
int isp_ctl_set_contrast(uint_16 level, struct tuning_param *default_param);
int isp_ctl_set_brightness(uint_16 level, struct tuning_param *default_param);
int isp_ctl_set_saturation(uint_16 level, struct tuning_param *default_param);
int isp_ctl_set_wb(uint_16 level);
int isp_ctl_set_effect(uint_16 level);
int isp_ctl_set_banding(uint_16 level, SENSOR_CTRL_T *sensor_ctrl );

#endif

