#ifndef __CRANE_SENSOR_H__
#define __CRANE_SENSOR_H__

#include <ui_os_api.h> 
#include <gpio_api.h>
#include "../../chip/isp/online_drv/cam_hal_drv_interface.h"
#include "../../chip/isp/camera_common_log.h"
#include "gpio_cus.h"
#include "pmic_api.h"

//#define OTHER_SENSOR   /* for optimize code size  */

#define msleep ui_delay_ms
/* camera interface, only support spi(1lan\2lan\4lan both sdr and ddr) and mipi(1lan) */
enum cam_interface{
	CAM_INF_SPI = 0,
	CAM_INF_MIPI = 1,
	CAM_INF_PARALLEL = 2,
};

enum cam_interface_lane{
	SPI_1_LAN = 0x0,
	SPI_2_LAN = 0x1,
	SPI_4_LAN = 0x2,
	MIPI_1_LAN =  0x3,
	PARA_8_LAN = 0x4,
};

#define I2C_INDEX 2
/* gc isp nead extra 6 lines with raw sensor */
#define EXTRA_LINES_RAW_SENSOR 6
/*
 * if want to 5m delay, set regval_tab to
 * [SENSOR_MDELAY, SENSOR_MDELAY, 5]
 */
#define SENSOR_MDELAY    (0xffff)

//#define ARRAY_SIZE(x) (sizeof(x)/sizeof(x[0]))

#define REG_CTRL_1 0x40
#define REG_CLOCK_CTRL 0x88

#if defined (CONFIG_BOARD_CRANE_EVB_Z1)
#define CAMERA_PWDN0 HAL_GPIO_79
#define CAMERA_PWDN1 HAL_GPIO_80
#define CAMERA_PWDN2 HAL_GPIO_48
#define CAMERA_RST HAL_GPIO_55
#else
#define CAMERA_PWDN0 HAL_GPIO_69
#define CAMERA_PWDN1 HAL_GPIO_61
#define CAMERA_PWDN2 HAL_GPIO_66
#define CAMERA_RST HAL_GPIO_70
#endif

#define POWER_ON 1
#define POWER_OFF 0

/* forward references */
struct crane_sensor_ops;


/* sensor position */
enum crane_sensor_pos {
	CAMERA_BACK = 0,
	CAMERA_FRONT,
	CAMERA_MAX,
};


enum crane_sensor_i2c_pos {
	AQUILAV1_SENSOR_I2C_NONE = 0,
	AQUILAV1_SENSOR_I2C_PRIMARY_MASTER,
	AQUILAV1_SENSOR_I2C_SECONDARY_MASTER,
	AQUILAV1_SENSOR_I2C_BOTH,
};

/*cropped for video*/
enum crane_sensor_res_prop {
	SENSOR_RES_BINING1,
	SENSOR_RES_BINING2,
	SENSOR_RES_CROPPED,
	SENSOR_RES_MAX,
};

struct regval_tab {
	uint_16 reg;
	uint_16 val;
	uint_16 mask;
};

enum OTP_TYPE {
	SENSOR_TO_SENSOR = 1,
	SENSOR_TO_ISP = 2,
	ISP_TO_SENSOR = 3,
	VERIFY_CRC = 4,
	GEN_CRC = 5,
	/*
	 * This OTP type will be called after detected the sensor,
	 * to get module info for module detection.
	 * Please enable this type in the sensor update_otp function,
	 * if the sensor need to support multi-module feature.
	 */
	READ_MODULE_INFO = 6,
	APPLY_TYPICAL_VALUE = 7,
	READ_OTP_SIZE = 8,
};

struct sensor_otp{
	enum OTP_TYPE otp_type;
	uint_16 lsc_otp_len;
	uint_16 wb_otp_len;
	uint_16 vcm_otp_len;
	uint_16 module_data_len;
	uint_32 crc_status;
	uint_8 otp_data[512];
	uint_8 module_data[512];
	uint_8 full_otp[4096];
	uint_16 full_otp_offset;
	uint_16 full_otp_len;
	int read_otp_len;
	uint_32 rg_typical_ratio;
	uint_32 bg_typical_ratio;
	uint_32 gg_typical_ratio;
};


