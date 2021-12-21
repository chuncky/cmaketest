#include <ui_os_api.h>
#include "intc_api.h"

#include "..\..\..\clock\media-clk\media_clk.h"
#include "../../camera_common_log.h"

#include "afbc_drv.h"
#include "jpeg_reg.h"
#include "jpeg_drv.h"
#include "jpeg_table.h"
#include "cp_include.h"

/* close CAMLOGD */
#undef CAMLOGD
#define CAMLOGD(fmt, args...) do {} while(0)

/*
 * #define HUFF_TAB_DUMP 0
 * #define Q_TAB_DUMP 0
 * #define JPEG_HEADER_DUMP 0
 */
#define BU_REG_RDMDFYWR(x,y)  (BUREG_WRITE(x,((BUREG_READ(x))|(y))))
#define BU_REG_RDCLEAR(x,y)  (BUREG_WRITE(x,((BUREG_READ(x))&~(y))))

static u8 g_jpeg_interrupt_flag = INVALID_FLAG_ID;
static struct jpu_device jpu_dev =
{
	JPU_STATE_BOOT,
	0,
	0,
	0,
};
static int jpeg_quality_level = JPEG_QUALITY_LOW;

static void jpeg_write_ahb32(uint32_t addr, uint32_t data)
{
	BUREG_WRITE(JPEG_BASE_ADDR + addr, data);
#ifdef JPEG_REG_DUMP
	CAMLOGE("jpeg w:0x%08x = 0x%08x\r\n", JPEG_BASE_ADDR + addr, data);
#endif
}

static uint32_t jpeg_read_ahb32(uint32_t addr)
{
	uint32_t val = 0xBABABABA;
	val = BUREG_READ(JPEG_BASE_ADDR + addr);
#ifdef JPEG_REG_DUMP
	CAMLOGE("jpeg r:0x%08x = 0x%08x\r\n", JPEG_BASE_ADDR+addr, val);
#endif

	return val;
}

void JpgEncHuffTabSetUp(void)
{
    int i, val;
    const unsigned char *hufftab = jpgEncHuffTable[jpeg_quality_level].data;

    jpeg_write_ahb32(MJPEG_HUFF_CTRL_REG, 0x03);
    for ( i = 0; i < 544; i++ ) {
        val = (hufftab[3*i] << 16) +
            (hufftab[3*i+1] << 8) +
            (hufftab[3*i+2]);
        val &= MASK_(0,20);
#ifdef HUFF_TAB_DUMP
        CAMLOGI("<0x%08x>[huff-%d-0x%08x]",JPEG_BASE_ADDR+MJPEG_HUFF_DATA_REG, i, val);
#endif
        jpeg_write_ahb32(MJPEG_HUFF_DATA_REG, val);
    }
    jpeg_write_ahb32(MJPEG_HUFF_CTRL_REG, 0x00);
}

void JpgEncQMatTabSetUp(void)
{
    int i, val, quant_val;
    const unsigned char *qtab = jpgEncQTable[jpeg_quality_level].data;

    jpeg_write_ahb32(MJPEG_QMAT_CTRL_REG, 0x03);
    for ( i = 0; i < 64; i++ ) {
        val = (qtab[4*i] << 16) +
            (qtab[4*i+1] << 8) +
            qtab[4*i+2];
        val &= MASK_(0,20);
        quant_val = qtab[4*i+3];
        quant_val &= MASK_(0,8);
#ifdef Q_TAB_DUMP
        CAMLOGI("<0x%08x>[qtbl-%d-0x%08x]",JPEG_BASE_ADDR+MJPEG_QMAT_DATA_REG, i, (quant_val<<20) + val);
#endif
        jpeg_write_ahb32(MJPEG_QMAT_DATA_REG, (quant_val<<20) + val);
    }
    jpeg_write_ahb32(MJPEG_QMAT_CTRL_REG, 0x00);

    jpeg_write_ahb32(MJPEG_QMAT_CTRL_REG, 0x43);
    for ( i = 0; i < 64; i++ ) {
        val = (qtab[256+4*i] << 16) +
            (qtab[256+4*i+1] << 8) +
            qtab[256+4*i+2];
        val &= MASK_(0,20);
        quant_val = qtab[256+4*i+3];
        quant_val &= MASK_(0,8);
#ifdef Q_TAB_DUMP
        CAMLOGI("<0x%08x>[qtbl-%d-0x%08x]",JPEG_BASE_ADDR+MJPEG_QMAT_DATA_REG, i, (quant_val<<20) + val);
#endif
        jpeg_write_ahb32(MJPEG_QMAT_DATA_REG, (quant_val<<20) + val);
    }
    jpeg_write_ahb32(MJPEG_QMAT_CTRL_REG, 0x40);

    jpeg_write_ahb32(MJPEG_QMAT_CTRL_REG, 0x83);
    for ( i = 0; i < 64; i++ ) {
        val = (qtab[512+4*i] << 16) +
            (qtab[512+4*i+1] << 8) +
            qtab[512+4*i+2];
        val &= MASK_(0,20);
        quant_val = qtab[512+4*i+3];
        quant_val &= MASK_(0,8);
#ifdef Q_TAB_DUMP
        CAMLOGI("<0x%08x>[qtbl-%d-0x%08x]",JPEG_BASE_ADDR+MJPEG_QMAT_DATA_REG, i, (quant_val<<20) + val);
#endif
        jpeg_write_ahb32(MJPEG_QMAT_DATA_REG, (quant_val<<20) + val);
    }
    jpeg_write_ahb32(MJPEG_QMAT_CTRL_REG, 0x80);
}

