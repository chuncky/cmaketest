/*------------------------------------------------------------
(C) Copyright [2018-2020] ASR Ltd.
All Rights Reserved
------------------------------------------------------------*/

#ifndef MED_NOT_PRESENT

#ifndef _MED_API_H
#define _MED_API_H

#include "plat_types.h"

/*==== CONSTANT ====*/

enum {
   AUD_BLOCK_ALL=0,
   AUD_BLOCK_ALL_EXCEPT_TONE,
   AUD_NO_OF_BLOCK_LEVEL
};

/* MMAPI media type */
enum {
   MMA_TYPE_MIDI=0,
   MMA_TYPE_TONE,
   MMA_TYPE_WAV,
   MMA_TYPE_IMY,
   MMA_TYPE_AMR,
   MMA_TYPE_DAF,
   MMA_TYPE_AAC,
   MMA_TYPE_AU,
   MMA_NO_OF_TYPE
};



typedef struct
{
   kal_uint8   audio_id;
   kal_uint8   play_style;
   kal_uint8   volume;
   kal_uint8   output_path;
   kal_uint16  identifier;
}
aud_play_id_struct;

typedef struct
{
   kal_uint8   src_id;
   kal_uint8   *data_p;
   kal_uint32  len;
   kal_uint8   format;
   kal_uint8   play_style;
   kal_uint8   volume;
   kal_uint8   output_path;
   kal_uint16  identifier;
   kal_uint8   blocking;
}
aud_play_string_struct;

typedef struct
{
   kal_uint8   src_id;
   kal_wchar  *file_name_p;
   kal_uint8   play_style;
   kal_uint8   volume;
   kal_uint8   output_path;
   kal_uint16  identifier;
// set both start_offset and end_offset to 0 for backward compatible
   kal_uint32   start_offset; /* 0 : file begining */
   kal_uint32   end_offset;  /* 0 or file data len-1: file end */
}
aud_play_file_struct;
typedef struct {
    kal_uint16 *text_string;
    kal_uint16 str_type;    /* String Type */
    kal_uint16 lang;        /* Language */
    kal_uint16 app_type;    /* Application Type */
    kal_uint16 gander;      /* Gender */
    kal_uint16 identifier;  /* id for the request */
    kal_uint8 volume;       /* Volume, from 0~6 */
    kal_uint8 path;         /* Audio Path */
    kal_uint8 pitch;        /* Audio Pitch, from 0~100 */
    kal_uint8 speed;        /* Audio Speed, from 0~100 */
} aud_tts_struct;


/************  MMA  ***********/
/* This must be sync with those defined in l1audio.h */
typedef enum {
   MED_SUCCESS,
   MED_FAIL,
   MED_REENTRY,
   MED_NOT_INITIALIZED,
   MED_BAD_FORMAT,
   MED_BAD_PARAMETER,
   MED_BAD_COMMAND,
   MED_NO_HANDLER,
   MED_UNSUPPORTED_CHANNEL,
   MED_UNSUPPORTED_FREQ,
   MED_UNSUPPORTED_TYPE
} med_status_enum;

/* This must be sync with those defined in l1audio.h */
typedef enum {
   MED_NONE,
   MED_DATA_REQUEST,
   MED_DATA_NOTIFICATION,
   MED_END,
   MED_ERROR,
   MED_REPEATED,
   MED_TERMINATED,
   MED_LED_ON,
   MED_LED_OFF,
   MED_VIBRATOR_ON,
   MED_VIBRATOR_OFF,
   MED_BACKLIGHT_ON,
   MED_BACKLIGHT_OFF
} med_event_enum;

typedef  kal_int32 med_handle;
typedef  kal_int32 med_event;
typedef  kal_int32 med_status;
typedef  void (*med_handler)( kal_int32 handle, kal_int32 event );

typedef struct
{
   kal_uint8   *data;
   kal_int32   size;           
   kal_int16   repeats;           
}
mma_player_struct;



#define GET_ARGB_A(_x_) (kal_uint8)((((kal_uint32)_x_)&0xff000000)>>24)
#define GET_ARGB_R(_x_) (kal_uint8)((((kal_uint32)_x_)&0x00ff0000)>>16)
#define GET_ARGB_G(_x_) (kal_uint8)((((kal_uint32)_x_)&0x0000ff00)>>8)
#define GET_ARGB_B(_x_) (kal_uint8)((((kal_uint32)_x_)&0x000000ff))

