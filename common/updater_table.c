#include <string.h>
#include "updater_table.h"
#include "ptable.h"

#if (CONFIG_STARTUP_OR_CP == BUILD_IN_STARTUP)
#include "common.h"
#include "loadtable.h"
#else if(CONFIG_STARTUP_OR_CP == BUILD_IN_CP)
#include "gbl_types.h"
#include "utilities.h"
#include "loadTable.h"
#endif



#define UPDATER_TABLE_INVALID_ADDRESS   0x4F4F4F4F


static UpdaterHeaderInfo *pUpdaterHeaderInfo = (UpdaterHeaderInfo *)UPDATER_TABLE_INVALID_ADDRESS;

static UpdaterMagicType_Enum s_updater_function_magic_type = updater_no_magic;





/*********************************** static function declare ********************************************/
static UINT32 updater_function_magic_value( void );
static void  init_updater_function_magic_type(UpdaterMagicType_Enum *p_updater_function_magic_type);
static void updater_get_fota_type(FotaSolution_Enum *p_fota_solution_enmu);
static void set_updater_valid_header_table_address(void);

/*********************************** external function definition  ****     ************************************/
void updater_header_table_init(void)
{
	UINT32 updater_flash_start_address;
	static BOOL init_flag = FALSE;
	
	if(init_flag == FALSE){
		set_updater_valid_header_table_address();		// set value to globale variabe ---pUpdaterHeaderInfo
		init_updater_function_magic_type(&s_updater_function_magic_type);
		CP_LOGD("func:%s,s_updater_function_magic_type:%d\r\n",__func__,s_updater_function_magic_type);
		init_flag = TRUE;
	}
}

UpdaterMagicType_Enum get_updater_function_magic_type(void)
{	
	return s_updater_function_magic_type;
}

BOOL IsUpdaterMagicValid(void)
{
	return pUpdaterHeaderInfo->loadtable_union.loadtable_block.magic == UPDATER_VALID_MAGIC;
	
}

BOOL IsUpdaterSupportQueryFotaInfo(void)
{
	UpdaterMagicType_Enum magic_type;

	magic_type = get_updater_function_magic_type();
	
	return magic_type == updater_magic1_support_query_info || magic_type == updater_magic2_support_query_info_and_append_dsp_adc;
}

BOOL IsUpdaterSupportQueryExtImgInfo(void)
{
	UpdaterMagicType_Enum magic_type;

	magic_type = get_updater_function_magic_type();
	
	return magic_type == updater_magic2_support_query_info_and_append_dsp_adc;
}


BOOL IsUpdaterAppendDspAdc(void)
{
	UINT32 dsp_adc_size;

	dsp_adc_size = updater_get_ext_dsp_adc_size();

	if( dsp_adc_size != UPDTAER_INVALID_VALUE ){
		return TRUE;
	}else{
		return FALSE;
	}
}





char *updater_get_fota_solution(void)
{	
	if( IsUpdaterSupportQueryFotaInfo() ){
		return pUpdaterHeaderInfo->fota_solution_union.fota_solution_block.string;
	}else{
		CP_LOGE("file:%s,func:%s,line:%d\r\n",__FILE__,__func__,__LINE__);
		CP_LOGE("[ERROR] not support get fota solution from updater.bin\r\n");
		while(1);
	}
}

char *updater_get_version_info(void)
{	
	if( IsUpdaterSupportQueryFotaInfo() ){
		return pUpdaterHeaderInfo->version_union.version_info_block.string;
	}else{
		CP_LOGE("file:%s,func:%s,line:%d\r\n",__FILE__,__func__,__LINE__);
		CP_LOGE("[ERROR] not support get version info from updater.bin\r\n");
		while(1);
	}
}

char *updater_get_lcd_info(void)
{
	if( IsUpdaterSupportQueryFotaInfo() ){
		return pUpdaterHeaderInfo->lcd_info_union.UpdaterLcdTypeInfoStruct.oem_lcd_type;
	}else{
		CP_LOGE("file:%s,func:%s,line:%d\r\n",__FILE__,__func__,__LINE__);
		CP_LOGE("[ERROR] not support get lcd info from updater.bin\r\n");
		while(1);
	}
}



