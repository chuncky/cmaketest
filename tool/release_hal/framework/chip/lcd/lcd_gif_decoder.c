#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "plat_config_defs.h"

#include "lcd_gif_decoder.h"
#ifdef LCD_Gif_Decoder
#ifndef CONFIG_RELEASE_WITH_NO_ASR_UI
#include "../../gui/mgapollo/inner-res/asr-240x320/bootup/include/bootup_bootup_img_animate.h"
#endif
extern const unsigned char _bootup_bootup_img_animate_bitmap[];
#endif

static int InternalRead(GifFileType *gif, GifByteType *buf, int len)
{
	int length = len;
	if(gif->pos + length > gif->size)
		length = gif->size - gif->pos;

	memcpy(buf, gif->data + gif->pos, length);
	gif->pos += length;
	return length;
}

static int DGifGetWord(GifFileType *GifFile, GifWord *Word)
{
    unsigned char c[2];

    if (InternalRead(GifFile, c, 2) != 2) {
        GifFile->Error = D_GIF_ERR_READ_FAILED;
        return GIF_ERROR;
    }

    *Word = (GifWord)UNSIGNED_LITTLE_ENDIAN(c[0], c[1]);
    return GIF_OK;
}

static int GifBitSize(int n)
{
    register int i;

    for (i = 1; i <= 8; i++)
        if ((1 << i) >= n)
            break;
    return (i);
}

static ColorMapObject *
GifMakeMapObject(int ColorCount, const GifColorType *ColorMap)
{
    ColorMapObject *Object;

    if (ColorCount != (1 << GifBitSize(ColorCount))) {
        return ((ColorMapObject *) NULL);
    }
    
    Object = (ColorMapObject *)malloc(sizeof(ColorMapObject));
    if (Object == (ColorMapObject *) NULL) {
        return ((ColorMapObject *) NULL);
    }

    Object->Colors = (GifColorType *)calloc(ColorCount, sizeof(GifColorType));
    if (Object->Colors == (GifColorType *) NULL) {
	free(Object);
        return ((ColorMapObject *) NULL);
    }

    Object->ColorCount = ColorCount;
    Object->BitsPerPixel = GifBitSize(ColorCount);
    Object->SortFlag = 0;

    if (ColorMap != NULL) {
        memcpy((char *)Object->Colors,
               (char *)ColorMap, ColorCount * sizeof(GifColorType));
    }

    return (Object);
}

static int DGifGetScreenDesc(GifFileType *GifFile)
{
    int BitsPerPixel;
    int SortFlag;
    GifByteType Buf[3];

    /* Put the screen descriptor into the file: */
    if (DGifGetWord(GifFile, &GifFile->SWidth) == GIF_ERROR ||
        DGifGetWord(GifFile, &GifFile->SHeight) == GIF_ERROR)
        return GIF_ERROR;

    InternalRead(GifFile, Buf, 3);
    GifFile->SColorResolution = (((Buf[0] & 0x70) + 1) >> 4) + 1;
    SortFlag = (Buf[0] & 0x08) != 0;
    BitsPerPixel = (Buf[0] & 0x07) + 1;
    GifFile->SBackGroundColor = Buf[1];
    GifFile->AspectByte = Buf[2]; 
    if (Buf[0] & 0x80) {    /* Do we have global color map? */
		int i;
		GifFile->SColorMap = GifMakeMapObject(1 << BitsPerPixel, NULL);

		GifFile->SColorMap->SortFlag = SortFlag;
		for (i = 0; i < GifFile->SColorMap->ColorCount; i++) {
			InternalRead(GifFile, Buf, 3);
			GifFile->SColorMap->Colors[i].Red = Buf[0];
			GifFile->SColorMap->Colors[i].Green = Buf[1];
			GifFile->SColorMap->Colors[i].Blue = Buf[2];
	    }
    }
    
    return GIF_OK;
}

