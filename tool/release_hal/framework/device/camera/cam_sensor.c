#include "cam_sensor.h"
#include "i2c_api.h"
#include "..\..\chip\clock\media-clk\media_clk.h"
#include "..\..\chip\isp\online_drv\pipeline\cam_pipeline_reg.h"

//#define DEBUG_AEC

#ifdef SENSOR_GC030A_ACTIVE
extern struct crane_sensor_data crane_GC030a_mipi;
#endif

#ifdef SENSOR_GC032A_ACTIVE
extern struct crane_sensor_data crane_GC032a_spi;
#endif

#ifdef SENSOR_GC0312_ACTIVE
extern struct crane_sensor_data crane_GC0312_dvp;
#endif

#ifdef SENSOR_GC2145_ACTIVE
extern struct crane_sensor_data crane_GC2145_dvp;
#endif

#ifdef SENSOR_GC2385_ACTIVE
extern struct crane_sensor_data crane_GC2385_mipi;
#endif

unsigned int sensorDetected = 0;
static struct crane_sensor_data *sensor_data_tab[] = {
#ifdef SENSOR_GC030A_ACTIVE
	&crane_GC030a_mipi,
#endif

#ifdef SENSOR_GC032A_ACTIVE
	&crane_GC032a_spi,
#endif

#ifdef SENSOR_GC0312_ACTIVE
	&crane_GC0312_dvp,
#endif

#ifdef SENSOR_GC2145_ACTIVE
	&crane_GC2145_dvp,
#endif

#ifdef SENSOR_GC2385_ACTIVE
	&crane_GC2385_mipi,
#endif
};

static char name_debug[2][32];

SENSOR_CTRL_T *g_CurSensorCtrl = NULL;

int crane_i2c_read(const struct crane_sensor_i2c_attr *attr, uint_32 reg, uint_32 *val)
{
	if (I2C_8BIT == attr->reg_len && I2C_8BIT == attr->val_len) {
		*val = TWSI_REG_READ_BYTE(I2C_INDEX, attr->addr, reg);
	} else if (I2C_16BIT == attr->reg_len && I2C_16BIT == attr->val_len) {
		*val = TWSI_REG_READ_SHORT(I2C_INDEX, attr->addr, reg);
	} else if (I2C_32BIT == attr->reg_len && I2C_32BIT == attr->val_len) {
		*val = TWSI_REG_READ_WORD(I2C_INDEX, attr->addr, reg);
	}

	return 0;
}

static int crane_i2c_write(const struct crane_sensor_i2c_attr *attr, struct crane_sensor_regs regs)
{
	int i = 0;
	int rtn = 0;

	//uint_32 *val = 0;
	//int ret = 0;
	
	if (I2C_8BIT == attr->reg_len && I2C_8BIT == attr->val_len) {
		for (i = 0; i < regs.num; i++) {
			if (SENSOR_MDELAY != regs.tab[i].reg || SENSOR_MDELAY != regs.tab[i].val) {
				rtn = TWSI_REG_WRITE_BYTE(I2C_INDEX, attr->addr, regs.tab[i].reg, regs.tab[i].val);
				if (rtn < 0) {
					CAMLOGE("crane_i2c_write: Error!!  reg = 0x%x  val = 0x%x", regs.tab[i].reg, regs.tab[i].val);
					return -1;
				}
			} else {
			 	// if want to 5m delay, set regval_tab to
 				// [SENSOR_MDELAY, SENSOR_MDELAY, 5]
				msleep(regs.tab[i].mask);
			}
		}

	} else if (I2C_16BIT == attr->reg_len && I2C_16BIT == attr->val_len) {
		for (i = 0; i < regs.num; i++) {
			if (SENSOR_MDELAY != regs.tab[i].reg || SENSOR_MDELAY != regs.tab[i].val) {
				rtn = TWSI_REG_WRITE_SHORT(I2C_INDEX, attr->addr, regs.tab[i].reg, regs.tab[i].val);
				if (rtn < 0) {
					CAMLOGE("crane_i2c_write: Error!!  reg = 0x%x  val = 0x%x", regs.tab[i].reg, regs.tab[i].val);
					return -1;
				}
			} else {
			 	// if want to 5m delay, set regval_tab to
 				// [SENSOR_MDELAY, SENSOR_MDELAY, 5]
				msleep(regs.tab[i].mask);
			}

		}

	} else if (I2C_32BIT == attr->reg_len && I2C_32BIT == attr->val_len) {
		for (i = 0; i < regs.num; i++) {
			if (SENSOR_MDELAY != regs.tab[i].reg || SENSOR_MDELAY != regs.tab[i].val) {
				rtn = TWSI_REG_WRITE_WORD(I2C_INDEX, attr->addr, regs.tab[i].reg, regs.tab[i].val);
				if (rtn < 0) {
					CAMLOGE("crane_i2c_write: Error!!  reg = 0x%x  val = 0x%x", regs.tab[i].reg, regs.tab[i].val);
					return -1;
				}
			} else {
				// if want to 5m delay, set regval_tab to
				// [SENSOR_MDELAY, SENSOR_MDELAY, 5]
				msleep(regs.tab[i].mask);
			}
		}
	}

	return rtn;
}

