
#include "hal_drv_conf.h"
#include "halsoc_lcd_mipi_dsi.h"

#if USE_CRANE_LCD_MIPI

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bsp_common.h"
#include "halsoc_lcd_mipi_dphy.h"

#define DSI_ESC_CLK_DEFAULT DSI_ESC_CLK_52M
#define MIPI_CLK_MIN 700000

#define DSI_MAX_TX_FIFO_BYTES 256
#define DSI_MAX_RX_FIFO_BYTES 64

#define DSI_MAX_CMD_FIFO_BYTES  0x302 /*1024*/

#define LPM_FRAME_EN_DEFAULT 0
#define LAST_LINE_TURN_DEFAULT 0
#define HEX_SLOT_EN_DEFAULT 0
#define HSA_PKT_EN_DEFAULT_SYNC_PULSE 1
#define HSA_PKT_EN_DEFAULT_OTHER 0
#define HSE_PKT_EN_DEFAULT_SYNC_PULSE 1
#define HSE_PKT_EN_DEFAULT_OTHER 0
#define HBP_PKT_EN_DEFAULT 1
//#define HFP_PKT_EN_DEFAULT 1
#define HFP_PKT_EN_DEFAULT 0
#define HEX_PKT_EN_DEFAULT 0
#define HLP_PKT_EN_DEFAULT 0
#define AUTO_DLY_DIS_DEFAULT 0
#define TIMING_CHECK_DIS_DEFAULT 0
#define HACT_WC_EN_DEFAULT 1
#define AUTO_WC_DIS_DEFAULT 0
#define VSYNC_RST_EN_DEFAULT 1

#define HS_PREP_CONSTANT_DEFAULT 40
#define HS_PREP_UI_DEFAULT 4
#define HS_ZERO_CONSTANT_DEFAULT 145
#define HS_ZERO_UI_DEFAULT 10
#define HS_TRAIL_CONSTANT_DEFAULT 60
#define HS_TRAIL_UI_DEFAULT 4
#define HS_EXIT_CONSTANT_DEFAULT 100
#define HS_EXIT_UI_DEFAULT 0
#define CK_ZERO_CONSTANT_DEFAULT 300
#define CK_ZERO_UI_DEFAULT 0
#define CK_TRAIL_CONSTANT_DEFAULT 60
#define CK_TRAIL_UI_DEFAULT 0
#define REQ_READY_DEFAULT 0x3C
#define WAKEUP_CONSTANT_DEFAULT 1000000
#define WAKEUP_UI_DEFAULT 0
#define LPX_CONSTANT_DEFAULT 60
#define LPX_UI_DEFAULT 0

#define LCD_SMPN_CTRL     0x188
#define LCD_MISC_CTRL     0x1F8
#define LCD_DUMB_CONTROL      0x1B8

struct s_dsi_ctx g_dsi_ctx;

#define to_dsi_bcnt(timing, bpp)        (((timing) * (bpp)) >> 3)

static unsigned int asr_dsi_lane[5] = {0, 0x1, 0x3, 0x7, 0xf};

uint32_t lcd_dsi_read(uint32_t reg)
{
    return readl(DSI_BASE_ADDR + reg);
}

void lcd_dsi_write(uint32_t reg, uint32_t val)
{
    writel(val, DSI_BASE_ADDR + reg);
}

void lcd_dsi_set_bits(uint32_t reg, uint32_t bits)
{
    lcd_dsi_write(reg, (lcd_dsi_read(reg) | bits));
}

void lcd_dsi_clear_bits(uint32_t reg, uint32_t bits)
{
    lcd_dsi_write(reg, (lcd_dsi_read(reg) & ~bits));
}

void lcd_dsi_write_bits(uint32_t reg, uint32_t value, uint32_t mask, uint32_t shifts)
{
    uint32_t reg_val;
    reg_val = lcd_dsi_read(reg);
    reg_val &= ~(mask << shifts);
    reg_val |= (value << shifts);
    lcd_dsi_write(reg, reg_val);
}

static unsigned char dsi_bit(unsigned int index, unsigned char *pdata)
{
    unsigned char ret;
    unsigned int cindex, bindex;
    cindex = index / 8;
    bindex = index % 8;

    if (pdata[cindex] & (0x1 << bindex)) {
      ret = (unsigned char) 0x1;
    } else {
      ret = (unsigned char) 0x0;
    }

    return ret;
}

#define PMU_LCD_CLK_RES_CTRL1 0x20
static void __set_bitclk(uint32_t parent_id, uint32_t dividor)
{
    uint32_t reg;

    reg = pmu_read(PMU_LCD_CLK_RES_CTRL1);
    reg &= ~(0x3F << 17);
    reg |= (parent_id & 0x7) <<20;
    reg |= (dividor & 0x7) << 17;
    reg |= BIT_15 | BIT_16| BIT_31;
    pmu_write(PMU_LCD_CLK_RES_CTRL1, reg);
    //printf("0x%x write 0x%x\n", PMU_LCD_CLK_RES_CTRL1, reg);
    mdelay(1);
    reg = pmu_read(PMU_LCD_CLK_RES_CTRL1);
    reg &= ~(BIT_31);
    pmu_write(PMU_LCD_CLK_RES_CTRL1, reg);
    //printf("0x%x write 0x%x\n", PMU_LCD_CLK_RES_CTRL1, reg);
    mdelay(1);
    reg = pmu_read(PMU_LCD_CLK_RES_CTRL1);
    //printf("0x%x readl 0x%x\n", PMU_LCD_CLK_RES_CTRL1, reg);

#if 0
    while(0 != (reg & 0x80000000)){
      cnt++;
      if(0 == (cnt % 10))
        LCDLOGW("set bit_clk (parent:%d, div:%d) not finished!", parent_id, dividor);
      lcd_udelay(5);
      reg = LCD_BU_REG_READ(PMU_BASE_ADDR + PMU_LCD_CLK_RES_CTRL1);
    }
#endif
}

static void __set_escclk(uint32_t parent_id)
{
    uint32_t reg;
    reg = pmu_read(PMU_LCD_CLK_RES_CTRL1);
    reg &= ~(BIT_0 | BIT_1);
    reg |= BIT_2 | BIT_3;
    reg |= parent_id;
    pmu_write(PMU_LCD_CLK_RES_CTRL1, reg);
}

/*return value: real escclk*/
static uint32_t lcd_dsi_set_escclk(uint32_t esc_clk /*KHz*/)
{
    uint32_t real_esc_clk= 0;
    uint32_t expec_esc_clk= 0;

#if 0
    if(ready){
      reg = (PMU_BASE_ADDR + PMU_LCD_CLK_RES_CTRL1);
      reg = reg&0x3;
      if(reg == 0)
        real_esc_clk = 52000;
      else if(reg == 1)
        real_esc_clk = 48000;
      else if(reg == 2)
        real_esc_clk = 26000;
      else
        real_esc_clk = 78000;
      LCDLOGI("got real escclk:%d.",real_esc_clk);
      return real_esc_clk;
    }
#endif

    switch(esc_clk){
    case DSI_ESC_CLK_52M:
        __set_escclk(0);
        expec_esc_clk = 52000;
        real_esc_clk = 52000;
        break;
    case DSI_ESC_CLK_48M:
        __set_escclk(1);
        expec_esc_clk = 48000;
        real_esc_clk = 48000;
        break;
    case DSI_ESC_CLK_26M:
        __set_escclk(2);
        expec_esc_clk = 26000;
        real_esc_clk = 26000;
        break;
    case DSI_ESC_CLK_78M:
        __set_escclk(3);
        expec_esc_clk = 78000;
        real_esc_clk = 78000;
        break;
    default:
        __set_escclk(0);
        expec_esc_clk = 0;
        real_esc_clk = 52000;
    };
    printf("need escclk:%d, real escclk:%d.\n",expec_esc_clk, real_esc_clk);
    return real_esc_clk;
}

