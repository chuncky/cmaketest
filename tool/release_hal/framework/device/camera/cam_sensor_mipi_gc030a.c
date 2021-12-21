#include "cam_sensor.h"
#include "tuning/cam_gc030a_tuningtab.h"

static const struct regval_tab gc030a_init_mipi_1lane[] = {
	/*SYS*/
	{0xfe, 0x80},
	{0xfe, 0x80},
	{0xfe, 0x80},
	{0xf7, 0x01},  //PLL_mode1   [2] auto_div2en   [1] div2en    [0] pll_en
	{0xf8, 0x05}, //05 //PLL_mode2  [7]freq_div2_en   [6] freq_div2_close frame mode   [5:0] divx4     0x05:30fps   0x02:15fps    
	{0xf9, 0x0f},
	{0xfa, 0x00},
	{0xfc, 0x0f},
	{0xfe, 0x00},
	
	/*ANALOG & CISCTL*/
	{0x03, 0x01},  //exp [11:8]
	{0x04, 0xc8},  //exp [ 7:0]
	{0x05, 0x03},  //HB[11:8]    0x03
	{0x06, 0x7b},  //HB[7:0]
	{0x07, 0x00},  //VB[11:8]
	{0x08, 0x06},  //VB[7:0]     0x06
	{0x0a, 0x00},
	{0x0c, 0x08},
	{0x0d, 0x01},
	{0x0e, 0xe8},
	{0x0f, 0x02},
	{0x10, 0x88},
	{0x12, 0x28},//23 add 20170110 	
	{0x17, 0x54},//Don't Change Here!!!
	{0x18, 0x12},
	{0x19, 0x07},
	{0x1a, 0x1b},
	{0x1d, 0x48},//40 travis20160318
	{0x1e, 0x50},
	{0x1f, 0x80},
	{0x23, 0x01},
	{0x24, 0xc8},
	{0x27, 0xaf},
	{0x28, 0x24},
	{0x29, 0x1a},
	{0x2f, 0x14},
	{0x30, 0x00},
	{0x31, 0x04},	
	{0x32, 0x08},
	{0x33, 0x0c},
	{0x34, 0x0d},
	{0x35, 0x0e},
	{0x36, 0x0f},
	{0x72, 0x98},
	{0x73, 0x9a},
	{0x74, 0x47},
	{0x76, 0x82},
	{0x7a, 0xcb},
	{0xc2, 0x0c},
	{0xce, 0x03},
	{0xcf, 0x48},
	{0xd0, 0x10},
	{0xdc, 0x75},
	{0xeb, 0x78},
		  
	/*ISP*/
	{0x90, 0x01},
	{0x92, 0x01},//Don't Change Here!!!
	{0x94, 0x01},//Don't Change Here!!!
	{0x95, 0x01},
	{0x96, 0xe6},//e0-nancy
	{0x97, 0x02},
	{0x98, 0x80},
//	{0x8b, 0xb0},//add nancy
//	{0x8d, 0x03},//add nancy color bar
	
	/*Gain*/
	{0xb0, 0xa0}, // 46 nancy
	{0xb1, 0x01},
	{0xb2, 0x00},	
	{0xb3, 0x40},
	{0xb4, 0x40},
	{0xb5, 0x40},
	{0xb6, 0x00},
	
	/*BLK*/
	{0x40, 0x26}, 
	{0x4e, 0x00},
	{0x4f, 0x3c},
	
	/*Dark Sun*/
	{0xe0, 0x9f},
	{0xe1, 0x90},
	{0xe4, 0x0f},
	{0xe5, 0xff},
	
	/*MIPI*/
	{0xfe, 0x03},
	{0x10, 0x00},	
	{0x01, 0x03},
	{0x02, 0x33},
	{0x03, 0x96},
	{0x04, 0x01},
	{0x05, 0x00},
	{0x06, 0x80},
	{0x11, 0x2b},
	{0x12, 0x20},
	{0x13, 0x03},
	{0x15, 0x00},
	{0x21, 0x10},
	{0x22, 0x00},
	{0x23, 0x30},
	{0x24, 0x02},
	{0x25, 0x12},
	{0x26, 0x02},
	{0x29, 0x01},
	{0x2a, 0x0a},
	{0x2b, 0x03},
	
	{0xfe, 0x00},
	{0xf9, 0x0e},
	{0xfc, 0x0e},
	{0xfe, 0x00},
	{0x25, 0xa2},
	{0x3f, 0x1a},

	{SENSOR_MDELAY, SENSOR_MDELAY, 1},

//	{0xff, 0x80},//add nancy
	{0x25, 0xe2},//add nancy


        //{0xfe, 0x00}, //shutter
	{0x03, 0x00},
        {0x04, 0x10},
        {0xb6, 0x00},  //analog gain
};

