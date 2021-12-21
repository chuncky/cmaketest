#include "common.h"
#include "asr_lzma.h"

/*********************************** Macro Definition ********************************************/
// MACRO for config cache
// NOTE : cache start addr must br multiple of cache size and cache size must 2,4,8..
#define PSRAM_CONFIG_CACHE_START_ADDR ( 0x7E000000 )
#define PSRAM_CONFIG_CACHE_SIZE 	  ( 0x01000000 )

// Compress Algorithm Macro
#define LZOP_COMPRESSED_MAGIC (0x4f5a4c89)
#define LZMA_COMPRESSED_MAGIC (0x8000005D)
#define LZMA_COMPRESSED_MAGIC1 (0x0800005D)


	
/*********************************** static function declare ********************************************/
/* global variable for record "enable cache and improve core freq" for LZMA */
static volatile BOOL s_LzmaOptimizeFlag = FALSE;


/*********************************** static function declare ********************************************/
static void config_LzmaOptimizeFlag_value(const BOOL value);



/*********************************** external function declare ********************************************/
void disable_cache(unsigned int addr,unsigned int size);
void enable_cache(unsigned int addr,unsigned int size);


/*********************************** external function definition ********************************************/
BOOL IsLzmaOptimize(void)
{
	return s_LzmaOptimizeFlag;
}

void LzmaOptimizeSwitch(const BOOL SwitchFlag)
{
	/*
	 *@Note:  enbale and disable cache must use in pairs
     *   and can only be used in pairs ONCE!!!
	 */
	if(SwitchFlag == TRUE){
		enable_cache(PSRAM_CONFIG_CACHE_START_ADDR,PSRAM_CONFIG_CACHE_SIZE);
		CpCoreFreqChangeTo624();
		PsramPhyFreqChangeTo416();
		config_LzmaOptimizeFlag_value(TRUE);
		CP_LOGD("enable 0 ~ 16M psram as cache\r\n");
	}else if(SwitchFlag == FALSE){
		disable_cache(PSRAM_CONFIG_CACHE_START_ADDR,PSRAM_CONFIG_CACHE_SIZE);
		CpCoreFreqChangeTo416();
		CP_LOGD("disable cache\r\n");
		config_LzmaOptimizeFlag_value(FALSE);
	}
	
	return;
}





CompressedType_e GetCompressedType(UINT32 magic)
{
	CompressedType_e type;
	
	switch(magic)
	{
		case LZOP_COMPRESSED_MAGIC:
			CP_LOGD("lzop cpmpressed image\r\n");
			type = LZOP;
			break;		
		case LZMA_COMPRESSED_MAGIC:
		case LZMA_COMPRESSED_MAGIC1:
			CP_LOGD("lzma cpmpressed image\r\n");
			type = LZMA;
			break;
		default:
			CP_LOGW("not a compressed image\r\n");
			type = NO_COMPRESSED;
			break;		
	}

	return type;
}





/*********************************** static function definition ********************************************/
static void config_LzmaOptimizeFlag_value(const BOOL value)
{
	s_LzmaOptimizeFlag = value;
}

