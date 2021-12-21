#ifndef __AFBC_DRV_H__
#define __AFBC_DRV_H__

#define ALIGN_TO(x, iAlign) ( (((unsigned int)(x)) + (iAlign) - 1) & (~((iAlign) - 1)) )
/*=========afbc encoder relate register and control define==============*/
/* global registers */
#if 0 /* aquilac project */
#define CAM_AFBC_ENC_2K_BASE_ADDR 0xd4206800
#define CAM_AFBC_ENC_4K_BASE_ADDR 0xd4206a00
#else /* crane project */
#define CAM_AFBC_ENC_2K_BASE_ADDR 0xf0000000 /* have no 2kafbc */
#define CAM_AFBC_ENC_4K_BASE_ADDR 0xd420ca00
#endif

#define CAM_AFBC_ENC_BLOCK_ID (0x00)
#define CAM_AFBC_ENC_IRQ_RAW_STATUS (0x04)
#define CAM_AFBC_ENC_IRQ_CLEAR (0x08)
#define CAM_AFBC_ENC_IRQ_MASK (0x0c)
#define CAM_AFBC_ENC_IRQ_STATUS (0x10)
#define CAM_AFBC_ENC_COMMAND (0x14)
#define CAM_AFBC_ENC_STATUS (0x18)
#define CAM_AFBC_ENC_SURFACE_CFG (0x1c)
#define CAM_AFBC_ENC_AXI_CFG (0x20)
#define CAM_AFBC_ENC_HEAD_BUF_ADR_LOW (0x40)
#define CAM_AFBC_ENC_HEAD_BUF_ADR_HIGH (0x44)
#define CAM_AFBC_ENC_BODY_BUF_ADR_LOW (0x48)
#define CAM_AFBC_ENC_BODY_BUF_ADR_HIGH (0x4c)
#define CAM_AFBC_ENC_FMT_SPE (0x50)
#define CAM_AFBC_ENC_BUF_SIZE (0x54)
#define CAM_AFBC_ENC_BOUND_BOX_X (0x58)
#define CAM_AFBC_ENC_BOUND_BOX_Y (0x5c)
#define CAM_AFBC_ENC_INPUT_BUF_ADR_LOW (0x60)
#define CAM_AFBC_ENC_INPUT_BUF_ADR_HIGH (0x64)
#define CAM_AFBC_ENC_INPUT_BUF_STRIDE (0x68)
#define CAM_AFBC_ENC_INPUT_BUF_CHROMA_ADR_LOW (0x6c)
#define CAM_AFBC_ENC_INPUT_BUF_CHROMA_ADR_HIGH (0x70)
#define CAM_AFBC_ENC_INPUT_BUF_CHROMA_STRIDE (0x74)
#define CAM_AFBC_ENC_INPUT_BUF_WR_ORDER (0x78)

#define CAM_AFBC_ENC_COMMAND_AUTO_STOP (0x1<<2)
#define CAM_AFBC_ENC_COMMAND_PENDING_SWAP (0x1<<1)
#define CAM_AFBC_ENC_COMMAND_DIRECT_SWAP (0x1)

#define CAM_AFBC_ENC_FMT_SPE_SUPER_BLOCK_16x16 (0x0)
#define CAM_AFBC_ENC_FMT_SPE_SUPER_BLOCK_32x8 (0x1<<16)
#define CAM_AFBC_ENC_FMT_SPE_SPARSE_ON  (0x1<<10)
#define CAM_AFBC_ENC_FMT_SPE_BLOCK_SPLIT_ON  (0x1<<9)
#define CAM_AFBC_ENC_FMT_SPE_YUV_TRANSFORM_ON  (0x1<<8)
#define CAM_AFBC_ENC_FMT_SPE_BYPASS_RANGE_CONVERSION  (0x1<<4)