unsigned int updater_get_ext_combine_bin_size(void)
{
	if( IsUpdaterSupportQueryExtImgInfo() ){
		return (pUpdaterHeaderInfo->symbol_info_uinon.updater_symbol_info.ext_combine_bin_size.value);
	}else{
		CP_LOGE("file:%s,func:%s,line:%d\r\n",__FILE__,__func__,__LINE__);
		CP_LOGE("[ERROR] not support get ext combine size from updater.bin\r\n");
		while(1);
	}
}
unsigned int updater_get_ext_combine_crc_size(void)
{
	if( IsUpdaterSupportQueryExtImgInfo() ){
		return (pUpdaterHeaderInfo->symbol_info_uinon.updater_symbol_info.ext_combine_crc_size.value);
	}else{
		CP_LOGE("file:%s,func:%s,line:%d\r\n",__FILE__,__func__,__LINE__);
		CP_LOGE("[ERROR] not support get ext combine crc size from updater.bin\r\n");
		while(1);
	}
}
unsigned int updater_get_ext_updater_size(void)
{	
	if( IsUpdaterSupportQueryExtImgInfo() ){
		return (pUpdaterHeaderInfo->symbol_info_uinon.updater_symbol_info.ext_updater_size.value);
	}else{
		CP_LOGE("file:%s,func:%s,line:%d\r\n",__FILE__,__func__,__LINE__);
		CP_LOGE("[ERROR] not support get ext updater size from updater.bin\r\n");
		while(1);
	}
}

unsigned int updater_get_ext_dsp_exec_addr(void)
{	
	if( IsUpdaterSupportQueryExtImgInfo() ){
		return (pUpdaterHeaderInfo->symbol_info_uinon.updater_symbol_info.ext_dsp_adc_exec_addr.value);
	}else{
		CP_LOGE("file:%s,func:%s,line:%d\r\n",__FILE__,__func__,__LINE__);
		CP_LOGE("[ERROR] not support get dsp_adc exec addr from updater.bin\r\n");
		while(1);
	}
}



unsigned int updater_get_ext_dsp_adc_size(void)
{
	if( IsUpdaterSupportQueryExtImgInfo() ){
		return (pUpdaterHeaderInfo->symbol_info_uinon.updater_symbol_info.ext_dsp_adc_size.value);
	}else{
		CP_LOGE("file:%s,func:%s,line:%d\r\n",__FILE__,__func__,__LINE__);
		CP_LOGE("[ERROR] not support get ext dsp_adc size from updater.bin\r\n");
		while(1);
	}
}




unsigned int updater_get_ext_dsp_adc_flash_start_addr(void)
{

#if (CONFIG_STARTUP_OR_CP == BUILD_IN_STARTUP)
	return (get_updater_start_addr() + updater_get_ext_updater_size());
#else if(CONFIG_STARTUP_OR_CP == BUILD_IN_CP)
	return (get_updater_start_address() + updater_get_ext_updater_size());
#endif	
}

// only "get updater version info" and "fota type info"
void get_updater_fota_info(updater_fota_info_struct *p_UpdaterFotaInfoStruct)
{
	char *p_updater_version_info;
	
	switch( p_UpdaterFotaInfoStruct->updater_high_prio_info.updater_function_magic_type )
	{
		case updater_magic1_support_query_info:
		case updater_magic2_support_query_info_and_append_dsp_adc:
			/* [1] get updater version info */
			memset(p_UpdaterFotaInfoStruct->updater_high_prio_info.updater_version_info,'\0',sizeof(p_UpdaterFotaInfoStruct->updater_high_prio_info.updater_version_info));
			p_updater_version_info = updater_get_version_info();
			strncpy(p_UpdaterFotaInfoStruct->updater_high_prio_info.updater_version_info,p_updater_version_info,strlen(p_updater_version_info));
			
			/* [2] get updater fota type info */
			updater_get_fota_type(&(p_UpdaterFotaInfoStruct->updater_high_prio_info.updater_fota_type_info));
			
			p_UpdaterFotaInfoStruct->updater_info_res = get_info_is_ok;
			break;
		case updater_no_magic:
		default:
			p_UpdaterFotaInfoStruct->updater_info_res = get_info_error;
			CP_LOGE("[UPDATER] magic is not right,magic:%d\r\n",p_UpdaterFotaInfoStruct->updater_high_prio_info.updater_function_magic_type);
	}
}












