#include <string.h>
#include "logo_table.h"
#include "updater_table.h"
#if (CONFIG_STARTUP_OR_CP == BUILD_IN_STARTUP)
#include "common.h"
#include "loadtable.h"
#include "asr_nor_flash.h"
#include "ptable.h"
#else if(CONFIG_STARTUP_OR_CP == BUILD_IN_CP)
#include "gbl_types.h"
#include "loadTable.h"
#include "utilities.h"
#endif


/*********************************** Macro definition ********************************************/
#if (CONFIG_STARTUP_OR_CP == BUILD_IN_CP)
#define IS_INTERNAL_FLASH(addr)      ( (addr) >= 0x90000000 ? 1 : 0 )
#define GET_FLASH_OFFSET(IS_IN_INTERNAL_FLASH_FLAG,ADDR) ((IS_IN_INTERNAL_FLASH_FLAG) == 1 ? (ADDR) - 0x90000000 : (ADDR) - 0x80000000)
#endif


/*********************************** new variable type definition ********************************************/



/*********************************** static variable definition ********************************************/
static UINT32 s_logo_flash_start_addr = LOGO_INVALID_VALUE;
static UINT32 s_logo_flash_size       = LOGO_INVALID_VALUE;

static LogoHeaderTableStruct s_LogoHeaderInfo;
static LogoHeaderTableStruct *pLogoHeaderInfo = LOGO_INVALID_VALUE;
static LogoFunctionMagicType_Enum s_logo_function_magic_type = logo_no_function_magic;


/*********************************** static function declare ********************************************/
static BOOL init_logo_flash_info(UINT32 *pFlashStartAddr, UINT32 *pFlashSize);
static UINT32 get_logo_function_magic_value( void );
static void  init_logo_function_magic_type(LogoFunctionMagicType_Enum *p_logo_function_magic_type);
static BOOL IsLogoValidMagicRight(const UINT32 LogoFlashAddr);

/*********************************** external function definition  ****     ************************************/
void logo_header_table_init(void)
{
	UINT32 logo_flash_start_address;
	static BOOL init_flag = FALSE;
	
	if(init_flag == FALSE){
		if(TRUE != init_logo_flash_info(&s_logo_flash_start_addr,&s_logo_flash_size) ){
			CP_LOGE("[ERROR] func:%s, line:%d,get logo flash start addr error!\r\n",__func__,__LINE__);
			while(1);
		}
		logo_flash_start_address = get_logo_flash_start_addr();
		CP_LOGI("logo flash start addr:0x%x\r\n",logo_flash_start_address);
		CP_LOGI("LogoHeaderInfo size:0x%x\r\n",sizeof(LogoHeaderTableStruct));
#if (CONFIG_STARTUP_OR_CP == BUILD_IN_STARTUP)	
		asr_norflash_read(logo_flash_start_address, &s_LogoHeaderInfo, sizeof(LogoHeaderTableStruct));
#else if(CONFIG_STARTUP_OR_CP == BUILD_IN_CP)	
		UINT8 is_in_internal_flash = IS_INTERNAL_FLASH(logo_flash_start_address);
		UINT32 offset = GET_FLASH_OFFSET(is_in_internal_flash,logo_flash_start_address);
		spi_nor_do_read(offset,(unsigned int)&s_LogoHeaderInfo,sizeof(LogoHeaderTableStruct),is_in_internal_flash);
#endif		
		pLogoHeaderInfo = (LogoHeaderTableStruct *)&s_LogoHeaderInfo;
		init_logo_function_magic_type(&s_logo_function_magic_type);
		CP_LOGD("func:%s,s_logo_function_magic_type:%d\r\n",__func__,s_logo_function_magic_type);
		init_flag = TRUE;
	}
}






LogoFunctionMagicType_Enum get_logo_function_magic_type(void)
{	
	return s_logo_function_magic_type;
}

BOOL IsLogoSupportQueryExtImgInfo(void)
{
	LogoFunctionMagicType_Enum logo_function_magic_type;

	logo_function_magic_type = get_logo_function_magic_type();
	
	return logo_function_magic_type == logo_magic2_support_query_info_and_append_dsp_adc;
}


