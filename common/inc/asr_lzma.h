#ifndef __ASR_LZMA_H_
#define __ASR_LZMA_H_

typedef enum{
	LZOP = 0,
	LZMA,
	NO_COMPRESSED
}CompressedType_e;


BOOL IsLzmaOptimize(void);
void LzmaOptimizeSwitch(const BOOL SwitchFlag);
CompressedType_e GetCompressedType(UINT32 magic);


#endif