#define MASK_CAM_AFBC_ENC_IRQ_STATUS_SURFACE_COMPLETED    0x00000001
#define MASK_CAM_AFBC_ENC_IRQ_STATUS_CFG_SWAPPED                0x00000002
#define MASK_CAM_AFBC_ENC_IRQ_STATUS_ENC_ERR                          0x00000004
#define MASK_CAM_AFBC_ENC_IRQ_STATUS_TILING_ERR                      0x00000008
#define MASK_CAM_AFBC_ENC_IRQ_STATUS_SOF                                   0x00000010
#define MASK_CAM_AFBC_ENC_IRQ_AUTOSTOP_SWAPPED                    0x00000020
#define MASK_CAM_AFBC_ENC_IRQ_ALL                                                  0x0000003f

#define CAM_AFBC_ENC_SURFACE_CFG_ENABLE (BIT_0)
#define CAM_AFBC_ENC_SURFACE_CFG_DISABLE_4KAFBCENC  (BIT_2)
#define CAM_AFBC_ENC_SURFACE_CFG_DISABLE_JPEG  (BIT_3)

typedef enum
{
    CAM_AFBC_FMT_SPE_RGB565  =0x0,
    CAM_AFBC_FMT_SPE_RGB5551 =0x1,
    CAM_AFBC_FMT_SPE_ARGB1010102 =0x2,
    CAM_AFBC_FMT_SPE_10BIT_YUV420 =0x3,
    CAM_AFBC_FMT_SPE_RGB888 =0x4,
    CAM_AFBC_FMT_SPE_ARGB8888 =0x5,
    CAM_AFBC_FMT_SPE_ARGB4444 =0x6,
    CAM_AFBC_FMT_SPE_R8 =0x7,
    CAM_AFBC_FMT_SPE_RG88 =0x8,
    CAM_AFBC_FMT_SPE_8BIT_YUV420 =0x9,
    CAM_AFBC_FMT_SPE_8BIT_YUV422 =0xb,
    CAM_AFBC_FMT_SPE_10BIT_YUV422 =0xe,
    CAM_AFBC_FMT_SPE_IMGFMT_MAX      = 0xf,
} CAM_afbc_ImageFormat;

typedef enum
{
    CAM_AFBC_ELEMENT_OFF = 0,
    CAM_AFBC_ELEMENT_ON = 1,
}CAM_afbc_element_switch;

typedef enum
{
    CAM_AFBC_ENC_SEL_2K_AFBC = 0,
    CAM_AFBC_ENC_SEL_4K_AFBC_ONLY = 1,
    CAM_AFBC_ENC_SEL_JPEG_ONLY = 2,
    CAM_AFBC_ENC_SEL_4K_AFBC_AND_JPEG = 4,
    CAM_AFBC_ENC_SEL_MAX = 8,
}CAM_afbc_enc_index;

typedef struct
{
    unsigned int base_addr;
    int afbc_enc_index;
    unsigned int iwidth;
    unsigned int iheight;
    CAM_afbc_ImageFormat  eFormat;
    int sparse_mode;
    int block_split_mode;
    int yuv_transform;
    int bypass_range_conversion;
    int surface_active;
    int context_active;
    int bExitEnc;
} cam_afbc_enc_context;

typedef struct
{
    int req_enc_idx;
    unsigned int iwidth;
    unsigned int iheight;
    unsigned int headbuf_sz;
    unsigned int total_sz;
    uint32_t headbuf_addr_low;
    uint32_t headbuf_addr_high;
    uint32_t bodybuf_addr_low;
    uint32_t bodybuf_addr_high;
    uint32_t inputbuf_addr_low;
    uint32_t inputbuf_addr_high;
    uint32_t input_chroma_buf_addr_low;
    uint32_t input_chroma_buf_addr_high;
}cam_afbc_enc_metadata;