BOOL IsLogoAppendDspAdc(void)
{
	UINT32 dsp_adc_size;

	dsp_adc_size = logo_get_ext_dsp_adc_size();

	if( dsp_adc_size != LOGO_INVALID_VALUE ){
		return TRUE;
	}else{
		return FALSE;
	}
}


UINT32 get_logo_flash_start_addr(void)
{
	if( s_logo_flash_start_addr != LOGO_INVALID_VALUE ){
		return s_logo_flash_start_addr;
	}else{
		CP_LOGE("[ERROR] logo flash start addr error:0x%x\r\n",LOGO_INVALID_VALUE);
		while(1);
	}
}

UINT32 get_logo_flash_size(void)
{
	if( s_logo_flash_size != LOGO_INVALID_VALUE ){
		return s_logo_flash_size;
	}else{
		CP_LOGE("[ERROR] logo flash size error:0x%x\r\n",LOGO_INVALID_VALUE);
		while(1);
	}
}




UINT32 logo_get_ext_combine_bin_size(void)
{
	if( IsLogoSupportQueryExtImgInfo() ){
		return (pLogoHeaderInfo->symbol_info_union.logo_symbol_info.ext_combine_bin_size.value);
	}else{
		CP_LOGE("file:%s,func:%s,line:%d\r\n",__FILE__,__func__,__LINE__);
		CP_LOGE("[ERROR] not support get ext combine size from logo.bin\r\n");
		while(1);
	}
}


UINT32 logo_get_ext_logo_size(void)
{	
	if( IsLogoSupportQueryExtImgInfo() ){
		return (pLogoHeaderInfo->symbol_info_union.logo_symbol_info.ext_logo_size.value);
	}else{
		CP_LOGE("file:%s,func:%s,line:%d\r\n",__FILE__,__func__,__LINE__);
		CP_LOGE("[ERROR] not support get ext logo size from logo.bin\r\n");
		while(1);
	}
}

UINT32 logo_get_ext_dsp_adc_size(void)
{
	if( IsLogoSupportQueryExtImgInfo() == TRUE ){
		return (pLogoHeaderInfo->symbol_info_union.logo_symbol_info.ext_dsp_adc_size.value);
	}else{
		CP_LOGE("file:%s,func:%s,line:%d\r\n",__FILE__,__func__,__LINE__);
		CP_LOGE("[ERROR] not support get ext dsp_adc size from logo.bin\r\n");
		while(1);
	}
}

UINT32 logo_get_ext_dsp_adc_flash_start_addr(void)
{
	return (get_logo_flash_start_addr() + logo_get_ext_logo_size());
}


UINT32 logo_get_ext_dsp_adc_exec_addr(void)
{	
	if( IsLogoAppendDspAdc() == TRUE ){
		return (pLogoHeaderInfo->symbol_info_union.logo_symbol_info.ext_dsp_adc_exec_addr.value);
	}else{
		CP_LOGE("file:%s,func:%s,line:%d\r\n",__FILE__,__func__,__LINE__);
		CP_LOGE("[ERROR] not support get dsp_adc exec addr from logo.bin\r\n");
		while(1);
	}
}

#if (CONFIG_ADD_LOGO_HEADER_TABLE_TEST_CASE == 1)
void dump_logo_image_info(void)
{
	if( IsLogoSupportQueryExtImgInfo() ){
		CP_LOGD("[LOGO] logo function magic type:%d\r\n",get_logo_function_magic_type());
		CP_LOGD("[LOGO] ext combine bin size : 0x%x\r\n",logo_get_ext_combine_bin_size());	
		CP_LOGD("[LOGO] logo size: 0x%x\r\n",logo_get_ext_logo_size());
		CP_LOGD("[LOGO] dsp_adc size: %d\r\n",logo_get_ext_dsp_adc_size());
	}else{
		CP_LOGD("[LOGO] logo not append dsp_adc\r\n");
	}
}