#define SET_ARGB(_a_,_r_,_g_,_b_) ((((kal_uint32)_a_)<<24)|\
                                    (((kal_uint32)_r_)<<16)|\
                                    (((kal_uint32)_g_)<<8)|\
                                    (((kal_uint32)_b_)))

enum {
   CAM_SRC_ISP=0,
   CAM_SRC_MEM,
   CAM_NO_OF_SRC
};

/* camera param enum */
enum {
   CAM_PARAM_NONE=0,
   CAM_PARAM_ZOOM_FACTOR,
   CAM_PARAM_CONTRAST,
   CAM_PARAM_BRIGHTNESS,
   CAM_PARAM_HUE,
   CAM_PARAM_GAMMA,
   CAM_PARAM_WB,
   CAM_PARAM_EXPOSURE,
   CAM_PARAM_EFFECT,
   CAM_PARAM_BANDING,    //9  //add by WeiD
   CAM_PARAM_SATURATION,
   CAM_PARAM_NIGHT_MODE,
   CAM_PARAM_EV_VALUE,
   CAM_PARAM_FLASH,
   CAM_PARAM_FLASH_MODE=CAM_PARAM_FLASH,
   CAM_PARAM_AE_METERING,
   CAM_PARAM_AF_KEY,
   CAM_PARAM_AF_METERING,
   CAM_PARAM_AF_MODE,
   CAM_PARAM_MANUAL_FOCUS_DIR,
   CAM_PARAM_SHUTTER_PRIORITY,
   CAM_PARAM_APERTURE_PRIORITY,
   CAM_PARAM_ISO,
   CAM_PARAM_SCENE_MODE,
   CAM_PARAM_QUALITY, //2//23  //add by WeiD
   CAM_PARAM_IMAGESIZE, //24 add by WeiD
   CAM_PARAM_ADD_FRAME,//25,chenhe add
   CAM_NO_OF_PARAM
};

/* zoom factor enum */
enum {
	CAM_ZOOM_1X= 0,
	CAM_ZOOM_2X,
	CAM_ZOOM_4X,
	CAM_NO_OF_ZOOM
};

/* exposure compensation value enum */
enum {
	CAM_EV_NEG_4_3 = 0,
	CAM_EV_NEG_3_3,
	CAM_EV_NEG_2_3,
	CAM_EV_NEG_1_3,
	CAM_EV_ZERO,
	CAM_EV_POS_1_3,
	CAM_EV_POS_2_3,
	CAM_EV_POS_3_3,
	CAM_EV_POS_4_3,
	CAM_EV_NIGHT_SHOT,
	CAM_NO_OF_EV	
};

/* Image effect enum */
enum {
	CAM_EFFECT_NOMRAL = 0,
	CAM_EFFECT_SEPIA,
	CAM_EFFECT_WHITELINE,
	CAM_EFFECT_BLACKLINE,
	CAM_EFFECT_BW,
	CAM_EFFECT_GRAYEDGE,
	CAM_EFFECT_FILM,
	CAM_NO_OF_EFFECT
};

/* White balance enum */
enum {
	CAM_WB_AUTO = 0,
	CAM_WB_CLOUD,
	CAM_WB_DAYLIGHT,
	CAM_WB_INCANDESCENCE,
	CAM_WB_FLUORESCENT,
	CAM_WB_TUNGSTEN,
	CAM_WB_MANUAL,
	CAM_NO_OF_WB
};

/* Camera- Jpeg encode quality enum */
enum {
	CAM_JPG_QTY_LOW = 20,  //modified by WeiD
	CAM_JPG_QTY_NORMAL = 14,
	CAM_JPG_QTY_HIGH = 10,
	CAM_JPG_QTY_FINE = 7,
	CAM_NO_OF_JPG_QTY
};

/* camera image rotate/mirror enum */
enum
{
	CAM_IMAGE_NORMAL=0,
	CAM_IMAGE_ROTATE_90,
	CAM_IMAGE_ROTATE_180,
	CAM_IMAGE_ROTATE_270,
	CAM_IMAGE_MIRROR,
	CAM_IMAGE_MIRROR_ROTATE_90,
	CAM_IMAGE_MIRROR_ROTATE_180,
	CAM_IMAGE_MIRROR_ROTATE_270,
	CAM_NO_OF_IMAGE_ROTATE
};

