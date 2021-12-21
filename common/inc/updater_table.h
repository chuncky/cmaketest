#ifndef _UPDATER_TABLE_H_
#define _UPDATER_TABLE_H_

/**********************CONFIG PARAMETER *******************************
 * CONFIG_ADD_UPDATER_TABLE_TEST_CASE:  1         --- add updater test case
 *                                              0  --- remove updater test case
 *
 * CONFIG_STARTUP_OR_CP:                 0  --- STARTUP
 *                                       1  --- CP
 **********************************************************************/
#define CONFIG_ADD_UPDATER_TABLE_TEST_CASE 			(0)

#define BUILD_IN_STARTUP             (0)
#define BUILD_IN_CP                  (1)

#define CONFIG_STARTUP_OR_CP         			BUILD_IN_STARTUP


#if (CONFIG_STARTUP_OR_CP == BUILD_IN_STARTUP)
#include "common.h"
#else if(CONFIG_STARTUP_OR_CP == BUILD_IN_CP)
#include "gbl_types.h"
#endif

#include "updater_table_scatter.h"


// - UPDATER VALID MAGIC
#define UPDATER_VALID_MAGIC          ( 0x1A1A1A1A )

// - UPDATER FUNCTION MAGIC
// - aa_bb_cc_dd
// UPDATER_FUNCTION_MAGIC_1 --- place important info in ther updater header
// UPDATER_FUNCTION_MAGIC_2 --- append dsp_adc.bin after updater.bin
#define UPDATER_FUNCTION_MAGIC_1              ( 0xBABC5A5A )
#define UPDATER_FUNCTION_MAGIC_2              ( 0x11111122 )



/************************************** FOTA_INFO ********************************************/
#define FOTA_ADUPS_NAME       "FOTA_"##"ADUPS"
#define FOTA_REDSTONE_NAME    "FOTA_"##"REDSTONE"
#define FOTA_RED_ALI_NAME     "FOTA_"##"RED_ALI"
#define FOTA_ASRSD_NAME       "FOTA_"##"ASRSD"
#define FOTA_ASRSPI_NAME      "FOTA_"##"ASRSPI"
#define FOTA_ASRDM_NAME       "FOTA_"##"ASRDM"
#define FOTA_DEFAULT_NAME     "FOTA_"##"NO_FOTA"

#define UPDTAER_INVALID_VALUE           0x4c4c554E //NULL

/************************************** TABLE SIZE INFO ********************************************/
// Note: The following MACRO can not MODIFY

/*------------------ 1. Header info size union ------------------*/
//#define UPDATER_HEADER_TABLE_MAX_SIZE 			( 256 )  // max size define in updater_table_scatter.h

//    1.1 updater loadtable info
#define UPDATER_LOADTABLE_SIZE                  ( 16  )

//    1.2 ext*.bin and component bin info
#define LOADTABLE_UPDATER_SYMBOL_SIZE           ( 256 )

//	  1.3 updater fota solution info
#define FOTA_SOLUTION_MAX_SIZE                  ( 48 )

//	  1.4 updater version info
#define UPDATER_VERSION_INFO_MAX_SIZE           ( 48 )

//    1.5 lcd info
#define UPDATER_LCD_INFO_MAX_SIZE               ( 48 )
#define UPDATER_LCD_TYPE_INFO_MAX_SIZE          ( UPDATER_LCD_INFO_MAX_SIZE )

//    1.6 updater header reserved size
#define UPDATER_HEADER_TABLE_RESERVED_SIZE      ( UPDATER_HEADER_TABLE_MAX_SIZE   - UPDATER_LOADTABLE_SIZE \
												  - LOADTABLE_UPDATER_SYMBOL_SIZE - FOTA_SOLUTION_MAX_SIZE \
												  - UPDATER_VERSION_INFO_MAX_SIZE - UPDATER_LCD_INFO_MAX_SIZE )
											  
/*---------------------------------------------------------------------------------------------*/

#define PROMPT_MAX_SIZE (16)



/************************************** TABLE STRUCT ********************************************/
// 1. Header Info Table
//	 1.1 updater loadtable
//   1.2 ext*.bin and component bin info
// 	 1.3 updater ota solution
//   1.4 updater version info
//   1.5 lcd info
//   1.6 reserved

/* 1.1 updater loadtable */
typedef union{
	struct _loadtable_block{
		UINT32 b2init;
		UINT32 init;
		UINT32 load_addr;
		UINT32 magic;
	}loadtable_block;

	UINT8 filer[UPDATER_LOADTABLE_SIZE];
}UpdaterLoadTableUnion;

/* 1.2 ext*.bin and component bin info */
typedef struct{
	char	name[12];
	unsigned int	value;
}updater_symbol_item;

typedef struct {
		// updater function magic
		updater_symbol_item updater_function_magic;

		//combine ext.bin
		updater_symbol_item ext_combine_bin_size; // ext_partition size wihtout crc append
		updater_symbol_item ext_combine_crc_size; // ext_partition size + crc append

		//updater.bin
		updater_symbol_item ext_updater_size;

		//dsp_adc.bin
		updater_symbol_item ext_dsp_adc_exec_addr;
		updater_symbol_item ext_dsp_adc_size;

		//updater header table size
		updater_symbol_item ext_header_table_size;
}loadtable_updater_symbol_info;