void JpgEncHeaderSetup(int w, int h)
{
    const unsigned char *puchar = jpgEncHeaderTable[jpeg_quality_level].data;
    uint32_t length = ((*puchar << 8) + *(puchar + 1));
    uint8_t height_h, height_l, width_h, width_l;
	int i, j;
    int write_cnt = 0;

    if (w < 0xFFFF && h < 0xFFFF) {
	    height_h = (h & 0xFF00) >> 8;
	    height_l = (h & 0x00FF);
	    width_h = (w & 0xFF00) >> 8;
	    width_l = (w & 0x00FF);
    } else {
    	CAMLOGE("\n\n\n!!!!!!NO HEADER FOR %dx%d!!!!\n\n\n", w, h);
    }

    CAMLOGD("jpeg header length = %d\n", length);

    for ( i = 0; i < length; i++ ) {
        if ( i % 4 == 3 ) {
            int val = 0x01;
			uint32_t data;
            while ( val == 0x01 ) {
                val = jpeg_read_ahb32(MJPEG_GBU_PBIT_BUSY_REG);
            }

            data = (*(puchar+2+i-3)<<24) + (*(puchar+2+i-2)<<16) + (*(puchar+2+i-1)<<8) + *(puchar+2+i);

            if ( write_cnt == 1 ) {
                uint32_t fn = 1;
                data = (data & MASK_(16,16)) + (fn & MASK_(0,16));
            }
	    /* setup jpeg header with image size */
            if ( write_cnt == 145) {
                uint32_t fn = height_h;
                data = (data & MASK_(8,24)) + (fn & MASK_(0,8));
            }
            if ( write_cnt == 146 ) {
                uint32_t fn = (height_l << 24) + (width_h << 16) + (width_l << 8);
                data = (data & MASK_(0,8)) + (fn & MASK_(8,24));
            }
            write_cnt++;

#ifdef JPEG_HEADER_DUMP
            CAMLOGI("<0x%08x>[jheader-%d-0x%08x]", JPEG_BASE_ADDR + MJPEG_GBU_PBIT_32_REG, i-3, data);
#endif
            jpeg_write_ahb32(MJPEG_GBU_PBIT_32_REG, data);

            if ( i+4 >= length ) {
                for ( j = i+1; j < length; j++ ) {
                    uint32_t data = (uint32_t)(*(puchar+2+j));
#ifdef JPEG_HEADER_DUMP
                    CAMLOGI("<0x%08x>[jheaderemainder-%d-0x%08x]", JPEG_BASE_ADDR + MJPEG_GBU_PBIT_08_REG, j, data);
#endif
                    jpeg_write_ahb32(MJPEG_GBU_PBIT_08_REG, data);
                }
                break;
            }
        }
    }
}