static void dump_mipi_info(struct s_mipi_info *mipi_info)
{
    printf("height: %d\n",mipi_info->height);
    printf("width: %d\n",mipi_info->width);
    printf("hfp: %d\n",mipi_info->hfp);
    printf("hbp: %d\n",mipi_info->hbp);
    printf("hsync: %d\n",mipi_info->hsync);
    printf("vfp: %d\n",mipi_info->vfp);
    printf("vbp: %d\n",mipi_info->vbp);
    printf("vsync: %d\n",mipi_info->vsync);
    printf("fps: %d\n",mipi_info->fps);

    printf("work_mode: %d\n",mipi_info->work_mode);
    printf("rgb_mode: %d\n",mipi_info->rgb_mode);
    printf("lane_number: %d\n",mipi_info->lane_number);
    printf("phy_freq: %d\n",mipi_info->phy_freq);
    printf("split_enable: %d\n",mipi_info->split_enable);
    printf("eotp_enable: %d\n",mipi_info->eotp_enable);

    printf("burst_mode: %d\n",mipi_info->burst_mode);

    printf("te_enable: %d\n",mipi_info->te_enable);
    printf("vsync_pol: %d\n",mipi_info->vsync_pol);
    printf("te_pol: %d\n",mipi_info->te_pol);
    printf("te_mode: %d\n",mipi_info->te_mode);

    printf("real_fps: %d\n",mipi_info->real_fps);
}

static unsigned char calculate_ecc(unsigned char *pdata)
{
    unsigned char ret;
    unsigned char p[8];

    p[7] = (unsigned char) 0x0;
    p[6] = (unsigned char) 0x0;

    p[5] = (unsigned char) (
    (
      dsi_bit(10, pdata) ^
      dsi_bit(11, pdata) ^
      dsi_bit(12, pdata) ^
      dsi_bit(13, pdata) ^
      dsi_bit(14, pdata) ^
      dsi_bit(15, pdata) ^
      dsi_bit(16, pdata) ^
      dsi_bit(17, pdata) ^
      dsi_bit(18, pdata) ^
      dsi_bit(19, pdata) ^
      dsi_bit(21, pdata) ^
      dsi_bit(22, pdata) ^
      dsi_bit(23, pdata)
      )
    );
    p[4] = (unsigned char) (
      dsi_bit(4, pdata) ^
      dsi_bit(5, pdata) ^
      dsi_bit(6, pdata) ^
      dsi_bit(7, pdata) ^
      dsi_bit(8, pdata) ^
      dsi_bit(9, pdata) ^
      dsi_bit(16, pdata) ^
      dsi_bit(17, pdata) ^
      dsi_bit(18, pdata) ^
      dsi_bit(19, pdata) ^
      dsi_bit(20, pdata) ^
      dsi_bit(22, pdata) ^
      dsi_bit(23, pdata)
    );
    p[3] = (unsigned char) (
    (
      dsi_bit(1, pdata) ^
      dsi_bit(2, pdata) ^
      dsi_bit(3, pdata) ^
      dsi_bit(7, pdata) ^
      dsi_bit(8, pdata) ^
      dsi_bit(9, pdata) ^
      dsi_bit(13, pdata) ^
      dsi_bit(14, pdata) ^
      dsi_bit(15, pdata) ^
      dsi_bit(19, pdata) ^
      dsi_bit(20, pdata) ^
      dsi_bit(21, pdata) ^
      dsi_bit(23, pdata)
      )
    );
    p[2] = (unsigned char) (
    (
      dsi_bit(0, pdata) ^
      dsi_bit(2, pdata) ^
      dsi_bit(3, pdata) ^
      dsi_bit(5, pdata) ^
      dsi_bit(6, pdata) ^
      dsi_bit(9, pdata) ^
      dsi_bit(11, pdata) ^
      dsi_bit(12, pdata) ^
      dsi_bit(15, pdata) ^
      dsi_bit(18, pdata) ^
      dsi_bit(20, pdata) ^
      dsi_bit(21, pdata) ^
      dsi_bit(22, pdata)
      )
    );
    p[1] = (unsigned char) (
      (
      dsi_bit(0, pdata) ^
      dsi_bit(1, pdata) ^
      dsi_bit(3, pdata) ^
      dsi_bit(4, pdata) ^
      dsi_bit(6, pdata) ^
      dsi_bit(8, pdata) ^
      dsi_bit(10, pdata) ^
      dsi_bit(12, pdata) ^
      dsi_bit(14, pdata) ^
      dsi_bit(17, pdata) ^
      dsi_bit(20, pdata) ^
      dsi_bit(21, pdata) ^
      dsi_bit(22, pdata) ^
      dsi_bit(23, pdata)
      )
    );
    p[0] = (unsigned char) (
      (
      dsi_bit(0, pdata) ^
      dsi_bit(1, pdata) ^
      dsi_bit(2, pdata) ^
      dsi_bit(4, pdata) ^
      dsi_bit(5, pdata) ^
      dsi_bit(7, pdata) ^
      dsi_bit(10, pdata) ^
      dsi_bit(11, pdata) ^
      dsi_bit(13, pdata) ^
      dsi_bit(16, pdata) ^
      dsi_bit(20, pdata) ^
      dsi_bit(21, pdata) ^
      dsi_bit(22, pdata) ^
      dsi_bit(23, pdata)
      )
    );
    ret = (unsigned char)(
      p[0] |
      (p[1] << 0x1) |
      (p[2] << 0x2) |
      (p[3] << 0x3) |
      (p[4] << 0x4) |
      (p[5] << 0x5)
    );
    return ret;
}

static unsigned short gs_crc16_generation_code = 0x8408;
static unsigned short calculate_crc16(unsigned char *pdata, unsigned
    short count)
{
    unsigned short byte_counter;
    unsigned char bit_counter;
    unsigned char data;
    unsigned short crc16_result = 0xFFFF;
    if (count > 0) {
        for (byte_counter = 0; byte_counter < count;
            byte_counter++) {
            data = *(pdata + byte_counter);
            for (bit_counter = 0; bit_counter < 8; bit_counter++) {
                if (((crc16_result & 0x0001) ^ ((0x0001 *
                  data) & 0x0001)) > 0) {
                  crc16_result = ((crc16_result >> 1)
                  & 0x7FFF) ^ gs_crc16_generation_code;
                } else {
                  crc16_result = (crc16_result >> 1) & 0x7FFF;
                }
                data = (data >> 1) & 0x7F;
            }
        }
    }
    return crc16_result;
}