int crane_i2c_write_single(const struct crane_sensor_i2c_attr *attr, uint_32 reg, uint_32 val)
{
	int rtn = 0;

	if(attr == NULL){
		CAMLOGE("crane_i2c_write: Error!! attr is NULL");
		return -1;
	}
	
	if (I2C_8BIT == attr->reg_len && I2C_8BIT == attr->val_len) {
		rtn = TWSI_REG_WRITE_BYTE(I2C_INDEX, attr->addr, reg, val);
		if (rtn < 0) {
			CAMLOGE("crane_i2c_write: Error!!  reg = 0x%x  val = 0x%x", reg, val);
			return -1;

		} 
	} else if (I2C_16BIT == attr->reg_len && I2C_16BIT == attr->val_len) {
		rtn = TWSI_REG_WRITE_SHORT(I2C_INDEX, attr->addr,reg, val);
		if (rtn < 0) {
			CAMLOGE("crane_i2c_write: Error!!  reg = 0x%x  val = 0x%x", reg, val);
			return -1;
		}
	} else if (I2C_32BIT == attr->reg_len && I2C_32BIT == attr->val_len) {
		rtn = TWSI_REG_WRITE_WORD(I2C_INDEX, attr->addr, reg, val);
		if (rtn < 0) {
			CAMLOGE("crane_i2c_write: Error!!  reg = 0x%x  val = 0x%x", reg, val);
			return -1;
		}
	}

	return rtn;
}

static int __detect_sensor(const struct crane_sensor_regs *id,
				const struct crane_sensor_i2c_attr *attr)
{
	int i = 0;
	uint_32 val = 0;
	int ret = 0;
	CAMLOGV("__detect_sensor:E!!!! id->num = %d", id->num);

	for (i = 0; i < id->num; i++) {
		ret = crane_i2c_read(attr, id->tab[i].reg, &val);
		if (ret || val != id->tab[i].val) {
			CAMLOGE("__detect_sensor:error! addr = 0x%x reg:0x%x, val: got 0x%x, req 0x%x  i = %d",attr->addr, id->tab[i].reg, val, id->tab[i].val, i);
			return -1;
		}
		CAMLOGI("__detect_sensor:addr = 0x%x reg:0x%x, val: got 0x%x, req 0x%x   i = %d",attr->addr, id->tab[i].reg, val, id->tab[i].val, i);
	}
	if (i == id->num) {
		CAMLOGI("__detect_sensor:success!!");
	}
	return 1;
}

/* return the num of sensor */
static int get_sensor_list(char (*name)[32])
{
	int i = 0;

	strcpy(name[0], name_debug[0]);
	strcpy(name[1], name_debug[1]);

	while(strlen(name[i]) != 0){
		CAMLOGV("sensor name[%d]=__%s__",i,name[i]);
		i++;
	}
	return i;
}

