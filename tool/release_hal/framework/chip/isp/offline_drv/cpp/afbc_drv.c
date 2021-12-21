#include <ui_os_api.h>
#include "plat_basic_api.h"
#include "intc_api.h"

#include "../../camera_common_log.h"
#include "afbc_drv.h"

#undef trace

#ifdef AFBC_REG_DUMP
#define trace(fmt, arg...)			\
	do {					\
		raw_uart_log("afbc " fmt "\r\n",## arg);	\
	} while (0)
#else
#define trace(...) do {} while(0)
#endif

int afbc_check_reg = 1;
int afbc_dump_reg = 1;

void cam_afbc_mdelay(uint32_t ms)
{
    ui_delay_ms(ms);
    trace("[%d]", ms);

    return;
}

void cam_afbc_write32(uint32_t addr, uint32_t data)
{
#ifdef AFBC_REG_DUMP
    uint32_t before = 0;
    uint32_t after = 0;
    before = BUREG_READ(addr);
    BUREG_WRITE(addr, data);
    //cam_afbc_mdelay(2);

    if(afbc_check_reg) {
        after = BUREG_READ(addr);
        if ( after != data ) {
            trace("CONFLICT!!!!!!!!!!addr[0x%x]=0x%x,write=0x%x,after=0x%x",addr,before,data,after);
        }
        else if (afbc_dump_reg) {
            trace("addr[0x%x]=0x%x,write=0x%x,after=0x%x",addr,before,data,after);
        }
    }
#else
	BUREG_WRITE(addr, data);
#endif
}

cam_afbc_enc_context enc_context_4k;

int sensor_continue_output_mode = CAM_AFBC_ELEMENT_OFF;

/////////////////encoder function///////////////////////////////////////////////
int cam_afbc_enc_setbufsize(cam_afbc_enc_context*  context)
{
    unsigned int wr_data = 0;
    wr_data = (context->iwidth|((context->iheight)<<16));
    cam_afbc_write32(context->base_addr+CAM_AFBC_ENC_BUF_SIZE, wr_data);
    return 0;
}

int cam_afbc_enc_setup_bbox(cam_afbc_enc_context*  context)
{
    unsigned int wr_data = 0;
    wr_data = ((context->iwidth)-1)<<16;
    //x bounding is from 0 to width-1
    cam_afbc_write32(context->base_addr+CAM_AFBC_ENC_BOUND_BOX_X, wr_data);

    wr_data = 0;
    wr_data = ((context->iheight)-1)<<16;
    //y bounding is from 0 to height-1
    cam_afbc_write32(context->base_addr+CAM_AFBC_ENC_BOUND_BOX_Y, wr_data);
    return 0;
}

//active surface.this fuction has to be called before enable shadow register set.
int cam_afbc_enc_enable_surface(cam_afbc_enc_context*  context)
{
    unsigned int wr_data = 0x0;
    switch (context->afbc_enc_index) {
        case CAM_AFBC_ENC_SEL_2K_AFBC:
            wr_data = CAM_AFBC_ENC_SURFACE_CFG_ENABLE;
            break;
        case CAM_AFBC_ENC_SEL_4K_AFBC_ONLY:
            wr_data = CAM_AFBC_ENC_SURFACE_CFG_ENABLE |CAM_AFBC_ENC_SURFACE_CFG_DISABLE_JPEG;
            break;
        case CAM_AFBC_ENC_SEL_JPEG_ONLY:
            wr_data = CAM_AFBC_ENC_SURFACE_CFG_ENABLE |CAM_AFBC_ENC_SURFACE_CFG_DISABLE_4KAFBCENC;
            break;
        case CAM_AFBC_ENC_SEL_4K_AFBC_AND_JPEG:
            wr_data = CAM_AFBC_ENC_SURFACE_CFG_ENABLE;
            break;
        default:
            trace("ERR:cam_afbc_enc_enable_surface,enc index:%d\n",context->afbc_enc_index);
            break;
    }

    cam_afbc_write32(context->base_addr+CAM_AFBC_ENC_SURFACE_CFG, wr_data);
    context->surface_active = CAM_AFBC_ELEMENT_ON;
    return 0;
}

int cam_afbc_enc_disable_surface(cam_afbc_enc_context*  context)
{
    unsigned int wr_data = 0x0;
    cam_afbc_write32(context->base_addr+CAM_AFBC_ENC_SURFACE_CFG, wr_data);
    context->surface_active = CAM_AFBC_ELEMENT_OFF;
    return 0;
}
//fixed me:how many interrupts? TBD.
int cam_afbc_enc_enable_interupt(cam_afbc_enc_context*  context)
{
    unsigned int wr_data = MASK_CAM_AFBC_ENC_IRQ_ALL;
    unsigned int clear_irq_wr_data = MASK_CAM_AFBC_ENC_IRQ_ALL;
    cam_afbc_write32(context->base_addr+CAM_AFBC_ENC_IRQ_MASK, wr_data);
    cam_afbc_write32(context->base_addr+CAM_AFBC_ENC_IRQ_CLEAR, clear_irq_wr_data);
    return 0;
}

//fixed me:how many interrupts? TBD.
int cam_afbc_enc_disable_interupt(cam_afbc_enc_context*  context)
{
    //unsigned int clear_irq_wr_data = MASK_CAM_AFBC_ENC_IRQ_ALL;
    //cam_afbc_write32(context->base_addr+CAM_AFBC_ENC_IRQ_CLEAR, clear_irq_wr_data);
    cam_afbc_write32(context->base_addr+CAM_AFBC_ENC_IRQ_MASK, 0x0);
    return 0;
}

int cam_afbc_enc_setfmtspec(cam_afbc_enc_context*  context)
{
    unsigned int wr_data = 0;
    if(context->sparse_mode == CAM_AFBC_ELEMENT_ON)
        wr_data  |= CAM_AFBC_ENC_FMT_SPE_SPARSE_ON;  //sparse mode
    if(context->block_split_mode == CAM_AFBC_ELEMENT_ON)
        wr_data  |= CAM_AFBC_ENC_FMT_SPE_BLOCK_SPLIT_ON;  //block split mode
    if(context->yuv_transform== CAM_AFBC_ELEMENT_ON)
        wr_data  |= CAM_AFBC_ENC_FMT_SPE_YUV_TRANSFORM_ON;  //yuv transform
    if(context->bypass_range_conversion== CAM_AFBC_ELEMENT_ON)
        wr_data  |= CAM_AFBC_ENC_FMT_SPE_BYPASS_RANGE_CONVERSION; //bypass_range_conversion

    wr_data  |= context->eFormat; //8bit yuv420 format
    cam_afbc_write32(context->base_addr+CAM_AFBC_ENC_FMT_SPE, wr_data);
    return 0;
}

//todo:output head buf addr.comes from ddr allocation addr.
//head buf and body buf is localed a large buffer.we have to divide it and have to avoid conflict
//64 bytes align in low addr.
int cam_afbc_enc_set_headbuf_adr(cam_afbc_enc_context*  context,unsigned int addr_low,unsigned int addr_high)
{
    //unsigned int wr_data = 0x0;
    cam_afbc_write32(context->base_addr+CAM_AFBC_ENC_HEAD_BUF_ADR_LOW, addr_low);
    cam_afbc_write32(context->base_addr+CAM_AFBC_ENC_HEAD_BUF_ADR_HIGH, addr_high);
    return 0;
}
//todo:output body buf addr.comes from ddr allocation addr.
//head buf and body buf is localed a large buffer.we have to divide it and have to avoid conflict
//64 bytes align in low addr.
int cam_afbc_enc_set_bodybuf_adr(cam_afbc_enc_context*  context,unsigned int addr_low,unsigned int addr_high)
{
    //unsigned int wr_data = 0x0;
    cam_afbc_write32(context->base_addr+CAM_AFBC_ENC_BODY_BUF_ADR_LOW, addr_low);
    cam_afbc_write32(context->base_addr+CAM_AFBC_ENC_BODY_BUF_ADR_HIGH, addr_high);
    return 0;
}

//todo: input buffer is comes from ovt isp virtual buf addr.It is use for Y component in YUV format
//this addr is the same value with ovt isp output buf addr.
//128 bytes align in low addr.
int cam_afbc_enc_set_inputbuf_adr(cam_afbc_enc_context*  context,unsigned int addr_low,unsigned int addr_high)
{
    //unsigned int wr_data = 0x0;
    cam_afbc_write32(context->base_addr+CAM_AFBC_ENC_INPUT_BUF_ADR_LOW, addr_low);
    cam_afbc_write32(context->base_addr+CAM_AFBC_ENC_INPUT_BUF_ADR_HIGH, addr_high);
    return 0;
}

// 128 bytes align
int cam_afbc_enc_set_inputbuf_stride(cam_afbc_enc_context*  context)
{
    unsigned int wr_data = context->iwidth;
    //wr_data = (context->iwidth/0x80+1)*0x80;
    wr_data = ALIGN_TO(context->iwidth,0x80);
    cam_afbc_write32(context->base_addr+CAM_AFBC_ENC_INPUT_BUF_STRIDE, wr_data);
    return 0;
}

//todo: input chroma buffer is comes from ovt isp virtual buf addr.It is use for UV component in YUV format
//this addr is the same value with ovt isp output buf addr.
//128 bytes align in low addr.
int cam_afbc_enc_set_input_chroma_buf_adr(cam_afbc_enc_context*  context,unsigned int addr_low,unsigned int addr_high)
{
    //unsigned int wr_data = 0x0;
    cam_afbc_write32(context->base_addr+CAM_AFBC_ENC_INPUT_BUF_CHROMA_ADR_LOW, addr_low);
    cam_afbc_write32(context->base_addr+CAM_AFBC_ENC_INPUT_BUF_CHROMA_ADR_HIGH, addr_high);
    return 0;
}

//128 bytes align
int cam_afbc_enc_set_input_chroma_buf_stride(cam_afbc_enc_context*  context)
{
    unsigned int wr_data = context->iwidth;
    //wr_data = (context->iwidth/0x80+1)*0x80;
    wr_data = ALIGN_TO(context->iwidth,0x80);
    cam_afbc_write32(context->base_addr+CAM_AFBC_ENC_INPUT_BUF_CHROMA_STRIDE, wr_data);
    return 0;
}

int cam_afbc_enc_set_input_wr_order(cam_afbc_enc_context*  context,unsigned int order)
{
    unsigned int wr_data = order;
    cam_afbc_write32(context->base_addr+CAM_AFBC_ENC_INPUT_BUF_WR_ORDER, wr_data);
    return 0;
}

//shadow register set.start encoder
int cam_afbc_enc_start( cam_afbc_enc_context*  context,int direct_swap)
{
    unsigned int wr_data = 0;
    if(direct_swap == 1)
        wr_data = CAM_AFBC_ENC_COMMAND_DIRECT_SWAP;
    else
        wr_data = CAM_AFBC_ENC_COMMAND_PENDING_SWAP;
    cam_afbc_write32(context->base_addr+CAM_AFBC_ENC_COMMAND, wr_data);
    return 0;
}

int cam_afbc_enc_auto_stop( cam_afbc_enc_context*  context)
{
    unsigned int wr_data = 0;
    wr_data = CAM_AFBC_ENC_COMMAND_AUTO_STOP;
    cam_afbc_write32(context->base_addr+CAM_AFBC_ENC_COMMAND, wr_data);
    return 0;
}

int cam_afbc_enc_init_module(int afbc_enc_index,unsigned int width, unsigned int height,CAM_afbc_ImageFormat  isp_out_format)
{
    if(width>8192 || height >8192 ||width == 0||height==0)
    {
        trace("ERR:cam_afbc_enc_init_module:format error:%dx%d\n",width,height);
        return  -1;
    }
    if(afbc_enc_index != CAM_AFBC_ENC_SEL_JPEG_ONLY)
    {
        trace("ERR:cam_afbc_enc_init_module:invalid enc index:%d\n",afbc_enc_index);
        return  -1;
    }

    // 4k afbc encoder/jpeg
    memset(&enc_context_4k, 0, sizeof(cam_afbc_enc_context));
    enc_context_4k.base_addr = CAM_AFBC_ENC_4K_BASE_ADDR;
    enc_context_4k.context_active = CAM_AFBC_ELEMENT_ON;
    enc_context_4k.afbc_enc_index = afbc_enc_index;
    enc_context_4k.iwidth = width;
    enc_context_4k.iheight = height;
    enc_context_4k.eFormat = isp_out_format;
    enc_context_4k.sparse_mode = CAM_AFBC_ELEMENT_OFF;
    enc_context_4k.block_split_mode = CAM_AFBC_ELEMENT_OFF;
    enc_context_4k.yuv_transform = CAM_AFBC_ELEMENT_OFF; //encoder rgb can open.yuv must close
    enc_context_4k.bypass_range_conversion = CAM_AFBC_ELEMENT_ON;
    enc_context_4k.surface_active = CAM_AFBC_ELEMENT_OFF;
    enc_context_4k.bExitEnc = CAM_AFBC_ELEMENT_OFF;

    return 0;
}

int cam_afbc_enc_deinit_module(cam_afbc_enc_context*  context)
{
    context->bExitEnc = CAM_AFBC_ELEMENT_ON;
    return 0;
}

///////////////interface for outside/////////////////////////////////////////////////////////////////////
void camafbc_test_enc_callback_irq_opt(INTC_InterruptInfo interruptInfo)
{
    uint32_t irq_status = 0;
    uint32_t wr_data = 0;
    cam_afbc_enc_context *context = &enc_context_4k;
    //int enc_index = CAM_AFBC_ENC_SEL_4K_AFBC_ONLY;

    if(context->context_active == CAM_AFBC_ELEMENT_OFF){
        trace("encirq:enc index:%d enc stopped.\n",enc_index);
    }
    //CAM_AFBC_ENC_IRQ_STATUS = CAM_AFBC_ENC_IRQ_RAW_STATUS&CAM_AFBC_ENC_IRQ_MASK
    irq_status = BUREG_READ(context->base_addr+CAM_AFBC_ENC_IRQ_STATUS);
    //afbc sof.the configuration is effect in HW.
    if (irq_status & MASK_CAM_AFBC_ENC_IRQ_STATUS_SOF)
    {
        trace("encirq:enc index:%d afbc sof===\n",enc_index);
        wr_data = MASK_CAM_AFBC_ENC_IRQ_STATUS_SOF;
        cam_afbc_write32(context->base_addr+CAM_AFBC_ENC_IRQ_CLEAR, wr_data);
    }
    //encoder eof
    if (irq_status & MASK_CAM_AFBC_ENC_IRQ_STATUS_SURFACE_COMPLETED)
    {
        trace("encirq:enc index:%d afbc eof===\n",enc_index);
        wr_data = MASK_CAM_AFBC_ENC_IRQ_STATUS_SURFACE_COMPLETED;
        cam_afbc_write32(context->base_addr+CAM_AFBC_ENC_IRQ_CLEAR, wr_data);
    }
     //cfg swapped.the configuration is effect in HW.
    if (irq_status & MASK_CAM_AFBC_ENC_IRQ_STATUS_CFG_SWAPPED)
    {
        trace("encirq:enc index:%d afbc swapped===\n",enc_index);
        wr_data = MASK_CAM_AFBC_ENC_IRQ_STATUS_CFG_SWAPPED;
        cam_afbc_write32(context->base_addr+CAM_AFBC_ENC_IRQ_CLEAR, wr_data);
    }
    //encoder err.
    if (irq_status & MASK_CAM_AFBC_ENC_IRQ_STATUS_ENC_ERR)
    {
        trace("encirq:enc index:%d encoder err===\n",enc_index);
        wr_data = MASK_CAM_AFBC_ENC_IRQ_STATUS_ENC_ERR;
        cam_afbc_write32(context->base_addr+CAM_AFBC_ENC_IRQ_CLEAR, wr_data);
    }
    //tiling err
    if (irq_status & MASK_CAM_AFBC_ENC_IRQ_STATUS_TILING_ERR)
    {
        trace("encirq:enc index:%d tiling err===\n",enc_index);
        wr_data = MASK_CAM_AFBC_ENC_IRQ_STATUS_TILING_ERR;
        cam_afbc_write32(context->base_addr+CAM_AFBC_ENC_IRQ_CLEAR, wr_data);
    }
       //auto stop swapped
    if (irq_status & MASK_CAM_AFBC_ENC_IRQ_AUTOSTOP_SWAPPED)
    {
        trace("encirq:enc index:%d auto stop swapped===\n",enc_index);
        wr_data = MASK_CAM_AFBC_ENC_IRQ_AUTOSTOP_SWAPPED;
        cam_afbc_write32(context->base_addr+CAM_AFBC_ENC_IRQ_CLEAR, wr_data);
    }
}

int camafbc_test_enc_start(int afbc_enc_index,uint32_t width,uint32_t high,uint32_t headbuf_addr_low,uint32_t headbuf_addr_high,uint32_t bodybuf_addr_low,uint32_t bodybuf_addr_high,
            uint32_t inputbuf_addr_low,uint32_t inputbuf_addr_high,uint32_t input_chroma_buf_addr_low,uint32_t input_chroma_buf_addr_high)
{
	int ret = 0;
    cam_afbc_enc_context*  context = NULL;

    trace("camafbc_test_enc_start:index:%d,%dx%d,headbuf_addr_low:0x%x,bodybuf_addr_low:0x%x,inputbuf_addr_low:0x%x,input_chroma_buf_addr_low:%0x\r\n",
        afbc_enc_index,width,high,headbuf_addr_low,bodybuf_addr_low,inputbuf_addr_low,input_chroma_buf_addr_low);
    ret = cam_afbc_enc_init_module(afbc_enc_index,width,high,CAM_AFBC_FMT_SPE_8BIT_YUV420);
	if (ret)
		return ret;
    context = &enc_context_4k;

    cam_afbc_enc_setbufsize(context);
    cam_afbc_enc_setup_bbox(context);
    cam_afbc_enc_setfmtspec(context);

    //output buf addr.comes from ddr addr
    cam_afbc_enc_set_headbuf_adr(context,headbuf_addr_low,headbuf_addr_high);
    cam_afbc_enc_set_bodybuf_adr(context,bodybuf_addr_low,bodybuf_addr_high);
    //input buf addr. comes from ovt isp virtual buffer addr
    cam_afbc_enc_set_inputbuf_adr(context,inputbuf_addr_low,inputbuf_addr_high);
    cam_afbc_enc_set_input_chroma_buf_adr(context,input_chroma_buf_addr_low,input_chroma_buf_addr_high);
    cam_afbc_enc_set_inputbuf_stride(context);
    cam_afbc_enc_set_input_chroma_buf_stride(context);

    cam_afbc_enc_enable_interupt(context);
    cam_afbc_enc_enable_surface(context);
    cam_afbc_enc_start(context,1);//direct_swap
    trace("camafbc_test_enc_start: out\r\n");
    return 0;
}

void camafbc_test_enc_stop()
{
    trace("camafbc_test_enc_stop:stop afbc\n");
    if( enc_context_4k.context_active == CAM_AFBC_ELEMENT_ON){
        trace("stop 4k afbc\n");
    }
    sensor_continue_output_mode = CAM_AFBC_ELEMENT_OFF;
}
/*asic decide to stop when one frame enc finish.sw can trigger stop anytime*/
void camafbc_test_enc_auto_stop()
{
    trace("camafbc_test_enc_stop:stop afbc\n");
    if( enc_context_4k.context_active == CAM_AFBC_ELEMENT_ON){
        trace("stop 4k afbc\n");
        cam_afbc_enc_auto_stop(&enc_context_4k);
    }
    //sensor_continue_output_mode = CAM_AFBC_ELEMENT_OFF;
}

#define IRQ_MMP_CAM_4KAFBC (8)
uint32_t camafbc_irq_enable(void)
{
	INTC_ReturnCode intcStatus;
	uint32_t INT_NUM;

	CAMLOGV(" E");
	INT_NUM = INTCGetIntVirtualNum(IRQ_MMP_CAM_4KAFBC);
	intcStatus = INTCConfigure(INT_NUM, INTC_IRQ, INTC_HIGH_LEVEL);
	ASSERT(intcStatus == INTC_RC_OK);
	intcStatus = INTCBind(INT_NUM, camafbc_test_enc_callback_irq_opt);
	ASSERT(intcStatus == INTC_RC_OK);

	INTCEnable(INT_NUM);

	return INT_NUM;
}

uint32_t camafbc_irq_disable(void)
{
	INTC_ReturnCode INTCStatus;
	uint32_t INT_NUM;

	CAMLOGV(" E");
	INT_NUM = INTCGetIntVirtualNum(IRQ_MMP_CAM_4KAFBC);
	INTCStatus = INTCUnbind(INT_NUM);
	INTCStatus = INTCDisable(INT_NUM);
	if(INTC_RC_OK != INTCStatus){
		CAMLOGE("deinit irq fail!!!");
	}

	return 0;
}
