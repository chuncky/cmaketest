#ifndef __LOGO_TABLE_H__
#define __LOGO_TABLE_H__
#include "updater_table.h"

#if (CONFIG_STARTUP_OR_CP == BUILD_IN_STARTUP)
#include "common.h"
#else if(CONFIG_STARTUP_OR_CP == BUILD_IN_CP)
#include "gbl_types.h"
#endif


/**********************CONFIG PARAMETER ****************************************
 * CONFIG_ADD_LOGO_HEADER_TABLE_TEST_CASE:            1  --- add logo header table test case
 *                                                    0  --- remove logo header table test case
 *******************************************************************************/
#define CONFIG_ADD_LOGO_HEADER_TABLE_TEST_CASE (0)

#define LOGO_VALID_MAGIC (0xBABE5F5F)


// - LOGO FUNCTION MAGIC
// - magic function definition:
// -   aa_bb_cc_dd
// LOGO_FUNCTION_MAGIC_2 --- append dsp_adc.bin after updater.bin
#define LOGO_FUNCTION_MAGIC_2              ( 0x11111122 )


#define ENABLE_LOGO_DISPLAY_FLAG   "[11]"
#define DISABLE_LOGO_DISPLAY_FLAG  "[00]"



#define LOGO_INVALID_VALUE           (0x4c4c554E) //NULL

#define LOGO_HEADER_TOTAL_SIZE (LOGO_IMAGE_HEADER_SIZE + LOGO_ENABLE_LOGO_AND_VIRATOR_FEEDBACK \
	                            + LOGO_VERSION_BLOCK_SIZE + LOADTABLELOGO_SYMBOL_SIZE)


//++++++++++++++++++++++++++++++++++++++++++++++++++
#define LOGO_HEADER_OFFSET_START           ( 0 )


//AREA_1: LOGO_LOAD_BLOCK
#define LOGO_IMAGE_HEADER_SIZE 16
typedef union{
	struct _ImageHeader{
		UINT32 ImageMagic;
		UINT32 ImageLoadAddr;
		UINT32 ImageStartAddr;
		UINT32 Logo_HeaderTableSize;
	}ImageHeaderBlock;

	UINT8 filer[LOGO_IMAGE_HEADER_SIZE];
}LOGO_IMAGE_HEADER_BLOCK;

//AREA_2: LOGO_FEEDBACK_BLOCK
#define LOGO_ENABLE_LOGO_AND_VIRATOR_FEEDBACK 16
typedef union{
	struct _feedback_block{
		char Description[11];
		char LogoDisplayLogoFlag[5];
	}feedback_block;
	UINT8 filer[LOGO_ENABLE_LOGO_AND_VIRATOR_FEEDBACK];
}LOGO_FEEDBACK_BLOCK;

//AREA_3: LOGO_VERSION_BLOCK
#define LOGO_VERSION_BLOCK_SIZE 128
typedef union{
	struct _logo_version_block{
		char version_date[16];
		char oem_lcd_type[48];
		char COMPRESSED_ALGORITHM_SUPPORT[32];
		char build_info_string[32];
	}logo_version_block;

	UINT8 filer[LOGO_VERSION_BLOCK_SIZE];
}LOGO_VERSION_BLOCK;


//AREA_4: ext*.bin info
#define LOADTABLELOGO_SYMBOL_SIZE           ( 256 )

typedef struct{
	char	name[12];
	unsigned int	value;
}logo_symbol_item;

typedef struct {
	// updater function magic
	logo_symbol_item logo_function_magic;

	//combine ext.bin
	logo_symbol_item ext_combine_bin_size; // ext_partition size wihtout crc append

	//updater.bin
	logo_symbol_item ext_logo_size;

	//dsp_adc.bin
	logo_symbol_item ext_dsp_adc_exec_addr;
	logo_symbol_item ext_dsp_adc_size;
}loadtable_logo_symbol_info;

typedef union{
    loadtable_logo_symbol_info logo_symbol_info;

	unsigned char filer[LOADTABLELOGO_SYMBOL_SIZE];       /* max size*/
}LOADTABLE_AREA_LOGO_SYMBOL;


typedef struct
{
	LOGO_IMAGE_HEADER_BLOCK HeaderBlock;
	LOGO_FEEDBACK_BLOCK     FeedbackBlock;
	LOGO_VERSION_BLOCK      vb;
	LOADTABLE_AREA_LOGO_SYMBOL symbol_info_union;
}LogoHeaderTableStruct;



typedef enum{
	logo_no_function_magic = 0,
	logo_reserved1_no_use,
	logo_magic2_support_query_info_and_append_dsp_adc,
}LogoFunctionMagicType_Enum;



/****************************** external function declaration *******************************/
void logo_header_table_init(void);
LogoFunctionMagicType_Enum get_logo_function_magic_type(void);
BOOL IsLogoSupportQueryExtImgInfo(void);
BOOL IsLogoAppendDspAdc(void);
UINT32 logo_get_ext_combine_bin_size(void);

/**
 * @Description:    get logo flash addr, 
 * @Return Value:    logo flash start address
 */
UINT32 get_logo_flash_start_addr(void);
UINT32 get_logo_flash_size(void);


UINT32 logo_get_ext_logo_size(void);
UINT32 logo_get_ext_dsp_adc_size(void);
UINT32 logo_get_ext_dsp_adc_flash_start_addr(void);
UINT32 logo_get_ext_dsp_adc_exec_addr(void);

#if (CONFIG_ADD_LOGO_HEADER_TABLE_TEST_CASE == 1)
void dump_logo_image_info(void);
void test_logo_header_table(void);
#endif


#endif

