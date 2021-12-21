#ifndef _HALSOC_LCD_MIPI_DPHY_H_
#define _HALSOC_LCD_MIPI_DPHY_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "hal_drv_conf.h"

#if USE_CRANE_LCD_MIPI

enum {
    DPHY_LANE_MAP_0123 = 0,
    DPHY_LANE_MAP_0312 = 1,
    DPHY_LANE_MAP_0231 = 2,
    DPHY_LANE_MAP_LIMIT
};

enum {
    DPHY_STATUS_UNINIT = 0,
    DPHY_STATUS_INIT = 1,
    DPHY_STATUS_LIMIT
};

enum {
    DPHY_BIT_CLK_SRC_PLL5 = 1,
    DPHY_BIT_CLK_SRC_MUX = 2,
    DPHY_BIT_CLK_SRC_LIMIT
};

enum {
    DPHY_TEST_PATTERN_00 = 0,
    DPHY_TEST_PATTERN_11 = 1,
    DPHY_TEST_PATTERN_CLK = 2,
    DPHY_TEST_PATTERN_PRBS7 = 3,
    DPHY_TEST_PATTERN_LIMIT
};

struct s_dphy_timing {
    uint32_t hs_prep_constant;    /* Unit: ns. */
    uint32_t hs_prep_ui;
    uint32_t hs_zero_constant;
    uint32_t hs_zero_ui;
    uint32_t hs_trail_constant;
    uint32_t hs_trail_ui;
    uint32_t hs_exit_constant;
    uint32_t hs_exit_ui;
    uint32_t ck_zero_constant;
    uint32_t ck_zero_ui;
    uint32_t ck_trail_constant;
    uint32_t ck_trail_ui;
    uint32_t req_ready;
    uint32_t wakeup_constant;
    uint32_t wakeup_ui;
    uint32_t lpx_constant;
    uint32_t lpx_ui;
};

struct s_dphy_ctx {
    uint32_t base_addr;
    uint32_t phy_freq; /*kHz*/
    uint32_t lane_num;
    uint32_t esc_clk; /*kHz*/
    uint32_t half_clk;
    int clk_src;
    struct s_dphy_timing dphy_timing;
    uint32_t dphy_status0; /*status0 reg*/
    uint32_t dphy_status1; /*status1 reg*/
    uint32_t dphy_status2; /*status2 reg*/

    int status;
};

void lcd_dphy_init(struct s_dphy_ctx *dphy_ctx);
void lcd_dphy_reset(struct s_dphy_ctx *dphy_ctx);
void lcd_dphy_uninit(struct s_dphy_ctx *dphy_ctx);
void lcd_dphy_enter_test_mode(struct s_dphy_ctx *dphy_ctx, int lp, uint32_t pattern);
void lcd_dphy_exit_test_mode(struct s_dphy_ctx *dphy_ctx);

#endif /* USE_CRANE_LCD_MIPI */

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