static GifFileType *DGifOpenFileName(int *Error)
{
    char Buf[GIF_STAMP_LEN + 1];
    GifFileType *GifFile;
    GifFilePrivateType *Private;

    GifFile = (GifFileType *)malloc(sizeof(GifFileType));

    memset(GifFile, '\0', sizeof(GifFileType));

    GifFile->SavedImages = NULL;
    GifFile->SColorMap = NULL;
	GifFile->pos = 0;
#ifdef LCD_Gif_Decoder
	GifFile->data = _bootup_bootup_img_animate_bitmap;
	GifFile->size = sizeof(_bootup_bootup_img_animate_bitmap);
#endif

    Private = (GifFilePrivateType *)calloc(1, sizeof(GifFilePrivateType));

    memset(Private, '\0', sizeof(GifFilePrivateType));

    GifFile->Private = (void *)Private;
    Private->FileState = FILE_STATE_READ;

    InternalRead(GifFile, (unsigned char *)Buf, GIF_STAMP_LEN);

    Buf[GIF_STAMP_LEN] = 0;

    DGifGetScreenDesc(GifFile);

    GifFile->Error = 0;
    Private->gif89 = (Buf[GIF_VERSION_POS] == '9');

    return GifFile;
}

static int DGifGetRecordType(GifFileType *GifFile, GifRecordType* Type)
{
    GifByteType Buf;

    InternalRead(GifFile, &Buf, 1);

    switch (Buf) {
      case DESCRIPTOR_INTRODUCER:
          *Type = IMAGE_DESC_RECORD_TYPE;
          break;
      case EXTENSION_INTRODUCER:
          *Type = EXTENSION_RECORD_TYPE;
          break;
      case TERMINATOR_INTRODUCER:
          *Type = TERMINATE_RECORD_TYPE;
          break;
    }

    return GIF_OK;
}

static int DGifSetupDecompress(GifFileType *GifFile)
{
    int i, BitsPerPixel;
    GifByteType CodeSize;
    GifPrefixType *Prefix;
    GifFilePrivateType *Private = (GifFilePrivateType *)GifFile->Private;

    InternalRead(GifFile, &CodeSize, 1);
    BitsPerPixel = CodeSize;

    Private->Buf[0] = 0;    /* Input Buffer empty. */
    Private->BitsPerPixel = BitsPerPixel;
    Private->ClearCode = (1 << BitsPerPixel);
    Private->EOFCode = Private->ClearCode + 1;
    Private->RunningCode = Private->EOFCode + 1;
    Private->RunningBits = BitsPerPixel + 1;    /* Number of bits per code. */
    Private->MaxCode1 = 1 << Private->RunningBits;    /* Max. code + 1. */
    Private->StackPtr = 0;    /* No pixels on the pixel stack. */
    Private->LastCode = NO_SUCH_CODE;
    Private->CrntShiftState = 0;    /* No information in CrntShiftDWord. */
    Private->CrntShiftDWord = 0;

    Prefix = Private->Prefix;
    for (i = 0; i <= LZ_MAX_CODE; i++)
        Prefix[i] = NO_SUCH_CODE;

    return GIF_OK;
}

static int DGifGetImageHeader(GifFileType *GifFile)
{
    //unsigned int BitsPerPixel;
    GifByteType Buf[3];
    GifFilePrivateType *Private = (GifFilePrivateType *)GifFile->Private;

    if (DGifGetWord(GifFile, &GifFile->Image.Left) == GIF_ERROR ||
        DGifGetWord(GifFile, &GifFile->Image.Top) == GIF_ERROR ||
        DGifGetWord(GifFile, &GifFile->Image.Width) == GIF_ERROR ||
        DGifGetWord(GifFile, &GifFile->Image.Height) == GIF_ERROR)
        return GIF_ERROR;
    InternalRead(GifFile, Buf, 1);
    //BitsPerPixel = (Buf[0] & 0x07) + 1;
    GifFile->Image.Interlace = (Buf[0] & 0x40) ? 1 : 0;

    Private->PixelCount = (long)GifFile->Image.Width *
       (long)GifFile->Image.Height;

    /* Reset decompress algorithm parameters. */
    return DGifSetupDecompress(GifFile);
}