void test_logo_header_table(void)
{
	LogoFunctionMagicType_Enum magic_type;
	
	/* [1] header table init */
	logo_header_table_init();
	
	/* [2] get logo load addr and exec addr */

	/* [3] get logo function magic type */
	CP_LOGD("logo function magic type:%d\r\n",get_logo_function_magic_type());
	if( IsLogoSupportQueryExtImgInfo() ){
		CP_LOGD("logo ext combine size:0x%x\r\n",logo_get_ext_combine_bin_size());
		CP_LOGD("logo ext logo size:0x%x\r\b",logo_get_ext_logo_size());
		CP_LOGD("logo ext dsp_adc size:0x%x\r\b",logo_get_ext_dsp_adc_size());
		CP_LOGD("logo ext dsp_adc start addr:0x%x\r\b",logo_get_ext_dsp_adc_flash_start_addr());
		CP_LOGD("logo ext dsp_adc start addr:0x%x\r\b",logo_get_ext_dsp_adc_exec_addr());
	}
}
#endif



/*********************************** static function definition         ****************************************/
/**
 * @Description:
 *     init_logo_flash_info 
 *          note: Firstly, give more priority to the existence of logo partition.
 *                Then, consider to the existence of the logo.bin at the end of cp.bin.
 * @Parameter: pFlashStartAddr <OUT>: save logo.bin flash start address.
 * @Parameter: pFlashSize       <OUT>: logo.bin in flash size
 * @Return Value:  
 *               TRUE:   If logo.bin exist in flash and valid, return TRUE.
 *               FALSE:  If logo.bin not exist in flash or invalid, return FALSE.
 */
static BOOL init_logo_flash_info(UINT32 *pFlashStartAddr, UINT32 *pFlashSize)
{
	_ptentry *logo_entry = ptable_find_entry("logo");
	UINT32 StartAddr;
	UINT32 FlashSize;
	//[1] check logo.bin exist in ptable or not ?
	if(logo_entry != NULL){
		StartAddr = logo_entry->vstart;
		if(IsLogoValidMagicRight(StartAddr)){
			CP_LOGI("load logo in ptable\r\n");
			*pFlashStartAddr = StartAddr;
			*pFlashSize 	 = logo_entry->size;
			return TRUE;
		}
	}
	//[2] check logo.bin exist in cp.bin or not ?
	FlashSize = get_logo_length_in_cp();
	if(INVALID_ADDRESS != FlashSize){
		StartAddr = get_cp_load_addr() + get_cp_binary_size();
		if(IsLogoValidMagicRight(StartAddr)){
			CP_LOGI("load logo in cp.bin\r\n");
			*pFlashStartAddr = StartAddr;
			*pFlashSize      = FlashSize;
			return TRUE;
		}
	}

	CP_LOGD("func:%s,logo.bin not exist in flash or invalid!\r\n",__func__);
	return FALSE;
}


static UINT32 get_logo_function_magic_value( void )
{
	return pLogoHeaderInfo->symbol_info_union.logo_symbol_info.logo_function_magic.value;
}

static void  init_logo_function_magic_type(LogoFunctionMagicType_Enum *p_logo_function_magic_type)
{
	UINT32 function_magic_value;

	function_magic_value = get_logo_function_magic_value();
	CP_LOGD("func:%s,function_magic_value:0x%x\r\n",__func__,function_magic_value);
	if( function_magic_value == LOGO_FUNCTION_MAGIC_2 ){
		*p_logo_function_magic_type = logo_magic2_support_query_info_and_append_dsp_adc;
	}else{
		*p_logo_function_magic_type = logo_no_function_magic;
	}
}


static BOOL IsLogoValidMagicRight(const UINT32 LogoFlashAddr)
{
	UINT32 LogoValidMagic;
#if (CONFIG_STARTUP_OR_CP == BUILD_IN_STARTUP)	
	asr_norflash_read(LogoFlashAddr,(UINT8 *)&LogoValidMagic,sizeof(LogoValidMagic));
#else if(CONFIG_STARTUP_OR_CP == BUILD_IN_CP)
	CP_LOGD("LogoFlashAddr:0x%x\r\n",LogoFlashAddr);
	UINT8 is_in_internal_flash = IS_INTERNAL_FLASH(LogoFlashAddr);
	UINT32 offset = GET_FLASH_OFFSET(is_in_internal_flash,LogoFlashAddr);
	spi_nor_do_read(offset,(unsigned int)&LogoValidMagic,sizeof(LogoValidMagic),is_in_internal_flash);
#endif
	return LogoValidMagic == LOGO_VALID_MAGIC;
}


