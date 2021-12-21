
#include "hal_drv_conf.h"
#include "halsoc_lcd_mipi_dphy.h"

#if USE_CRANE_LCD_MIPI
#include "bsp_common.h"
#include "halsoc_lcd_mipi_dsi.h"

static unsigned int asr_dphy_lane[5] = {0, 0x1, 0x3, 0x7, 0xf};

extern void mdelay(int ms);


static void dphy_ana_reset(void)
{
    lcd_dsi_clear_bits(DSI_PHY_ANA_PWR_CTRL, CFG_DPHY_ANA_RESET);
    mdelay(5);
    lcd_dsi_set_bits(DSI_PHY_ANA_PWR_CTRL, CFG_DPHY_ANA_RESET);
}

static void dphy_set_power(int poweron)
{
    if(poweron) {
        lcd_dsi_set_bits(DSI_PHY_ANA_PWR_CTRL, CFG_DPHY_ANA_PU);
        lcd_dsi_set_bits(DSI_PHY_CTRL_1, CFG_DPHY_ADD_VALID | CFG_DPHY_VDD_VALID);
    } else {
        lcd_dsi_clear_bits(DSI_PHY_ANA_PWR_CTRL, CFG_DPHY_ANA_PU);
        lcd_dsi_clear_bits(DSI_PHY_CTRL_1, CFG_DPHY_ADD_VALID | CFG_DPHY_VDD_VALID);
    }
}

#if 0
static void dphy_set_ulps(bool ulps)
{
  if(ulps)
    lcd_dsi_set_bits(DSI_PHY_CTRL_1,
       CFG_DPHY_ULPS_DATA | CFG_DPHY_ULPS_CLK);
  else
    lcd_dsi_clear_bits(DSI_PHY_CTRL_1,
      CFG_DPHY_ULPS_DATA | CFG_DPHY_ULPS_CLK);
}
#endif

static void dphy_set_cont_clk(int cont_clk)
{
    if(cont_clk) {
        lcd_dsi_set_bits(DSI_PHY_CTRL_1, CFG_DPHY_CONT_CLK);
    } else {
        lcd_dsi_clear_bits(DSI_PHY_CTRL_1, CFG_DPHY_CONT_CLK);
    }
}

#if 0
static void dphy_set_lane_map(uint32_t map)
{
  if(map >= DPHY_LANE_MAP_MAX) {
    LCDLOGE("Invalid lane map (%d)", map);
    return;
  }

  lcd_dsi_write_bits(DSI_PHY_CTRL_2, map,
    CFG_DPHY_LANE_MAP_MASK,  CFG_DPHY_LANE_MAP_SHIFT);
}
#endif

static void dphy_set_lane_num(uint32_t lane_num)
{
    printf("lane_num: (%d, %d)", lane_num, asr_dphy_lane[lane_num]);
    lcd_dsi_write_bits(DSI_PHY_CTRL_2, asr_dphy_lane[lane_num], CFG_DPHY_LANE_EN_MASK,  CFG_DPHY_LANE_EN_SHIFT);
    //printf("0x188 = 0x%x", lcd_dsi_read(DSI_PHY_CTRL_2));
}

static void dphy_set_bit_clk_src(uint32_t bit_clk_src,
  uint32_t half_clk)
{
    printf("bit_clk_src: %d, half_clk:%d", bit_clk_src, half_clk);
    if(bit_clk_src >= DPHY_BIT_CLK_SRC_LIMIT) {
        printf("Invalid bit clk src (%d)", bit_clk_src);
        return;
    }

    if(bit_clk_src == DPHY_BIT_CLK_SRC_MUX){
        printf("bit_clk_src = DPHY_BIT_CLK_SRC_MUX");
        lcd_dsi_set_bits(DSI_PHY_ANA_CTRL1, CFG_CLK_SEL);
    } else {
        printf("bit_clk_src = DPHY_BIT_CLK_SRC_PLL5");
        lcd_dsi_clear_bits(DSI_PHY_ANA_CTRL1, CFG_CLK_SEL);
    }

    if(1 == half_clk){
        printf("half_clk = 1");
        lcd_dsi_set_bits(DSI_PHY_ANA_CTRL1, CFG_CLK_DIV2);
    } else {
        printf("half_clk = 0");
        lcd_dsi_clear_bits(DSI_PHY_ANA_CTRL1, CFG_CLK_DIV2);
    }
}

#if 0
static void dphy_force_bta(bool bta)
{
  if(bta)
    lcd_dsi_set_bits(DSI_PHY_CTRL_2, CFG_DPHY_FORCE_BTA);
  else
    lcd_dsi_clear_bits(DSI_PHY_CTRL_2, CFG_DPHY_FORCE_BTA);
}
#endif

