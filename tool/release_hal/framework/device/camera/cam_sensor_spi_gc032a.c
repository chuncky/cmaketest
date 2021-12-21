#include "cam_sensor.h"
#include "tuning/cam_gc032a_tuningtab.h"

//raw8 640 * 486 spi 2 lan sdr crc  20fps
static const struct regval_tab GC032a_init_raw8_spi2lan_crc_sdr[] = {
    {0xf3, 0x83},   /*System*/
    {0xf5, 0x0c},
    {0xf7, 0x13},   //	{0xf7, 0x11},
    {0xf8, 0x0f},   //20fps
    {0xf9, 0x4e},
    {0xfa, 0x10},
    {0xfc, 0x02},
    {0xfe, 0x02},
    {0x81, 0x03},
    {0xfe, 0x00},   /*Analog&Cisctl*/
    {0x03, 0x01},
    {0x04, 0xc2},
    {0x05, 0x01},
    {0x06, 0xa3},
    {0x07, 0x00},
    {0x08, 0x08},
    {0x0a, 0x04},
    {0x0c, 0x04},
    {0x0d, 0x01},
    {0x0e, 0xe8},
    {0x0f, 0x02},
    {0x10, 0x88},
    {0x17, 0x54},
    {0x19, 0x04},
    {0x1a, 0x0a},
    {0x1f, 0x40},
    {0x20, 0x30},
    {0x2e, 0x80},
    {0x2f, 0x2b},
    {0x30, 0x1a},
    {0xfe, 0x02},
    {0x03, 0x02},   //[4:0]post_tx_width
    {0x06, 0x60},   //[5:4]stsbp_mode
    {0x05, 0xd7},   //drv
    {0x12, 0x89},   //[7:6]init_ramp_mode
    {0xfe, 0x03},   /*SPI*/
    {0x51, 0x00},   //stream off
    {0x52, 0xda},
    {0x53, 0xa4},   //no crc 0x24     crc 0xa4
    {0x54, 0x00},
    {0x55, 0x00},
    {0x59, 0x30},   //	{0x59, 0x30},
    {0x5a, 0x02},   //0x01  type
    {0x5b, 0x80},
    {0x5c, 0x02},
    {0x5d, 0xe6}, // e0
    {0x5e, 0x01},
    {0x64, 0x06},
    {0xfe, 0x00},   /*blk*/
    {0x18, 0x02},
    {0xfe, 0x02},
    {0x40, 0x22},
    {0x45, 0x00},
    {0x46, 0x00},
    {0x49, 0x20},
    {0x4b, 0x3c},
    {0x50, 0x20},
    {0x42, 0x10},
    {0xfe, 0x01},   /*isp*/
    {0x0a, 0xc5},
    {0x45, 0x00},   //[6]darksun_en
    {0xfe, 0x00},
    {0x40, 0xff},
    {0x41, 0x25},
    {0x42, 0xcf},
    {0x43, 0x10},
    {0x44, 0x99},
    {0x46, 0x26},
    {0x49, 0x03},
    {0x4f, 0x01},   //[0]AEC_en
    {0xde, 0x84},
    {0xfe, 0x02},
    {0x22, 0xf6},   //CISCTL_SUN_TH_R
    {0xfe, 0x01},   /*Shading*/
    {0xc1, 0x3c},
    {0xc2, 0x50},
    {0xc3, 0x00},
    {0xc4, 0x32},
    {0xc5, 0x24},
    {0xc6, 0x16},
    {0xc7, 0x08},
    {0xc8, 0x08},
    {0xc9, 0x00},
    {0xca, 0x20},
    {0xdc, 0x8a},
    {0xdd, 0xa0},
    {0xde, 0xa6},
    {0xdf, 0x75},
    {0xfe, 0x01},   /*AWB*/
    {0x7c, 0x09},
    {0x65, 0x06},
    {0x7c, 0x08},
    {0x56, 0xf4},
    {0x66, 0x0f},
    {0x67, 0x84},
    {0x6b, 0x80},
    {0x6d, 0x12},
    {0x6e, 0xb0},
    {0x86, 0x00},
    {0x87, 0x00},
    {0x88, 0x00},
    {0x89, 0x00},
    {0x8a, 0x00},
    {0x8b, 0x00},
    {0x8c, 0x00},
    {0x8d, 0x00},
    {0x8e, 0x00},
    {0x8f, 0x00},
    {0x90, 0xef},
    {0x91, 0xe1},
    {0x92, 0x0c},
    {0x93, 0xef},
    {0x94, 0x65},
    {0x95, 0x1f},
    {0x96, 0x0c},
    {0x97, 0x2d},
    {0x98, 0x20},
    {0x99, 0xaa},
    {0x9a, 0x3f},
    {0x9b, 0x2c},
    {0x9c, 0x5f},
    {0x9d, 0x3e},
    {0x9e, 0xaa},
    {0x9f, 0x67},
    {0xa0, 0x60},
    {0xa1, 0x00},
    {0xa2, 0x00},
    {0xa3, 0x0a},
    {0xa4, 0xb6},
    {0xa5, 0xac},
    {0xa6, 0xc1},
    {0xa7, 0xac},
    {0xa8, 0x55},
    {0xa9, 0xc3},
    {0xaa, 0xa4},
    {0xab, 0xba},
    {0xac, 0xa8},
    {0xad, 0x55},
    {0xae, 0xc8},
    {0xaf, 0xb9},
    {0xb0, 0xd4},
    {0xb1, 0xc3},
    {0xb2, 0x55},
    {0xb3, 0xd8},
    {0xb4, 0xce},
    {0xb5, 0x00},
    {0xb6, 0x00},
    {0xb7, 0x05},
    {0xb8, 0xd6},
    {0xb9, 0x8c},
    {0xfe, 0x01},   /*CC*/
    {0xd0, 0x40},   //3a
    {0xd1, 0xf8},
    {0xd2, 0x00},
    {0xd3, 0xfa},
    {0xd4, 0x45},
    {0xd5, 0x02},
    {0xd6, 0x30},
    {0xd7, 0xfa},
    {0xd8, 0x08},
    {0xd9, 0x08},
    {0xda, 0x58},
    {0xdb, 0x02},
    {0xfe, 0x00},
    {0xfe, 0x00},   /*Gamma*/
    {0xba, 0x00},
    {0xbb, 0x04},
    {0xbc, 0x0a},
    {0xbd, 0x0e},
    {0xbe, 0x22},
    {0xbf, 0x30},
    {0xc0, 0x3d},
    {0xc1, 0x4a},
    {0xc2, 0x5d},
    {0xc3, 0x6b},
    {0xc4, 0x7a},
    {0xc5, 0x85},
    {0xc6, 0x90},
    {0xc7, 0xa5},
    {0xc8, 0xb5},
    {0xc9, 0xc2},
    {0xca, 0xcc},
    {0xcb, 0xd5},
    {0xcc, 0xde},
    {0xcd, 0xea},
    {0xce, 0xf5},
    {0xcf, 0xff},
    {0xfe, 0x00},   /*Auto Gamma*/
    {0x5a, 0x08},
    {0x5b, 0x0f},
    {0x5c, 0x15},
    {0x5d, 0x1c},
    {0x5e, 0x28},
    {0x5f, 0x36},
    {0x60, 0x45},
    {0x61, 0x51},
    {0x62, 0x6a},
    {0x63, 0x7d},
    {0x64, 0x8d},
    {0x65, 0x98},
    {0x66, 0xa2},
    {0x67, 0xb5},
    {0x68, 0xc3},
    {0x69, 0xcd},
    {0x6a, 0xd4},
    {0x6b, 0xdc},
    {0x6c, 0xe3},
    {0x6d, 0xf0},
    {0x6e, 0xf9},
    {0x6f, 0xff},
    {0xfe, 0x00},   /*Gain*/
    {0x70, 0x50},
    {0xfe, 0x00},   /*AEC*/
    {0x4f, 0x01},
    {0xfe, 0x01},
    {0x44, 0x04},
    {0x1f, 0x30},
    {0x20, 0x40},
    {0x26, 0x27},
    {0x27, 0x01},
    {0x28, 0xd4},
    {0x29, 0x03},
    {0x2a, 0x0c},
    {0x2b, 0x03},
    {0x2c, 0xe9},
    {0x2d, 0x07},
    {0x2e, 0xd2},
    {0x2f, 0x0b},
    {0x30, 0x6e},
    {0x31, 0x0e},
    {0x32, 0x70},
    {0x33, 0x12},
    {0x34, 0x0c},
    {0x3c, 0x10},   //[5:4] Max level setting
    {0x3e, 0x20},
    {0x3f, 0x2d},
    {0x40, 0x40},
    {0x41, 0x5b},
    {0x42, 0x82},
    {0x43, 0xb7},
    {0x04, 0x0a},
    {0x02, 0x79},
    {0x03, 0xc0},
    {0xcc, 0x08},   /*measure window*/
    {0xcd, 0x08},
    {0xce, 0xa4},
    {0xcf, 0xec},
    {0xfe, 0x00},   /*DNDD*/
    {0x81, 0xb8},   //f8
    {0x82, 0x12},
    {0x83, 0x0a},
    {0x84, 0x01},
    {0x86, 0x50},
    {0x87, 0x18},
    {0x88, 0x10},
    {0x89, 0x70},
    {0x8a, 0x20},
    {0x8b, 0x10},
    {0x8c, 0x08},
    {0x8d, 0x0a},
    {0xfe, 0x00},   /*Intpee*/
    {0x8f, 0xaa},
    {0x90, 0x9c},
    {0x91, 0x52},
    {0x92, 0x03},
    {0x93, 0x03},
    {0x94, 0x08},
    {0x95, 0x44},
    {0x97, 0x00},
    {0x98, 0x00},
    {0xfe, 0x00},   /*ASDE*/
    {0xa1, 0x30},
    {0xa2, 0x41},
    {0xa4, 0x30},
    {0xa5, 0x20},
    {0xaa, 0x30},
    {0xac, 0x32},
    {0xfe, 0x00},   /*YCP*/
    {0xd1, 0x3c},
    {0xd2, 0x3c},
    {0xd3, 0x38},
    {0xd6, 0xf4},
    {0xd7, 0x1d},
    {0xdd, 0x73},
    {0xde, 0x84},
         //{0x4c, 0x08}   //for color bar


    {0xfe, 0x00},
    {0x0a, 0x02},
    {0x55, 0x01},
    {0x56, 0xe6},

    {0xfe, 0x01},
    {0x0a, 0x65},
    {0xfe, 0x00},
    {0x4f, 0x00}, //aec off
    {0x42, 0x80}, //awb off
    {0x40, 0x1f}, //cc off
    {0x43, 0x00}, //gamma off

    {0xfe, 0x00},   //Analog&Cisctl
    {0x03, 0x03},    //0x01   exp_high[3:0]   exposure[11:8]
    {0x04, 0x58},    //0xc2   exp_low[7:0]    exposure[7:0]
};