#define MUL_NO_OVERFLOW	((size_t)1 << (sizeof(size_t) * 4))
#define SIZE_MAX 4294967295U

static void *reallocarray(void *optr, size_t nmemb, size_t size)
{
	if ((nmemb >= MUL_NO_OVERFLOW || size >= MUL_NO_OVERFLOW) &&
	    nmemb > 0 && SIZE_MAX / nmemb < size) {
		errno = ENOMEM;
		return NULL;
	}

	if (size == 0 || nmemb == 0)
	    return NULL;
	return realloc(optr, size * nmemb);
}

static int DGifGetImageDesc(GifFileType *GifFile)
{
    SavedImage *sp;

    DGifGetImageHeader(GifFile);

    if (GifFile->SavedImages) {
        SavedImage* new_saved_images =
            (SavedImage *)reallocarray(GifFile->SavedImages,
                            (GifFile->ImageCount + 1), sizeof(SavedImage));
        if (new_saved_images == NULL) {
            GifFile->Error = D_GIF_ERR_NOT_ENOUGH_MEM;
            return GIF_ERROR;
        }
        GifFile->SavedImages = new_saved_images;
    } else {
        if ((GifFile->SavedImages =
             (SavedImage *) malloc(sizeof(SavedImage))) == NULL) {
            GifFile->Error = D_GIF_ERR_NOT_ENOUGH_MEM;
            return GIF_ERROR;
        }
    }

    sp = &GifFile->SavedImages[GifFile->ImageCount];
    memcpy(&sp->ImageDesc, &GifFile->Image, sizeof(GifImageDesc));

    sp->RasterBits = (unsigned char *)NULL;
    sp->ExtensionBlockCount = 0;
    sp->ExtensionBlocks = (ExtensionBlock *) NULL;
    GifFile->ImageCount++;

    return GIF_OK;
}

static int DGifBufferedInput(GifFileType *GifFile, GifByteType *Buf, GifByteType *NextByte)
{
    if (Buf[0] == 0) {
        if (InternalRead(GifFile, Buf, 1) != 1) {
            GifFile->Error = D_GIF_ERR_READ_FAILED;
            return GIF_ERROR;
        }

        if (Buf[0] == 0) {
            GifFile->Error = D_GIF_ERR_IMAGE_DEFECT;
            return GIF_ERROR;
        }
        if (InternalRead(GifFile, &Buf[1], Buf[0]) != Buf[0]) {
            GifFile->Error = D_GIF_ERR_READ_FAILED;
            return GIF_ERROR;
        }
        *NextByte = Buf[1];
        Buf[1] = 2;    /* We use now the second place as last char read! */
        Buf[0]--;
    } else {
        *NextByte = Buf[Buf[1]++];
        Buf[0]--;
    }

    return GIF_OK;
}

static int DGifDecompressInput(GifFileType *GifFile, int *Code)
{
    static const unsigned short CodeMasks[] = {
	0x0000, 0x0001, 0x0003, 0x0007,
	0x000f, 0x001f, 0x003f, 0x007f,
	0x00ff, 0x01ff, 0x03ff, 0x07ff,
	0x0fff
    };

    GifFilePrivateType *Private = (GifFilePrivateType *)GifFile->Private;
    GifByteType NextByte;

    /* The image can't contain more than LZ_BITS per code. */
    if (Private->RunningBits > LZ_BITS) {
        GifFile->Error = D_GIF_ERR_IMAGE_DEFECT;
        return GIF_ERROR;
    }
    
    while (Private->CrntShiftState < Private->RunningBits) {
        /* Needs to get more bytes from input stream for next code: */
        if (DGifBufferedInput(GifFile, Private->Buf, &NextByte) == GIF_ERROR) {
            return GIF_ERROR;
        }
        Private->CrntShiftDWord |=
			((unsigned long)NextByte) << Private->CrntShiftState;
        Private->CrntShiftState += 8;
    }
    *Code = Private->CrntShiftDWord & CodeMasks[Private->RunningBits];

    Private->CrntShiftDWord >>= Private->RunningBits;
    Private->CrntShiftState -= Private->RunningBits;

    if (Private->RunningCode < LZ_MAX_CODE + 2 &&
	++Private->RunningCode > Private->MaxCode1 &&
	Private->RunningBits < LZ_BITS) {
        Private->MaxCode1 <<= 1;
        Private->RunningBits++;
    }
    return GIF_OK;
}