static void dphy_set_timing(struct s_dphy_ctx *dphy_ctx)
{
    uint32_t bitclk;
    int ui, lpx_clk, lpx_time, ta_get, ta_go, wakeup, reg;
    int hs_prep, hs_zero, hs_trail, hs_exit, ck_zero, ck_trail, ck_exit;
    int esc_clk, esc_clk_t;
    struct s_dphy_timing *phy_timing;

    if(NULL == dphy_ctx) {
        printf("dphy_set_timing : Invalid param! \n");
        return;
    }

    phy_timing = &(dphy_ctx->dphy_timing);

    esc_clk = dphy_ctx->esc_clk/1000;
    esc_clk_t = 1000/esc_clk;
    printf("esc_clk = %d, esc_clk_t = %d\n",esc_clk, esc_clk_t);

    bitclk = dphy_ctx->phy_freq / 1000;
    ui = 1000/bitclk + 1;
    printf("bitclk = %d, ui = %d\n",bitclk, ui);

    /*Jessica: Why +1?, should be -1?*/
    lpx_clk = (phy_timing->lpx_constant + phy_timing->lpx_ui * ui) / esc_clk_t + 1;
    lpx_time = lpx_clk * esc_clk_t;
    printf("lpx_time: %d \n", lpx_time);
    /* Below is for NT35451 */
    ta_get = lpx_time * 5 / esc_clk_t - 1;
    ta_go = lpx_time * 4 / esc_clk_t - 1;

    /*Jessica: Why no wakeup_ui?*/
    wakeup = phy_timing->wakeup_constant;
    wakeup = wakeup / esc_clk_t + 1;

    hs_prep = phy_timing->hs_prep_constant + phy_timing->hs_prep_ui * ui;
    printf("hs_prep: %d \n", hs_prep);
    hs_prep = hs_prep / esc_clk_t + 1;

    /* Our hardware added 3-byte clk automatically.
     * 3-byte 3 * 8 * ui.
     */
    hs_zero = phy_timing->hs_zero_constant + phy_timing->hs_zero_ui * ui -
      (hs_prep + 1) * esc_clk_t;
    printf("hs_zero: %d \n", hs_zero);
    hs_zero = (hs_zero - (3 * ui << 3)) / esc_clk_t + 4;
    if (hs_zero < 0) {
        hs_zero = 0;
    }

    hs_trail = phy_timing->hs_trail_constant + phy_timing->hs_trail_ui * ui;
    hs_trail = ((8 * ui) >= hs_trail) ? (8 * ui) : hs_trail;
    hs_trail = hs_trail / esc_clk_t + 1;
    if (hs_trail > 3) {
        hs_trail -= 3;
    } else {
        hs_trail = 0;
    }

    hs_exit = phy_timing->hs_exit_constant + phy_timing->hs_exit_ui * ui;
    hs_exit = hs_exit / esc_clk_t + 1;

    ck_zero = phy_timing->ck_zero_constant + phy_timing->ck_zero_ui * ui -
      (hs_prep + 1) * esc_clk_t;
    ck_zero = ck_zero / esc_clk_t + 1;

    ck_trail = phy_timing->ck_trail_constant + phy_timing->ck_trail_ui * ui;
    ck_trail = ck_trail / esc_clk_t + 1;

    ck_exit = hs_exit;

    reg = (hs_exit << CFG_DPHY_TIME_HS_EXIT_SHIFT)
      | (hs_trail << CFG_DPHY_TIME_HS_TRAIL_SHIFT)
      | (hs_zero << CFG_DPHY_TIME_HS_ZERO_SHIFT)
      | (hs_prep << CFG_DPHY_TIME_HS_PREP_SHIFT);
    printf("dphy timing 0 :0x%x \n", reg);
    lcd_dsi_write(DSI_PHY_TIME_0, reg);

    reg = (ta_get << CFG_DPHY_TIME_TA_GET_SHIFT)
      | (ta_go << CFG_DPHY_TIME_TA_GO_SHIFT)
      | (wakeup << CFG_DPHY_TIME_WAKEUP_SHIFT);
    printf("dphy timing 1 :0x%x \n", reg);
    lcd_dsi_write(DSI_PHY_TIME_1, reg);

    reg = (ck_exit << CFG_DPHY_TIME_CLK_EXIT_SHIFT)
      | (ck_trail << CFG_DPHY_TIME_CLK_TRAIL_SHIFT)
      | (ck_zero << CFG_DPHY_TIME_CLK_ZERO_SHIFT)
      | (lpx_clk << CFG_DPHY_TIME_CLK_LPX_SHIFT);
    printf("dphy timing 2 :0x%x \n", reg);
    lcd_dsi_write(DSI_PHY_TIME_2, reg);

    reg = (lpx_clk << CFG_DPHY_TIME_LPX_SHIFT)
      | phy_timing->req_ready << CFG_DPHY_TIME_REQRDY_SHIFT;
    printf("dphy timing 3 :0x%x \n", reg);
    lcd_dsi_write(DSI_PHY_TIME_3, reg);
    /* calculated timing on brownstone:
     * DSI_PHY_TIME_0 0x06080204
     * DSI_PHY_TIME_1 0x6d2bfff0
     * DSI_PHY_TIME_2 0x603130a
     * DSI_PHY_TIME_3 0xa3c
     */
}

