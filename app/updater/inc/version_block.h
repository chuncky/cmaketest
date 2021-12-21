#include "common.h"

//++++++++++++++++++++++++++++++++++++++++++++++++++
//AREA_1: UPDATER_VERSION_BLOCK
#define UPDATER_VERSION_BLOCK_SIZE 128
typedef union{
	struct _version_block{
		char version_date[16];
		char oem_lcd_type[32];
		char fota_solution[16];
		char build_info_string[64];
	}version_block;

	UINT8 filer[UPDATER_VERSION_BLOCK_SIZE];
}UPDATER_VERSION_BLOCK;

typedef struct
{
	UPDATER_VERSION_BLOCK  vb;
}UpdaterVerBlockType;
