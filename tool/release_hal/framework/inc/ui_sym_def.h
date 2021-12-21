/*------------------------------------------------------------
(C) Copyright [2018-2020] ASR Ltd.
All Rights Reserved
------------------------------------------------------------*/

#ifndef __UI_SYM_DEF_H__
#define __UI_SYM_DEF_H__

#ifdef __cplusplus
extern "C" {
#endif

extern UINT32  Load$$DDR_ITCM$$Base		;/* Load Address of DDR_ITCM region			*/
extern UINT32 Image$$DDR_ITCM$$Base		;/* Exec Address of DDR_ITCM region			*/
extern UINT32 Image$$DDR_ITCM$$Limit	;
extern UINT32 Image$$DDR_ITCM$$Length	;/* Length of DDR_ITCM region				*/
extern UINT32  Load$$DDR_DTCM$$Base		;/* Load Address of DDR_DTCM region			*/
extern UINT32 Image$$DDR_DTCM$$Base		;/* Exec Address of DDR_DTCM region			*/
extern UINT32 Image$$DDR_DTCM$$Length	;/* Length of DDR_DTCM region  				*/
extern UINT32 Image$$DDR_DTCM$$ZI$$Base	;
extern UINT32 Image$$DDR_DTCM$$ZI$$Limit;
extern UINT32 Image$$DDR_DTCM_ENDMARKER$$Base;
extern UINT32  Load$$DDR_RW_DATA$$Base	;  /* Load Address of DDR_RW_DATA region	*/
extern UINT32 Image$$DDR_RW_DATA$$Base	;  /* Exec Address of DDR_RW_DATA region	*/
extern UINT32 Image$$DDR_RW_DATA$$Length;  /* Length of DDR_RW_DATA region			*/
extern UINT32 Image$$DDR_RW_DATA$$ZI$$Base;
extern UINT32 Image$$DDR_RW_DATA$$ZI$$Limit;
extern UINT32 Image$$DDR_HEAP_ENDMARK$$Base;
extern UINT32 Image$$DDR_HEAP_GUARD$$Base;

extern UINT32  Load$$MMI_CODE$$Base;
extern UINT32 Image$$MMI_CODE$$Base;
extern UINT32 Image$$MMI_CODE$$Length;

extern UINT32  Load$$MMI_DATA$$Base;
extern UINT32 Image$$MMI_DATA$$Base;
extern UINT32 Image$$MMI_DATA$$Length;
extern UINT32 Image$$MMI_DATA$$ZI$$Base;
extern UINT32 Image$$MMI_DATA$$ZI$$Limit;

extern UINT32 Image$$PS_NONCACHE_DATA$$ZI$$Base;
extern UINT32 Image$$PS_NONCACHE_DATA$$ZI$$Limit;
extern UINT32  Load$$PS_NONCACHE_DATA$$Base	;  /* Load Address of PS_NONCACHE_DATA region	*/
extern UINT32 Image$$PS_NONCACHE_DATA$$Base	;  /* Exec Address of PS_NONCACHE_DATA region	*/
extern UINT32 Image$$PS_NONCACHE_DATA$$Length; /* Length of PS_NONCACHE_DATA region			*/

extern UINT32  Load$$INIT_DATA$$Base;
extern UINT32 Image$$INIT_DATA$$Base;
extern UINT32 Image$$INIT_DATA$$Length;

extern UINT32  Load$$D2_VECT$$Base;
extern UINT32 Image$$D2_VECT$$Base;
extern UINT32 Image$$D2_VECT$$Length;

extern UINT32  Image$$DDR_RO_INIT_CODE$$Base;
extern UINT32  Image$$DDR_RO_INIT_CODE$$Limit;

extern UINT32  Image$$DDR_RO_EXEC$$Base;
extern UINT32  Image$$DDR_RO_EXEC$$Limit;
extern UINT32  Image$$DDR_MMI_RO_EXEC$$Base;
extern UINT32  Image$$DDR_MMI_RO_EXEC$$Limit;
extern UINT32  Image$$DDR_MMI_RO_EXEC$$Length;
extern UINT32  Image$$PS_CODE_IN_PSRAM$$Base;
extern UINT32  Image$$PS_CODE_IN_PSRAM$$Limit;

extern UINT32  Image$$BOOT_SP_AREA$$Base;

extern unsigned char mmiPool[];
extern UINT32 mmiPoolSize;

#define IS_IN_IMAGE_HEAP(ptr)		(\
	((((UINT32)(ptr)) >= (UINT32)&mmiPool) && \
	(((UINT32)(ptr)) < (UINT32)((UINT32)&mmiPool + mmiPoolSize))) \
	|| ((((UINT32)(ptr)) >= (UINT32)&Image$$DDR_HEAP_GUARD$$Base) && \
	(((UINT32)(ptr)) < (UINT32)((UINT32)&Image$$DDR_HEAP_ENDMARK$$Base))) )

#define IS_IN_IMAGE_UICODE(ptr)		\
	((((UINT32)(ptr)) >= (UINT32)&Image$$DDR_MMI_RO_EXEC$$Base) && \
	 (((UINT32)(ptr)) <= (UINT32)((UINT32)&Image$$DDR_MMI_RO_EXEC$$Base + (UINT32)&Image$$DDR_MMI_RO_EXEC$$Length)))

#ifdef __cplusplus
}
#endif

#endif /* __UI_SYM_DEF_H__ */