struct crane_sensor_otp {
	uint_32 customer_id;
	/*
	 * The module_id should same with the value wrote in the sensor OTP,
	 * if the module use a string as id, the module_id can be set to any integer,
	 * but must same with the value return from update_otp function.
	 */
	uint_32 module_id;
	uint_32 lens_id;
	uint_32 af_cal_dir;
	uint_32 af_inf_cur;
	uint_32 af_mac_cur;
	uint_32 rg_ratio;
	uint_32 bg_ratio;
	uint_32 gg_ratio;
	uint_32 golden_rg_ratio;
	uint_32 golden_bg_ratio;
	uint_32 golden_gg_ratio;
	uint_32 awb_write;
	uint_32 b;
	uint_32 gb;
	uint_32 gr;
	uint_32 r;
	struct sensor_otp *user_otp;
	uint_32 user_data[5];
	uint_32 otp_ctrl;
	enum OTP_TYPE	otp_type;
};

enum sensor_i2c_len {
	I2C_8BIT = 0,
	I2C_16BIT,
	I2C_32BIT
};

/* MIPI related */
/* Sensor MIPI behavior descriptor, sensor driver should pass it to controller
 * driver, and let controller driver decide how to config its PHY registers */
struct csi_dphy_desc {
	uint_32 clk_mul;
	uint_32 clk_div;	/* clock_lane_freq = input_clock * clk_mul / clk_div */
	uint_32 clk_freq;
	uint_32 cl_prepare;	/* cl_* describes clock lane timing in the unit of ns */
	uint_32 cl_zero;
	uint_32 hs_prepare;	/* hs_* describes data LP to HS transition timing */
	uint_32 hs_zero;	/* in the unit of clock lane period(DDR period) */
	uint_32 nr_lane;	/* When set to 0, S/W will try to figure out a value */
};

struct mipi_csi2 {
	int dphy[5]; /* DPHY:  CSI2_DPHY1, CSI2_DPHY2, CSI2_DPHY3, CSI2_DPHY5, CSI2_DPHY6 */
	int calc_dphy;
	struct csi_dphy_desc dphy_desc;
	int enable_dpcm;
};

struct crane_cmd_i2c_data {
	const struct crane_sensor_i2c_attr *attr;
	struct regval_tab *tab;
	uint_32 num; /* the number of sensor regs*/
	uint_8 pos;
};

struct crane_size {
	uint_32 width;
	uint_32 height;
};

struct crane_sensor_ata{
	uint_32 on;
	int read_crc_value;
};

typedef struct crane_sensor_aec{
	uint_32 exp_high;
	uint_32 exp_low;
	uint_32 gain;
	uint_32 aec_en;
	uint_32 min_fps;
	uint_32 max_fps;
	CAM_BANDING banding;
}SENSOR_AEC_T;

struct crane_sensor_i2c_attr {
	enum sensor_i2c_len reg_len;
	enum sensor_i2c_len val_len;
	uint_8 addr; /* 7 bit i2c address*/
};

struct crane_sensor_regs {
	const struct regval_tab *tab;
	uint_32 num;
};

struct crane_sensor_global {
	struct crane_sensor_regs regs;
	int sensor_out_format;
};

struct crane_sensor_resolution {
	uint_32 sensor_out_format; // CZM
	uint16_t mbus_code;
	uint_16 width;
	uint_16 height;
	uint_32 hts;
	uint_32 min_vts;
	uint_8 max_fps;
	uint_8 min_fps;
	enum crane_sensor_res_prop prop;
	struct crane_sensor_regs regs;
};

struct crane_sensor_module {
	uint_32 id;
	char *name;/* as module id send to userspace, limit the size to 7 characters*/
	uint_32 apeture_size;
};

struct isp_tuning_tab {
	uint_16 reg;
	uint_16 val;
};

struct crane_isp_tuning_data {
	const struct isp_tuning_tab *tab;
	uint_32 num;
};

