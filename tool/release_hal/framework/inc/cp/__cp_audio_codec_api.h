/*------------------------------------------------------------
(C) Copyright [2018-2020] ASR Ltd.
All Rights Reserved
------------------------------------------------------------*/
#ifndef __GUI_CP_INTERNEL_HEADER_FILE__
#error "Please don't include this file directly, please include cp_include.h"
#endif

#ifndef __CP_AUDIO_CODEC_API_H__
#define __CP_AUDIO_CODEC_API_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "__cp_audio_hal_api.h"

#define AUDIOCODEC_REG_NUM  27      //num of audio codec registers

#define AUDIOCODEC_REG_DEFAULT_MASK (unsigned int)(0xFFFFFFFFu)        // 
#define AUDIOCODEC_REG_NONE_MASK (unsigned int)(0x0u)        // 

#define AUX_ALG_CONFIG_0_MASK (unsigned int)((0x7u << 29) | (1u << 17))        // 

#define AUX_ALG_CONFIG_1_MASK (unsigned int)((0xFFu << 1) | (1u << 10))        // 

#define AUX_ALG_CONFIG_3_MASK (unsigned int)((0x1FFu << 22) | (0x1FFu << 7) | 7U)        // 


#define AUDIOCODEC_DFE_DAC_DMA_DIG_GAIN_MASK (unsigned int)(0xFFu << 8)
#define AUDIOCODEC_DFE_ADC_DIG_GAIN_MASK (unsigned int)(0x1Fu)
#define AUDIOCODEC_DFE_ADC_NOTCH_FILT_EN_COEF_MASK (unsigned int)((0xFu << 4) | 1u)
#define AUDIOCODEC_DFE_SIDE_TONE__MASK (unsigned int)((0xFu << 16) | (0x1FFu << 4))


#define AUDIOCODEC_CONTROL_BASE  (0xD4820000)



//ICAT EXPORTED ENUM
typedef enum
{
    AUDIOCODEC_AUDIO_CTRL               =   0x00,
    AUDIOCODEC_AUDIO_CTRL2              =   0x04,
    AUDIOCODEC_AUDIO_BUS_CLK_CTRL       =   0x10,
    AUDIOCODEC_AUDIO_TX_RX_CLK_CTRL     =   0x14,
    AUDIOCODEC_AUDIO_DFE_CLK_CTRL       =   0x1c,
    AUDIOCODEC_AUDIO_STATUS             =   0x24,
    
    AUDIOCODEC_DFE_DAC_ENABLE           =   0x100,
    AUDIOCODEC_DFE_DAC_STG_AMP          =   0x104,
    AUDIOCODEC_DFE_DAC_STG_FREQ         =   0x108,
    AUDIOCODEC_DFE_DAC_CH_SRC           =   0x10c,
    AUDIOCODEC_DFE_DAC_REG_SRC_L        =   0x110,
    AUDIOCODEC_DFE_DAC_REG_SRC_R        =   0x114,
    AUDIOCODEC_DFE_DAC_OFFSET           =   0x118,
    AUDIOCODEC_DFE_DAC_DMA_DIG_GAIN     =   0x11c,
    AUDIOCODEC_DFE_ADC_EN               =   0x120,
    AUDIOCODEC_DFE_ADC_DIG_GAIN         =   0x124,
    AUDIOCODEC_DFE_ADC_NOTCH_FILT_EN_COEF=  0x128,
    AUDIOCODEC_DFE_SIDE_TONE            =   0x130,
    AUDIOCODEC_DFE_INT_ENABLE           =   0x134,
    AUDIOCODEC_DFE_INT_CLR              =   0x138,
    AUDIOCODEC_DFE_INT_ST               =   0x13c,
    AUDIOCODEC_DFE_ADC_PATH_OUT         =   0x144,

    AUDIOCODEC_AUDIO_CONFIG_0           =   0x200,
    AUDIOCODEC_AUDIO_CONFIG_1           =   0x204,
    AUDIOCODEC_AUDIO_CONFIG_2           =   0x208,
    AUDIOCODEC_AUDIO_CONFIG_3           =   0x20c,
    AUDIOCODEC_AUDIO_CONFIG_4           =   0x210,
    
    /* Must be at the end */
    AUDIOCODEC_ENUM_16_BIT              =   0x7FFF ,//16bit enum compiling enforcement
    AUDIOCODEC_ENUM_32_BIT              =   0x7FFFFFFF //32bit enum compiling enforcement
} AUDIOCODEC_REG_T;


//ICAT EXPORTED STRUCT
typedef struct {
    UINT32  index;
    UINT32  reg;
    UINT32  val;
} AUDIOHAL_AIF_CODEC_REGSHADOW_T;


//ICAT EXPORTED STRUCT
typedef struct {
    UINT32  reg;
    UINT32  mask;
    UINT32  val;
} AUDIOHAL_AIF_CODEC_REG_T;



#ifdef __cplusplus
}
#endif
#endif  /* __CP_AUDIO_CODEC_API_H__ */