typedef union{
    loadtable_updater_symbol_info updater_symbol_info;

	unsigned char filer[LOADTABLE_UPDATER_SYMBOL_SIZE];       /* max size*/
}LOADTABLE_AREA_UPDATER_SYMBOL;

/* 1.3 updater ota solution */
#define FOTA_SOLUTION_STRING_MAX_SIZE   ( FOTA_SOLUTION_MAX_SIZE - PROMPT_MAX_SIZE )
#define FOTA_SOLUTION_PROMPT                    ( "fota_solution:" )
typedef union{
	struct _fota_solution_block{
		char prompt[PROMPT_MAX_SIZE];
		char string[FOTA_SOLUTION_STRING_MAX_SIZE];
	}fota_solution_block;

	UINT8 filer[FOTA_SOLUTION_MAX_SIZE];
}UpdaterFotaSolutionUnion;

/* 1.4 updater version info */
#define UPDATER_VERSION_INFO_STRING_MAX_SIZE   ( UPDATER_VERSION_INFO_MAX_SIZE - PROMPT_MAX_SIZE )
#define UPDATER_VERSION_INFO_PROMPT             ( "updater_version:" )
typedef union{
	struct _version_info_block{
		char prompt[PROMPT_MAX_SIZE];
		char string[UPDATER_VERSION_INFO_STRING_MAX_SIZE];
	}version_info_block;

	UINT8 filer[UPDATER_VERSION_INFO_MAX_SIZE];
}UpdaterVersionInfo;

/* 1.5 lcd type info */
typedef union{
	struct _UpdaterLcdTypeInfo{
		char oem_lcd_type[UPDATER_LCD_INFO_MAX_SIZE];
	}UpdaterLcdTypeInfoStruct;

	UINT8 filer[UPDATER_LCD_INFO_MAX_SIZE];
}UpdaterLcdTypeInfoUnion;




typedef struct
{
	UpdaterLoadTableUnion     loadtable_union;
	LOADTABLE_AREA_UPDATER_SYMBOL symbol_info_uinon;
	UpdaterFotaSolutionUnion  fota_solution_union;
	UpdaterVersionInfo        version_union;
	UpdaterLcdTypeInfoUnion   lcd_info_union;
	UINT8                     reserved[UPDATER_HEADER_TABLE_RESERVED_SIZE];
}UpdaterHeaderInfo;


typedef enum{
	fota_no_solution = 0,
	fota_adups,
	fota_redstone,
	asrsd,
	fota_asrspi,
	fota_asrdm,
}FotaSolution_Enum;

typedef enum{
	updater_no_magic = 0,
	updater_magic1_support_query_info,
	updater_magic2_support_query_info_and_append_dsp_adc,
}UpdaterMagicType_Enum;

typedef enum{
	get_info_is_ok = 0,    // choose download server according fota solution in updater.bin
	get_info_error,        // occur error when get info from updater.bin 
	get_no_info,           // get no info in updater.bin
}GetUpdaterInfoRes_enum;

typedef struct{
	char                   updater_version_info[UPDATER_VERSION_INFO_STRING_MAX_SIZE];
	FotaSolution_Enum      updater_fota_type_info;
	UpdaterMagicType_Enum  updater_function_magic_type;
}UpdaterHighPrioInfo_struct;


typedef struct{
	UpdaterHighPrioInfo_struct updater_high_prio_info;
	GetUpdaterInfoRes_enum updater_info_res;	
}updater_fota_info_struct;







/**************************** EXTERNAL   FUNCTION STATEMENT **************************/
// 1 User use
void updater_header_table_init(void);
BOOL IsUpdaterMagicValid(void);
UpdaterMagicType_Enum get_updater_function_magic_type(void);
BOOL IsUpdaterSupportQueryFotaInfo(void);
BOOL IsUpdaterSupportQueryExtImgInfo(void);
BOOL IsUpdaterAppendDspAdc(void);
// 1.1 - support query fota info
void get_updater_fota_info(updater_fota_info_struct *p_UpdaterFotaInfoStruct);
void dump_updater_fota_info(updater_fota_info_struct *p_UpdaterFotaInfo);

// 1.2 - support updater.bin append dsp_adc.bin
unsigned int updater_get_ext_combine_bin_size(void);
unsigned int updater_get_ext_combine_crc_size(void);
unsigned int updater_get_ext_updater_size(void);
unsigned int updater_get_ext_dsp_exec_addr(void);
unsigned int updater_get_ext_dsp_adc_size(void);
unsigned int updater_get_ext_dsp_adc_flash_start_addr(void);



// 2. internal debug
char *updater_get_fota_solution(void);
char *updater_get_version_info(void);
char *updater_get_lcd_info(void);






#if (CONFIG_ADD_UPDATER_TABLE_TEST_CASE == 1)
void test_get_fota_info_from_updater_header_table(void);

void test_get_fota_info_from_updater_header_table(void);

BOOL IsDspAdcValid(void);


#endif





#endif 