//yuv422 640 * 480 spi 2 lan sdr
static const struct regval_tab gc032a_init_yuv422_spi2lan_crc_sdr[] = {
    {0xf3, 0x83},   /*System*/
    {0xf5, 0x0c},
    {0xf7, 0x13},   //	{0xf7, 0x11},
	{0xf8, 0x0f},	//20fps
    {0xf9, 0x4e},
    {0xfa, 0x31}, //{0xfa, 0x10},
    {0xfc, 0x02},
    {0xfe, 0x02},
    {0x81, 0x03},
    {0xfe, 0x00},   /*Analog&Cisctl*/
    {0x03, 0x01},
    {0x04, 0xc2},
    {0x05, 0x01},
    {0x06, 0xa3},
    {0x07, 0x00},
    {0x08, 0x08},
    {0x0a, 0x04},
    {0x0c, 0x04},
    {0x0d, 0x01},
    {0x0e, 0xe8},
    {0x0f, 0x02},
    {0x10, 0x88},
    {0x17, 0x54},
    {0x19, 0x04},
    {0x1a, 0x0a},
    {0x1f, 0x40},
    {0x20, 0x30},
    {0x2e, 0x80},
    {0x2f, 0x2b},
    {0x30, 0x1a},
    {0xfe, 0x02},
    {0x03, 0x02},   //[4:0]post_tx_width
    {0x06, 0x60},   //[5:4]stsbp_mode
    {0x05, 0xd7},   //drv
    {0x12, 0x89},   //[7:6]init_ramp_mode
    {0xfe, 0x03},   /*SPI*/
    {0x51, 0x00},   //stream off
    {0x52, 0xda},
    {0x53, 0xa4},   //no crc 0x24     crc 0xa4
    {0x54, 0x20},
    {0x55, 0x00},
    {0x59, 0x10},   //	{0x59, 0x30},
    {0x5a, 0x00},   //0x01  type
    {0x5b, 0x80},
    {0x5c, 0x02},
    {0x5d, 0xe0}, // e0
    {0x5e, 0x01},
    {0x64, 0x06},
    {0xfe, 0x00},   /*blk*/
    {0x18, 0x02},
    {0xfe, 0x02},
    {0x40, 0x22},
    {0x45, 0x00},
    {0x46, 0x00},
    {0x49, 0x20},
    {0x4b, 0x3c},
    {0x50, 0x20},
    {0x42, 0x10},
    {0xfe, 0x01},   /*isp*/
    {0x0a, 0xc5},
    {0x45, 0x00},   //[6]darksun_en
    {0xfe, 0x00},
    {0x40, 0xff},
    {0x41, 0x25},
    {0x42, 0xcf},
    {0x43, 0x10},
    {0x44, 0x83},
    {0x46, 0x26},
    {0x49, 0x03},
    {0x4f, 0x01},   //[0]AEC_en
    {0xde, 0x84},
    {0xfe, 0x02},
    {0x22, 0xf6},   //CISCTL_SUN_TH_R
    {0xfe, 0x01},   /*Shading*/
    {0xc1, 0x3c},
    {0xc2, 0x50},
    {0xc3, 0x00},
    {0xc4, 0x32},
    {0xc5, 0x24},
    {0xc6, 0x16},
    {0xc7, 0x08},
    {0xc8, 0x08},
    {0xc9, 0x00},
    {0xca, 0x20},
    {0xdc, 0x8a},
    {0xdd, 0xa0},
    {0xde, 0xa6},
    {0xdf, 0x75},
    {0xfe, 0x01},   /*AWB*/
    {0x7c, 0x09},
    {0x65, 0x06},
    {0x7c, 0x08},
    {0x56, 0xf4},
    {0x66, 0x0f},
    {0x67, 0x84},
    {0x6b, 0x80},
    {0x6d, 0x12},
    {0x6e, 0xb0},
    {0x86, 0x00},
    {0x87, 0x00},
    {0x88, 0x00},
    {0x89, 0x00},
    {0x8a, 0x00},
    {0x8b, 0x00},
    {0x8c, 0x00},
    {0x8d, 0x00},
    {0x8e, 0x00},
    {0x8f, 0x00},
    {0x90, 0xef},
    {0x91, 0xe1},
    {0x92, 0x0c},
    {0x93, 0xef},
    {0x94, 0x65},
    {0x95, 0x1f},
    {0x96, 0x0c},
    {0x97, 0x2d},
    {0x98, 0x20},
    {0x99, 0xaa},
    {0x9a, 0x3f},
    {0x9b, 0x2c},
    {0x9c, 0x5f},
    {0x9d, 0x3e},
    {0x9e, 0xaa},
    {0x9f, 0x67},
    {0xa0, 0x60},
    {0xa1, 0x00},
    {0xa2, 0x00},
    {0xa3, 0x0a},
    {0xa4, 0xb6},
    {0xa5, 0xac},
    {0xa6, 0xc1},
    {0xa7, 0xac},
    {0xa8, 0x55},
    {0xa9, 0xc3},
    {0xaa, 0xa4},
    {0xab, 0xba},
    {0xac, 0xa8},
    {0xad, 0x55},
    {0xae, 0xc8},
    {0xaf, 0xb9},
    {0xb0, 0xd4},
    {0xb1, 0xc3},
    {0xb2, 0x55},
    {0xb3, 0xd8},
    {0xb4, 0xce},
    {0xb5, 0x00},
    {0xb6, 0x00},
    {0xb7, 0x05},
    {0xb8, 0xd6},
    {0xb9, 0x8c},
    {0xfe, 0x01},   /*CC*/
    {0xd0, 0x40},   //3a
    {0xd1, 0xf8},
    {0xd2, 0x00},
    {0xd3, 0xfa},
    {0xd4, 0x45},
    {0xd5, 0x02},
    {0xd6, 0x30},
    {0xd7, 0xfa},
    {0xd8, 0x08},
    {0xd9, 0x08},
    {0xda, 0x58},
    {0xdb, 0x02},
    {0xfe, 0x00},
    {0xfe, 0x00},   /*Gamma*/
    {0xba, 0x00},
    {0xbb, 0x04},
    {0xbc, 0x0a},
    {0xbd, 0x0e},
    {0xbe, 0x22},
    {0xbf, 0x30},
    {0xc0, 0x3d},
    {0xc1, 0x4a},
    {0xc2, 0x5d},
    {0xc3, 0x6b},
    {0xc4, 0x7a},
    {0xc5, 0x85},
    {0xc6, 0x90},
    {0xc7, 0xa5},
    {0xc8, 0xb5},
    {0xc9, 0xc2},
    {0xca, 0xcc},
    {0xcb, 0xd5},
    {0xcc, 0xde},
    {0xcd, 0xea},
    {0xce, 0xf5},
    {0xcf, 0xff},
    {0xfe, 0x00},   /*Auto Gamma*/
    {0x5a, 0x08},
    {0x5b, 0x0f},
    {0x5c, 0x15},
    {0x5d, 0x1c},
    {0x5e, 0x28},
    {0x5f, 0x36},
    {0x60, 0x45},
    {0x61, 0x51},
    {0x62, 0x6a},
    {0x63, 0x7d},
    {0x64, 0x8d},
    {0x65, 0x98},
    {0x66, 0xa2},
    {0x67, 0xb5},
    {0x68, 0xc3},
    {0x69, 0xcd},
    {0x6a, 0xd4},
    {0x6b, 0xdc},
    {0x6c, 0xe3},
    {0x6d, 0xf0},
    {0x6e, 0xf9},
    {0x6f, 0xff},
    {0xfe, 0x00},   /*Gain*/
    {0x70, 0x50},
    {0xfe, 0x00},   /*AEC*/
    {0x4f, 0x01},
    {0xfe, 0x01},
    {0x44, 0x04},
    {0x1f, 0x30},
    {0x20, 0x40},
    {0x26, 0x27},
    {0x27, 0x01},
    {0x28, 0xd4},
    {0x29, 0x03},
    {0x2a, 0x0c},
    {0x2b, 0x03},
    {0x2c, 0xe9},
    {0x2d, 0x07},
    {0x2e, 0xd2},
    {0x2f, 0x0b},
    {0x30, 0x6e},
    {0x31, 0x0e},
    {0x32, 0x70},
    {0x33, 0x12},
    {0x34, 0x0c},
    {0x3c, 0x10},   //[5:4] Max level setting
    {0x3e, 0x20},
    {0x3f, 0x2d},
    {0x40, 0x40},
    {0x41, 0x5b},
    {0x42, 0x82},
    {0x43, 0xb7},
    {0x04, 0x0a},
    {0x02, 0x79},
    {0x03, 0xc0},
    {0xcc, 0x08},   /*measure window*/
    {0xcd, 0x08},
    {0xce, 0xa4},
    {0xcf, 0xec},
    {0xfe, 0x00},   /*DNDD*/
    {0x81, 0xb8},   //f8
    {0x82, 0x12},
    {0x83, 0x0a},
    {0x84, 0x01},
    {0x86, 0x50},
    {0x87, 0x18},
    {0x88, 0x10},
    {0x89, 0x70},
    {0x8a, 0x20},
    {0x8b, 0x10},
    {0x8c, 0x08},
    {0x8d, 0x0a},
    {0xfe, 0x00},   /*Intpee*/
    {0x8f, 0xaa},
    {0x90, 0x9c},
    {0x91, 0x52},
    {0x92, 0x03},
    {0x93, 0x03},
    {0x94, 0x08},
    {0x95, 0x44},
    {0x97, 0x00},
    {0x98, 0x00},
    {0xfe, 0x00},   /*ASDE*/
    {0xa1, 0x30},
    {0xa2, 0x41},
    {0xa4, 0x30},
    {0xa5, 0x20},
    {0xaa, 0x30},
    {0xac, 0x32},
    {0xfe, 0x00},   /*YCP*/
    {0xd1, 0x3c},
    {0xd2, 0x3c},
    {0xd3, 0x38},
    {0xd6, 0xf4},
    {0xd7, 0x1d},
    {0xdd, 0x73},
    {0xde, 0x84},
         //{0x4c, 0x08}   //for color bar
};