typedef struct crane_sensor_data {
	char * const name;
	struct crane_sensor_spec_ops *ops;
	int position;
	struct crane_sensor_i2c_attr *i2c_attr;
	int num_i2c_attr;
	struct crane_sensor_regs id;
	struct crane_sensor_global *global_setting;
	int num_global;
	struct crane_sensor_resolution *res;
	int num_res;
	struct crane_sensor_regs streamon;
	struct crane_sensor_regs streamoff;
	struct crane_sensor_regs expo_reg;
	struct crane_sensor_regs vts_reg;
	struct crane_sensor_regs gain_reg;
	struct crane_sensor_regs hflip;
	struct crane_sensor_regs vflip;
	struct crane_sensor_module *module;
	int num_module;
	const struct isp_tuning_tab *tuning_tab;
	int num_tuning_tab;
	struct crane_isp_tuning_data banding_tab[2];
	int interface_type;
	uint_8 lane_num;
	uint_8 spi_sdr;
	uint_8 spi_crc;
	int reset_delay;
	int cur_i2c_index;
	int cur_res_index;
}SENSOR_DATA_T;

typedef struct crane_sensor_ctrl {
	struct crane_sensor_data *sensor_data;
	struct crane_sensor_ops *ops;
	int sensor_init;
	int stream_cnt;
	u8 sensor_lock;/*lock interface.*/
	uint_t ata_on;
} SENSOR_CTRL_T;

struct crane_sensor_spec_ops {
	 /*pixel clk rate unit HZ*/
	int (*get_pixel_rate)(uint_32 *rate, uint_32 mclk);
	int (*get_dphy_desc)(struct csi_dphy_desc *dphy_desc, uint_32 mclk);
	int (*update_otp)(struct crane_sensor_otp *otp);
	int (*s_power)(int onoff);
	int (*ata_open)(struct crane_sensor_ata *ata);
	int (*update_aec)(struct crane_sensor_aec *aec);
	int (*set_effect)(SENSOR_CTRL_T *sensor_ctrl, uint_16 level);
	int (*set_brightness)(SENSOR_CTRL_T *sensor_ctrl, uint_16 level);
	int (*set_contrast)(SENSOR_CTRL_T *sensor_ctrl, uint_16 level);
	int (*set_wb)(SENSOR_CTRL_T *sensor_ctrl, uint_16 level);
	int (*set_saturation)(SENSOR_CTRL_T *sensor_ctrl, uint_16 level);
	int (*set_banding)(SENSOR_CTRL_T *sensor_ctrl, uint_16 level);
};

struct crane_sensor_ops {
	int (*stream_off)(SENSOR_CTRL_T *sensor_ctrl);
	int (*stream_on)(SENSOR_CTRL_T *sensor_ctrl);
	int (*set_fmt)(SENSOR_CTRL_T *sensor_ctrl, int res_idx);
	int (*config_ccic)(SENSOR_CTRL_T *sensor_ctrl);
	int (*release)(SENSOR_CTRL_T *sensor_ctrl);
	int (*power_down)(SENSOR_CTRL_T *sensor_ctrl);
	int (*power_up)(SENSOR_CTRL_T *sensor_ctrl);
	int (*get_fmt)(SENSOR_CTRL_T *sensor_ctrl, struct isp_sensor_info *info);
	int (*set_ata)(SENSOR_CTRL_T *sensor_ctrl);
	int (*get_tuning_tab)(SENSOR_CTRL_T *sensor_ctrl, struct crane_isp_tuning_data *tuning_data);
	int (*get_banding_tab)(SENSOR_CTRL_T *sensor_ctrl, struct crane_isp_tuning_data *tuning_data, int index);
};

#define ENODEV		19	/* No such device */
#define EINVAL		22	/* Invalid argument */

#define cam_sensor_ops_call(s, f, args...)                                 \
	(!(s) ? -ENODEV : (((s)->ops->f) ? (s)->ops->f((s), ##args)   \
		: -EINVAL))

int enable_MCLK(void);
int disable_MCLK(void);
int config_MCLK(uint_32 clk);
int get_sensor_ctrl(SENSOR_CTRL_T *sensor_ctrl, int camera_id);
int sensor_detect(void);
//void CameraOnlineTest(void);
int update_sensor_aec(SENSOR_CTRL_T *sensor_ctrl,SENSOR_AEC_T aec);
int crane_i2c_read(const struct crane_sensor_i2c_attr *attr, uint_32 reg, uint_32 *val);
int crane_i2c_write_single(const struct crane_sensor_i2c_attr *attr, uint_32 reg, uint_32 val);
void pinmux_config(uint32 interface);
int sensor_tuning_set_reg(uint_32 regAddr, uint_32 regValue);

#endif