static int DGifGetPrefixChar(GifPrefixType *Prefix, int Code, int ClearCode)
{
    int i = 0;

    while (Code > ClearCode && i++ <= LZ_MAX_CODE) {
        if (Code > LZ_MAX_CODE) {
            return NO_SUCH_CODE;
        }
        Code = Prefix[Code];
    }
    return Code;
}

static int DGifDecompressLine(GifFileType *GifFile, GifPixelType *Line, int LineLen)
{
    int i = 0;
    int j, CrntCode, EOFCode, ClearCode, CrntPrefix, LastCode, StackPtr;
    GifByteType *Stack, *Suffix;
    GifPrefixType *Prefix;
    GifFilePrivateType *Private = (GifFilePrivateType *) GifFile->Private;

    StackPtr = Private->StackPtr;
    Prefix = Private->Prefix;
    Suffix = Private->Suffix;
    Stack = Private->Stack;
    EOFCode = Private->EOFCode;
    ClearCode = Private->ClearCode;
    LastCode = Private->LastCode;

    if (StackPtr > LZ_MAX_CODE) {
        return GIF_ERROR;
    }

    if (StackPtr != 0) {
        /* Let pop the stack off before continueing to read the GIF file: */
        while (StackPtr != 0 && i < LineLen)
            Line[i++] = Stack[--StackPtr];
    }

    while (i < LineLen) {    /* Decode LineLen items. */
        if (DGifDecompressInput(GifFile, &CrntCode) == GIF_ERROR)
            return GIF_ERROR;

        if (CrntCode == EOFCode) {
	    GifFile->Error = D_GIF_ERR_EOF_TOO_SOON;
	    return GIF_ERROR;
        } else if (CrntCode == ClearCode) {
            /* We need to start over again: */
            for (j = 0; j <= LZ_MAX_CODE; j++)
                Prefix[j] = NO_SUCH_CODE;
            Private->RunningCode = Private->EOFCode + 1;
            Private->RunningBits = Private->BitsPerPixel + 1;
            Private->MaxCode1 = 1 << Private->RunningBits;
            LastCode = Private->LastCode = NO_SUCH_CODE;
        } else {
            if (CrntCode < ClearCode) {
                /* This is simple - its pixel scalar, so add it to output: */
                Line[i++] = CrntCode;
            } else {
                if (Prefix[CrntCode] == NO_SUCH_CODE) {
                    CrntPrefix = LastCode;
                    if (CrntCode == Private->RunningCode - 2) {
                        Suffix[Private->RunningCode - 2] =
                           Stack[StackPtr++] = DGifGetPrefixChar(Prefix,
                                                                 LastCode,
                                                                 ClearCode);
                    } else {
                        Suffix[Private->RunningCode - 2] =
                           Stack[StackPtr++] = DGifGetPrefixChar(Prefix,
                                                                 CrntCode,
                                                                 ClearCode);
                    }
                } else
                    CrntPrefix = CrntCode;

                while (StackPtr < LZ_MAX_CODE &&
                       CrntPrefix > ClearCode && CrntPrefix <= LZ_MAX_CODE) {
                    Stack[StackPtr++] = Suffix[CrntPrefix];
                    CrntPrefix = Prefix[CrntPrefix];
                }
                if (StackPtr >= LZ_MAX_CODE || CrntPrefix > LZ_MAX_CODE) {
                    GifFile->Error = D_GIF_ERR_IMAGE_DEFECT;
                    return GIF_ERROR;
                }
                /* Push the last character on stack: */
                Stack[StackPtr++] = CrntPrefix;

                /* Now lets pop all the stack into output: */
                while (StackPtr != 0 && i < LineLen)
                    Line[i++] = Stack[--StackPtr];
            }
            if (LastCode != NO_SUCH_CODE && Private->RunningCode - 2 < LZ_MAX_CODE && Prefix[Private->RunningCode - 2] == NO_SUCH_CODE) {
                Prefix[Private->RunningCode - 2] = LastCode;

                if (CrntCode == Private->RunningCode - 2) {
                    Suffix[Private->RunningCode - 2] =
                       DGifGetPrefixChar(Prefix, LastCode, ClearCode);
                } else {
                    Suffix[Private->RunningCode - 2] =
                       DGifGetPrefixChar(Prefix, CrntCode, ClearCode);
                }
            }
            LastCode = CrntCode;
        }
    }

    Private->LastCode = LastCode;
    Private->StackPtr = StackPtr;

    return GIF_OK;
}