static int sensor_stream_on(SENSOR_CTRL_T *sensor_ctrl)
{
	int ret = 0;

	UOS_TakeMutex(sensor_ctrl->sensor_lock);

	if (NULL == sensor_ctrl || NULL == sensor_ctrl->sensor_data) {
		CAMLOGE("sensor_stream_on: param is NULL, return error!");
		ret = -1;
		goto end_0;
	}

	if (sensor_ctrl->stream_cnt > 0) {
		CAMLOGE("sensor_stream_on: sensor already stream on!");
		ret = 0;
		goto end_0;
	}

	ret = crane_i2c_write(&sensor_ctrl->sensor_data->i2c_attr[sensor_ctrl->sensor_data->cur_i2c_index],
			sensor_ctrl->sensor_data->streamon);
	if (ret < 0) {
		CAMLOGE("sensor_stream_on: error!");
	} else {  //stream on success
		sensor_ctrl->stream_cnt = 1;
	}
	CAMLOGI("sensor_stream_on: success!!!");
end_0:
	UOS_ReleaseMutex(sensor_ctrl->sensor_lock);
	return ret;
}

static int sensor_stream_off(SENSOR_CTRL_T *sensor_ctrl)
{
	int ret = 0;

	CAMLOGV("sensor_stream_off: E!!!");

	UOS_TakeMutex(sensor_ctrl->sensor_lock);

	if (NULL == sensor_ctrl || NULL == sensor_ctrl->sensor_data) {
		CAMLOGE("sensor_stream_off: param is NULL, return error!");
		ret = -1;
		goto end_0;
	}

	if (0 == sensor_ctrl->stream_cnt) {
		CAMLOGE("sensor_stream_off: sensor already stream off!");
		ret = 0;
		goto end_0;
	}

	ret = crane_i2c_write(&sensor_ctrl->sensor_data->i2c_attr[sensor_ctrl->sensor_data->cur_i2c_index],
			sensor_ctrl->sensor_data->streamoff);
	if (ret < 0) {
		CAMLOGE("sensor_stream_off: error!");
	} else {
		sensor_ctrl->stream_cnt = 0;
		CAMLOGI("sensor_stream_off: successful!");
	}

end_0:
	UOS_ReleaseMutex(sensor_ctrl->sensor_lock);
	return ret;
}

static int sensor_set_fmt(SENSOR_CTRL_T *sensor_ctrl, int res_idx)
{
	int ret = 0;
	int num_res = 0;

	if (NULL == sensor_ctrl || NULL == sensor_ctrl->sensor_data) {
		CAMLOGE("config_sensor_setting: param is NULL, return error!");
		ret = -1;
		goto end_0;
	}

	UOS_TakeMutex(sensor_ctrl->sensor_lock);

	num_res = sensor_ctrl->sensor_data->num_res;
	if (res_idx >= num_res) {
		CAMLOGE("invalid resolution index %d!", res_idx);
		ret = -1;
		goto end_0;
	}

	ret = crane_i2c_write(&sensor_ctrl->sensor_data->i2c_attr[sensor_ctrl->sensor_data->cur_i2c_index],
					sensor_ctrl->sensor_data->res[res_idx].regs);
	if (ret) {
		CAMLOGE("failed to config_sensor_setting width");
		ret = -1;
		goto end_0;
	}

	sensor_ctrl->sensor_init = 1;
	CAMLOGI("%s init success with res%d!",
		sensor_ctrl->sensor_data->name, res_idx);
end_0:
	UOS_ReleaseMutex(sensor_ctrl->sensor_lock);

	return ret;
}

static int sensor_set_ata(SENSOR_CTRL_T *sensor_ctrl)
{
	int ret = 0;
	struct crane_sensor_ata ata = {0x0, 0x0};

	UOS_TakeMutex(sensor_ctrl->sensor_lock);
	if (NULL == sensor_ctrl || NULL == sensor_ctrl->sensor_data) {
		CAMLOGE("sensor_set_ata: param is NULL, return error!");
		ret = -1;
		goto end_0;
	}

	ata.on = sensor_ctrl->ata_on;
	ret = sensor_ctrl->sensor_data->ops->ata_open(&ata);
	
	if (ret) {
		CAMLOGE("failed to sensor_set_ata");
		ret = -1;
		goto end_0;
	}

end_0:
	UOS_ReleaseMutex(sensor_ctrl->sensor_lock);

	return ret;
}

