#ifndef _LCD_GIF_DECODER_H_
#define _LCD_GIF_DECODER_H_

#include "lcd_predefine.h"

#define GIF_STAMP "GIFVER"
#define GIF_STAMP_LEN sizeof(GIF_STAMP) - 1
#define GIF_VERSION_POS 3           /* Version first character in stamp. */

#define NO_SUCH_CODE        4098
#define LZ_MAX_CODE         4095
#define HT_SIZE			8192
#define GIF_ERROR   0
#define GIF_OK      1
#define LZ_BITS             12

#define D_GIF_SUCCEEDED          0
#define D_GIF_ERR_READ_FAILED    102
#define D_GIF_ERR_DATA_TOO_BIG   108
#define D_GIF_ERR_NOT_ENOUGH_MEM 109
#define D_GIF_ERR_NOT_READABLE   111
#define D_GIF_ERR_IMAGE_DEFECT   112
#define D_GIF_ERR_EOF_TOO_SOON   113

#define FILE_STATE_READ     0x08

#define EXTENSION_INTRODUCER      0x21
#define DESCRIPTOR_INTRODUCER     0x2c
#define TERMINATOR_INTRODUCER     0x3b

#define UNSIGNED_LITTLE_ENDIAN(lo, hi)	((lo) | ((hi) << 8))

typedef unsigned char GifPixelType;
typedef unsigned char *GifRowType;
typedef unsigned char GifByteType;
typedef unsigned int GifPrefixType;
typedef int GifWord;

typedef enum {
    UNDEFINED_RECORD_TYPE,
    SCREEN_DESC_RECORD_TYPE,
    IMAGE_DESC_RECORD_TYPE, /* Begin with ',' */
    EXTENSION_RECORD_TYPE,  /* Begin with '!' */
    TERMINATE_RECORD_TYPE   /* Begin with ';' */
} GifRecordType;

typedef struct GifColorType {
    GifByteType Red, Green, Blue;
} GifColorType;

typedef struct ColorMapObject {
    int ColorCount;
    int BitsPerPixel;
    int SortFlag;
    GifColorType *Colors;    /* on malloc(3) heap */
} ColorMapObject;

typedef struct GifImageDesc {
    GifWord Left, Top, Width, Height;   /* Current image dimensions. */
    int Interlace;                     /* Sequential/Interlaced lines. */
    ColorMapObject *ColorMap;           /* The local color map */
} GifImageDesc;

typedef struct ExtensionBlock {
    int ByteCount;
    GifByteType *Bytes; /* on malloc(3) heap */
    int Function;       /* The block function code */
#define CONTINUE_EXT_FUNC_CODE    0x00    /* continuation subblock */
#define COMMENT_EXT_FUNC_CODE     0xfe    /* comment */
#define GRAPHICS_EXT_FUNC_CODE    0xf9    /* graphics control (GIF89) */
#define PLAINTEXT_EXT_FUNC_CODE   0x01    /* plaintext */
#define APPLICATION_EXT_FUNC_CODE 0xff    /* application block (GIF89) */
} ExtensionBlock;

typedef struct SavedImage {
    GifImageDesc ImageDesc;
    GifByteType *RasterBits;         /* on malloc(3) heap */
    int ExtensionBlockCount;         /* Count of extensions before image */    
    ExtensionBlock *ExtensionBlocks; /* Extensions before image */    
} SavedImage;

typedef struct GifFileType {
    GifWord SWidth, SHeight;         /* Size of virtual canvas */
    GifWord SColorResolution;        /* How many colors can we generate? */
    GifWord SBackGroundColor;        /* Background color for virtual canvas */
    GifByteType AspectByte;	     /* Used to compute pixel aspect ratio */
    ColorMapObject *SColorMap;       /* Global colormap, NULL if nonexistent. */
    int ImageCount;                  /* Number of current image (both APIs) */
    GifImageDesc Image;              /* Current image (low-level API) */
    SavedImage *SavedImages;         /* Image sequence (high-level API) */
    int ExtensionBlockCount;         /* Count extensions past last image */
    ExtensionBlock *ExtensionBlocks; /* Extensions past last image */    
    int Error;			     /* Last error condition reported */
    void *Private;                   /* Don't mess with this! */
	const unsigned char *data;
	int pos;
	unsigned int size;
} GifFileType;

typedef struct GifHashTableType {
    uint32_t HTable[HT_SIZE];
} GifHashTableType;

typedef struct GifFilePrivateType {
    GifWord FileState,  /* Where all this data goes to! */
      BitsPerPixel,     /* Bits per pixel (Codes uses at least this + 1). */
      ClearCode,   /* The CLEAR LZ code. */
      EOFCode,     /* The EOF LZ code. */
      RunningCode, /* The next code algorithm can generate. */
      RunningBits, /* The number of bits required to represent RunningCode. */
      MaxCode1,    /* 1 bigger than max. possible code, in RunningBits bits. */
      LastCode,    /* The code before the current code. */
      CrntCode,    /* Current algorithm code. */
      StackPtr,    /* For character stack (see below). */
      CrntShiftState;    /* Number of bits in CrntShiftDWord. */
    unsigned long CrntShiftDWord;   /* For bytes decomposition into codes. */
    unsigned long PixelCount;   /* Number of pixels in image. */
    GifByteType Buf[256];   /* Compressed input is buffered here. */
    GifByteType Stack[LZ_MAX_CODE]; /* Decoded pixels are stacked here. */
    GifByteType Suffix[LZ_MAX_CODE + 1];    /* So we can trace the codes. */
    GifPrefixType Prefix[LZ_MAX_CODE + 1];
    GifHashTableType *HashTable;
    int gif89;
} GifFilePrivateType;

typedef enum {
    LCD_GIF_FORMAT_RGB565 = 0, /*RGB565*/
    LCD_GIF_FORMAT_RGB888, /*RGB888*/
    LCD_GIF_FORMAT_RGBA8888, /*ARGB8888*/
} GifFormat;

void lcd_gif_decoder(unsigned char * framebuffer, GifFormat format, unsigned int fb_width);

#endif