static int DGifGetCodeNext(GifFileType *GifFile, GifByteType **CodeBlock)
{
    GifByteType Buf;
    GifFilePrivateType *Private = (GifFilePrivateType *)GifFile->Private;

    if (InternalRead(GifFile, &Buf, 1) != 1) {
        GifFile->Error = D_GIF_ERR_READ_FAILED;
        return GIF_ERROR;
    }

    /* coverity[lower_bounds] */
    if (Buf > 0) {
        *CodeBlock = Private->Buf;    /* Use private unused buffer. */
        (*CodeBlock)[0] = Buf;  /* Pascal strings notation (pos. 0 is len.). */
	/* coverity[tainted_data] */
        if (InternalRead(GifFile, &((*CodeBlock)[1]), Buf) != Buf) {
            GifFile->Error = D_GIF_ERR_READ_FAILED;
            return GIF_ERROR;
        }
    } else {
        *CodeBlock = NULL;
        Private->Buf[0] = 0;    /* Make sure the buffer is empty! */
        Private->PixelCount = 0;    /* And local info. indicate image read. */
    }

    return GIF_OK;
}

static int DGifGetLine(GifFileType *GifFile, GifPixelType *Line, int LineLen)
{
    GifByteType *Dummy;
    GifFilePrivateType *Private = (GifFilePrivateType *) GifFile->Private;

    if (!LineLen)
        LineLen = GifFile->Image.Width;

    if ((Private->PixelCount -= LineLen) > 0xffff0000UL) {
        GifFile->Error = D_GIF_ERR_DATA_TOO_BIG;
        return GIF_ERROR;
    }

    if (DGifDecompressLine(GifFile, Line, LineLen) == GIF_OK) {
        if (Private->PixelCount == 0) {
            do
                if (DGifGetCodeNext(GifFile, &Dummy) == GIF_ERROR)
                    return GIF_ERROR;
            while (Dummy != NULL) ;
        }
        return GIF_OK;
    } else
        return GIF_ERROR;
}

static int DGifGetExtensionNext(GifFileType *GifFile, GifByteType ** Extension)
{
    GifByteType Buf;
    GifFilePrivateType *Private = (GifFilePrivateType *)GifFile->Private;

    if (InternalRead(GifFile, &Buf, 1) != 1) {
        GifFile->Error = D_GIF_ERR_READ_FAILED;
        return GIF_ERROR;
    }

    if (Buf > 0) {
        *Extension = Private->Buf;    /* Use private unused buffer. */
        (*Extension)[0] = Buf;  /* Pascal strings notation (pos. 0 is len.). */
	/* coverity[tainted_data,check_return] */
        if (InternalRead(GifFile, &((*Extension)[1]), Buf) != Buf) {
            GifFile->Error = D_GIF_ERR_READ_FAILED;
            return GIF_ERROR;
        }
    } else
        *Extension = NULL;

    return GIF_OK;
}

