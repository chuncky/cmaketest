#ifndef UTILITIES_H
#define UTILITIES_H

/*------------------------ DSP_ADC ----------------------------------*/
// MACRO of register for release dsp_adc.bin
#define PMUM_CPRR 		*(volatile UINT32*)0xD4050020
#define DSP_ADC_SYS_CTRL 	*(volatile UINT32*)0xD4070030
#define DSP_ADC_VECTOR 	*(volatile UINT32*)0xD4070034

#define PTCM_ADDR							0xF0200000
#define DSP_ADC_COPY_LEN						0x4000
#define DSP_ADC_EXTERNAL_BOOT_ADDR			0x10
#if 1
#define DSP_ADC_EXECUTE_ADDR					0xD1F00000
#else
#define DSP_ADC_EXECUTE_ADDR					0x7E100000
#endif
// dsp_adc.bin share memory base address
#define DSP_ADC_SHARE_MEMORY_ADDR 				( 0xD1000000 )

// dsp_adc.bin ready flag
#define DSP_ADC_VBATTERY_MEASURE_READY_FLAG 		( 0xA5A5 )


/*------------------------ MRD ----------------------------------*/
typedef enum
{
	ADC_VBAT = 0x0,
	ADC_RTP = 0x1,
	ADC_MAX = 0x2
}PM803_ADC_TYPE;

typedef enum
{
	TEMP_NORMAL=0,
	TEMP_LOW,
	TEMP_HIGH,
	TEMP_TYPE_MAX
}ADC_TEMP_t;

#define ITEM_ADC_SIZE 	(MAX_ADC_Data_LEN*2*MAX_ADC_GROUP_CNT*(TEMP_TYPE_MAX-1)+8+8)	//416 bytes
#define MAX_DEVID_SIZE		512
#define MAX_SECURE_SIZE	2048
/* string length */
#define MAX_ADC_Data_LEN	10
/* Num of data */
#define MAX_ADC_GROUP_CNT	10


#define ASR_PROPERTY_LCD_PANEL_ID_NAME 		( "lcd_panel_id" )

#define ASR_PROPERTY_LCD_INIT_FLAG_NAME	    ( "lcd_init_flag" )

#define ASR_PROPERTY_PM803_BATTERY_VALUE_NAME ("pm803_bat_v_f_logo")


/*------------------------ Debug Macro ----------------------------------*/
/*
*****************************************************************************************
*			some debug macros
*
*Note:  the following MACROs default state: "CLOSED".
*1. DSP_ADC_DEBUG              			If define this MACRO, and memcmp during load dsp_adc and print dsp_adc debug info from dsp_adc share memory 
*2. MRD_DEBUG_CODE						If define this MACRO, add test code and log for MRD
*3. PM803_DEBUG_CODE					If define this MACRO, add test code for test get battery voltage.( Platform SH)
*										Must open this MACRO if test vbat api with logo_no_lcd.bin.
*****************************************************************************************
*/
//#define DSP_ADC_DEBUG
//#define MRD_DEBUG_CODE
//#define PM803_DEBUG_CODE

//#define AUX_ADC_ATE


typedef enum{
	dsp_adc_type_min_error = 0, 
	dsp_adc_type1,				// dsp_adc_b, this image is placed in system.img
	dsp_adc_type2,				// dsp_adc, this image is placed near NVM.
	dsp_adc_type3_in_updater,	// dsp_adc, this image is inside updater ext
	dsp_adc_type4_in_logo,		// dsp_adc, this image is inside logo ext
	dsp_adc_type_max_error,
}Dsp_adc_type_t;


/* Load Res */
typedef enum{
	Dsp_adc_LoadOk = 0,
	Dsp_adc_para_error,
	Dsp_adc_ImageNotExist,
	Dsp_adc_LoadFail,	
}Dsp_adc_LoadImage_Res_t;


typedef struct{
	UINT32 AdcValue;
	UINT32 VoltageValue;
}MrdAdc_t;