static void dsi_get_default_advanced_setting(struct s_dsi_ctx* dsi_ctx)
{
    struct s_dsi_adv_setting *adv_setting = &dsi_ctx->adv_setting;

    adv_setting->lpm_frame_en = LPM_FRAME_EN_DEFAULT;
    adv_setting->last_line_turn = LAST_LINE_TURN_DEFAULT;
    adv_setting->hex_slot_en = HEX_SLOT_EN_DEFAULT;
    if(dsi_ctx->mipi_info.burst_mode == DSI_BURST_MODE_NON_BURST_SYNC_PULSE) {
        adv_setting->hsa_pkt_en = HSA_PKT_EN_DEFAULT_SYNC_PULSE;
    } else {
        adv_setting->hsa_pkt_en = HSA_PKT_EN_DEFAULT_OTHER;
    }
    if(dsi_ctx->mipi_info.burst_mode == DSI_BURST_MODE_NON_BURST_SYNC_PULSE) {
        adv_setting->hse_pkt_en = HSE_PKT_EN_DEFAULT_SYNC_PULSE;
    } else {
        adv_setting->hse_pkt_en = HSE_PKT_EN_DEFAULT_OTHER;
    }
    adv_setting->hbp_pkt_en = HBP_PKT_EN_DEFAULT;
    adv_setting->hfp_pkt_en = HFP_PKT_EN_DEFAULT;
    adv_setting->hex_pkt_en = HEX_PKT_EN_DEFAULT;
    adv_setting->hlp_pkt_en = HLP_PKT_EN_DEFAULT;
    adv_setting->auto_dly_dis = AUTO_DLY_DIS_DEFAULT;
    adv_setting->timing_check_dis = TIMING_CHECK_DIS_DEFAULT;
    adv_setting->hact_wc_en = HACT_WC_EN_DEFAULT;
    adv_setting->auto_wc_dis = AUTO_WC_DIS_DEFAULT;
    adv_setting->vsync_rst_en = VSYNC_RST_EN_DEFAULT;
}

static void dsi_get_default_dphy_setting(struct s_dsi_ctx* dsi_ctx)
{
    struct s_dphy_timing *dphy_timing;

    dphy_timing = &dsi_ctx->dphy_config.dphy_timing;
    dphy_timing->hs_prep_constant = HS_PREP_CONSTANT_DEFAULT;
    dphy_timing->hs_prep_ui = HS_PREP_UI_DEFAULT;
    dphy_timing->hs_zero_constant = HS_ZERO_CONSTANT_DEFAULT;
    dphy_timing->hs_zero_ui = HS_ZERO_UI_DEFAULT;
    dphy_timing->hs_trail_constant = HS_TRAIL_CONSTANT_DEFAULT;
    dphy_timing->hs_trail_ui = HS_TRAIL_UI_DEFAULT;
    dphy_timing->hs_exit_constant = HS_EXIT_CONSTANT_DEFAULT;
    dphy_timing->hs_exit_ui = HS_EXIT_UI_DEFAULT;
    dphy_timing->ck_zero_constant = CK_ZERO_CONSTANT_DEFAULT;
    dphy_timing->ck_zero_ui = CK_ZERO_UI_DEFAULT;
    dphy_timing->ck_trail_constant = CK_TRAIL_CONSTANT_DEFAULT;
    dphy_timing->ck_zero_ui = CK_TRAIL_UI_DEFAULT;
    dphy_timing->req_ready = REQ_READY_DEFAULT;
    dphy_timing->wakeup_constant = WAKEUP_CONSTANT_DEFAULT;
    dphy_timing->wakeup_ui = WAKEUP_UI_DEFAULT;
    dphy_timing->lpx_constant = LPX_CONSTANT_DEFAULT;
    dphy_timing->lpx_ui = LPX_UI_DEFAULT;
}

#if 0
static void dsi_hw_reset(void)
{
    uint32_t reg;

    reg = CFG_SOFT_RST | CFG_SOFT_RST_REG | CFG_CLR_PHY_FIFO | CFG_RST_TXLP |
      CFG_RST_CPU | CFG_RST_CPN | CFG_RST_VPN | CFG_DSI_PHY_RST;

    /* software reset DSI module */
    lcd_dsi_write(DSI_CTRL_0, reg);
    /* Note: there need some delay after set CFG_SOFT_RST */
    lcd_udelay(1000);
    lcd_dsi_write(DSI_CTRL_0, 0);
}
#endif

static void dsi_enable_video_mode(int enable)
{
    if(enable) {
        lcd_dsi_set_bits(DSI_CTRL_0, CFG_VPN_TX_EN | CFG_VPN_SLV | CFG_VPN_EN);
        printf("dsi 0x1e8 = 0x%x\n", lcd_dsi_read(DSI_PHY_ANA_CTRL1));
    } else {
        lcd_dsi_clear_bits(DSI_CTRL_0, CFG_VPN_TX_EN | CFG_VPN_EN);
    }
}

static void dsi_enable_cmd_mode(int enable)
{
    if(enable) {
        lcd_dsi_set_bits(DSI_CTRL_0, CFG_VPN_TX_EN | CFG_VPN_SLV | CFG_CPN_EN);
    } else {
        lcd_dsi_clear_bits(DSI_CTRL_0, CFG_VPN_TX_EN | CFG_CPN_EN);
    }
}

static void dsi_enable_eotp(int enable)
{
    if(enable) {
        lcd_dsi_set_bits(DSI_CTRL_1, CFG_EOTP_EN);
    } else {
        lcd_dsi_clear_bits(DSI_CTRL_1, CFG_EOTP_EN);
    }
}

static void dsi_set_cvc(int vc)
{
    lcd_dsi_write_bits(DSI_CTRL_1, vc&0x3, 0x3, CFG_CPN_VCH_NO_SHIFT);
}

static void dsi_set_vvc(int vc)
{
    lcd_dsi_write_bits(DSI_CTRL_1, vc&0x3, 0x3, CFG_VPN_VCH_NO_SHIFT);
}

static void dsi_enable_lptx_lanes(uint32_t lane_num)
{
    lcd_dsi_write_bits(DSI_CPU_CMD_1, lane_num,
        CFG_TXLP_LPDT_MASK, CFG_TXLP_LPDT_SHIFT);
}

static void dsi_enable_split_mode(int splite_mode)
{
    if(splite_mode) {
      lcd_dsi_set_bits(DSI_LCD_BDG_CTRL0, CFG_SPLIT_EN);
    } else {
      lcd_dsi_clear_bits(DSI_LCD_BDG_CTRL0, CFG_SPLIT_EN);
    }
}