enum
{
	CAM_BANDING_50HZ=0,
	CAM_BANDING_60HZ,
	CAM_NO_OF_BANDING
};

 

/* Special Effect for Preview, Capture and MPEG4 Encode */
enum {
	CAM_EFFECT_ENC_NORMAL=0,
	CAM_EFFECT_ENC_GRAYSCALE,
	CAM_EFFECT_ENC_SEPIA,
	CAM_EFFECT_ENC_SEPIAGREEN,
	CAM_EFFECT_ENC_SEPIABLUE,
	CAM_EFFECT_ENC_COLORINV,
	CAM_EFFECT_ENC_GRAYINV,
	CAM_EFFECT_ENC_BLACKBOARD,
	CAM_EFFECT_ENC_WHITEBOARD,
	CAM_EFFECT_ENC_COPPERCARVING,
	CAM_EFFECT_ENC_EMBOSSMENT,
	CAM_EFFECT_ENC_BLUECARVING,
	CAM_EFFECT_ENC_CONTRAST,
	CAM_EFFECT_ENC_JEAN,
	CAM_EFFECT_ENC_SKETCH,
	CAM_EFFECT_ENC_OIL,
	CAM_NO_OF_EFFECT_ENC,
	
      //add by gugogi
      CAM_EFFECT_ENC_ANTIQUE,
      CAM_EFFECT_ENC_REDRISH,
      CAM_EFFECT_ENC_GREENRISH,
      CAM_EFFECT_ENC_BLUERISH,
      CAM_EFFECT_ENC_BLACKWHITE,
      CAM_EFFECT_ENC_NEGATIVENESS,
      
      
	
};

typedef enum _VIM_CAM_EFFECT
{
	VIM_CAM_EFFECT_ENC_NORMAL=0,
	VIM_CAM_EFFECT_ENC_MONOCHROME,
	VIM_CAM_EFFECT_ENC_SEPIA,
	VIM_CAM_EFFECT_ENC_NEGATIVE,
	VIM_CAM_EFFECT_ENC_REDONLY
}VIM_CAM_EFFECT;

/* Special Effect for Playback and Post-processing */
enum {
	CAM_EFFECT_DEC_NORMAL=0,
	CAM_EFFECT_DEC_GRAYSCALE,
	CAM_EFFECT_DEC_SEPIA,
	CAM_EFFECT_DEC_SEPIAGREEN,
	CAM_EFFECT_DEC_SEPIABLUE,
	CAM_EFFECT_DEC_COLORINV,
	CAM_EFFECT_DEC_GRAYINV,
	CAM_EFFECT_DEC_WATERCOLOR,
	CAM_EFFECT_DEC_LIGHTBLUR,
	CAM_EFFECT_DEC_BLUR,
	CAM_EFFECT_DEC_STRONGBLUR,
	CAM_EFFECT_DEC_UNSHARP,
	CAM_EFFECT_DEC_SHARPEN,
	CAM_EFFECT_DEC_MORESHARPEN,
	CAM_EFFECT_DEC_MEDIAN,
	CAM_EFFECT_DEC_DILATION,
	CAM_EFFECT_DEC_EROSION,
	CAM_NO_OF_EFFECT_DEC
};

/* Adjustment Effect for Preview, Capture and MPEG4 Encode */
enum {
   CAM_ADJ_ENC_NONE=0,
   CAM_ADJ_ENC_HUE,
   CAM_ADJ_ENC_SATURATION,
   CAM_ADJ_ENC_GAMMA,
   CAM_ADJ_ENC_ADJR,
   CAM_ADJ_ENC_ADJG,
   CAM_ADJ_ENC_ADJB,
   CAM_ADJ_ENC_CONTRAST,
   CAM_ADJ_ENC_BRIGHTNESS,
   CAM_NO_OF_ADJ_ENC
};

/* Adjustment Effect for Playback and Post-processing */
enum {
   CAM_ADJ_DEC_NONE=0,
   CAM_ADJ_DEC_HUE,
   CAM_ADJ_DEC_SATURATION,
   CAM_ADJ_DEC_GAMMA,
   CAM_ADJ_DEC_ADJR,
   CAM_ADJ_DEC_ADJG,
   CAM_ADJ_DEC_ADJB,
   CAM_ADJ_DEC_CONTRAST,
   CAM_ADJ_DEC_BRIGHTNESS,
   CAM_NO_OF_ADJ_DEC
};