#define CAM_AFBC_ENC_CHG_BUFSIZE (BIT(0))
#define CAM_AFBC_ENC_CHG_HEADBUF_ADR (BIT(1))
#define CAM_AFBC_ENC_CHG_BODYBUF_ADR (BIT(2))
#define CAM_AFBC_ENC_CHG_INPUTBUF_ADR (BIT(3))
#define CAM_AFBC_ENC_CHG_INPUTCHROMABUF_ADR (BIT(4))

#define CAM_AFBC_REQ_POOL_NUM 16
#define AFBC_OUT_MEM_NUM 4
#define AFBC_MAX_FRAME_NUM 4

/*=========afbc encoder end==============*/

/*=========afbc Decoder relate register and control define==============*/
#define CAM_AFBC_DEC_4K_BASE_ADDR 0xf8000000

#define CAM_AFBC_DEC_BLOCK_ID (0x00)
#define CAM_AFBC_DEC_IRQ_RAW_STATUS (0x04)
#define CAM_AFBC_DEC_IRQ_CLEAR (0x08)
#define CAM_AFBC_DEC_IRQ_MASK (0x0c)
#define CAM_AFBC_DEC_IRQ_STATUS (0x10)
#define CAM_AFBC_DEC_COMMAND (0x14)
#define CAM_AFBC_DEC_STATUS (0x18)
#define CAM_AFBC_DEC_SURFACE_CFG (0x1c)
#define CAM_AFBC_DEC_AXI_CFG (0x20)

#define CAM_AFBC_DEC_S0_HEAD_BUF_ADR_LOW (0x40)
#define CAM_AFBC_DEC_S0_HEAD_BUF_ADR_HIGH (0x44)
#define CAM_AFBC_DEC_S0_FMT_SPE (0x48)
#define CAM_AFBC_DEC_S0_BUF_SIZE (0x4C)
#define CAM_AFBC_DEC_S0_BOUND_BOX_X (0x50)
#define CAM_AFBC_DEC_S0_BOUND_BOX_Y (0x54)
#define CAM_AFBC_DEC_S0_OUTPUT_BUF_ADDR_LOW (0x58)
#define CAM_AFBC_DEC_S0_OUTPUT_BUF_ADDR_HIGH (0x5C)
#define CAM_AFBC_DEC_S0_OUTPUT_BUF_STRIDE (0x60)
#define CAM_AFBC_DEC_S0_PREFETCH (0x64)

#define CAM_AFBC_DEC_S1_HEAD_BUF_ADR_LOW (0x70)
#define CAM_AFBC_DEC_S1_HEAD_BUF_ADR_HIGH (0x74)
#define CAM_AFBC_DEC_S1_FMT_SPE (0x78)
#define CAM_AFBC_DEC_S1_BUF_SIZE (0x7C)
#define CAM_AFBC_DEC_S1_BOUND_BOX_X (0x80)
#define CAM_AFBC_DEC_S1_BOUND_BOX_Y (0x84)
#define CAM_AFBC_DEC_S1_OUTPUT_BUF_ADDR_LOW (0x88)
#define CAM_AFBC_DEC_S1_OUTPUT_BUF_ADDR_HIGH (0x8C)
#define CAM_AFBC_DEC_S1_OUTPUT_BUF_STRIDE (0x90)
#define CAM_AFBC_DEC_S1_PREFETCH (0x94)

#define CAM_AFBC_DEC_S2_HEAD_BUF_ADR_LOW (0xA0)
#define CAM_AFBC_DEC_S2_HEAD_BUF_ADR_HIGH (0xA4)
#define CAM_AFBC_DEC_S2_FMT_SPE (0xA8)
#define CAM_AFBC_DEC_S2_BUF_SIZE (0xAC)
#define CAM_AFBC_DEC_S2_BOUND_BOX_X (0xB0)
#define CAM_AFBC_DEC_S2_BOUND_BOX_Y (0xB4)
#define CAM_AFBC_DEC_S2_OUTPUT_BUF_ADDR_LOW (0xB8)
#define CAM_AFBC_DEC_S2_OUTPUT_BUF_ADDR_HIGH (0xBC)
#define CAM_AFBC_DEC_S2_OUTPUT_BUF_STRIDE (0xC0)
#define CAM_AFBC_DEC_S2_PREFETCH (0xC4)