static int dsi_write_cmd(uint8_t *parameter, uint8_t count, int lp)
{
    uint32_t send_data = 0, reg, timeout, tmp, i;
    int turnaround;
    uint32_t len;

    if(lp) {
        printf("%d packet data will be write in lp mode\n", count);
    } else {
        printf("%d data will be write in hs mode\n", count);
    }

    /* write all packet bytes to packet data buffer */
    for (i = 0; i < count; i++) {
        send_data |= parameter[i] << ((i % 4) * 8);
        if (0 ==((i + 1) % 4)) {
            lcd_dsi_write(DSI_CPU_WDAT, send_data);
            reg = CFG_CPU_DAT_REQ | CFG_CPU_DAT_RW |((i - 3) << CFG_CPU_DAT_ADDR_SHIFT);
            lcd_dsi_write(DSI_CPU_CMD_3, reg);
            /* wait write operation done */
            timeout = 1000;
            do {
              timeout--;
              tmp = lcd_dsi_read(DSI_CPU_CMD_3);
            } while ((tmp & CFG_CPU_DAT_REQ) && timeout);
            if (timeout == 0)
              printf("DSI write data to the packet data buffer not done.\n");
            send_data = 0;
        }
    }

    /* handle last none 4Byte align data */
    if (0 != i % 4) {
        lcd_dsi_write(DSI_CPU_WDAT, send_data);
        reg = CFG_CPU_DAT_REQ | CFG_CPU_DAT_RW |((4 * (i / 4)) << CFG_CPU_DAT_ADDR_SHIFT);
        lcd_dsi_write(DSI_CPU_CMD_3, reg);
        /* wait write operation done */
        timeout = 1000;
        do {
          timeout--;
          tmp = lcd_dsi_read(DSI_CPU_CMD_3);
        } while ((tmp & CFG_CPU_DAT_REQ) && timeout);
        if (timeout == 0)
          printf("DSI write data to the packet data buffer not done.\n");
        send_data = 0;
    }

    if (parameter[0] == DSI_DCS_READ || parameter[0] == DSI_GENERIC_READ1) {
        turnaround = TRUE;
    } else {
        turnaround = FALSE;
    }

    len = count;
#if 0
    /* The packet length should contain  CRC_bytes_length in Aquilac_DSI version */
    if ((parameter[0] == DSI_DCS_LWRITE ||
      parameter[0] == DSI_GENERIC_LWRITE) && !lp)
      len = count - 6;
#endif
    reg = CFG_CPU_CMD_REQ |
      ((count == 4) ? CFG_CPU_SP : 0) |
      (turnaround ? CFG_CPU_TURN : 0) |
      (lp ? CFG_CPU_TXLP : 0) |
      (len << CFG_CPU_WC_SHIFT);

    /* send out the packet */
    lcd_dsi_write(DSI_CPU_CMD_0, reg);
    /* wait packet be sent out */
    timeout = 1000;
    do {
        timeout--;
        tmp = lcd_dsi_read(DSI_CPU_CMD_0);
        mdelay(50);
    } while ((tmp & CFG_CPU_CMD_REQ) && timeout);
    if (0 == timeout) {
        printf("DSI send out packet maybe failed. \n");
        return -1;
    }

    return 0;
}

static void dsi_config_video_mode(struct s_dsi_ctx *dsi_ctx)
{
    uint32_t hsync_b, hbp_b, hact_b, hex_b, hfp_b, httl_b;
    uint32_t hsync, hbp, hact, httl, v_total;
    uint32_t hsa_wc, hbp_wc, hact_wc, hex_wc, hfp_wc, hlp_wc;
    uint32_t bpp, hss_bcnt = 4, hse_bct = 4, lgp_over_head = 6, reg;
    uint32_t slot_cnt0, slot_cnt1;
    uint32_t dsi_ex_pixel_cnt = 0;
    uint32_t dsi_hex_en = 0;
    uint32_t width, lane_number;
    struct s_dsi_adv_setting *adv_setting = &dsi_ctx->adv_setting;
    struct s_mipi_info *mipi_info = &dsi_ctx->mipi_info;

    switch(mipi_info->rgb_mode){
    case DSI_INPUT_DATA_RGB_MODE_565:
      bpp = 16;
      break;
    case DSI_INPUT_DATA_RGB_MODE_666PACKET:
      bpp = 18;
      break;
    case DSI_INPUT_DATA_RGB_MODE_666UNPACKET:
      bpp = 18;
      break;
    case DSI_INPUT_DATA_RGB_MODE_888:
      bpp = 24;
      break;
    default:
      bpp = 24;
    }

    v_total = mipi_info->height + mipi_info->vfp + mipi_info->vbp + mipi_info->vsync;

    if(mipi_info->split_enable) {
      if(( 0 != (mipi_info->width & 0x1)) || (0 != (mipi_info->lane_number & 0x1))){
        printf("warning:Invalid split config(lane = %d, width = %d)\n",
          mipi_info->lane_number, mipi_info->width);
      }
      width = mipi_info->width >> 1;
      lane_number = mipi_info->lane_number >> 1;
    } else {
      width = mipi_info->width;
      lane_number = mipi_info->lane_number;
    }

    hact_b = to_dsi_bcnt(width, bpp);
    hfp_b = to_dsi_bcnt(mipi_info->hfp, bpp);
    hbp_b = to_dsi_bcnt(mipi_info->hbp, bpp);
    hsync_b = to_dsi_bcnt(mipi_info->hsync, bpp);
    hex_b = to_dsi_bcnt(dsi_ex_pixel_cnt, bpp);
    httl_b = hact_b + hsync_b + hfp_b + hbp_b + hex_b;
    slot_cnt0 = (httl_b - hact_b) / lane_number + 3;
    slot_cnt1 = slot_cnt0;

    hact = hact_b / lane_number;
    hbp = hbp_b / lane_number;
    hsync = hsync_b / lane_number;
    httl = (hact_b + hfp_b + hbp_b + hsync_b) / lane_number;

    /* word count in the unit of byte */
    hsa_wc = (mipi_info->burst_mode == DSI_BURST_MODE_NON_BURST_SYNC_PULSE) ?
      (hsync_b - hss_bcnt - lgp_over_head) : 0;

    /* Hse is with backporch */
    hbp_wc = (mipi_info->burst_mode == DSI_BURST_MODE_NON_BURST_SYNC_PULSE) ?
      (hbp_b - hse_bct - lgp_over_head)
      : (hsync_b + hbp_b - hss_bcnt - lgp_over_head);

    hfp_wc = ((mipi_info->burst_mode == DSI_BURST_MODE_BURST) && (dsi_hex_en == 0)) ?
      (hfp_b + hex_b - lgp_over_head - lgp_over_head) :
      (hfp_b - lgp_over_head - lgp_over_head);

    hact_wc =  (width * bpp) >> 3;

    /* disable Hex currently */
    hex_wc = 0;

    /*  There is no hlp with active data segment.  */
    hlp_wc = (mipi_info->burst_mode == DSI_BURST_MODE_NON_BURST_SYNC_PULSE) ?
      (httl_b - hsync_b - hse_bct - lgp_over_head) :
      (httl_b - hss_bcnt - lgp_over_head);

    /* FIXME - need to double check the (*3) is bytes_per_pixel
    * from input data or output to panel
    */

    /*Jessica: need be calculated by really case*/
    //lcd_dsi_write(DSI_VPN_CTRL_0, (0x50<<16) | 0xc08); //aquilac
    lcd_dsi_write(DSI_VPN_CTRL_0, (0x200<<16) | 0x10); //crane.

      /* SET UP LCD1 TIMING REGISTERS FOR DSI BUS */
    lcd_dsi_write(DSI_VPN_TIMING_0, (hact << 16) | httl);
    lcd_dsi_write(DSI_VPN_TIMING_1, (hsync << 16) | hbp);
    lcd_dsi_write(DSI_VPN_TIMING_2, ((mipi_info->height)<<16) | (v_total));
    lcd_dsi_write(DSI_VPN_TIMING_3, ((mipi_info->vsync) << 16) | (mipi_info->vbp));

      /* SET UP LCD1 WORD COUNT REGISTERS FOR DSI BUS */
    lcd_dsi_write(DSI_VPN_WC_0, (hbp_wc << 16) | hsa_wc);
    lcd_dsi_write(DSI_VPN_WC_1, (hfp_wc << 16) | hact_wc);
    lcd_dsi_write(DSI_VPN_WC_2, (hex_wc << 16) | hlp_wc);

    lcd_dsi_write(DSI_VPN_SLOT_CNT_0, (slot_cnt0 << 16) | slot_cnt0);
    lcd_dsi_write(DSI_VPN_SLOT_CNT_1, (slot_cnt1 << 16) | slot_cnt1);

      /* Configure LCD control register 1 FOR DSI BUS */
    reg = adv_setting->vsync_rst_en << CFG_VPN_VSYNC_RST_EN_SHIFT |
        adv_setting->auto_wc_dis << CFG_VPN_AUTO_WC_DIS_SHIFT |
        adv_setting->hact_wc_en << CFG_VPN_HACT_WC_EN_SHIFT |
        adv_setting->timing_check_dis << CFG_VPN_TIMING_CHECK_DIS_SHIFT |
        adv_setting->auto_dly_dis << CFG_VPN_AUTO_DLY_DIS_SHIFT |
        adv_setting->hlp_pkt_en << CFG_VPN_HLP_PKT_EN_SHIFT |
        adv_setting->hex_pkt_en << CFG_VPN_HEX_PKT_EN_SHIFT |
        adv_setting->hfp_pkt_en << CFG_VPN_HFP_PKT_EN_SHIFT |
        adv_setting->hbp_pkt_en << CFG_VPN_HBP_PKT_EN_SHIFT |
        adv_setting->hse_pkt_en << CFG_VPN_HSE_PKT_EN_SHIFT |
        adv_setting->hsa_pkt_en << CFG_VPN_HSA_PKT_EN_SHIFT |
        adv_setting->hex_slot_en<< CFG_VPN_HEX_SLOT_EN_SHIFT |
        adv_setting->last_line_turn << CFG_VPN_LAST_LINE_TURN_SHIFT |
        adv_setting->lpm_frame_en << CFG_VPN_LPM_FRAME_EN_SHIFT |
        mipi_info->burst_mode << CFG_VPN_BURST_MODE_SHIFT |
        mipi_info->rgb_mode << CFG_VPN_RGB_TYPE_SHIFT;
    lcd_dsi_write(DSI_VPN_CTRL_1,reg);

    lcd_dsi_write_bits(DSI_LCD_BDG_CTRL0, 0, CFG_VPN_FIFO_AFULL_CNT_MASK,
      CFG_VPN_FIFO_AFULL_CNT_SHIT);
#ifdef LCD_ESD_SUPPORT
    lcd_dsi_set_bits(DSI_LCD_BDG_CTRL0, CFG_VPN_FIFO_AFULL_BYPASS);
#else
    lcd_dsi_clear_bits(DSI_LCD_BDG_CTRL0, CFG_VPN_FIFO_AFULL_BYPASS);
#endif
    lcd_dsi_set_bits(DSI_LCD_BDG_CTRL0, CFG_PIXEL_SWAP);

    dsi_enable_cmd_mode(FALSE);
    dsi_enable_video_mode(TRUE);

    dsi_set_vvc(0);

    /*crane*/
    hal_lcdc_clear_bits(LCD_SMPN_CTRL, BIT_0);
//  lcd_set_bits(DUMB_CONTROL, BIT_0);
//    lcdc_set_bits(LCD_MISC_CTRL, BIT_1);//should be set, otherwise color will be error
}