static int DGifGetExtension(GifFileType *GifFile, int *ExtCode, GifByteType **Extension)
{
    GifByteType Buf;

    if (InternalRead(GifFile, &Buf, 1) != 1) {
        GifFile->Error = D_GIF_ERR_READ_FAILED;
        return GIF_ERROR;
    }
    *ExtCode = Buf;

    return DGifGetExtensionNext(GifFile, Extension);
}

static void DumpScreen2RGB(unsigned char *framebuffer,
			   ColorMapObject *ColorMap, GifRowType *ScreenBuffer,
			   int ScreenWidth, int ScreenHeight, GifFormat format, unsigned int fb_width)
{
    int i, j;
    GifRowType GifRow;
    GifColorType *ColorMapEntry;
    int num = 0;

    for (i = 0; i < ScreenHeight; i++) {
        GifRow = ScreenBuffer[i];
        for (j = 0; j < ScreenWidth; j++) {
            ColorMapEntry = &ColorMap->Colors[GifRow[j]];
			switch(format) {
			case LCD_GIF_FORMAT_RGB565:
				framebuffer[num++] = ((ColorMapEntry->Green & 0xfc) << 3) | ((ColorMapEntry->Blue & 0xf8) >> 3);
				framebuffer[num++] = (ColorMapEntry->Red & 0xf8) | ((ColorMapEntry->Green & 0xfc) >> 5);
				break;
			case LCD_GIF_FORMAT_RGB888:
				framebuffer[num++] = ColorMapEntry->Blue;
				framebuffer[num++] = ColorMapEntry->Green;
				framebuffer[num++] = ColorMapEntry->Red;
				break;
			case LCD_GIF_FORMAT_RGBA8888:
				framebuffer[num++] = ColorMapEntry->Blue;
				framebuffer[num++] = ColorMapEntry->Green;
				framebuffer[num++] = ColorMapEntry->Red;
				framebuffer[num++] = 0xFF;
				break;
			default:
				LCDLOGE("Format is not support, use argb8888 as default\n");
				framebuffer[num++] = ColorMapEntry->Blue;
				framebuffer[num++] = ColorMapEntry->Green;
				framebuffer[num++] = ColorMapEntry->Red;
				framebuffer[num++] = 0xFF;
				break;
			}
		}

		switch(format) {
		case LCD_GIF_FORMAT_RGB565:
			num += ((fb_width - ScreenWidth) * 2);
			break;
		case LCD_GIF_FORMAT_RGB888:
			num += ((fb_width - ScreenWidth) * 3);
			break;
		case LCD_GIF_FORMAT_RGBA8888:
			num += ((fb_width - ScreenWidth) * 4);
			break;
		default:
			num += ((fb_width - ScreenWidth) * 4);
			break;
		}
	}
}

static void GifFreeMapObject(ColorMapObject *Object)
{
    if (Object != NULL) {
        (void)free(Object->Colors);
        (void)free(Object);
    }
}

static void GifFreeExtensions(int *ExtensionBlockCount,
		  ExtensionBlock **ExtensionBlocks)
{
    ExtensionBlock *ep;

    if (*ExtensionBlocks == NULL)
        return;

    for (ep = *ExtensionBlocks;
	 ep < (*ExtensionBlocks + *ExtensionBlockCount); 
	 ep++)
        (void)free((char *)ep->Bytes);
    (void)free((char *)*ExtensionBlocks);
    *ExtensionBlocks = NULL;
    *ExtensionBlockCount = 0;
}