static const struct regval_tab GC032a_id[] = {
	{0xf0, 0x23},
	{0xf1, 0x2a},
};
static const struct regval_tab GC032a_vts[] = {
	{0x00, 0x00},
};

static const struct regval_tab GC032a_stream_on[] = {
	{0xfe, 0x03},
	{0x51, 0x01},
};
static const struct regval_tab GC032a_stream_off[] = {
	{0xfe, 0x03},
	{0x51, 0x00},
};
static const struct regval_tab GC032a_expo[] = {
	{0xfe, 0x00},    //page select
	{0x03, 0x01},    //exp_high
	{0x04, 0x00},    //exp_low
};
static const struct regval_tab GC032a_gain[] = {
	{0xfe, 0x00},    //page select
	{0x48, 0x01},    //gain
};


static const struct regval_tab GC032a_vflip[] = {
	{0x00, 0x00},
};
static const struct regval_tab GC032a_hflip[] = {
	{0x00, 0x00},
};

static struct crane_sensor_i2c_attr GC032a_i2c_attr[] = {
	{
		I2C_8BIT,
		I2C_8BIT,
		0x21,//0x42,
	}
};

static struct crane_sensor_module GC032a_MODULE_INFO[] = {
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

static struct crane_sensor_resolution GC032a_resolution[] = {
	{
		SENSOR_FMT_RAW8,
		MEDIA_BUS_FMT_SRGGB8_1X8,
		640,
		480 + EXTRA_LINES_RAW_SENSOR,
		0x0708,
		0x0d06,
		30,
		15,
		SENSOR_RES_BINING1,
		{
			GC032a_init_raw8_spi2lan_crc_sdr,
			ARRAY_SIZE(GC032a_init_raw8_spi2lan_crc_sdr),
		},
	},
	{
		SENSOR_FMT_YUV422,
		MEDIA_BUS_FMT_YVYU8_2X8,
		640,
		480,
		0x0708,
		0x0d06,
		30,
		15,
		SENSOR_RES_BINING1,
		{
			gc032a_init_yuv422_spi2lan_crc_sdr,
			ARRAY_SIZE(gc032a_init_yuv422_spi2lan_crc_sdr),
		},
	},
};

int GC032a_get_pixelclock(uint_32 *rate, uint_32 mclk)
{
	return 0;
}
int GC032a_get_dphy_desc(struct csi_dphy_desc *dphy_desc, uint_32 mclk)
{
	return 0;
}
int GC032a_update_otp(struct crane_sensor_otp *opt)
{
	return 0;
}


int GC032a_ata_open(struct crane_sensor_ata *ata)
{
	return 0;
}

int GC032a_update_aec(struct crane_sensor_aec *aec)
{
	int ret = 0;
//	uint_32 val1,val2,val3 = 0;

	struct crane_sensor_i2c_attr *i2c_attr = (struct crane_sensor_i2c_attr *)GC032a_i2c_attr;

//	CAMLOGI("GC032a_update_aec: exp= 0x%x 0x%x! gain =0x%x",aec->exp_high,aec->exp_low,aec->gain);
	ret = crane_i2c_write_single(i2c_attr, 0xfe,0x00);
	if (ret < 0)
		CAMLOGE("sensor update_aec: error!");
	ret = crane_i2c_write_single(i2c_attr, 0x03,aec->exp_high);
	if (ret < 0)
		CAMLOGE("sensor update_aec exp: error!");
	ret = crane_i2c_write_single(i2c_attr, 0x04,aec->exp_low);
	if (ret < 0)
		CAMLOGE("sensor update_aec exp: error!");
	ret = crane_i2c_write_single(i2c_attr, 0x48,aec->gain);
	if (ret < 0)
		CAMLOGE("sensor update_aec gain: error!");
#if 0
	ret = crane_i2c_read(i2c_attr,0x03,&val1);
	ret = crane_i2c_read(i2c_attr,0x04,&val2);
	ret = crane_i2c_read(i2c_attr,0x48,&val3);
	CAMLOGI("sensor read exp = 0x%x 0x%x gain=0x%x",val1,val2,val3);
	ret = crane_i2c_read(i2c_attr,0xf0,&val2);
	ret = crane_i2c_read(i2c_attr,0xf1,&val3);
	CAMLOGI("sensor read ID = 0x%x 0x%x ",val2,val3);
#endif

	return ret;
}


int GC032a_s_power(int on)
{
	CAMLOGI("GC032a_s_power: %s", on?"on":"off");

	gpio_direction_output(CAMERA_PWDN0);
	gpio_set_value(CAMERA_PWDN0, 1);
	gpio_direction_output(CAMERA_PWDN2);
	gpio_set_value(CAMERA_PWDN2, 0);

	gpio_direction_output(CAMERA_PWDN1);
	gpio_set_value(CAMERA_PWDN1, 0);

	if (on > 0) {
		gpio_set_value(CAMERA_PWDN1, 0);
		msleep(2);
		camera_ldo_cfg(POWER_OFF,CAMERA_AVDD,CAM_VDD_2V80);
		msleep(2);
		camera_ldo_cfg(POWER_OFF,CAMERA_IOVDD,CAM_VDD_1V80);
		msleep(2);

		disable_MCLK();
		UOS_Sleep(1); //delay 5ms
		camera_ldo_cfg(POWER_ON,CAMERA_IOVDD,CAM_VDD_1V80);
		msleep(1);
		camera_ldo_cfg(POWER_ON,CAMERA_AVDD,CAM_VDD_2V80);
		msleep(1);
		enable_MCLK();
		msleep(1);
		gpio_set_value(CAMERA_PWDN1, 1);
		msleep(2);
		gpio_set_value(CAMERA_PWDN1, 0);
        	msleep(2);

	} else {
		gpio_set_value(CAMERA_PWDN1, 1);
		msleep(1);
		disable_MCLK();
		msleep(1);
		camera_ldo_cfg(POWER_OFF,CAMERA_AVDD,CAM_VDD_2V80);
		msleep(2);
		camera_ldo_cfg(POWER_OFF,CAMERA_IOVDD,CAM_VDD_1V80);
		msleep(1);
		gpio_set_value(CAMERA_PWDN1, 0);
	}
	return 0;
}

int GC032a_YUV_set_effect(SENSOR_CTRL_T *sensor_ctrl, uint_16 level)
{
	int ret = 0;
	int cur_i2c_index = 0;
	struct crane_sensor_i2c_attr *i2c_attr = NULL;

	CAMLOGI("GC032a_YUV_set_effect mode %d", level);

	cur_i2c_index = sensor_ctrl->sensor_data->cur_i2c_index;
	i2c_attr = &(sensor_ctrl->sensor_data->i2c_attr[cur_i2c_index]);

	switch (level){
		case HAL_EFFECT_NORMAL:
			crane_i2c_write_single(i2c_attr,0xfe, 0x00);
			crane_i2c_write_single(i2c_attr,0x43, 0x10);
			break;
		case HAL_EFFECT_COLORINV:
			crane_i2c_write_single(i2c_attr,0xfe, 0x00);
			crane_i2c_write_single(i2c_attr,0x43, 0x11);
			break;
		case HAL_EFFECT_BLACKBOARD:
			crane_i2c_write_single(i2c_attr,0xfe, 0x00);
			crane_i2c_write_single(i2c_attr,0x43, 0x15);
			break;
		case HAL_EFFECT_WHITEBOARD:
			crane_i2c_write_single(i2c_attr,0xfe, 0x00);
			crane_i2c_write_single(i2c_attr,0x43, 0x14);
			break;
		case HAL_EFFECT_ANTIQUE:
			crane_i2c_write_single(i2c_attr,0xfe, 0x00);
			crane_i2c_write_single(i2c_attr,0x43, 0x12);
			crane_i2c_write_single(i2c_attr,0xda, 0x90);
			crane_i2c_write_single(i2c_attr,0xdb, 0x30);
			break;
		case HAL_EFFECT_RED:
			crane_i2c_write_single(i2c_attr,0xfe, 0x00);
			crane_i2c_write_single(i2c_attr,0x43, 0x12);
			crane_i2c_write_single(i2c_attr,0xda, 0x12);
			crane_i2c_write_single(i2c_attr,0xdb, 0x12);
			break;
		case HAL_EFFECT_GREEN:
			crane_i2c_write_single(i2c_attr,0xfe, 0x00);
			crane_i2c_write_single(i2c_attr,0x43, 0x12);
			crane_i2c_write_single(i2c_attr,0xda, 0xc0);
			crane_i2c_write_single(i2c_attr,0xdb, 0xc0);
			break;
		case HAL_EFFECT_BLUE:
			crane_i2c_write_single(i2c_attr,0xfe, 0x00);
			crane_i2c_write_single(i2c_attr,0x43, 0x12);
			crane_i2c_write_single(i2c_attr,0xda, 0x60);
			crane_i2c_write_single(i2c_attr,0xdb, 0xc0);
			break;
		case HAL_EFFECT_BLACKWHITE:
			crane_i2c_write_single(i2c_attr,0xfe, 0x00);
			crane_i2c_write_single(i2c_attr,0x43, 0x12);
			crane_i2c_write_single(i2c_attr,0xda, 0x00);
			crane_i2c_write_single(i2c_attr,0xdb, 0x00);
			break;
		case HAL_EFFECT_NEGATIVE:
			crane_i2c_write_single(i2c_attr,0xfe, 0x00);
			crane_i2c_write_single(i2c_attr,0x43, 0x11);
			break;
		default:
			CAMLOGE("invalid effect mode %d", level);
			ret = -EINVAL;
	}
	return ret;

}

int GC032a_YUV_set_brightness(SENSOR_CTRL_T *sensor_ctrl, uint_16 level)
{
	int ret = 0;
	int cur_i2c_index = 0;
	struct crane_sensor_i2c_attr *i2c_attr = NULL;

	CAMLOGI("GC032a_YUV_set_brightness level %d", level);

	cur_i2c_index = sensor_ctrl->sensor_data->cur_i2c_index;
	i2c_attr = &(sensor_ctrl->sensor_data->i2c_attr[cur_i2c_index]);

	switch (level){
		case HAL_BRIGHTNESS_1:
			crane_i2c_write_single(i2c_attr,0xfe, 0x00);
			break;
		case HAL_BRIGHTNESS_2:
			crane_i2c_write_single(i2c_attr,0xfe, 0x00);
			break;
		case HAL_BRIGHTNESS_3:
			crane_i2c_write_single(i2c_attr,0xfe, 0x00);
			break;
		case HAL_BRIGHTNESS_4:
			crane_i2c_write_single(i2c_attr,0xfe, 0x00);
			break;
		case HAL_BRIGHTNESS_mid:
			crane_i2c_write_single(i2c_attr,0xfe, 0x00);
			break;
		case HAL_BRIGHTNESS_6:
			crane_i2c_write_single(i2c_attr,0xfe, 0x00);
			break;
		case HAL_BRIGHTNESS_7:
			crane_i2c_write_single(i2c_attr,0xfe, 0x00);
			break;
		case HAL_BRIGHTNESS_8:
			crane_i2c_write_single(i2c_attr,0xfe, 0x00);
			break;
		case HAL_BRIGHTNESS_9:
			crane_i2c_write_single(i2c_attr,0xfe, 0x00);
			break;
		default:
			CAMLOGE("invalid brightness level %d", level);
			ret = -EINVAL;
	}
	return ret;

}

int GC032a_YUV_set_banding(SENSOR_CTRL_T *sensor_ctrl, uint_16 level)
{
	int ret = 0;
	int cur_i2c_index = 0;
	struct crane_sensor_i2c_attr *i2c_attr = NULL;

	CAMLOGI("GC032a_YUV_set_banding mode %d", level);

	cur_i2c_index = sensor_ctrl->sensor_data->cur_i2c_index;
	i2c_attr = &(sensor_ctrl->sensor_data->i2c_attr[cur_i2c_index]);

	switch (level){
		case HAL_BANDING_AUTO:
			crane_i2c_write_single(i2c_attr,0xfe, 0x00);
			break;
		case HAL_BANDING_50HZ:
			crane_i2c_write_single(i2c_attr,0xfe, 0x00);
			break;
		case HAL_BANDING_60HZ:
			crane_i2c_write_single(i2c_attr,0xfe, 0x00);
			break;
		default:
			CAMLOGE("invalid banding mode %d", level);
			ret = -EINVAL;
	}
	return ret;

}

struct crane_sensor_spec_ops GC032a_ops = {
	GC032a_get_pixelclock,
	GC032a_get_dphy_desc,
	GC032a_update_otp,
	GC032a_s_power,
	GC032a_ata_open,
	GC032a_update_aec,
	GC032a_YUV_set_effect,
	GC032a_YUV_set_brightness,
	NULL,
	NULL,
	NULL,
	GC032a_YUV_set_banding,
};

struct crane_sensor_data crane_GC032a_spi = {
	"gc032a_spi",
	&GC032a_ops,
	//CAMERA_BACK,	//	#ifndef CAM_SUPPORT_MULTIPLE
	CAMERA_FRONT,	//	#ifdef CAM_SUPPORT_MULTIPLE	
	GC032a_i2c_attr,
	ARRAY_SIZE(GC032a_i2c_attr),
	{
		GC032a_id,
		ARRAY_SIZE(GC032a_id),
	},
	NULL,
	0,
	GC032a_resolution,
	ARRAY_SIZE(GC032a_resolution),
	{
		GC032a_stream_on,
		ARRAY_SIZE(GC032a_stream_on),
	},
	{
		GC032a_stream_off,
		ARRAY_SIZE(GC032a_stream_off),
	},
	{
		GC032a_expo,
		ARRAY_SIZE(GC032a_expo),
	},
	{
		GC032a_vts,
		ARRAY_SIZE(GC032a_vts),
	},
	{
		GC032a_gain,
		ARRAY_SIZE(GC032a_gain),
	},
	{
		GC032a_hflip,
		ARRAY_SIZE(GC032a_hflip),
	},
	{
		GC032a_vflip,
		ARRAY_SIZE(GC032a_vflip),
	},
	GC032a_MODULE_INFO,
	ARRAY_SIZE(GC032a_MODULE_INFO),
	gc032a_tuning_tab,
	ARRAY_SIZE(gc032a_tuning_tab),
	{
		{
			gc032a_50hz_banding_tab,
			ARRAY_SIZE(gc032a_50hz_banding_tab),
		},
		{
			gc032a_60hz_banding_tab,
			ARRAY_SIZE(gc032a_60hz_banding_tab),
		},
	},
	CAM_INF_SPI,  //CAM_SPI: spi    CAM_MIPI: mipi
	SPI_2_LAN,// .lane_num
	0,  //0x0:sdr   0x1:ddr spi_sdr
	1,  //0x0:no crc   0x1:crc,.spi_crc
	100,//.reset_delay
	0,  //auto set .cur_i2c_index
	0, //int cur_res_index
};