/* Image - Jpeg encode quality enum */
enum {
	IMG_JPG_QTY_LOW = 0,
	IMG_JPG_QTY_NORMAL,
	IMG_JPG_QTY_HIGH,
	IMG_JPG_QTY_FINE,
	IMG_NO_OF_JPG_QTY
};

/* Display output device */
enum {
	MED_DISPLAY_NONE = 0,
	MED_DISPLAY_TO_MAIN_LCD,
	MED_NO_OF_DISPLAY_DEVICE
};

/* flash mode enum */
enum {
	CAM_FLASH_OFF= 0,
	CAM_FLASH_ON,
	CAM_FLASH_AUTO,
	CAM_FLASH_REDEYE,
	CAM_NO_OF_FLASH
};

/* preview frame rate enum */
enum {
	CAMERA_PERVIEW_FRAME_RATE= 0,
	VIDEO_PERVIEW_FRAME_RATE=1
};

/* camera operation enum */
enum{
	CAM_AE_METERING=0,
	CAM_AF_KEY,
	CAM_AF_METERING,
	CAM_AF_MODE,
	CAM_MANUAL_FOCUS_DIR,
	CAM_SHUTTER_PRIORITY,
	CAM_APERTURE_PRIORITY,
	CAM_ISO,
	CAM_SCENE_MODE,
	CAM_FLASH_MODE,
	CAM_NO_OF_CAMERA_OPERATION
};

/* ae metering enum */
enum{
	CAM_AE_METER_AUTO=0,
	CAM_AE_METER_SPOT,
	CAM_AE_METER_CENTRAL,
	CAM_AE_METER_AVERAGE,
	CAM_NO_OF_AE_METERING
};

/* af key enum */
enum{
	CAM_AF_RELEASE=0,
	CAM_AF_ONE_TIME,
	CAM_AF_CONTINUE,
	CAM_NO_OF_AF_KEY
};

/* af metering enum */
enum{
	CAM_AF_SINGLE_ZONE=0,
	CAM_AF_MULTI3_ZONE,
	CAM_AF_MULTI5_ZONE,
	CAM_NO_OF_AF_METERING
};

/* af mode enum */
enum{
	CAM_AF_AUTO_MODE=0,
	CAM_AF_MACRO_MODE,
	CAM_AF_INFINITE_MODE,
	CAM_AF_MANUAL_MODE,
	CAM_NO_OF_AF_MODE
};

/* menual focus direction enum */
enum{
	CAM_FOCUS_STOP=0,
	CAM_FOCUS_FORWARD,
	CAM_FOCUS_BACKWARD,
	CAM_FOCUS_FORWARD_CONTINUE,
	CAM_FOCUS_BACKWARD_CONTINUE,
	CAM_NO_OF_FOCUS_DIR
};

/* iso number enum */
enum{
	CAM_ISO_AUTO=0,
	CAM_ISO_100,
	CAM_ISO_200,
	CAM_ISO_400,
	CAM_NO_OF_ISO
};

/* scene mode enum */
enum{
	CAM_NORMALSCENE=0,
	CAM_PORTRAIT,
	CAM_LANDSCAPE,
	CAM_SPORT,
	CAM_FLOWER,
	CAM_NIGHTSCENE,
	CAM_NO_OF_SCENE_MODE
};

/*==== DATATYPE ====*/

typedef struct {
   kal_uint16   image_width;
   kal_uint16   image_height;
   kal_uint16   buffer_width;
   kal_uint16   buffer_height;
   kal_uint16   preview_width;
   kal_uint16   preview_height;
   kal_uint16   preview_offset_x;
   kal_uint16   preview_offset_y;
   void*      image_buffer_p;
   kal_uint32  image_buffer_size;
   kal_int16   frame_rate;
   kal_int16   contrast;
   kal_int16   brightness;
   kal_int16   saturation;
   kal_int16   hue;
   kal_int16   gamma;
   kal_int16   WB;
   kal_int16   exposure;
   kal_int16   effect;
   kal_int16   zoom_factor;
   kal_int16   rotate;
   kal_int16   lcd_rotate;
   kal_int16   flash_mode;
   kal_uint8   snapshot_number;
   kal_bool   night_mode;
   kal_uint8   banding_freq;

   kal_uint8   lcd_id;
   kal_uint16  lcd_start_x;
   kal_uint16  lcd_start_y;
   kal_uint16  lcd_end_x;
   kal_uint16  lcd_end_y;
   kal_uint16  roi_offset_x;
   kal_uint16  roi_offset_y;
   kal_uint32   update_layer;
   kal_uint32   hw_update_layer;
	kal_uint32  src_key_color;	

   /* LCD Setting */
   kal_uint32   roi_background_color;         /* set the background color */

 } 