typedef enum
{
	PM803_GetBatteryOk = 0,
	PM803_CalbrationFlagNotSet,
	PM803_BatteryFlagNotReady,
	PM803_FitFail,
	PM803_Load_Dsp_adc_Fail,
	PM803_Dsp_adc_Not_Exist
}PM803_GetBattery_Status_Res;

typedef struct{
	UINT32 BatteryValue;
	PM803_GetBattery_Status_Res pm803_status_res;
}PM803_Battery_Res_t;


void uint2str( UINT32 x, char *Str );
void hex2str( UINT32 x, char *Str );
UINT32 m_atoi_dec(const char * string_ptr);
UINT32 m_hex_string2i_dec(const char * string_ptr);

/**
 * @brief:	load dsp_adc from flash into psram
 * @param[in]	dsp_type			choose "dsp_adc_b"  or "dsp_adc"		
 * @retval
 * - Dsp_adc_LoadOk
 * - Dsp_adc_para_error
 * - Dsp_adc_ImageNotExist
 * - Dsp_adc_LoadFail
 */
Dsp_adc_LoadImage_Res_t load_dsp_adc( Dsp_adc_type_t dsp_type );
void boot_dsp_adc_external_from_squ_new(void);
void hold_dsp_adc( void );



/***********************************************************************
 *
 * Name:	MRDAdcValueGet
 *
 * Description: Get the value of ADC info which is written in prod mode. 
 *
 * Parameters:
 *  UINT32	index					[IN]    const character string for file name specifier.
 *  UINT32	AdcValue,VoltageValue	[OUT] 	const character string for type specification;
 *
 * Returns:
 *	0: 	ok.
 * -1:	invalid input or value get
 *	1:	the AdcValue/VoltageValue get is null
 *
 ***********************************************************************/
INT32 MRDAdcValueGet_boot(UINT32 index, UINT32 *AdcValue, UINT32 *VoltageValue, PM803_ADC_TYPE type, ADC_TEMP_t tempType);
/***********************************************************************
 *
 * Name:	MRDAdcFlagGet
 *
 * Description: Get the flag of ADC info which is written in prod mode. 
  * Returns:
 *	0: 	the ADC calibration flag is not set.
 *	1:	the ADC calibration flag is set.
***********************************************************************/
INT32 MRDAdcFlagGet_boot(PM803_ADC_TYPE type, ADC_TEMP_t tempType);

UINT32 MRDAdcCntGet_boot(PM803_ADC_TYPE type, ADC_TEMP_t tempType);

void MrdBubbleSort( MrdAdc_t *p_MrdAdcData,const UINT32 DataNum );


void transmit_data(const char *name, const UINT32 data );

/**
 * @brief: get transmit info from asr property share buffer
 * @param
 * 	 name: transmit info name
 * @retval:
 *	0xFFFFFFFF  -  no transmit info name content in asr property share buffer
 *  other       -  transmitted content 
 */
UINT32 get_transmit_data( const char *name );


UINT8 check_if_DCS_mode(void);

void PM803_CHARGER_INIT(void);
BOOL PM803_CHARGER_IS_DETECTED(void);
/**
 * @brief:get battery voltage by pm803 and ADC module from RF
 * @para  pm803_battery_res: store voltage value and status info
 * @retval 
 */
void pm803_get_battery_voltage( PM803_Battery_Res_t *p_pm803_battery_res );

/**
 * @Description: 
 *     electricity interception
 * @Parameter
 *     battery_value: threshold batterh value
 * @Return Value
 *       <NULL>
 * Note:
 *        case1 - battery > threshold value 
 *              ---> jump to api and execute following code
 *		  case2 - battery < threshold value && charger not connect
 *              ---> PMIC DOWN
 *        case3 - battery < threshold value && charger connect
 *              ---> wait until battery value is more than threshold value
 *        OTHER CASE - TODO. ( currently, print error log and execute next code.)
 */
void Startup_SoftwareNotProceedUntilReachOneValue(const UINT32 battery_value);


#ifdef MRD_DEBUG_CODE
void test_MRD_api( void );
#endif


void VibratorEnable(void);
void VibratorDisable(void);


#endif	/* UTILITIES_H */