static int sensor_release(SENSOR_CTRL_T *sensor_ctrl)
{
	int ret = 0;

	UOS_TakeMutex(sensor_ctrl->sensor_lock);
	CAMLOGI("sensor_release:start!");
	if (NULL == sensor_ctrl || NULL == sensor_ctrl->sensor_data) {
		CAMLOGE("sensor_release:param is NULL, return error!");
		ret = -1;
		goto end_0;
	}

	if (!sensor_ctrl->sensor_init) {
		CAMLOGE("sensor_release:sensor has not inited!");
		ret = 0;
		goto end_0;
	}

	sensor_ctrl->sensor_init = 0;
	g_CurSensorCtrl = NULL;
	CAMLOGI("sensor_release:success!");

end_0:
	UOS_ReleaseMutex(sensor_ctrl->sensor_lock);
	UOS_FreeMutex(sensor_ctrl->sensor_lock);
	return ret;
}

/*
static int set_init_setting(struct crane_sensor_data *sensor_data)
{
	int ret = 0;
	int i = 0;

	CAMLOGI("set_init_setting in, num_global=%d, cur_sensor_format=%d!", sensor_data->num_global, sensor_data->cur_sensor_format);
	for (i = 0; i < sensor_data->num_global; i++) {
		if (sensor_data->cur_sensor_format == sensor_data->global_setting[i].sensor_out_format) {
			ret = crane_i2c_write(&sensor_data->i2c_attr[sensor_data->cur_i2c_index], sensor_data->global_setting[i].regs);
			if (ret < 0) {
				CAMLOGE("set_init_setting: error!");
			}
		}
	}
	return ret;
}

static int sensor_init(SENSOR_CTRL_T *sensor_ctrl)
{
	int ret = 0;
	int retry = 3;

	CAMLOGI("sensor_init in!");
	UOS_TakeMutex(sensor_ctrl->sensor_lock);

	if (NULL == sensor_ctrl || NULL == sensor_ctrl->sensor_data) {
		CAMLOGE("sensor_init:param is NULL, return error!");
		ret = -1;
		goto end_0;
	}

	if (sensor_ctrl->sensor_init) {
		CAMLOGE("sensor_init:sensor has already inited!");
		ret = 0;
		goto end_0;
	}

	ret = set_init_setting(sensor_ctrl->sensor_data);
	if (ret < 0) {
		CAMLOGE("sensor_init:set_init_setting error!");
		goto end_0;
	}

	CAMLOGI("sensor_init:success!");
	sensor_ctrl->sensor_init = 1;

end_0:
	UOS_ReleaseMutex(sensor_ctrl->sensor_lock);
	return ret;
}
*/

static int sensor_power_up(SENSOR_CTRL_T *sensor_ctrl)
{
	int ret = 0;
	CAMLOGI("sensor_power_up");
	
	UOS_TakeMutex(sensor_ctrl->sensor_lock);
	if (NULL == sensor_ctrl || NULL == sensor_ctrl->sensor_data) {
		CAMLOGE("sensor_power_up:param is NULL, return error!");
		ret = -1;
		goto end_0;
	}

	ret = config_MCLK(24);
	if (ret < 0) {
		CAMLOGE("sensor_power_up: config_MCLK failed!");
		goto end_0;
	}

	if(CAM_INF_PARALLEL == sensor_ctrl->sensor_data->interface_type)
		Camera_Pinmuxcfg(CAM_INF_PARALLEL);
	else
		Camera_Pinmuxcfg(0);

	if (NULL != sensor_ctrl->sensor_data->ops && NULL != sensor_ctrl->sensor_data->ops->s_power) {
		ret = sensor_ctrl->sensor_data->ops->s_power(1);
		if (ret < 0) {
			CAMLOGE("sensor_power_up: s_power failed!");
			goto end_0;
		}
	}

	//for camera iovdd is i2c pull up.
	TWSI_Init(STANDARD_MODE, I2C_INDEX);
end_0:
	
	UOS_ReleaseMutex(sensor_ctrl->sensor_lock);
	return ret;
}

