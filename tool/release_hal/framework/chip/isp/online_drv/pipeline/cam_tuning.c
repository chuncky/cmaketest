#include <ui_os_api.h> 
#include "ui_mem_api.h"
#include "fs_api.h"
#include <stdlib.h>
#include "../../camera_common_log.h"
#include "string.h"
#include "../cam_tuning.h"
#include "cam_pipeline_reg.h"

#define TUNING_FILE_NAME "D://vendor/camera/camera_tuning.data"
/* camera_tuning.data format
@@sensor!!
{sensor_reg_addr0,sensor_reg_data0},
{sensor_reg_addr1,sensor_reg_data1},
...
@@isp!!
{isp_reg_addr0,isp_reg_data0},
{isp_reg_addr1,isp_reg_data1},
...
*/

int cam_isp_tuning_file_analyze(void)
{
	int_32 fileHandle = 0, fileSize = 0, readLen = 0, ret = 0;
	char*pBuf = NULL, *endPtr = NULL, *startPtr = NULL;
	int_32 index = 0, arrayCnt = 0;
	uint_32 regAddr = 0, regValue = 0;
	u8 nextLine = 0;
	int_32 regType = -1; //0:sensor,1:isp,-1:invalid
	char strBuf[7];
	char debugIsp[] = "isp";
	char debugsensor[] = "sensor";
	BOOL isExist = FALSE;

	isExist = FS_IsExist(TUNING_FILE_NAME);

	if(isExist) {
		fileHandle = FS_Open(TUNING_FILE_NAME, FS_O_RDONLY, 0);}
	else {
		CAMLOGI("there is no debug tuning file [%s]",TUNING_FILE_NAME);
		return -1;
	}

	if (fileHandle < 0) {
		CAMLOGI("open debug tuning file failed [%s]",TUNING_FILE_NAME);
		return -1;
	}

	fileSize = FS_GetFileSize(fileHandle);
	if (fileSize <= 0) {
		CAMLOGW("the size of tuning file is invalid!");
		ret = -1;
		goto error_exit;
	}

	pBuf = UOS_MALLOC(fileSize);
	if (!pBuf) {
		CAMLOGW("malloc 0x%x bytes for tuning file failed!", fileSize);
		ret = -1;
		goto error_exit;
	}

	//read file to buffer.
	readLen = FS_Read(fileHandle, (uint_8 *)pBuf, fileSize);
	if (readLen <= 0 || readLen < fileSize) {
		CAMLOGW("read tuning file is failed,read 0x%x,filesize=0x%x!");
		ret = -1;
		goto error_exit;
	}

	while (index < readLen) {
		if (nextLine) {
			if (pBuf[index] == '\r' || pBuf[index] == '\n')
				nextLine = 0;
		} else if (pBuf[index] == ' ' || pBuf[index] == '\t') {
			//do nothing.
		} else if (pBuf[index] == '/' && pBuf[index+1] == '/') {
			while (pBuf[index] != '\r' && pBuf[index] != '\n') {
				index++;
			}
		} else if (pBuf[index] == '/' && pBuf[index+1] == '*') {
			index+=1;
			while (1) {
				index++;
				if(pBuf[index] == '*' && pBuf[index+1] == '/')
					break;
			}
			index+=1;
			nextLine =1;
		} else if (pBuf[index] == '{') {
			index++;
			while (pBuf[index] != '0' || (pBuf[index+1] != 'x' && pBuf[index+1] != 'X')) {
				index++;
				if (index >= readLen) {
					CAMLOGW("can not get valid reg address!");
					goto error_exit;
				}
			}

			//reg addr
			regAddr = strtoul(pBuf + index, &endPtr, 0);
			if (endPtr == pBuf + index) {
				CAMLOGW("can not get valid reg address!");
				goto error_exit;
			}

			//reg value
			while (endPtr[0] != '0' || (endPtr[1] != 'x' && endPtr[1] != 'X')) {
				endPtr++;
				if ('\r' == *endPtr || '\n' == *endPtr) {
					CAMLOGW("can not get valid reg value!");
					goto error_exit;
				}
			}

			startPtr = endPtr;
			regValue = strtoul(startPtr, &endPtr, 0);
			if (startPtr == endPtr) {
				CAMLOGW("can not get valid reg value!");
				goto error_exit;
			}

			if (endPtr[0] == '}') {
				if (regType == 0) {
					CAMLOGI("get sensor tuning data reg=0x%x,vaule=0x%x", regAddr, regValue);
					ret = sensor_tuning_set_reg(regAddr, regValue);
					if (ret < 0)
						CAMLOGW("set sensor tuning reg=0x%x,vaule=0x%x failed!", regAddr, regValue);
				} else if (regType == 1) {
					CAMLOGI("get isp tuning data reg=0x%x,vaule=0x%x", regAddr, regValue);
					isp_reg_write(regAddr, regValue);
				}

				nextLine = 1;
				index += endPtr - (pBuf + index);
			}
		} else if (pBuf[index] == '@' && (pBuf[index+1] == '@')) {
			arrayCnt = 0;
			memset(strBuf, ' ', 7);
			index += 2;
			while (pBuf[index] != '!' || pBuf[index+1] != '!') {
				if (arrayCnt >= 7) {
					CAMLOGW("too long name for register type!");
					goto error_exit;
				}
				strBuf[arrayCnt] = pBuf[index];
				arrayCnt++;
				index++;
				if (index >= readLen) {
					CAMLOGW("get register name failed!");
					goto error_exit;
				}
			}

			//skip second '!'
			index++;
			if (0 == strncmp(strBuf, debugsensor, strlen(debugsensor))) {
				regType = 0;
			} else if (0 == strncmp(strBuf, debugIsp, strlen(debugIsp))) {
				regType = 1;
			} else {
				CAMLOGW("invalid reg type from tuning data!");
				goto error_exit;
			}
		}

		index++;
	}
error_exit:
	if (pBuf)
		UOS_FREE(pBuf);
	if (fileHandle > 0)
		FS_Close(fileHandle);

	return ret;
}