cam_preview_struct;

typedef struct
{
	kal_uint32	file_size[20];
	kal_uint32	file_address[20];
}	
jpegs_struct;


typedef struct {
   kal_uint8	media_mode;
   kal_uint8    source_device;
   kal_uint16    image_width;
   kal_uint16    image_height;
   void*       data;
   void**       capture_buffer_p;
   kal_uint32   file_buffer_len;
   kal_uint32   *file_size_p;
   kal_int16  flash_mode;
   kal_uint8   image_quality;
   kal_uint8   gray_mode;
   kal_uint8   snapshot_number;
   kal_uint32  capture_layer;                /* capture layer for capture from mem(lcd) case */
   kal_uint16  roi_offset_x;                 /* offset x for capture from mem(lcd) case */
   kal_uint16  roi_offset_y;                 /* offset y for capture from mem(lcd) case */
   jpegs_struct *jpegs_p;
   kal_uint8   continue_capture;

 } 
cam_capture_struct;


/* wufasong updated 2007.06.16 for compile pass. */
#if 1 //#if defined (JPG_DECODE) || defined (JPG_ENCODE) || defined (GIF_DECODE)
/* image */
typedef struct
{
   kal_uint16   display_width;	
   kal_uint16   display_height;	
	kal_uint16  image_clip_x1;						/* clip window start x for clipping decoded image */
	kal_uint16  image_clip_x2;						/* clip window end x for clipping decoded image */
	kal_uint16	image_clip_y1;						/* clip window start y for clipping decoded image */
	kal_uint16	image_clip_y2;						/* clip window end y for clipping decoded image */
   void*      image_buffer_p;	
   kal_uint32   image_buffer_size;
   kal_uint8   media_mode;	
   kal_uint8   media_type;	
   void*      data;
   kal_uint32   file_size;
   kal_uint16  seq_num;
   kal_uint8   blocking;
   kal_uint16  decoded_image_width;	
	kal_bool  	jpeg_thumbnail_mode;				/* decode thumbnail or not */
    void      (*callback)(kal_int32 result);
}
img_decode_struct;

typedef struct
{
   kal_uint16   image_width;
   kal_uint16   image_height;
   kal_uint8   image_quality;
   void*      image_buffer_p;
   kal_uint32   image_buffer_size;
   kal_uint16   target_width;
   kal_uint16   target_height;
   kal_uint8   media_mode;
   kal_uint8   media_type;
   void*      data;
   kal_uint32   file_buffer_len;
   kal_uint32   *file_size_p;
   kal_uint16  seq_num;
   kal_uint8   blocking;
   kal_uint8   gray_mode;   
 }
img_encode_struct;

typedef struct
{
	kal_uint32 src_image_address;				/* the source image buffer start address */
	kal_uint32 dest_image_address;			/* the destination image buffer start address */
	kal_uint32 dest_buffer_size;				/* the buffer size of destination image buffer size */
	kal_uint16 src_width;						/* the width of image source */
	kal_uint16 src_height;						/* the height of image source */
	kal_uint16 src_pitch;						/* the pitch width of image source */
	kal_uint16 target_width;					/* the width of target image */
	kal_uint16 target_height;					/* the height of target image */
	kal_uint16 image_clip_x1;					/* image clip window start x */
	kal_uint16 image_clip_x2;					/* image clip window end x */
	kal_uint16 image_clip_y1;					/* image clip window start y */
	kal_uint16 image_clip_y2;					/* image clip window end x */
   kal_uint16  seq_num;                   /* use for non-blocking mode */
   kal_uint8   blocking;
 }
img_resize_struct;

#endif

/* wufasong updated 2007.06.16 for compile pass. */
#if 1 //#if defined (MP4_CODEC) || defined (MJPG_SUPPORT)
/* video */

/* Video encoding quality */
enum {
	VID_REC_QTY_LOW = 0,
	VID_REC_QTY_NORMAL,
	VID_REC_QTY_HIGH,
	VID_REC_QTY_FINE,
	VID_NO_OF_REC_QTY
};