static const struct regval_tab GC030a_id[] = {
	{0xf0, 0x03},
	{0xf1, 0x0a},
};

static const struct regval_tab GC030a_vts[] = {
	{0x00, 0x00},
};

static const struct regval_tab GC030a_stream_on[] = {
	//{0xfe, 0x00},
	//{0x25, 0xe2},
	{0xfe, 0x03},
	{0x10, 0x90},
	{0xfe, 0x00},
};
static const struct regval_tab GC030a_stream_off[] = {
	{0xfe, 0x03},
	{0x10, 0x00},
	{0xfe, 0x00},
};
static const struct regval_tab GC030a_expo[] = {
	{0xfe, 0x00},    //page select
	{0x03, 0x01},    //exp_high
	{0x04, 0x00},    //exp_low
};
static const struct regval_tab GC030a_gain[] = {
	{0xfe, 0x00},    //page select
	{0xb6, 0x00},    //gain
};

static const struct regval_tab GC030a_vflip[] = {
	{0x00, 0x00},
};
static const struct regval_tab GC030a_hflip[] = {
	{0x00, 0x00},
};

static struct crane_sensor_i2c_attr GC030a_i2c_attr[] = {
	{
		I2C_8BIT,
		I2C_8BIT,
		0x21,//0x42,
	}
};

static struct crane_sensor_module GC030a_MODULE_INFO[] = {
	{
		0x01,
		"0x01",
		0,
	},
	{
		0xff,
		"0xff",
		0,
	},
};

static struct crane_sensor_resolution GC030a_resolution[] = {
	{
		SENSOR_FMT_RAW10,
		MEDIA_BUS_FMT_SBGGR10_1X10,
		640,
		480 + EXTRA_LINES_RAW_SENSOR,
		0x0708,
		0x0d06,
		30,
		15,
		SENSOR_RES_BINING1,
		{
			gc030a_init_mipi_1lane,
			ARRAY_SIZE(gc030a_init_mipi_1lane),
		},
	}
};

int GC030a_get_pixelclock(uint_32 *rate, uint_32 mclk)
{
	return 0;
}
int GC030a_get_dphy_desc(struct csi_dphy_desc *dphy_desc, uint_32 mclk)
{
	return 0;
}
int GC030a_update_otp(struct crane_sensor_otp *opt)
{
	return 0;
}


int GC030a_ata_open(struct crane_sensor_ata *ata)
{
	int ret = 0;
	struct crane_sensor_i2c_attr *i2c_attr = (struct crane_sensor_i2c_attr *)GC030a_i2c_attr;

	if(ata->on){
		ret = crane_i2c_write_single(i2c_attr, 0xfe,0x00);
		ret = crane_i2c_write_single(i2c_attr, 0x8b,0x30);
	}else {
		ret = crane_i2c_write_single(i2c_attr, 0xfe,0x00);
		ret = crane_i2c_write_single(i2c_attr, 0x8b,0x20);
	}

	return ret;
}

int GC030a_update_aec(struct crane_sensor_aec *aec)
{
	int ret = 0;
	//uint_32 val1,val2,val3 = 0;

	struct crane_sensor_i2c_attr *i2c_attr = (struct crane_sensor_i2c_attr *)GC030a_i2c_attr;

//	CAMLOGI("GC030a_update_aec: exp= 0x%x 0x%x! gain =0x%x",aec->exp_high,aec->exp_low,aec->gain);
	ret = crane_i2c_write_single(i2c_attr, 0xfe,0x00);
	if (ret < 0)
		return ret;
	ret = crane_i2c_write_single(i2c_attr, 0x03,aec->exp_high);
	if (ret < 0)
		return ret;
	ret = crane_i2c_write_single(i2c_attr, 0x04,aec->exp_low);
	if (ret < 0)
		return ret;
	ret = crane_i2c_write_single(i2c_attr, 0xb6,aec->gain);
	if (ret < 0)
		return ret;
#if 0
	ret = crane_i2c_read(i2c_attr,0x03,&val1);
	ret = crane_i2c_read(i2c_attr,0x04,&val2);
	ret = crane_i2c_read(i2c_attr,0xb6,&val3);
	CAMLOGI("sensor read exp = 0x%x 0x%x gain=0x%x",val1,val2,val3);
	ret = crane_i2c_read(i2c_attr,0xf0,&val2);
	ret = crane_i2c_read(i2c_attr,0xf1,&val3);
	CAMLOGI("sensor read ID = 0x%x 0x%x ",val2,val3);
#endif

	return ret;
}