static const struct isp_tuning_tab default_tuning_tab[] = {
	//////////////////  ISP reg  ////////////////////
	//isp_reg_write(0x5C,0x14); // [5:4] CFA
	{0x200, 0x48}, // [5:4] CFA//outdoor gamma_en, GAMMA_en, CC_en, EE_en, INTP_en, DN_en, DD_en,LSC_en
	{0x204, 0x00}, //BLK dither mode, ll_y_en ,skin_en, edge SA, new_skin_mode, autogray_en,ll_gamma_en,BFF test image
	{0x208, 0x02}, //auto_SA, auto_EE, auto_DN, auto_DD, auto_LSC, ABS_en, AWB_en, NA   0x00000002
	{0x20C, 0x00},
	{0x214, 0x08}, //frame start num thd
	{0x224, 0x03},
	{0x2C0, 0x40},
	{0x3B0, 0x02}, //AWB measure window
	{0x3B4, 0x04},
	{0x3B8, 0x26},
	{0x3BC, 0x39},
	{0x2D8, 0x01}, // AEC en   //0x00000001
	/////////// AEC ////////////////////////
	{0x404, 0x08}, //AEC X1
	{0x408, 0x50}, //AEC X2
	{0x40C, 0x04}, //AEC Y1
	{0x410, 0x39}, //AEC Y2
	{0x414, 0x14}, //AEC center X1
	{0x418, 0x3c}, //AEC center X2
	{0x41C, 0x0f}, //AEC center Y1
	{0x420, 0x2d}, //AEC center Y2
	{0x424, 0x00}, //AEC show mode
	{0x428, 0xc2}, //01 //81 //col gain
	{0x430, 0x10}, //AEC_mode3 [1]raw_skip_mode  [0]fix_target_mode
	{0x434, 0x00},
	{0x44C, 0x50}, //AEC Y target 0x2d
	{0x45C, 0x00}, //AEC ignore mode
	{0x470, 0x10}, // 0x11 i ratio
	{0x478, 0x60}, // 0x61 d ratio
	{0x47C, 0x30}, //max pre gain
	{0x480, 0x40}, //max post gain
	{0x488, 0x80}, //AEC outdoor THD
	{0x48C, 0x20}, //target_Y_low_limit

	{0x448, 0x35}, //[5:4]group_size [3]slope_disable [2]outdoor_enable [0]histogram_enable
	{0x454, 0x50}, //target_Y_high_limit
	{0x440, 0x31}, //num_thd_high
	{0x4F8, 0x28}, //num_thd_low
	{0x4FC, 0xe0}, //luma_thd
	{0x500, 0xe0}, //luma_slope
	{0x504, 0x08}, //color_diff
	{0x83C, 0x05}, //max_col_level
	{0x068, 0x22}, // [6:4] pregain and postgain frame delay num
	{0x080, 0x21}, // [6:4] exp frame delay num [2:0] col gain frame delay num
	{0xaa0, 0x00}, // [1:0] exp bits sel [4] exp swap byte [5] use vsync posedge [6] 16bit data [7] 16bit addr
	{0x0b4, 0x1e}, // [0] send en [1] page en [2] exp1 en [3] exp2 en [4] code en [5] again1 [6] again2
	{0x9b8, 0xb2}, // sensor AEC_delay mode
	{0x42c, 0x21}, // [6:4] AEC every_N    //0x00000021
	{0x9ac, 0x48}, // a_gain addr
	//////// INTPEE /////////////
	{0xA40, 0x6c}, //ac //eeintp mode1
	{0xA44, 0x03}, //eeintp mode2
	{0xA48, 0xc4}, //low criteria for direction
	{0xA5C, 0x64}, //54 //edge effect
	{0xA60, 0x88},
	{0xA74, 0x08},
	{0xA88, 0x11}, //fix direction
	/////////// ASDE ////////////////////////
	{0x484, 0x14}, //15 //luma_value_div_sel
	{0x8F0, 0x06}, //autogray_dec_slope
	{0x8F4, 0x40}, //autogray_dec_th
	{0x920, 0x30}, //asde_dark_offset_th
	{0x924, 0x06}, //asde_dark_offset_slope
	{0x92C, 0x08}, //asde_Y_offset_slope
	{0x930, 0x20}, //asde_Y_offset_limit
	{0xA8C, 0x50}, //ASDE_low_luma_value_LSC_th_H
	{0xA90, 0x30}, //ASDE_low_luma_value_LSC_th_L
	{0xA94, 0x40}, //ASDE_LSC_gain_dec_slope_H
	{0xA98, 0x80}, //ASDE_LSC_gain_dec_slope_L
	{0xAAC, 0x40}, //20 //ASDE_low_luma_value_OT_th
	{0xAB8, 0x0c}, //[3]EE1_inc_or_dec_high,[2]EE2_inc_or_dec_high,[1]EE1_inc_or_dec_low,[0]EE2_inc_or_dec_low
	{0xACC, 0x42}, //ASDE_EE1_effect_slope_low,ASDE_EE2_effect_slope_low
	{0xAD0, 0x24}, //ASDE_EE1_effect_slope_high,ASDE_EE2_effect_slope_high
	{0xAD8, 0x50}, //ASDE_auto_saturation_dec_slope
	{0xADC, 0x01}, //ASDE_sub_saturation_slope
	{0xAE4, 0x28}, //ASDE_auto_saturation_low_limit
	///////////////////gamma1////////////////////
	{0x840 ,0x0d},
	{0x844 ,0x12},
	{0x848 ,0x17},
	{0x84C ,0x1c},
	{0x850 ,0x27},
	{0x854 ,0x34},
	{0x858 ,0x44},
	{0x85C ,0x55},
	{0x860 ,0x6e},
	{0x864 ,0x81},
	{0x868 ,0x91},
	{0x86C ,0x9c},
	{0x870 ,0xaa},
	{0x874 ,0xbb},
	{0x878 ,0xca},
	{0x87C ,0xd5},
	{0x880 ,0xe0},
	{0x884 ,0xe7},
	{0x888 ,0xed},
	{0x88C ,0xf6},
	{0x890 ,0xfb},
	{0x894 ,0xff},
	///////////////////gamma2////////////////////
	{0x898 ,0x0d},
	{0x89C ,0x12},
	{0x8A0 ,0x17},
	{0x8A4 ,0x1c},
	{0x8A8 ,0x27},
	{0x8AC ,0x34},
	{0x8B0 ,0x44},
	{0x8B4 ,0x55},
	{0x8B8 ,0x6e},
	{0x8BC ,0x81},
	{0x8C0 ,0x91},
	{0x8C4 ,0x9c},
	{0x8C8 ,0xaa},
	{0x8CC ,0xbb},
	{0x8D0 ,0xca},
	{0x8D4 ,0xd5},
	{0x8D8 ,0xe0},
	{0x8DC ,0xe7},
	{0x8E0 ,0xed},
	{0x8E4 ,0xf6},
	{0x8E8 ,0xfb},
	{0x8EC ,0xff},
	///////////   YCP       ///////////////////////
	{0xB44 ,0x28}, //24 //saturation_Cb
	{0xB48 ,0x28}, //24 //saturation_Cr
	{0xB74 ,0x14}, //edge sa
	{0xB78 ,0x88}, //YCP_autogray
	{0xBB4 ,0x80}, //asde_autogray_en
	//////// LSC //////////////
	{0x708 ,0x1f},
	{0x70C ,0x13},
	{0x710 ,0x0e},
	{0x720 ,0x16},
	{0x724 ,0x0f},
	{0x728 ,0x0c},
	{0x6F0 ,0x52},
	{0x6F4 ,0x2c},
	{0x6F8 ,0x27},
	{0x6D8 ,0x47},
	{0x6DC ,0x32},
	{0x6E0 ,0x30},
	{0x714 ,0x00},
	{0x718 ,0x00},
	{0x71C ,0x00},
	{0x72C ,0x00},
	{0x730 ,0x00},
	{0x734 ,0x00},
	{0x6FC ,0x0e},
	{0x700 ,0x00},
	{0x704 ,0x00},
	{0x6E4 ,0x08},
	{0x6E8 ,0x00},
	{0x6EC ,0x00},
	{0x6A8 ,0x0a},
	{0x6AC ,0x0c},
	{0x6B0 ,0x0d},
	{0x6B4 ,0x02},
	{0x6B8 ,0x06},
	{0x6BC ,0x05},
	{0x6C0 ,0x00},
	{0x6C4 ,0x05},
	{0x6C8 ,0x02},
	{0x6CC ,0x04},
	{0x6D0 ,0x04},
	{0x6D4 ,0x05},
	{0x740 ,0x00},
	{0x744 ,0x00},
	{0x748 ,0x00},
	{0x758 ,0x02},
	{0x75C ,0x00},
	{0x760 ,0x00},
	{0x764 ,0x00},
	{0x768 ,0x00},
	{0x76C ,0x00},
	{0x74C ,0x00},
	{0x750 ,0x00},
	{0x754 ,0x00},
	{0x690 ,0x04},
	{0x694 ,0x00},
	{0x698 ,0x77},
	{0x69C ,0x77},
	{0x6A0 ,0x77},
	{0x6A4 ,0x77},
	{0x684 ,0x80},
	{0x688 ,0x80},
	{0x770 ,0x35},
	{0x774 ,0x28},
	{0x77C ,0x0d},
	{0x780 ,0x70},
	{0x784 ,0x78},
	{0x788 ,0x70},
	{0x78C ,0x78},
	{0x798 ,0x90},
	{0x79C ,0x70},
	{0x7A0 ,0x90},
	{0x7A4 ,0x70},
	///////////    AWB     ////////////////////////
	{0x53C ,0x00},
	{0x53C ,0x00},
	{0x52C ,0x01},
	{0x53C ,0x00},

	{0x530 ,0x01},
	{0x534 ,0x71},
	{0x538 ,0x02},
	{0x530 ,0x01},
	{0x534 ,0x91},
	{0x538 ,0x02},
	{0x530 ,0x01},
	{0x534 ,0x50},
	{0x538 ,0x02},
	{0x530 ,0x01},
	{0x534 ,0x70},
	{0x538 ,0x02},
	{0x530 ,0x01},
	{0x534 ,0x90},
	{0x538 ,0x02},
	{0x530 ,0x01},
	{0x534 ,0xb0},
	{0x538 ,0x02},
	{0x530 ,0x01},
	{0x534 ,0xd0},
	{0x538 ,0x02},
	{0x530 ,0x01},
	{0x534 ,0x4f},
	{0x538 ,0x02},
	{0x530 ,0x01},
	{0x534 ,0x6f},
	{0x538 ,0x02},
	{0x530 ,0x01},
	{0x534 ,0x8f},
	{0x538 ,0x02},
	{0x530 ,0x01},
	{0x534 ,0xaf},
	{0x538 ,0x02},
	{0x530 ,0x01},
	{0x534 ,0xcf},
	{0x538 ,0x02},
	{0x530 ,0x01},
	{0x534 ,0x6e},
	{0x538 ,0x03},
	{0x530 ,0x01},
	{0x534 ,0x8e},
	{0x538 ,0x03},
	{0x530 ,0x01},
	{0x534 ,0xae},
	{0x538 ,0x03},
	{0x530 ,0x01},
	{0x534 ,0xce},
	{0x538 ,0x03},
	{0x530 ,0x01},
	{0x534 ,0x4d},
	{0x538 ,0x03},
	{0x530 ,0x01},
	{0x534 ,0x6d},
	{0x538 ,0x03},
	{0x530 ,0x01},
	{0x534 ,0x8d},
	{0x538 ,0x03},
	{0x530 ,0x01},
	{0x534 ,0xad},
	{0x538 ,0x03},
	{0x530 ,0x01},
	{0x534 ,0xcd},
	{0x538 ,0x03},
	{0x530 ,0x01},
	{0x534 ,0x4c},
	{0x538 ,0x03},
	{0x530 ,0x01},
	{0x534 ,0x6c},
	{0x538 ,0x03},
	{0x530 ,0x01},
	{0x534 ,0x8c},
	{0x538 ,0x03},
	{0x530 ,0x01},
	{0x534 ,0xac},
	{0x538 ,0x03},
	{0x530 ,0x01},
	{0x534 ,0xcc},
	{0x538 ,0x03},
	{0x530 ,0x01},
	{0x534 ,0xec},
	{0x538 ,0x03},
	{0x530 ,0x01},
	{0x534 ,0x4b},
	{0x538 ,0x03},
	{0x530 ,0x01},
	{0x534 ,0x6b},
	{0x538 ,0x03},
	{0x530 ,0x01},
	{0x534 ,0x8b},
	{0x538 ,0x03},
	{0x530 ,0x01},
	{0x534 ,0xab},
	{0x538 ,0x03},
	{0x530 ,0x01},
	{0x534 ,0x8a},
	{0x538 ,0x04},
	{0x530 ,0x01},
	{0x534 ,0xaa},
	{0x538 ,0x04},
	{0x530 ,0x01},
	{0x534 ,0xca},
	{0x538 ,0x04},
	{0x530 ,0x01},
	{0x534 ,0xa9},
	{0x538 ,0x04},
	{0x530 ,0x01},
	{0x534 ,0xc9},
	{0x538 ,0x04},
	{0x530 ,0x01},
	{0x534 ,0xcb},
	{0x538 ,0x05},
	{0x530 ,0x01},
	{0x534 ,0xeb},
	{0x538 ,0x05},
	{0x530 ,0x02},
	{0x534 ,0x0b},
	{0x538 ,0x05},
	{0x530 ,0x02},
	{0x534 ,0x2b},
	{0x538 ,0x05},
	{0x530 ,0x02},
	{0x534 ,0x4b},
	{0x538 ,0x05},
	{0x530 ,0x01},
	{0x534 ,0xea},
	{0x538 ,0x05},
	{0x530 ,0x02},
	{0x534 ,0x0a},
	{0x538 ,0x05},
	{0x530 ,0x02},
	{0x534 ,0x2a},
	{0x538 ,0x05},
	{0x530 ,0x02},
	{0x534 ,0x6a},
	{0x538 ,0x06},
	{0x530 ,0x02},
	{0x534 ,0x29},
	{0x538 ,0x06},
	{0x530 ,0x02},
	{0x534 ,0x49},
	{0x538 ,0x06},
	{0x530 ,0x02},
	{0x534 ,0x69},
	{0x538 ,0x06},
	{0x530 ,0x02},
	{0x534 ,0x89},
	{0x538 ,0x06},
	{0x530 ,0x02},
	{0x534 ,0xa9},
	{0x538 ,0x06},
	{0x530 ,0x02},
	{0x534 ,0xc9},
	{0x538 ,0x06},
	{0x530 ,0x02},
	{0x534 ,0x48},
	{0x538 ,0x06},
	{0x530 ,0x02},
	{0x534 ,0x68},
	{0x538 ,0x06},
	{0x530 ,0x03},
	{0x534 ,0x09},
	{0x538 ,0x07},
	{0x530 ,0x02},
	{0x534 ,0xa8},
	{0x538 ,0x07},
	{0x530 ,0x02},
	{0x534 ,0xc8},
	{0x538 ,0x07},
	{0x530 ,0x02},
	{0x534 ,0xe8},
	{0x538 ,0x07},
	{0x530 ,0x03},
	{0x534 ,0x08},
	{0x538 ,0x07},
	{0x530 ,0x03},
	{0x534 ,0x28},
	{0x538 ,0x07},
	{0x530 ,0x02},
	{0x534 ,0x87},
	{0x538 ,0x07},
	{0x530 ,0x02},
	{0x534 ,0xa7},
	{0x538 ,0x07},
	{0x530 ,0x02},
	{0x534 ,0xc7},
	{0x538 ,0x07},
	{0x530 ,0x02},
	{0x534 ,0xe7},
	{0x538 ,0x07},
	{0x530 ,0x03},
	{0x534 ,0x07},
	{0x538 ,0x07},
	{0x53C ,0x01},

	{0x540 ,0x80}, //AWB_PRE_mode
	{0x544 ,0xa8},
	{0x548 ,0x57}, //AWB_dominate_num_THD
	{0x54C ,0x38},
	{0x550 ,0xc7}, //AWB_mix_num_THD
	{0x558 ,0x0e}, //AWB_tone mode
	{0x560 ,0x08}, //AWB_PRE_adjust_speed
	{0x56C ,0x00}, //AWB_C_num_sel,AWB_D_num_sel
	{0x570 ,0x74}, //AWB_mix_mode
	{0x574 ,0x8b}, //green_num0[15:8]
	{0x584 ,0xd3},
	{0x588 ,0x90},
	{0x58C ,0xaa}, //04
	{0x594 ,0x04},
	{0x59C ,0xb2},
	{0x5A0 ,0xac},
	{0x5A4 ,0x00},
	{0x5A8 ,0xb2},
	{0x5AC ,0xac},
	{0x5B0 ,0xdc},
	{0x5B4 ,0xb0},
	{0x5B8 ,0x30},
	{0x5BC ,0x40},
	{0x5C0 ,0x05}, //AWB low luma TH
	{0x5C4 ,0x80}, //AWB outdoor TH
	{0x5C8 ,0x80}, //AWB show mode,dark mode enable
	{0x5CC ,0x30}, //move mode
	{0x5D0 ,0x01}, //dark mode
	{0x5D4 ,0x01}, //AWB_every_N,AWB_adjust_margin
	{0x5FC ,0x08},
	{0x5D8 ,0x70}, //R limit
	{0x5DC ,0x48}, //G limit
	{0x5E0 ,0xa0}, //B limit
	///////////  CC   ////////////////////////
	{0xB00, 0x01}, //[5:4] CC mode [0]CCT enable
	{0xB04, 0x4a},
	{0xB08, 0xf3},
	{0xB0C, 0xfc},
	{0xB10, 0xe4},
	{0xB14, 0x48},
	{0xB18, 0xec}, //D65
	{0xB1C, 0x45},
	{0xB20, 0xf8},
	{0xB24, 0x02},
	{0xB28, 0xfe},
	{0xB2C, 0x42},
	{0xB30, 0x00}, //CWF
	{0xB34, 0x45},
	{0xB38, 0xf0},
	{0xB3C, 0x00},
	{0xB8C, 0xf0},
	{0xB90, 0x45},
	{0xB94, 0xe8}, //A
	///////////  OUTPUT   ////////////////////

	//////////////frame rate   50Hz
	{0x494, 0x00},
	{0x498, 0xfa}, //step
	{0x49C, 0x04},
	{0x4A0, 0xe2}, //20fps  level 1
	{0x4A4, 0x06},
	{0x4A8, 0xd6}, //14.2fps
	{0x4AC, 0x07},
	{0x4B0, 0xd0}, //12.5fps level 3
	{0x4B4, 0x0b},
	{0x4B8, 0xb8}, //night mode 8.3fps

//	isp_reg_write(0x04f0 , 0x06); // max_exp_level[6:5]
	{0x4f4, 0x04}, //min_exp [12:0]

	{0xaec, 0x0f},
	{0xaf8, 0x00},
	{0xafc, 0x0c},

// use sys i2c control aec
/*
	{0x0AE8, 0x42},  // sensor slave ID
	{0x09b4, 0xb6},  //isp set sensor gain reg addr
	{0x09ac, 0x03},   //isp set sensor exp time high addr
	{0x09b0, 0x04},   //isp set sensor exp time low addr
*/
	{0x804, 0x80},  // ISP gain
	{0x808, 0x00},
	{0x80c, 0xb4},
	{0x810, 0x00},
	{0x814, 0x11},
	{0x818, 0x68},
	{0x81c, 0xf8},
	{0x820, 0x21},
	{0x824, 0xd0},
	{0x828, 0xf0},
	{0x82c, 0x03},
	{0x83c, 0x06},

	//for debug sunao
	{0x47c, 0x20}, // max pre gain = 1x
	{0x480, 0x40}, // max post gain = 1x

	{0xaf8, 0x00},
	{0xafc, 0x94},
};

static struct crane_isp_tuning_data default_tuning_data = {
	default_tuning_tab,
	ARRAY_SIZE(default_tuning_tab),
};

void loading_tuning_data(SENSOR_CTRL_T *sensor_ctrl)
{
	struct crane_isp_tuning_data tuning_data;

	memset(&tuning_data, 0, sizeof(tuning_data));

	isp_reg_tab_write(&default_tuning_data);

	sensor_ctrl->ops->get_tuning_tab(sensor_ctrl, &tuning_data);
	if (!tuning_data.tab) {
		CAMLOGW("no specific sensor tuning,use default tuning data!");
	} else {
		CAMLOGI("write %s tuning data!", sensor_ctrl->sensor_data->name);
		isp_reg_tab_write(&tuning_data);
	}
}