static void InitNieuport(void)
{
    int val = 0x02;
    jpeg_write_ahb32(MJPEG_PIC_START_REG, val);
    while ( val == 0x02 ) {
        val = 0x02 & jpeg_read_ahb32(MJPEG_PIC_START_REG);
    }
    CAMLOGD("InitNieuport:0x%08x\r\n", jpeg_read_ahb32(MJPEG_PIC_STATUS_REG));

	return;
}

static void SetOutputAddr(uint32_t out_addr, uint32_t in_w, uint32_t in_h)
{
	jpeg_write_ahb32(SD_MJPEG_BBC_WR_PTR_REG, out_addr);
	jpeg_write_ahb32(SD_MJPEG_BBC_BAS_ADDR_REG, out_addr);
	jpeg_write_ahb32(SD_MJPEG_BBC_END_ADDR_REG, out_addr + in_w * in_h);

	jpeg_write_ahb32(MJPEG_BBC_WR_PTR_REG, out_addr);
	jpeg_write_ahb32(MJPEG_BBC_BAS_ADDR_REG, out_addr);
	jpeg_write_ahb32(MJPEG_BBC_END_ADDR_REG, out_addr + in_w * in_h);

	CAMLOGD("set encoding output addr 0x%x", out_addr);
	return;
}

static void SetInputSize(int w, int h)
{
	int val = ((w & MASK_(0, 16)) << 16) + (h & MASK_(0, 16));
	jpeg_write_ahb32(SD_MJPEG_PIC_SIZE_REG, val);
	jpeg_write_ahb32(MJPEG_PIC_SIZE_REG, val);

	CAMLOGD("set jpeg encoding size %dx%d", w, h);
	return;
}

static void EnableContinuousEncode(int on)
{
    int val = on ? 0x03 : 0x02;
    jpeg_write_ahb32(SD_MJPEG_PIC_CON_ENC_REG, val);//FIXME: write once only

    if ( on ) {
        uint32_t rdata;
        rdata = jpeg_read_ahb32(MJPEG_PIC_CTRL_REG);
        rdata |= BIT(31);
        jpeg_write_ahb32(MJPEG_PIC_CTRL_REG, rdata);
    }
}

int jpeg_quality_set(int level)
{
	if (level >= JPEG_QUALITY_MAX) {
		CAMLOGE("invalid jpeg quility %d", level);
		return -1;
	}

	jpeg_quality_level = level;
	return 0;
}

/**
 * @brief jpeg offline encoding streamed from ddr
 *
 * @param in_w
 * @param in_h
 * @param out_addr
 * @param prefix
 *
 * @return 
 */
int jpeg_enc_cfg(uint32_t in_w, uint32_t in_h, uint32_t out_addr)
{
	InitNieuport();
#if 1
	// shadow enable, tc_dir, encode, dummpy, op_mode // fixed
	jpeg_write_ahb32(MJPEG_PIC_CTRL_REG, BIT(5) + BIT(4) + BIT(3));   //FIXME: write once only
#else
	jpeg_write_ahb32(MJPEG_PIC_CTRL_REG, BIT(4) + BIT(3));   // direct mode
#endif
	JpgEncHuffTabSetUp();
	JpgEncQMatTabSetUp();

	SetOutputAddr(out_addr, in_w, in_h);
	SetInputSize(in_w, in_h);
	EnableContinuousEncode(0);
	jpeg_write_ahb32(MJPEG_PIC_START_REG, 1);

	JpgEncHeaderSetup(in_w, in_h);   // generate jpeg header by software

	return 0;
}