void dump_updater_fota_info(updater_fota_info_struct *p_UpdaterFotaInfo)
{
	CP_LOGD("[UPDATER] updater info res:%d\r\n",p_UpdaterFotaInfo->updater_info_res);
	CP_LOGD("[UPDATER] updater function magic type: %d\r\n",p_UpdaterFotaInfo->updater_high_prio_info.updater_function_magic_type);	
	CP_LOGD("[UPDATER] updater version info: %s\r\n",p_UpdaterFotaInfo->updater_high_prio_info.updater_version_info);
	CP_LOGD("[UPDATER] updater fota type: %d\r\n",p_UpdaterFotaInfo->updater_high_prio_info.updater_fota_type_info);
	
	if( p_UpdaterFotaInfo->updater_high_prio_info.updater_function_magic_type != updater_no_magic ){
		CP_LOGD("[UPDATER] updater fota string:\"%s\"\r\n",updater_get_fota_solution());
	}
}

void dump_updater_image_info(void)
{
	if( IsUpdaterSupportQueryExtImgInfo() ){
		CP_LOGD("[UPDATER] updater function magic type:%d\r\n",get_updater_function_magic_type());
		CP_LOGD("[UPDATER] ext combine bin size : 0x%x\r\n",updater_get_ext_combine_bin_size());	
		CP_LOGD("[UPDATER] updater size: 0x%x\r\n",updater_get_ext_updater_size());
		CP_LOGD("[UPDATER] dsp_adc size: %d\r\n",updater_get_ext_dsp_adc_size());
	}else{
		CP_LOGD("[UPDATER] updater not append dsp_adc\r\n");
	}
}


#if (CONFIG_ADD_UPDATER_TABLE_TEST_CASE == 1)
void test_updater_table(void)
{
	test_get_fota_info_from_updater_header_table();

	dump_updater_image_info();

	CP_LOGI("dsp_adc_valid:%d\r\n",IsDspAdcValid());
}



void test_get_fota_info_from_updater_header_table(void)
{
	UpdaterMagicType_Enum magic_type;
	updater_fota_info_struct UpdaterInfo;
	
	/* [1] header table init */
	updater_header_table_init();
	
	/* [2] get updater fuction magic type from updater header table */
	UpdaterInfo.updater_high_prio_info.updater_function_magic_type = get_updater_function_magic_type();
	CP_LOGI("[UPDATER] function magic type:%d\r\n",
		UpdaterInfo.updater_high_prio_info.updater_function_magic_type);

	/* [3] get updater info */
	if(IsUpdaterSupportQueryFotaInfo()){
		get_updater_fota_info(&UpdaterInfo);
	}else if( UpdaterInfo.updater_high_prio_info.updater_function_magic_type == updater_no_magic ){
#if 1	
		/* case1: set updater info manually */
		/* The old updater.bin not support header table, need set value manually */
		// set fota solution type manually
		UpdaterInfo.updater_high_prio_info.updater_fota_type_info = fota_adups;
		// set updater version info manually
		memset(UpdaterInfo.updater_high_prio_info.updater_version_info,'\0',sizeof(UpdaterInfo.updater_high_prio_info.updater_version_info));
		strncpy(UpdaterInfo.updater_high_prio_info.updater_version_info,"20210202_ver1",strlen("20210202_ver1"));
		UpdaterInfo.updater_info_res = get_info_is_ok;
#endif


#if 0
		/* case2: return get_no_info res and download update package by the old way */
		UpdaterInfo.updater_info_res = get_no_info;
#endif
	}

	/* [4] dump updater info if necessary */
	if( UpdaterInfo.updater_info_res == get_info_is_ok ){
		dump_updater_fota_info(&UpdaterInfo);
	}	
}