#define CAM_AFBC_DEC_S3_HEAD_BUF_ADR_LOW (0xD0)
#define CAM_AFBC_DEC_S3_HEAD_BUF_ADR_HIGH (0xD4)
#define CAM_AFBC_DEC_S3_FMT_SPE (0xD8)
#define CAM_AFBC_DEC_S3_BUF_SIZE (0xDC)
#define CAM_AFBC_DEC_S3_BOUND_BOX_X (0xE0)
#define CAM_AFBC_DEC_S3_BOUND_BOX_Y (0xE4)
#define CAM_AFBC_DEC_S3_OUTPUT_BUF_ADDR_LOW (0xE8)
#define CAM_AFBC_DEC_S3_OUTPUT_BUF_ADDR_HIGH (0xEC)
#define CAM_AFBC_DEC_S3_OUTPUT_BUF_STRIDE (0xF0)
#define CAM_AFBC_DEC_S3_PREFETCH (0xF4)

#define CAM_AFBC_DEC_COMMAND_PENDING_SWAP (0x1<<1)
#define CAM_AFBC_DEC_COMMAND_DIRECT_SWAP (0x1)

#define CAM_AFBC_DEC_FMT_SPE_SUPER_BLOCK_16x16 (0x0)
#define CAM_AFBC_DEC_FMT_SPE_SUPER_BLOCK_32x8 (0x1<<16)
#define CAM_AFBC_DEC_FMT_SPE_BLOCK_SPLIT_ON  (0x1<<9)
#define CAM_AFBC_DEC_FMT_SPE_YUV_TRANSFORM_ON  (0x1<<8)

#define CAM_AFBC_DEC_SURFACE_CFG_S0_ENABLE (0x1)
#define CAM_AFBC_DEC_SURFACE_CFG_S1_ENABLE (0x1<<1)
#define CAM_AFBC_DEC_SURFACE_CFG_S2_ENABLE (0x1<<2)
#define CAM_AFBC_DEC_SURFACE_CFG_S3_ENABLE (0x1<<3)
#define CAM_AFBC_DEC_SURFACE_CFG_CONTINUOUS_ENABLE (0x1<<16)

#define MASK_CAM_AFBC_DEC_IRQ_STATUS_SURFACE_COMPLETED    0x00000001
#define MASK_CAM_AFBC_DEC_IRQ_STATUS_CFG_SWAPPED                0x00000002
#define MASK_CAM_AFBC_DEC_IRQ_STATUS_DEC_ERR                          0x00000004
#define MASK_CAM_AFBC_DEC_IRQ_STATUS_DETILING_ERR                      0x00000008

typedef struct
{
    unsigned int base_addr;
    unsigned int iwidth;
    unsigned int iheight;
    CAM_afbc_ImageFormat  eFormat;
    int block_split_mode;
    int yuv_transform;
    int surface_active_map;
    int bExitDec;
    uint32_t bbox_x_min;
    uint32_t bbox_x_max;
    uint32_t bbox_y_min;
    uint32_t bbox_y_max;
} cam_afbc_dec_context;

int camafbc_test_enc_start(int afbc_enc_index,uint32_t width,uint32_t high,uint32_t headbuf_addr_low,uint32_t headbuf_addr_high,uint32_t bodybuf_addr_low,uint32_t bodybuf_addr_high,
            uint32_t inputbuf_addr_low,uint32_t inputbuf_addr_high,uint32_t input_chroma_buf_addr_low,uint32_t input_chroma_buf_addr_high);
uint32_t camafbc_irq_enable(void);
uint32_t camafbc_irq_disable(void);
#endif