static void dsi_config_cmd_mode(struct s_dsi_ctx *dsi_ctx)
{
    int reg;
    int rgb_mode, bpp;
    struct s_mipi_info *mipi_info = &dsi_ctx->mipi_info;

    //dump_mipi_info(mipi_info);

    switch(mipi_info -> rgb_mode) {
    case DSI_INPUT_DATA_RGB_MODE_565:
        bpp = 16;
        rgb_mode = 6;
        break;
    case DSI_INPUT_DATA_RGB_MODE_666UNPACKET:
        bpp = 18;
        rgb_mode = 1;
        break;
    case DSI_INPUT_DATA_RGB_MODE_888:
        bpp = 24;
        rgb_mode = 0;
        break;
    default:
        printf("dsi_config_cmd_mode :unsupported rgb format!\n");
        bpp = 24;
        rgb_mode = 0;
    }

    reg = mipi_info->te_enable << CFG_CPN_TE_EN_SHIFT |
        rgb_mode << CFG_CPN_RGB_TYPE_SHIFT |
        1 << CFG_CPN_BURST_MODE_SHIFT |
        0 << CFG_CPN_DMA_DIS_SHIFT |
        0 << CFG_CPN_ADDR0_EN_SHIFT;
    lcd_dsi_write(DSI_CPN_CMD, reg);
    reg = ((mipi_info->width * bpp / 8 + 1 ) << CFG_CPN_PKT_CNT_SHIFT) |
      (DSI_MAX_CMD_FIFO_BYTES << CFG_CPN_FIFO_FULL_LEVEL_SHIFT);
    lcd_dsi_write(DSI_CPN_CTRL_1,reg);

    lcd_dsi_write_bits(DSI_LCD_BDG_CTRL0, mipi_info->te_pol, CFG_CPN_TE_EDGE_MASK,
      CFG_CPN_TE_EDGE_SHIFT);
    lcd_dsi_write_bits(DSI_LCD_BDG_CTRL0, mipi_info->vsync_pol,  CFG_CPN_VSYNC_EDGE_MASK,
      CFG_CPN_VSYNC_EDGE_SHIFT);
    lcd_dsi_write_bits(DSI_LCD_BDG_CTRL0, mipi_info->te_mode, CFG_CPN_TE_MODE_MASK,
      CFG_CPN_TE_MODE_SHIFT);

    reg = 0x80 << CFG_CPN_TE_DLY_CNT_SHIFT |
        0 << CFG_CPN_TE_LINE_CNT_SHIFT;
    lcd_dsi_write(DSI_LCD_BDG_CTRL1, reg);

    dsi_set_cvc(0);

    dsi_enable_video_mode(FALSE);
    dsi_enable_cmd_mode(TRUE);

    /*crane*/
    hal_lcdc_set_bits(LCD_SMPN_CTRL, BIT_0);
    hal_lcdc_clear_bits(LCD_DUMB_CONTROL, BIT_0);
    hal_lcdc_set_bits(LCD_MISC_CTRL, BIT_1);//should be set, otherwise color will be error
}