static int sensor_power_down(SENSOR_CTRL_T *sensor_ctrl)
{
	int ret = 0;
	UOS_TakeMutex(sensor_ctrl->sensor_lock);
	if (NULL == sensor_ctrl || NULL == sensor_ctrl->sensor_data) {
		CAMLOGE("sensor_power_down:param is NULL, return error!");
		ret = -1;
		goto end_0;
	}
	
	if (NULL != sensor_ctrl->sensor_data->ops && NULL != sensor_ctrl->sensor_data->ops->s_power) {
		ret = sensor_ctrl->sensor_data->ops->s_power(0);
		if (ret < 0) {
			CAMLOGE("sensor_power_down:failed!");
		}
		goto end_0;
	}

end_0:
	UOS_ReleaseMutex(sensor_ctrl->sensor_lock);
	CAMLOGI("sensor_power_down:ret = %d!",ret);
	return ret;
}

static int sensor_get_fmt(SENSOR_CTRL_T *sensor_ctrl, struct isp_sensor_info *info)
{
	int res_idx = 0;

	res_idx = sensor_ctrl->sensor_data->cur_res_index;

	info->sensor_res_idx = res_idx;
	info->sensor_fmt = (SENSOR_FMT)sensor_ctrl->sensor_data->res[res_idx].sensor_out_format;
	info->mbus_code = sensor_ctrl->sensor_data->res[res_idx].mbus_code;
	info->sensor_outw = sensor_ctrl->sensor_data->res[res_idx].width;
	info->sensor_outh= sensor_ctrl->sensor_data->res[res_idx].height;

	CAMLOGD("%s: mbus_code = 0x%x, width = %d, height = %d",
		sensor_ctrl->sensor_data->name, info->mbus_code, info->sensor_outw, info->sensor_outh);

	return 0;
}

static int sensor_get_tuning_tab(SENSOR_CTRL_T *sensor_ctrl, struct crane_isp_tuning_data *tuning_data)
{
	if (!sensor_ctrl || !sensor_ctrl->sensor_data || !tuning_data) {
		CAMLOGE("invalid argument, return error!");
		return -1;
	}

	tuning_data->tab = sensor_ctrl->sensor_data->tuning_tab;
	tuning_data->num = sensor_ctrl->sensor_data->num_tuning_tab;

	return 0;
}

static int sensor_get_banding_tab(SENSOR_CTRL_T *sensor_ctrl, struct crane_isp_tuning_data *tuning_data, int index)
{
	if (!sensor_ctrl || !sensor_ctrl->sensor_data || !tuning_data) {
		CAMLOGE("invalid argument, return error!");
		return -1;
	}

	tuning_data->tab = sensor_ctrl->sensor_data->banding_tab[index].tab;
	tuning_data->num = sensor_ctrl->sensor_data->banding_tab[index].num;

	return 0;
}

static struct crane_sensor_ops sensor_ops = {
	sensor_stream_off,
	sensor_stream_on,
	sensor_set_fmt,
	NULL,
	sensor_release,
	sensor_power_down,
	sensor_power_up,
	sensor_get_fmt,
	sensor_set_ata,
	sensor_get_tuning_tab,
	sensor_get_banding_tab,
};

int enable_MCLK(void)
{
	camera_clr_bit(REG_CTRL_1, 1 << 28);
	return 0;
}

int disable_MCLK(void)
{
	camera_set_bit(REG_CTRL_1, 1 << 28);
	return 0;
}

int config_MCLK(uint_32 clk)
{
	CAMLOGV("config_MCLK: clk = %d !", clk);
	if (0 == clk) {
		clk = 1;
	}
	//set clk ratio
	camera_reg_write(REG_CLOCK_CTRL, (312 / clk) & 0xff);

	return 0;
}

