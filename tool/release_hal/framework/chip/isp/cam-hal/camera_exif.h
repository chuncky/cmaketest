#ifndef _CAMERA_EXIF_H_
#define _CAMERA_EXIF_H_

typedef enum data_tpye{
	EXIF_BYTE =1,
	EXIF_ASCII = 2,
	EXIF_USHORT = 3,
	EXIF_ULONG = 4,
	EXIF_URATIONAL = 5,
	EXIF_UNDEFINED = 7,
	EXIF_SLONG = 9,
	EXIF_SRATIONAL = 10,
}EXIF_TYPE_E;

typedef enum tag_number{
	EXIF_MAKE =0x010f,
	EXIF_MODULE = 0x0110,
	EXIF_ORIENTATION = 0x0112,
	EXIF_IFDPOINT = 0x8769,
	EXIF_EXPOSURETIME = 0x829A,
	EXIF_EXPOSUREPRAGRAME = 0x8822,
	EXIF_ISOSPEED = 0x8827,
	EXIF_METERINGMODE = 0x9207,
	EXIF_FLASH = 0x9209,
	EXIF_IMAGEWIDTH = 0xA002,
	EXIF_IMAGEHEIGHT = 0xA003,
	EXIF_WHITEBLANCE = 0xA403,

}EXIF_TAG_E;


typedef struct IFD
{
	uint16_t tagNO;  // 标签编号
	uint16_t format; // 数据格式
	uint32_t length; // 数据长度
	uint32_t offset; // 数据偏移量，
	char tagName[50];   // 标签名
} IFD_S;

static  IFD_S IFD0[] =
{
	{EXIF_MAKE, EXIF_ASCII, 32, 0x0C, "ASRMICRO"}, //相机制造商信息
	{EXIF_MODULE, EXIF_ASCII, 32, 0x0C, "crane"},  //相机模组信息
#ifdef ENABLE_ONLINE_JPEG
	{EXIF_ORIENTATION, EXIF_USHORT, 1,  0x06, "Orientation"}, // 方向 90:	6; 180: 3; 270: 8; 0: 1
#else
	{EXIF_ORIENTATION, EXIF_USHORT, 1,  0x01, "Orientation"}, // 方向
	{EXIF_IFDPOINT, EXIF_ULONG, 1,  0x05, "ExifIFDPoint"}, // EXIF 子目录offset. Must be the last paramerter
#endif
};

static  IFD_S IFD0_EXIF[] =
{
	{EXIF_EXPOSURETIME, EXIF_URATIONAL, 1,  0x000A, "ExposureTime"},
	{EXIF_EXPOSUREPRAGRAME, EXIF_USHORT, 1,  0x02, "ExposurePragram"},
	{EXIF_ISOSPEED, EXIF_USHORT, 1,  0x50, "ISOSpeed"},
	{EXIF_METERINGMODE, EXIF_USHORT, 1,  0x01, "MeteringMode"},
	{EXIF_FLASH, EXIF_USHORT, 1,  0x00, "FLASH"},
	{EXIF_IMAGEWIDTH, EXIF_ULONG, 1,  0x0280, "ImageWidth"},
	{EXIF_IMAGEHEIGHT, EXIF_ULONG, 1,  0x01E0, "ImageHeight"},
	{EXIF_WHITEBLANCE, EXIF_USHORT, 1,  0x01, "WhiteBlance"},
};

typedef struct _CCExifParamter {
	char *tiff_header_tag;
	char *exif_app1_flag_tag;
	char *exif_header_tag;
	uint16_t exif_ifd_ascii_len;
	uint16_t exif_ifd_offset_cnt;
	char *exif_para_header_tag;
	uint16_t exif_ascii_len;
	uint16_t exif_offset_cnt;
}CCExifParamter;

char exif_soi_marker[] = {0xff,0xd8};
char exif_app1_marker[]	= {0xff,0xe1};
char app1_exif_header_marker[] ={0x45,0x78,0x69,0x66,0x00,0x00};
char app1_tiff_header_marker[] = {0x49,0x49,0x2a,0x00,0x08,0x00,0x00,0x00};
char exif_end[]	= {0x00,0x00};

#define ascii_total_item  2
#define app1_size_item  2
#define ifd0_num_size  2
#define ifd0_item_size  12
#define last_ifd_dir_size  4
#define last_ifd_data_size  8
#define ifd0_length_size  4
#define exif_item_num_size  2
#define exif_end_num_size  2

#endif