int GC030a_s_power(int on)
{

	CAMLOGI("GC030a_s_power: %s", on?"on":"off");

	gpio_direction_output(CAMERA_PWDN1);
	gpio_set_value(CAMERA_PWDN1, 1);
	gpio_direction_output(CAMERA_PWDN2);
	gpio_set_value(CAMERA_PWDN2, 0);

	gpio_direction_output(CAMERA_PWDN0);
	gpio_set_value(CAMERA_PWDN0, 0);

	if (on > 0) {
		gpio_set_value(CAMERA_PWDN0, 1);

		camera_ldo_cfg(POWER_OFF,CAMERA_AVDD,CAM_VDD_2V80);
		msleep(2);
		camera_ldo_cfg(POWER_OFF,CAMERA_IOVDD,CAM_VDD_1V80);

		disable_MCLK();
		UOS_Sleep(1); //delay 5ms
		camera_ldo_cfg(POWER_ON,CAMERA_IOVDD,CAM_VDD_1V80);
		msleep(1);
		camera_ldo_cfg(POWER_ON,CAMERA_AVDD,CAM_VDD_2V80);
		msleep(1);
		enable_MCLK();
		msleep(1);
		gpio_set_value(CAMERA_PWDN0, 0);
		msleep(2);
		gpio_set_value(CAMERA_PWDN0, 1);
		msleep(2);
		gpio_set_value(CAMERA_PWDN0, 0);
        	msleep(2);

	} else {
		gpio_set_value(CAMERA_PWDN0, 1);
		msleep(1);
		disable_MCLK();
		msleep(1);
		camera_ldo_cfg(POWER_OFF,CAMERA_AVDD,CAM_VDD_2V80);
		msleep(2);
		camera_ldo_cfg(POWER_OFF,CAMERA_IOVDD,CAM_VDD_1V80);
		msleep(1);
		gpio_set_value(CAMERA_PWDN0, 0);
	}
	return 0;
}


struct crane_sensor_spec_ops GC030a_ops = {
	GC030a_get_pixelclock,
	GC030a_get_dphy_desc,
	GC030a_update_otp,
	GC030a_s_power,
	GC030a_ata_open,
	GC030a_update_aec,
};


struct crane_sensor_data crane_GC030a_mipi = {
	"gc030a_mipi",
	&GC030a_ops,
	CAMERA_BACK,
	GC030a_i2c_attr,
	ARRAY_SIZE(GC030a_i2c_attr),
	{
		GC030a_id,
		ARRAY_SIZE(GC030a_id),
	},
	NULL,
	0,
	GC030a_resolution,
	ARRAY_SIZE(GC030a_resolution),
	{
		GC030a_stream_on,
		ARRAY_SIZE(GC030a_stream_on),
	},
	{
		GC030a_stream_off,
		ARRAY_SIZE(GC030a_stream_off),
	},
	{
		GC030a_expo,
		ARRAY_SIZE(GC030a_expo),
	},

	{
		GC030a_vts,
		ARRAY_SIZE(GC030a_vts),
	},
	{
		GC030a_gain,
		ARRAY_SIZE(GC030a_gain),
	},
	{
		GC030a_hflip,
		ARRAY_SIZE(GC030a_hflip),
	},
	{
		GC030a_vflip,
		ARRAY_SIZE(GC030a_vflip),
	},
	GC030a_MODULE_INFO,
	ARRAY_SIZE(GC030a_MODULE_INFO),
	gc030a_tuning_tab,
	ARRAY_SIZE(gc030a_tuning_tab),
	{
		{
			gc030a_50hz_banding_tab,
			ARRAY_SIZE(gc030a_50hz_banding_tab),
		},
		{
			gc030a_60hz_banding_tab,
			ARRAY_SIZE(gc030a_60hz_banding_tab),
		},
	},
	CAM_INF_MIPI,  //CAM_SPI: spi    CAM_MIPI: mipi
	MIPI_1_LAN, //.lane_num
	0,  //0x0:sdr   0x1:ddr spi_sdr
	1,  //0x0:no crc   0x1:crc,.spi_crc
	100,//.reset_delay
	0,  //auto set .cur_i2c_index
	0, //int cur_res_index
};