int get_sensor_ctrl(SENSOR_CTRL_T *sensor_ctrl, int camera_id)
{
	int ret = 0;
	int i = 0, j = 0;
	char name[2][32];
	int sensor_num = 0;

	memset(name, 0, sizeof(name));
	if (NULL == sensor_ctrl) {
		CAMLOGE("get_sensor_ctrl:sensor_ctrl is NULL!");
		return -1;
	}

	if (camera_id >= CAMERA_MAX) {
		CAMLOGE("get_sensor_ctrl:camera id(%d) is not right!", camera_id);
		return -1;
	}

	sensor_ctrl->sensor_lock = UOS_NewMutex("cam_sensor");

	//read sensor name which detected
	sensor_num = get_sensor_list(&name[0]);
	if (sensor_num < 1) {
		CAMLOGE("get sensor list failed,return error!");
		goto err_exit;
	}
	CAMLOGV("get_sensor_ctrl: name[0] = %s   name[1] = %s", name[0], name[1]);

	UOS_TakeMutex(sensor_ctrl->sensor_lock);
	for (i = 0; i < sensor_num; i++) {
		for (j = 0; j < (sizeof(sensor_data_tab)/sizeof(sensor_data_tab[0])); j++) {
			if (0 == strcmp(name[i], sensor_data_tab[j]->name)) {	
				CAMLOGI("sensor_data_tab[%d]->name=%s,position=%d!", j, sensor_data_tab[j]->name, sensor_data_tab[j]->position);
				if (camera_id == sensor_data_tab[j]->position) {
					sensor_ctrl->sensor_data = sensor_data_tab[j];
					sensor_ctrl->ops = &sensor_ops;
					UOS_ReleaseMutex(sensor_ctrl->sensor_lock);
					CAMLOGI("sensor %d %s matched!", camera_id, name[i]);
					g_CurSensorCtrl = sensor_ctrl;
					return ret;
				}
			}
		}
	}
	UOS_ReleaseMutex(sensor_ctrl->sensor_lock);

	if (sensor_num > 1) {
		CAMLOGI("sensor_0:%s , sensor_1:%s", name[0], name[1]);
	} else {
		CAMLOGI("sensor_0:%s", name[0]);
	}
	CAMLOGE("sensor do not matched,return error! camera_id = %d", camera_id);
err_exit:
	UOS_FreeMutex(sensor_ctrl->sensor_lock);
	return -1;
}

#if 0
int enum_sensor_info(struct isp_sensor_info *sensor_info)
{
	int i = 0;

	if (NULL == sensor_info) {
		CAMLOGE("enum_sensor_info:sensor_info is NULL!");
		return -2;
	}

	for (i = 0; i < (sizeof(sensor_data_tab)/sizeof(sensor_data_tab[0])); i++) {
		if (sensor_info->index > sensor_data_tab[i]->num_res) {
			CAMLOGE("enum_sensor_info:sensor_info->index > resloution num!");
			return -1;
		}
		if (sensor_info->sensor_id == sensor_data_tab[i]->position) {
			sensor_info->sensor_fmt = sensor_data_tab[i]->res[sensor_info->index].sensor_out_format;
			sensor_info->sensor_outw = sensor_data_tab[i]->res[sensor_info->index].width;
			sensor_info->sensor_outh= sensor_data_tab[i]->res[sensor_info->index].height;
			sensor_info->min_fps= sensor_data_tab[i]->res[sensor_info->index].min_fps;
			sensor_info->max_fps= sensor_data_tab[i]->res[sensor_info->index].max_fps;
			return 0;
		}
	}
	return -2;
}
#endif

int init_PMU(void)
{
	media_power_on(MODULE_CAMERA);
	media_clk_enable(MEDIA_CLK_LCD_AHB|MEDIA_CLK_LCD_CI|MEDIA_CLK_PHY|MEDIA_CLK_ISP2X|MEDIA_CLK_INTERFACE|MEDIA_CLK_OFFLINE|MEDIA_CLK_OFFLINE_AXI, MODULE_CAMERA);
	return 0;
}

int deinit_PMU(void)
{
	media_power_off(MODULE_CAMERA);
	media_clk_disable(MEDIA_CLK_LCD_AHB|MEDIA_CLK_LCD_CI|MEDIA_CLK_PHY|MEDIA_CLK_ISP2X|MEDIA_CLK_INTERFACE|MEDIA_CLK_OFFLINE|MEDIA_CLK_OFFLINE_AXI, MODULE_CAMERA);
	return 0;
}