static int dsi_write_cmd_array(struct s_dsi_ctx *dsi_ctx,
                  struct s_dsi_cmd_desc *cmds,int count)
{
    struct s_dsi_cmd_desc cmd_line;
    uint8_t type, parameter[DSI_MAX_TX_FIFO_BYTES], len;
    uint32_t crc, loop;
    int ret = 0;

    //printf("%d cmd will be write \n", count);

    if(NULL == dsi_ctx) {
        printf("dsi_write_cmd_array : Invalid param\n");
        return -1;
    }

    for (loop = 0; loop < count; loop++) {
        cmd_line = cmds[loop];
        type = cmd_line.cmd_type;
        len = cmd_line.length;
        memset(parameter, 0x00, len + 6);
        parameter[0] = type & 0xff;
        switch (type) {
        case DSI_DCS_SWRITE:
        case DSI_DCS_SWRITE1:
        case DSI_DCS_READ:
        case DSI_GENERIC_READ1:
        case DSI_SET_MAX_PKT_SIZE:
            memcpy(&parameter[1], cmd_line.data, len);
            len = 4;
            break;
        case DSI_GENERIC_LWRITE:
        case DSI_DCS_LWRITE:
            parameter[1] = len & 0xff;
            parameter[2] = 0;
            memcpy(&parameter[4], cmd_line.data, len);
            crc = calculate_crc16(&parameter[4], len);
            parameter[len + 4] = crc & 0xff;
            parameter[len + 5] = (crc >> 8) & 0xff;
            len += 6;
            break;
        default:
            printf("data type not supported 0x%8x\n",type);
            break;
        }

        parameter[3] = calculate_ecc(parameter);

        /* send dsi commands */
        ret = dsi_write_cmd(parameter, len, cmd_line.lp);
        if(ret)
            return -1;

        if (0 != cmd_line.delay){
            mdelay(cmd_line.delay);
        }
    }
    return 0;
}

static int dsi_read_cmd_array(struct s_dsi_ctx *dsi_ctx, struct s_dsi_rx_buf *dbuf,
          struct s_dsi_cmd_desc *cmds, int count)
{
    uint8_t parameter[DSI_MAX_RX_FIFO_BYTES];
    uint32_t i, rx_reg, timeout, tmp, packet,
        data_pointer, byte_count;

    //printf("%d cmds will be write\n", count);

    if(NULL == dsi_ctx) {
        printf("Invalid param\n");
        return -1;
    }

    memset(dbuf, 0x0, sizeof(struct s_dsi_rx_buf));
    dsi_write_cmd_array(dsi_ctx, cmds, count);

    timeout = 1000;
    do {
        timeout--;
        tmp = lcd_dsi_read(DSI_IRQ_ST);
    } while (((tmp & IRQ_RX_PKT) == 0) && timeout);
    if (0 == timeout) {
        printf("dsi didn't receive packet, irq status 0x%x\n", tmp);
        return -1;
    }

    if (tmp & IRQ_RX_TRG3)
        printf("not defined package is received\n");
    if (tmp & IRQ_RX_TRG2)
        printf("ACK package is received\n");
    if (tmp & IRQ_RX_TRG1)
        printf("TE trigger is received\n");
    if (tmp & IRQ_RX_ERR) {
        tmp = lcd_dsi_read(DSI_RX_PKT_HDR_0);
        printf("ACK with error report (0x%x)", tmp);
    }

    packet = lcd_dsi_read(DSI_RX_PKT_ST_0);

    data_pointer = (packet & CFG_RX_PKT0_PTR_MASK) >> CFG_RX_PKT0_PTR_SHIFT;
    tmp = lcd_dsi_read(DSI_RX_PKT_CTRL_1);
    byte_count = tmp & CFG_RX_PKT_BCNT_MASK;

    memset(parameter, 0x00, byte_count);
    for (i = data_pointer; i < data_pointer + byte_count; i++) {
        rx_reg = lcd_dsi_read(DSI_RX_PKT_CTRL);
        rx_reg &= ~CFG_RX_PKT_RD_PTR_MASK;
        rx_reg |= CFG_RX_PKT_RD_REQ | (i << CFG_RX_PKT_RD_PTR_SHIFT);
        lcd_dsi_write(DSI_RX_PKT_CTRL, rx_reg);
        count = 10000;
        do {
          count--;
          rx_reg = lcd_dsi_read(DSI_RX_PKT_CTRL);
        } while (rx_reg & CFG_RX_PKT_RD_REQ && count);
        if ( 0 == count)
          printf("read Rx packet FIFO error\n");
        parameter[i - data_pointer] = rx_reg & 0xff;
    }
    switch (parameter[0]) {
    case DSI_ACK_ERR_RESP:
        printf("Acknowledge with error report\n");
        break;
    case DSI_EOTP:
        printf("End of Transmission packet\n");
        break;
    case DSI_GEN_READ1_RESP:
    case DSI_DCS_READ1_RESP:
        dbuf->data_type = parameter[0];
        dbuf->length = 1;
        memcpy(dbuf->data, &parameter[1], dbuf->length);
        break;
    case DSI_GEN_READ2_RESP:
    case DSI_DCS_READ2_RESP:
        dbuf->data_type = parameter[0];
        dbuf->length = 2;
        memcpy(dbuf->data, &parameter[1], dbuf->length);
        break;
    case DSI_GEN_LREAD_RESP:
    case DSI_DCS_LREAD_RESP:
        dbuf->data_type = parameter[0];
        dbuf->length = (parameter[2] << 8) | parameter[1];
        memcpy(dbuf->data, &parameter[4], dbuf->length);
        printf("read %d data: 0x%x, 0x%x, 0x%x, 0x%x, 0x%x\n", dbuf->length,
          parameter[4], parameter[5], parameter[6], parameter[7], parameter[8]);
        break;
    }
    return 0;
}

static void dsi_enable_dphy(struct s_dphy_ctx *dphy_config)
{
    lcd_dphy_init(dphy_config);
}

static void dsi_open_dphy(struct s_dsi_ctx* dsi_ctx, uint32_t half_clk)
{
    struct s_dphy_ctx *dphy_config = NULL;

    dsi_get_default_dphy_setting(dsi_ctx);
    dphy_config = &dsi_ctx->dphy_config;
    dphy_config->base_addr = dsi_ctx->base_addr;
    dphy_config->phy_freq = dsi_ctx->bit_clk_rate;
    dphy_config->esc_clk = dsi_ctx->esc_clk_rate;
    dphy_config->half_clk = half_clk;
    dphy_config->clk_src = DPHY_BIT_CLK_SRC_PLL5;/*CraneG_A0 hardcode the clk src*/
    if(dsi_ctx->mipi_info.split_enable) {
      dphy_config->lane_num = dsi_ctx->mipi_info.lane_number >> 1;
    } else {
      dphy_config->lane_num = dsi_ctx->mipi_info.lane_number;
    }
    printf("dphy_lane_num:%d", dphy_config->lane_num);
    dphy_config->status = DPHY_STATUS_UNINIT;

#if 0
    if(ready){
      dphy_config->status = DPHY_STATUS_INIT;
      return;
    }
#endif
    dsi_enable_dphy(dphy_config);
}