BOOL IsDspAdcValid(void)
{
	UINT32 dsp_adc_start_addr;
	UINT32 dsp_adc_size;
	UINT32 crc_cal = 0xFFFF, crc_internal;
	if( IsUpdaterAppendDspAdc() ){
		dsp_adc_start_addr = updater_get_ext_dsp_adc_flash_start_addr();
		dsp_adc_size = updater_get_ext_dsp_adc_size();
		crc_internal = *(UINT32 *)(dsp_adc_start_addr + dsp_adc_size - 4);
		crc_cal = common_crc_check_16((UINT8 * )dsp_adc_start_addr, dsp_adc_size - 4, crc_cal);
		if( crc_cal == crc_internal ){
			return TRUE;
		}else{
			CP_LOGE("crc_internal:0x%x,crc_cal:0x%x",crc_internal,crc_cal);
			return FALSE;
		}
	}else{
		CP_LOGE("[UPDATER] updater not append dsp_adc\r\n");
		return FALSE;
	}
}



#endif



/*********************************** static function definition         ****************************************/

static void set_updater_valid_header_table_address(void)
{
	UINT32 updater_flash_start_address;
	_ptentry *l_updater_Entry = ptable_find_entry("updater");

	/* [1] use updater.bin header as header table */
	updater_flash_start_address = l_updater_Entry->vstart;
	pUpdaterHeaderInfo = (UpdaterHeaderInfo *)(updater_flash_start_address + UPDATER_HEADER_OFFSET_START);

	/* [2] updater.bin magic is valid ? */
	if( !IsUpdaterMagicValid()){
		// if updater.bin magic is not valid and updater_bak is exist, use updater_bak.bin as header table */
		if(IsUpdaterbakExist()){
			_ptentry *l_updater_bak_Entry = ptable_find_entry("updater_bak");
			updater_flash_start_address = l_updater_bak_Entry->vstart;
			pUpdaterHeaderInfo = (UpdaterHeaderInfo *)(updater_flash_start_address + UPDATER_HEADER_OFFSET_START);
		}else{
			CP_LOGE("[Error] line:%d, func:%s\r\n",__LINE__,__func__);
			CP_LOGE("updater magic is not right and updater bak is not exit.\r\n");
			//while(1);   // can not while(1) as need to cover updater.bin of 4k size. ( can print uart log to warning user )
		}
	}
}

static void updater_get_fota_type(FotaSolution_Enum *p_fota_solution_enmu)
{
	char *p_updater_fota_type_string;

	/* [1] get fota solution string */
	p_updater_fota_type_string = updater_get_fota_solution();

	/* [2] parse "fota solution string" and give the fota solution type */
	if( 0 == strncmp(p_updater_fota_type_string,FOTA_ADUPS_NAME,strlen(FOTA_ADUPS_NAME)) ){
	   *p_fota_solution_enmu = fota_adups;
	}else if(0 == strncmp(p_updater_fota_type_string,FOTA_REDSTONE_NAME,strlen(FOTA_REDSTONE_NAME))){
	   *p_fota_solution_enmu = fota_redstone;
	}else if(0 == strncmp(p_updater_fota_type_string,FOTA_ASRSD_NAME,strlen(FOTA_ASRSD_NAME))){
	   *p_fota_solution_enmu = asrsd;
	}else if(0 == strncmp(p_updater_fota_type_string,FOTA_ASRSPI_NAME,strlen(FOTA_ASRSPI_NAME))){
	   *p_fota_solution_enmu = fota_asrspi;
	}else if(0 == strncmp(p_updater_fota_type_string,FOTA_ASRDM_NAME,strlen(FOTA_ASRDM_NAME))){
	   *p_fota_solution_enmu = fota_asrdm;
	}else{
	   *p_fota_solution_enmu = fota_no_solution;
	}
}

static UINT32 updater_function_magic_value( void )
{
	return pUpdaterHeaderInfo->symbol_info_uinon.updater_symbol_info.updater_function_magic.value;
}

static void  init_updater_function_magic_type(UpdaterMagicType_Enum *p_updater_function_magic_type)
{
	UINT32 function_magic_value;

	function_magic_value = updater_function_magic_value();
	CP_LOGD("func:%s,function_magic_value:0x%x\r\n",__func__,function_magic_value);
	if( function_magic_value == UPDATER_FUNCTION_MAGIC_1 ){
		*p_updater_function_magic_type = updater_magic1_support_query_info;
	}else if( function_magic_value == UPDATER_FUNCTION_MAGIC_2 ){
		*p_updater_function_magic_type = updater_magic2_support_query_info_and_append_dsp_adc;
	}else{
		*p_updater_function_magic_type = updater_no_magic;
	}
}