/* detect camera sensor,only support two sensor */
int sensor_detect(void)
{
	int i = 0, j = 0;
	int ret = 0;
	int index = 0;
	const struct crane_sensor_regs *id = NULL;
	const struct crane_sensor_i2c_attr *attr = NULL;

	if (sensorDetected)
		return ret;

	init_PMU();

	ret = config_MCLK(24);
	
	for (i = 0; i < (sizeof(sensor_data_tab)/sizeof(sensor_data_tab[0])); i++) {
		if (NULL != sensor_data_tab[i]->ops->s_power) {
			if(CAM_INF_PARALLEL == sensor_data_tab[i]->interface_type)
				Camera_Pinmuxcfg(CAM_INF_PARALLEL);
			else
				Camera_Pinmuxcfg(0);
			sensor_data_tab[i]->ops->s_power(1);
		}
		TWSI_Init(STANDARD_MODE, I2C_INDEX);

		for (j = 0; j < sensor_data_tab[i]->num_i2c_attr; j++) {
			id = &sensor_data_tab[i]->id;
			attr = &sensor_data_tab[i]->i2c_attr[j];
			ret = __detect_sensor(id, attr);
			if (ret > 0) {
				sensor_data_tab[i]->cur_i2c_index = j;
				CAMLOGI("sensor %s detected, i2c addr 0x%x", sensor_data_tab[i]->name, sensor_data_tab[i]->i2c_attr[j].addr);

				if (index < 2) {
					strcpy(name_debug[index], sensor_data_tab[i]->name);
				}
				index++;
			}

		}
		if (NULL != sensor_data_tab[i]->ops->s_power) {
			sensor_data_tab[i]->ops->s_power(0);
		}
	}

	deinit_PMU();
	if (index > 2) {  //max support sensor num is 2
		CAMLOGE("more than 2 sensor detected,return error!");
		return -5;
	} else if(index == 0) {
		CAMLOGE("no sensor detected ,return error!");
		return -1;
	}
	sensorDetected = 1;
	return 0;
}

//for tuning debug.
int sensor_tuning_set_reg(uint_32 regAddr, uint_32 regValue)
{
	int ret = 0;
	struct crane_sensor_regs sensor_regs;
	struct regval_tab reg_tab;

	memset(&sensor_regs, 0, sizeof(sensor_regs));
	memset(&reg_tab, 0, sizeof(reg_tab));

	reg_tab.reg = regAddr;
	reg_tab.val = regValue;

	sensor_regs.num = 1;
	sensor_regs.tab = &reg_tab;

	ret = crane_i2c_write(&g_CurSensorCtrl->sensor_data->i2c_attr[g_CurSensorCtrl->sensor_data->cur_i2c_index],
			sensor_regs);

	return ret;
}

#ifdef DEBUG_AEC
static uint_32 frame_index =0;
#endif
int update_sensor_aec(SENSOR_CTRL_T *sensor_ctrl,SENSOR_AEC_T aec)
{
	int ret = 0;

	if (NULL == sensor_ctrl || NULL == sensor_ctrl->sensor_data) {
		CAMLOGE("update_aec sensor param is NULL, return error!");
		ret = -1;
		goto end_0;
	}

	if(aec.aec_en){
#ifdef DEBUG_AEC
		raw_uart_log("sensor update_aec exp=0x%x gain=0x%x enable=0x%x index=%d \n",((aec.exp_high<<0x8)|aec.exp_low),aec.gain,aec.aec_en,frame_index);
		frame_index++;
#endif
		if (NULL != sensor_ctrl->sensor_data->ops && NULL != sensor_ctrl->sensor_data->ops->update_aec) {
			ret = sensor_ctrl->sensor_data->ops->update_aec(&aec);
			if (ret < 0) 
				goto end_0;
		}
	}
#if 0
	exp_high =  isp_reg_read(0x044c);
	exp_low =  isp_reg_read(0x0450);
	CAMLOGI("sensor update_aec 0x44c(target)=0x%x 0x450(measure)=0x%x ",exp_high,exp_low);
#endif

//	CAMLOGI("sensor update_aec:  success!!!");
	return ret;

end_0:
	CAMLOGE("sensor update_aec failed!");
	return ret;

}

