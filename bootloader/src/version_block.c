#include "version_block.h"

//for basic boot33 build version
#define VB_VERSION_DATE        "20211013"

#define BOOT33_INFO_STRING     VB_VERSION_DATE##"_ver1"

#ifdef BOOT33_SECBOOT_SUPPORT
#define VB_SECBOOT_SUPPORT     "SECBOOT"
#else
#define VB_SECBOOT_SUPPORT     "NON-SECBOOT"
#endif

#ifdef SUPPORT_COMPRESSED_LOGO
#define VB_COMPRESSED_LOGO_SUPPORT     "SUPPORT_COMPRESSED_LOGO"
#else
#define VB_COMPRESSED_LOGO_SUPPORT     "NOT_SUPPORT_COMPRESSED_LOGO"
#endif


#pragma arm section rodata="IMGVERBLOCK"
const Boot33VerBlockType boot33_vb =
{
	//+++++++++++++++++++++++++++++
	//AREA_1: VERSION_INFO
	{
		VB_VERSION_DATE,
		VB_SECBOOT_SUPPORT,
		VB_COMPRESSED_LOGO_SUPPORT,
		BOOT33_INFO_STRING
	}
};
#pragma arm section code