static void GifFreeSavedImages(GifFileType *GifFile)
{
    SavedImage *sp;

    if ((GifFile == NULL) || (GifFile->SavedImages == NULL)) {
        return;
    }
    for (sp = GifFile->SavedImages; sp < GifFile->SavedImages + GifFile->ImageCount; sp++) {
        if (sp->ImageDesc.ColorMap != NULL) {
            GifFreeMapObject(sp->ImageDesc.ColorMap);
            sp->ImageDesc.ColorMap = NULL;
        }

        if (sp->RasterBits != NULL)
            free((char *)sp->RasterBits);
	
		GifFreeExtensions(&sp->ExtensionBlockCount, &sp->ExtensionBlocks);
    }
    free((char *)GifFile->SavedImages);
    GifFile->SavedImages = NULL;
}

static int DGifCloseFile(GifFileType *GifFile, int *ErrorCode)
{
    //GifFilePrivateType *Private;

    if (GifFile->SColorMap) {
        GifFreeMapObject(GifFile->SColorMap);
        GifFile->SColorMap = NULL;
    }

    if (GifFile->SavedImages) {
        GifFreeSavedImages(GifFile);
        GifFile->SavedImages = NULL;
    }

    GifFreeExtensions(&GifFile->ExtensionBlockCount, &GifFile->ExtensionBlocks);

    //Private = (GifFilePrivateType *) GifFile->Private;

    free((char *)GifFile->Private);
    free(GifFile);
    if (ErrorCode != NULL)
		*ErrorCode = D_GIF_SUCCEEDED;
    return GIF_OK;
}

void lcd_gif_decoder(unsigned char *framebuffer, GifFormat format, unsigned int fb_width)
{
    int	i, Size, Row, Col, Width, Height, ExtCode;
    GifRecordType RecordType;
    GifByteType *Extension;
    GifRowType *ScreenBuffer;
    GifFileType *GifFile;
    ColorMapObject *ColorMap;
    int Error;

    GifFile = DGifOpenFileName(&Error);

    ScreenBuffer = (GifRowType *)malloc(GifFile->SHeight * sizeof(GifRowType));

    Size = GifFile->SWidth * sizeof(GifPixelType);
    ScreenBuffer[0] = (GifRowType) malloc(Size);

    for (i = 0; i < GifFile->SWidth; i++)
		ScreenBuffer[0][i] = GifFile->SBackGroundColor;

    for (i = 1; i < GifFile->SHeight; i++) {
		ScreenBuffer[i] = (GifRowType) malloc(Size);
		memcpy(ScreenBuffer[i], ScreenBuffer[0], Size);
    }

    do {
		DGifGetRecordType(GifFile, &RecordType);
		switch (RecordType) {
	    case IMAGE_DESC_RECORD_TYPE:
			DGifGetImageDesc(GifFile);
			Row = GifFile->Image.Top; /* Image Position relative to Screen. */
			Col = GifFile->Image.Left;
			Width = GifFile->Image.Width;
			Height = GifFile->Image.Height;

			for (i = 0; i < Height; i++) {
				DGifGetLine(GifFile, &ScreenBuffer[Row++][Col], Width);
			}
			break;
	    case EXTENSION_RECORD_TYPE:
			DGifGetExtension(GifFile, &ExtCode, &Extension);
			while (Extension != NULL) {
			    DGifGetExtensionNext(GifFile, &Extension);
			}
			break;
		}
    } while (RecordType != TERMINATE_RECORD_TYPE);

    ColorMap = GifFile->SColorMap;

    DumpScreen2RGB(framebuffer, ColorMap, ScreenBuffer, GifFile->SWidth, GifFile->SHeight, format, fb_width);

	for (i = 1; i < GifFile->SHeight; i++) {
		(void)free(ScreenBuffer[i]);
    }

    (void)free(ScreenBuffer);

    DGifCloseFile(GifFile, &Error);
}