#if 0
void dphy_get_status(struct s_dphy_ctx *dphy_ctx)
{
    if(NULL == dphy_ctx){
        printf("dphy_get_status : Invalid param \n");
        return;
    }

    dphy_ctx->dphy_status0 = lcd_dsi_read(DSI_PHY_STATUS_0);
    dphy_ctx->dphy_status1 = lcd_dsi_read(DSI_PHY_STATUS_1);
    dphy_ctx->dphy_status2 = lcd_dsi_read(DSI_PHY_STATUS_2);
    //printf("dphy_status0 = 0x%x \n", dphy_ctx->dphy_status0);
    //printf("dphy_status1 = 0x%x \n", dphy_ctx->dphy_status1);
    //printf("dphy_status2 = 0x%x \n", dphy_ctx->dphy_status2);
}
#endif
void lcd_dphy_reset(struct s_dphy_ctx *dphy_ctx)
{
    if(NULL == dphy_ctx){
        printf("lcd_dphy_reset : Invalid param \n");
        return;
    }

    dphy_ana_reset();
}

void lcd_dphy_enter_test_mode(struct s_dphy_ctx *dphy_ctx, int lp, uint32_t pattern)
{
    if(pattern >= DPHY_TEST_PATTERN_LIMIT){
        printf("lcd_dphy_enter_test_mode : Invalid param \n");
        return;
    }

    if(lp) {
        lcd_dsi_set_bits(DSI_PHY_ANA_CTRL1, DFG_SET_TEST_LP);
    } else {
        lcd_dsi_clear_bits(DSI_PHY_ANA_CTRL1, DFG_SET_TEST_LP);
    }

    lcd_dsi_write_bits(DSI_PHY_ANA_CTRL1, pattern, DFG_TEST_PATTERN_MASK,
      DFG_TEST_PATTERN_SHIFT);

    lcd_dsi_set_bits(DSI_PHY_ANA_CTRL1, CFG_SET_TEST);
    printf("0x%x = 0x%x \n", DSI_PHY_ANA_CTRL1, lcd_dsi_read(DSI_PHY_ANA_CTRL1));
}

void lcd_dphy_exit_test_mode(struct s_dphy_ctx *dphy_ctx)
{
    lcd_dsi_clear_bits(DSI_PHY_ANA_CTRL1, CFG_SET_TEST);
    printf("0x%x = 0x%x \n", DSI_PHY_ANA_CTRL1, lcd_dsi_read(DSI_PHY_ANA_CTRL1));
}

void lcd_dphy_init(struct s_dphy_ctx *dphy_ctx)
{
    if(NULL == dphy_ctx){
        printf("lcd_dphy_init : Invalid param\n");
        return;
    }

    if(DPHY_STATUS_UNINIT != dphy_ctx->status){
        printf("lcd_dphy_init : dphy has been initialized \n");
        return;
    }

    /*use DPHY_BIT_CLK_SRC_MUX as default clk src*/
    dphy_set_bit_clk_src(dphy_ctx->clk_src, dphy_ctx->half_clk);

    /* digital and analog power on */
    dphy_set_power(1);

    /* turn on DSI continuous clock for HS */
    dphy_set_cont_clk(1);

    /* set dphy */
    dphy_set_timing(dphy_ctx);

    /* enable data lanes */
    dphy_set_lane_num(dphy_ctx->lane_num);

    dphy_ctx->status = DPHY_STATUS_INIT;
}

void lcd_dphy_uninit(struct s_dphy_ctx *dphy_ctx)
{
    if(NULL == dphy_ctx) {
        printf("lcd_dphy_uninit : Invalid param \n");
        return;
    }

    if(DPHY_STATUS_INIT != dphy_ctx->status) {
        printf("lcd_dphy_uninit : dphy_ctx has not been initialized (%d)\n", dphy_ctx->status);
        return;
    }

    dphy_set_cont_clk(0);
    dphy_ana_reset();
    dphy_set_power(0);

    dphy_ctx->status = DPHY_STATUS_UNINIT;
}

#endif