void JpegIntrHandler(INTC_InterruptInfo interruptInfo)
{
	u8 rc = 0;

	uint32_t val = jpeg_read_ahb32(MJPEG_PIC_STATUS_REG);
	jpeg_write_ahb32(MJPEG_PIC_STATUS_REG, val);

	if (val & MJPEG_HW_IRQ_STATUS_CODEC_START) {
		CAMLOGD("jpeg sof");
	}
	if (val & MJPEG_HW_IRQ_STATUS_CONFIG_DONE) {
		CAMLOGD("cfg.done");
	}
	if (val & MJPEG_HW_IRQ_STATUS_DECODE_ERROR) {
		CAMLOGE("decode err");
	}
	if (val & MJPEG_HW_IRQ_STATUS_BBC_INTERRUPT) {
		CAMLOGE("bbc Interupt");
	}
	if (val & MJPEG_HW_IRQ_STATUS_PARTIAL_BUF0) {
		CAMLOGE("p0");
	}
	if (val & MJPEG_HW_IRQ_STATUS_PARTIAL_BUF1) {
		CAMLOGE("p1");
	}
	if (val & MJPEG_HW_IRQ_STATUS_PARTIAL_BUF2) {
		CAMLOGE("p2");
	}
	if (val & MJPEG_HW_IRQ_STATUS_PARTIAL_BUF3) {
		CAMLOGE("p3");
	}
	if (val & MJPEG_HW_IRQ_STATUS_PARTIAL_OVERFLOW) {
		CAMLOGE("jpeg.overflow");
	}
	if (val & MJPEG_HW_IRQ_STATUS_CODEC_DONE) {
		uint32_t ptr = jpeg_read_ahb32(LAST_MJPEG_BBC_WR_PTR_REG);
		CAMLOGD("jpeg.eof,[0x%08x]=0x%08x,WR_PTR=0x%08x", ptr & 0xFFF00000, BUREG_READ(ptr & 0xFFF00000), ptr);
	}

	rc = UOS_SetFlag(g_jpeg_interrupt_flag, val, OSA_FLAG_OR);
	if (rc != 0)
		CAMLOGE("jpeg flag set error");

	return;
}


#define IRQ_MMP_CAM_JPEG (63)
static uint32_t jpu_irq_enable(void)
{
	INTC_ReturnCode intcStatus;
	uint32_t INT_NUM;

	INT_NUM = INTCGetIntVirtualNum(IRQ_MMP_CAM_JPEG);
	intcStatus = INTCConfigure(INT_NUM, INTC_IRQ, INTC_HIGH_LEVEL);
	ASSERT(intcStatus == INTC_RC_OK);
	intcStatus = INTCBind(INT_NUM, JpegIntrHandler);
	ASSERT(intcStatus == INTC_RC_OK);

	INTCEnable(INT_NUM);

	return INT_NUM;
}

static uint32_t jpu_irq_disable(void)
{
	INTC_ReturnCode intcStatus;
	uint32_t INT_NUM;

	INT_NUM = INTCGetIntVirtualNum(IRQ_MMP_CAM_JPEG);
	intcStatus = INTCDisable(INT_NUM);
	intcStatus = INTCUnbind(INT_NUM);
	if(INTC_RC_OK != intcStatus){
		CAMLOGE("deinit irq fail!!!");
	}

	return 0;
}

static void media_clk_jpu_on(void)
{
	media_power_on(MODULE_CAMERA);

	media_clk_enable(MEDIA_CLK_LCD_CI, MODULE_CAMERA); /* MEDIA_CLK_LCD_AHB depend on MEDIA_CLK_LCD_CI */
	media_clk_enable(MEDIA_CLK_LCD_AHB | MEDIA_CLK_OFFLINE_AXI, MODULE_CAMERA);
	media_clk_enable(MEDIA_CLK_JPEG_FUNC, MODULE_CAMERA);
	media_clk_enable(MEDIA_CLK_INTERFACE, MODULE_CAMERA);
}

static void media_clk_jpu_off(void)
{
	media_clk_disable(MEDIA_CLK_INTERFACE, MODULE_CAMERA);
	media_clk_disable(MEDIA_CLK_JPEG_FUNC, MODULE_CAMERA);
	media_clk_disable(MEDIA_CLK_LCD_AHB | MEDIA_CLK_OFFLINE_AXI, MODULE_CAMERA);
	media_clk_disable(MEDIA_CLK_LCD_CI, MODULE_CAMERA);

	media_power_off(MODULE_CAMERA);
}

/* FIXME: workaround for dec err after enc, vice verse */
static void jpeg_s_reset(void)
{
#define AP_PMU_BASE 0xd4282800
	CAMLOGI("APMU: jpeg clk reset");
	BU_REG_RDCLEAR(AP_PMU_BASE + 0x50, BIT_15);
	BU_REG_RDMDFYWR(AP_PMU_BASE + 0x50, BIT_15);
}

/**
 * @brief This function initializes proper data structures/resources.
 *
 * @return 
 */
