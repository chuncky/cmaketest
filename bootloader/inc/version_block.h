#include "common.h"

//++++++++++++++++++++++++++++++++++++++++++++++++++
//AREA_1: BOOT33_VERSION_BLOCK
#define BOOT33_VERSION_BLOCK_SIZE 128
typedef union{
	struct _version_block{
		char version_date[16];
		char secboot_support[16];
		char logo_compress_support[32];
		char build_info_string[64];
	}version_block;

	UINT8 filer[BOOT33_VERSION_BLOCK_SIZE];
}BOOT33_VERSION_BLOCK;

typedef struct
{
	BOOT33_VERSION_BLOCK  vb;
}Boot33VerBlockType;