static void dsi_enable(struct s_dsi_ctx* dsi_ctx)
{
    int lane_number;

    dsi_enable_split_mode(dsi_ctx->mipi_info.split_enable);
    if(dsi_ctx->mipi_info.split_enable)
      lane_number = dsi_ctx->mipi_info.lane_number >> 1;
    else
      lane_number = dsi_ctx->mipi_info.lane_number;  
    printf("lane_num:%d\r\n",lane_number);
    dsi_enable_lptx_lanes(asr_dsi_lane[lane_number]);
    dsi_enable_eotp(dsi_ctx->mipi_info.eotp_enable);
}

static void dsi_close_dphy(struct s_dsi_ctx* dsi_ctx)
{
    lcd_dphy_uninit(&dsi_ctx->dphy_config);
}

static void dsi_ready_dphy(struct s_dsi_ctx* dsi_ctx)
{
}

static void dsi_isr_handler(uint32_t irq_num)
{
    uint32_t reg;

    reg = lcd_dsi_read(DSI_IRQ_ST);
    if(0 != (reg & (IRQ_PHY_FIFO_UNDERRUN |IRQ_REQ_CNT_ERR | IRQ_VPN_REQ_PHY_DLY_ERR |
      IRQ_VPN_BF_UNDERRUN_ERR | IRQ_VPN_REQ_ARB_DLY_ERR | IRQ_VPN_BF_OVERRUN_ERR |
      IRQ_VPN_TIMING_ERR | IRQ_VPN_BF_FULL | IRQ_CPN_BF_FULL))){
      printf("DSI error: 0x%x!\n", reg);
    }
}

static void dsi_irq_config(void)
{
    lcd_dsi_write(DSI_IRQ_MASK, IRQ_PHY_FIFO_UNDERRUN |IRQ_REQ_CNT_ERR | IRQ_VPN_REQ_PHY_DLY_ERR |
      IRQ_VPN_BF_UNDERRUN_ERR | IRQ_VPN_REQ_ARB_DLY_ERR | IRQ_VPN_BF_OVERRUN_ERR |
      IRQ_VPN_TIMING_ERR | IRQ_VPN_BF_FULL | IRQ_CPN_BF_FULL);
}

static void dsi_irq_en(void)
{
    dsi_irq_config();

    register_int_handler(DSI_IRQ, (int_handler)dsi_isr_handler, NULL);
    unmask_interrupt(DSI_IRQ);
}

static void dsi_irq_dis(void)
{
    lcd_dsi_write(DSI_IRQ_MASK, 0);
    mask_interrupt(DSI_IRQ);
}

static void dphy_poweron(void)
{
    //set ldo11 to 1.2v,
    //set ldo6 to 1.8v.
    //open vbuck 1 to 1v05
    lcd_ldo11_1v2_on();
}

static void dphy_poweroff(void)
{
    //set ldo11 to 1.2v,
    //set ldo6 to 1.8v.
    //open vbuck 1 to 1v05
    lcd_ldo11_1v2_off();
}

static void lcd_dsi_poweron(void)
{
    /*ESC_Clk = 52M*/
    __set_escclk(0);
    /*BIT_CLK = 819M as default*/
    __set_bitclk(0, 0);
    dphy_poweron();
}

static void lcd_dsi_poweroff(void)
{
    pmu_write(PMU_LCD_CLK_RES_CTRL1, 0);
    dphy_poweroff();
}

/*return value: real bitclk*/
uint32_t lcd_dsi_set_bitclk(uint32_t bit_clk /*KHz*/, uint32_t *half_clk)
{
#if USE_CRANE_LCD_MIPI_CMD
    uint32_t parent[8] = {819000,614000,491000,350000,0,0,1033000,688000}; /*KHz*/
    uint32_t real_clk = 0, diff = 0;
    int i, j;
    int best_id = -1, best_div = -1, best_diff = 0xFFFFFFFF, best_half = -1, best_real_clk = 0;
  //  uint32_t reg;

    if(NULL == half_clk) {
        printf("dsi_set_bitclk : Invalid param! \n");
        return 0;
    }
#if 0
    if(ready){
      reg = LCD_BU_REG_READ(PMU_BASE_ADDR + PMU_LCD_CLK_RES_CTRL1);
      best_id = (reg >> 20) & 0x3;
      best_div = (reg >> 17) & 0x7;
      reg = LCD_BU_REG_READ(DSI_BASE_ADDR + DSI_PHY_ANA_CTRL1);
      best_half = (reg >> 11) & 0x1;
      best_real_clk = parent[best_id] / (best_div + 1);
      if(best_half)
        best_real_clk = best_real_clk / 2;
      *half_clk = best_half;
      LCDLOGI("got real bitclk:%d. (parent: %d, div:%d, half:%d)",
        best_real_clk, parent[best_id], best_div + 1, best_half);
      return best_real_clk;
    }
#endif

    for(i = 0; i < 8; i++){
        if(0 == parent[i])
          continue;

        for(j = 1; j <= 8; j++) {
            real_clk = parent[i] / j;
            if(real_clk == bit_clk) {
                __set_bitclk(i, j - 1);
                *half_clk = 0;
                return real_clk;
            } else if(real_clk < bit_clk) {
                diff = bit_clk - real_clk;
            } else {
                diff = real_clk - bit_clk;
            }

            if(diff < best_diff) {
                best_id = i;
                best_div = j;
                best_diff = diff;
                best_half = 0;
                best_real_clk = real_clk;
            }

            real_clk = real_clk / 2;
            if(real_clk == bit_clk) {
                __set_bitclk(i, j - 1);
                *half_clk = 1;
                return real_clk;
            } else if(real_clk < bit_clk) {
                diff = bit_clk - real_clk;
            } else {
                diff = real_clk - bit_clk;
            }

            if(diff < best_diff) {
                best_id = i;
                best_div = j;
                best_diff = diff;
                best_half = 1;
                best_real_clk = real_clk;
            }
        }
    }

    printf("need bitclk:%d, real bitclk:%d. (parent: %d, div:%d, half:%d) \n",bit_clk,
      best_real_clk, parent[best_id], best_div, best_half);
    __set_bitclk(best_id, best_div-1);
    *half_clk = best_half;
    return best_real_clk;
#else  /* video mode */
    extern void lcd_phy_pll2_fc(unsigned pll2_vco);
    extern unsigned get_pll2_vco_freq(void);

    //#define HARD_DIVIDOR 4 //st7796s mipi video mode
    #define HARD_DIVIDOR 2 //ili9806e mipi video mode

    uint32_t reg;
    uint32_t pll_vco = bit_clk * 2 * HARD_DIVIDOR / 1000;
    uint32_t real_clk;
    lcd_phy_pll2_fc(pll_vco);
    reg = get_pll2_vco_freq();
    printf("pll2 set = %d, get = %d\n", pll_vco, reg);
    __set_bitclk(6, HARD_DIVIDOR - 1);
    *half_clk = 0;
    real_clk = reg * 1000 / 2 / HARD_DIVIDOR;
    printf("need bitclk:%d, real bitclk:%d. (parent: %d, div:%d, half:%d)\n",bit_clk,
           real_clk, pll_vco/2, HARD_DIVIDOR, 0);

    return real_clk;
#endif
}