/* Bit stream type, not the file format */
enum {
	VID_3GP_BIT_STREAM = 0,
	VID_MP4_BIT_STREAM,
	VID_NO_OF_BIT_STREAM
};

/* video em mode enum */
enum {
	VID_EM_MODE_DISABLED = 0,
	VID_EM_MODE_ULTRA_HIGH_BITRATE,
	VID_NO_OF_EM_MODE
};


typedef struct _media_vid_record_req_struct vid_record_struct; 


typedef struct
{
   kal_wchar* file_name_p;

   kal_uint16  image_x;
   kal_uint16  image_y;
   kal_uint32  total_frame_num;
}
vid_open_file_struct;

typedef struct {
   kal_uint8   media_mode;
   kal_uint8   media_type;
   void*      data;
   kal_uint32  data_size;
   kal_uint16  seq_num;
   kal_uint16  max_width;
   kal_uint16  max_height;
   kal_uint16  *image_width;
   kal_uint16  *image_height;
   kal_uint64  *total_time;
   kal_uint8   blocking;
   kal_uint8   play_audio;
}
vid_open_struct;

typedef struct
{
   kal_uint16	display_width;
   kal_uint16	display_height;
   kal_uint16   display_offset_x;
   kal_uint16   display_offset_y;
   void*	      image_buffer_p;
   kal_uint32    image_buffer_size;
   kal_int16   play_speed;
   kal_uint8   play_audio;
   kal_uint8   display_device;  /* display device, MED_DISPLAY_NONE, MED_DISPLAY_TO_MAIN_LCD, ... */
   kal_uint8   audio_path;   /* audio path for headset mode */
   kal_uint16  seq_num;
   kal_int16   lcd_rotate;
   kal_uint16  repeats;    /* repeat count, 0xffff: infinite */

   kal_uint8   lcd_id;
   kal_uint16  lcd_start_x;
   kal_uint16  lcd_start_y;
   kal_uint16  lcd_end_x;
   kal_uint16  lcd_end_y;
   kal_uint16  roi_offset_x;
   kal_uint16  roi_offset_y;
   kal_uint32   update_layer;
   kal_uint32   hw_update_layer;

   kal_uint16   video_display_offset_x;
   kal_uint16   video_display_offset_y;

 }
vid_play_struct;

typedef struct
{
   kal_uint8     get_frame;
   kal_uint32    frame_num;
   kal_uint64	time;
   kal_uint16	display_width;
   kal_uint16	display_height;
   kal_int16   display_offset_x;
   kal_int16   display_offset_y;
   void*	      image_buffer_p;
   kal_uint32    image_buffer_size;
   kal_uint8   blocking;
   kal_uint16  seq_num;
 }
vid_seek_struct;

typedef struct {
   void*	      image_buffer_p;
   kal_uint32    image_buffer_size;
} 
vid_snapshot_struct;


typedef struct
{
   kal_uint8	media_mode;
   kal_uint8	media_type;
   void*       data;
   kal_uint16	display_width;
   kal_uint16	display_height;
   void*	      image_buffer_p;
   kal_uint32    image_buffer_size;
   kal_uint16   frame_num;
   kal_uint16  seq_num;
   kal_uint8   blocking;
}
vid_get_iframe_struct;

#endif /* MP4_CODEC */

/* TV output mode */
enum
{
   MED_TV_OUT_DISABLE=0,
   MED_TV_OUT_MODE1,
   MED_TV_OUT_MODE2,
   MED_TV_OUT_MODE3,
   MED_TV_OUT_MODE4   
};

/* TV output format */
enum
{
   MED_TV_FORMAT_NTSC=0,
   MED_TV_FORMAT_PALM,
   MED_TV_FORMAT_PALC,
   MED_TV_FORMAT_PAL	
};


#if defined (JPG_DECODE) || defined (JPG_ENCODE) || defined (GIF_DECODE)
#include "gui/message/msg_def.h"

/* image */
kal_int32 media_img_decode(module_type src_mod_id, void *decode);
kal_int32 media_img_encode(module_type src_mod_id, img_encode_struct *encode);
void media_img_stop(module_type src_mod_id);
kal_int32 media_img_resize(module_type src_mod_id, img_resize_struct *resize);

#endif



#endif /* _MED_API_H */

#endif /* MED_NOT_PRESENT */