static int cam_jpu_init(void)
{
	if (jpu_dev.state != JPU_STATE_BOOT) {
		CAMLOGE("JPU have been inited!");
		return -1;
	}

	g_jpeg_interrupt_flag = UOS_CreateFlag();

	jpu_dev.jpeg_irq = jpu_irq_enable();
	jpu_dev.afbc_irq = camafbc_irq_enable();
	jpu_dev.jpu_open_cnt = 0;
	jpu_dev.state = JPU_STATE_IDLE;
	jpeg_s_reset(); // workaround: enc abnormal after dec
	return 0;
}

static int cam_jpu_deinit(void)
{
	UOS_DeleteFlag(g_jpeg_interrupt_flag);
	jpu_dev.jpeg_irq = jpu_irq_disable();
	jpu_dev.afbc_irq = camafbc_irq_disable();
	jpu_dev.jpu_open_cnt = 0;
	jpu_dev.state = JPU_STATE_BOOT;

	return 0;
}

/**
 * @brief In order to start a new encoder operation, applications must
 * 	open a new instance for this encoder operation.
 *
 * @return 
 */
int cam_jpu_encopen(int mode)
{
	int ret = 0;

	//OSAMutexLock(jpu_dev.mutex, OS_WAIT_FORERVER);
	media_clk_jpu_on();
	ret = cam_jpu_init();
	if (ret < 0)
		goto open_fail;

	/* jpeg hardmux */
	switch (mode) {
	case OFFLINE_ENC_MODE:
		BU_REG_RDMDFYWR(0xD420C2B0, BIT_1); /* jpeg_encoder '1' encoder '0' decoder */
		BU_REG_RDMDFYWR(0xD420C2B0, BIT_2); /* jpeg_offline '1' offline '0' online */
		break;
	case ONLINE_ENC_MODE:
		BU_REG_RDMDFYWR(0xD420C2B0, BIT_1);
		BU_REG_RDCLEAR(0xD420C2B0, BIT_2);
		break;
	case OFFLINE_DEC_MODE:
		BU_REG_RDCLEAR(0xD420C2B0, BIT_1);
		BU_REG_RDCLEAR(0xD420C2B0, BIT_2);
		break;
	default:
		CAMLOGE("invalid JPU mode %d", mode);
		ret = -1;
		goto open_fail;
	}
	CAMLOGD("0x2b0 = 0x%08x \n\r", BUREG_READ(0xD420C2B0));
	jpu_dev.state = JPU_STATE_ACTIVE;
	jpu_dev.jpu_open_cnt++;
	//OSAMutexUnlock(jpu_dev.mutex);
	goto open_end;

open_fail:
	media_clk_jpu_off();
open_end:
	return ret;
}

/**
 * @brief wait encoding operations finish
 *
 * @return  jpeg size
 */
uint32_t cam_jpu_encfinish(void)
{
	int rc = 0;
	uint32_t i = 0;
	uint32_t actual_flags = 0;
	uint32_t end_ptr = 0;

	while (i++ < 4) {
		rc = UOS_WaitFlag(g_jpeg_interrupt_flag, MJPEG_HW_IRQ_STATUS_CODEC_DONE,
					OSA_FLAG_OR_CLEAR, &actual_flags, JPU_TIMEOUT);
		if ((OS_SUCCESS == rc) && (actual_flags & MJPEG_HW_IRQ_STATUS_CODEC_DONE)) {
			CAMLOGD("jpu hardware encoding done");
			break;
		}
	}

	if(i >= 4){
		CAMLOGE(" jpu enc timeout!");
	}
	end_ptr = jpeg_read_ahb32(LAST_MJPEG_BBC_WR_PTR_REG);

	return end_ptr;
}

/**
 * @brief When an application finished encoding operations and wanted
 * 	to release this instance for other processing, the application
 * 	should close this instance by calling this function.
 *
 * @return 
 */
int cam_jpu_encclose(void)
{
	if (jpu_dev.jpu_open_cnt == 0) {
		return 0;
	}

	/* offline dma jpeg hardmux set */
	BU_REG_RDCLEAR(0xD420C2B0, BIT_1); /* jpeg_encoder '1' encoder '0' decoder */
	BU_REG_RDCLEAR(0xD420C2B0, BIT_2); /* jpeg_offline '1' offline '0' online */
	CAMLOGD("0x2b0 = 0x%08x \n\r", BUREG_READ(0xD420C2B0));

	cam_jpu_deinit();
	media_clk_jpu_off();

	return 0;
}