int lcd_dsi_init(const struct s_mipi_info *mipi_info, int32_t work_mode)
{
    struct s_dsi_ctx* dsi_ctx = &g_dsi_ctx;
    uint32_t half_clk = 0;

    if(NULL == mipi_info){
        printf("lcd_dsi_init : Invalid param\n");
        return -1;
    }

    /*used for assert mode*/
    if(DSI_STATUS_UNINIT != dsi_ctx->status){
        printf("lcd_dsi_init : dsi has been initialized! \n");
        return 0;
    }

    lcd_dsi_update(work_mode);

    dsi_ctx->work_mode = work_mode;
    lcd_dsi_poweron();

    dsi_ctx->base_addr = DSI_BASE_ADDR;
    memcpy(&dsi_ctx->mipi_info, mipi_info, sizeof(dsi_ctx->mipi_info));

    dsi_get_default_advanced_setting(dsi_ctx);
    dsi_ctx->bit_clk_rate = lcd_dsi_set_bitclk(mipi_info->phy_freq, &half_clk);
    dsi_ctx->esc_clk_rate = lcd_dsi_set_escclk(DSI_ESC_CLK_DEFAULT);

  //  if(!ready)
  //    lcd_dsi_reset();
      dsi_open_dphy(dsi_ctx, half_clk);
  //  if(!ready) {
      dsi_enable(dsi_ctx);
  //  }

    dsi_ctx->status = DSI_STATUS_INIT;
    return 0;
}

int lcd_dsi_uninit(void)
{
    struct s_dsi_ctx* dsi_ctx = &g_dsi_ctx;

    if(DSI_STATUS_INIT != dsi_ctx->status){
      printf("Invalid dsi status\n");
      return -1;
    }

    dsi_close_dphy(dsi_ctx);

    lcd_dsi_poweroff();

    dsi_ctx->status = DSI_STATUS_UNINIT;
    return 0;
}

void lcd_dsi_reset(void)
{
    struct s_dsi_ctx* dsi_ctx = &g_dsi_ctx;

    dsi_enable(dsi_ctx);
    dsi_enable_dphy(&dsi_ctx->dphy_config);
    lcd_dphy_reset(&dsi_ctx->dphy_config);
}

int lcd_dsi_update(int32_t work_mode)
{
    printf("lcd_dsi_update, workmode: %d \n", work_mode);

    struct s_dsi_ctx* dsi_ctx = &g_dsi_ctx;
    if(work_mode != dsi_ctx->work_mode){
      if(1 == dsi_ctx->work_mode) {
        dsi_irq_dis();
        dsi_ctx->work_mode = work_mode;
        /*In assert mode, must use polling mode*/
      } else {
        dsi_ctx->work_mode = work_mode;
        dsi_irq_en();
      }
    }
    return 0;
}

int lcd_dsi_update_mipi_info(uint16_t width, uint16_t height)
{
    struct s_dsi_ctx* dsi_ctx = &g_dsi_ctx;

    if(DSI_STATUS_INIT != dsi_ctx->status){
      printf("Invalid dsi status");
      return -1;
    }
    dsi_ctx->mipi_info.width = width;
    dsi_ctx->mipi_info.height = height;

    return 0;
}

int lcd_dsi_before_refresh(void)
{
    struct s_dsi_ctx* dsi_ctx = &g_dsi_ctx;
    if(DSI_STATUS_INIT != dsi_ctx->status){
      printf("Invalid dsi status \n");
      return -1;
    }
    if(dsi_ctx->mipi_info.work_mode == DSI_MODE_CMD){
        dsi_config_cmd_mode(dsi_ctx);
    } else {
        dsi_config_video_mode(dsi_ctx);
    }

    dsi_ready_dphy(dsi_ctx);
    return 0;
}
#if 0
int asr_dsi_suspend(void)
{
    struct s_dsi_ctx* dsi_ctx = &g_dsi_ctx;

    if(DSI_STATUS_INIT != dsi_ctx->status){
      printf("asr_dsi_suspend : Invalid dsi status \n");
      return -1;
    }

    dsi_close_dphy(dsi_ctx);

    return 0;
}

int asr_dsi_resume(void)
{
    struct s_dsi_ctx* dsi_ctx = &g_dsi_ctx;

    if(DSI_STATUS_INIT != dsi_ctx->status){
      printf("asr_dsi_resume : Invalid dsi status \n");
      return -1;
    }

    return 0;
}
#endif

void lcd_dump_dsi_registers(void)
{
    int i;
    for(i=0; i<0x1f0; i+=16){
        printf("dsi (0x%x): 0x%x, 0x%x, 0x%x, 0x%x \n",
          i, lcd_dsi_read(i),
          lcd_dsi_read(i+4),
          lcd_dsi_read(i+8),
          lcd_dsi_read(i+12));
    }
}

int lcd_dsi_write_cmds(struct s_dsi_cmd_desc *cmds, int count)
{
    struct s_dsi_ctx* dsi_ctx = &g_dsi_ctx;

    if((NULL == dsi_ctx) || (NULL == cmds)){
      printf("lcd_dsi_write_cmds : Invalid param \n");
      return -1;
    }

    if(DSI_STATUS_INIT != dsi_ctx->status){
      printf("lcd_dsi_write_cmds : Invalid dsi status \n");
      return -1;
    }

    return dsi_write_cmd_array(dsi_ctx, cmds, count);
}

int lcd_dsi_after_refresh(void)
{
    struct s_dsi_ctx* dsi_ctx = &g_dsi_ctx;

    if(DSI_STATUS_INIT != dsi_ctx->status){
      printf("err : lcd_dsi_after_refresh : Invalid dsi status\n");
      return -1;
    }

    dsi_enable_cmd_mode(FALSE);
    dsi_enable_video_mode(FALSE);

    return 0;
}

int lcd_dsi_read_cmds(struct s_dsi_rx_buf *dbuf,
                struct s_dsi_cmd_desc *cmds, int count)
{
    struct s_dsi_ctx* dsi_ctx = &g_dsi_ctx;

    if(NULL == cmds) {
      printf("lcd_dsi_read_cmds : Invalid param \n");
      return -1;
    }

    if(DSI_STATUS_INIT != dsi_ctx->status) {
      printf("lcd_dsi_read_cmds : Invalid dsi status \n");
      return -1;
    }

    return dsi_read_cmd_array(dsi_ctx, dbuf, cmds, count);
}

#if 0
int asr_dsi_test_mode(int test_mode)
{
    struct s_dsi_ctx* dsi_ctx = &g_dsi_ctx;

    if(DSI_STATUS_INIT != dsi_ctx->status){
      printf("asr_dsi_test_mode : Invalid dsi status \n");
      return -1;
    }

    if(test_mode) {
      lcd_dphy_enter_test_mode(&dsi_ctx->dphy_config, FALSE, DPHY_TEST_PATTERN_11);
    } else {
      lcd_dphy_exit_test_mode(&dsi_ctx->dphy_config);
    }

    return 0;
}
#endif

#endif
