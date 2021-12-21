#ifndef _CRANE_LCD_MIPI_DSI_H_
#define _CRANE_LCD_MIPI_DSI_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "hal_drv_conf.h"

#if USE_CRANE_LCD_MIPI

#include <stdint.h>
//#include "../common/utils.h"
#include "halsoc_lcd_mipi_dphy.h"

/*--------------dsi regs---------------------*/
#define DSI_BASE_ADDR 0xD420A800
#define DSI_IRQ 51

#define DSI_CTRL_0 0x0
#define DSI_CTRL_1 0x4
#define DSI_IRQ_ST1  0x8
#define DSI_IRQ_MASK1 0xC
#define DSI_IRQ_ST 0x10
#define DSI_IRQ_MASK 0x14

#define DSI_CPU_CMD_0 0x20
#define DSI_CPU_CMD_1 0x24
#define DSI_CPU_CMD_3 0x2C
#define DSI_CPU_WDAT 0x30
#define DSI_CPU_STATUS_0 0x34
#define DSI_CPU_STATUS_1 0x38
#define DSI_CPU_STATUS_2 0x3C
#define DSI_CPU_STATUS_3 0x40
#define DSI_CPU_STATUS_4 0x44

#define DSI_CPN_STATUS_1 0x4C
#define DSI_CPN_CMD 0x50
#define DSI_CPN_CTRL_0 0x54
#define DSI_CPN_CTRL_1 0x58
#define DSI_CPN_STATUS_0 0x5C

#define DSI_RX_PKT_ST_0 0x60
#define DSI_RX_PKT_HDR_0 0x64
#define DSI_RX_PKT_ST_1 0x68
#define DSI_RX_PKT_HDR_1 0x6C
#define DSI_RX_PKT_CTRL 0x70
#define DSI_RX_PKT_CTRL_1 0x74
#define DSI_RX_PKT_ST_2 0x78
#define DSI_RX_PKT_HDR_2 0x7C

#define DSI_LCD_BDG_CTRL0 0x84
#define DSI_LCD_BDG_CTRL1 0x88

#define DSI_TX_TIMER 0xE4
#define DSI_RX_TIMER 0xE8
#define DSI_TURN_TIMER 0xEC

#define DSI_VPN_CTRL_0 0x100
#define DSI_VPN_CTRL_1 0x104
#define DSI_VPN_TIMING_0 0x110
#define DSI_VPN_TIMING_1 0x114
#define DSI_VPN_TIMING_2 0x118
#define DSI_VPN_TIMING_3 0x11C
#define DSI_VPN_WC_0 0x120
#define DSI_VPN_WC_1 0x124
#define DSI_VPN_WC_2 0x128
#define DSI_VPN_SLOT_CNT_0 0x130
#define DSI_VPN_SLOT_CNT_1 0x134
#define DSI_VPN_SYNC_CODE 0x138
#define DSI_VPN_STATUS_0 0x140
#define DSI_VPN_STATUS_1 0x144
#define DSI_VPN_STATUS_2 0x148
#define DSI_VPN_STATUS_3 0x14C
#define DSI_VPN_STATUS_4 0x150

#define DSI_PHY_CTRL_0 0x180
#define DSI_PHY_CTRL_1 0x184
#define DSI_PHY_CTRL_2 0x188
#define DSI_PHY_CTRL_3 0x18C
#define DSI_PHY_STATUS_0 0x190
#define DSI_PHY_STATUS_1 0x194
#define DSI_PHY_LPRX_0 0x198
#define DSI_PHY_LPRX_1 0x19C
#define DSI_PHY_LPTX_0 0x1A0
#define DSI_PHY_LPTX_1 0x1A4
#define DSI_PHY_LPTX_2 0x1A8
#define DSI_PHY_STATUS_2 0x1AC
#define DSI_PHY_TIME_0 0x1C0
#define DSI_PHY_TIME_1 0x1C4
#define DSI_PHY_TIME_2 0x1C8
#define DSI_PHY_TIME_3 0x1CC
#define DSI_PHY_CODE_0 0x1D0
#define DSI_PHY_CODE_1 0x1D4
#define DSI_PHY_ANA_PWR_CTRL 0x1E0
#define DSI_PHY_ANA_CTRL0 0x1E4
#define DSI_PHY_ANA_CTRL1 0x1E8

//DSI_CTRL_0 0x0
#define CFG_SOFT_RST BIT(31)
#define CFG_SOFT_RST_REG BIT(30)
#define CFG_CLR_PHY_FIFO BIT(29)
#define CFG_RST_TXLP BIT(28)
#define CFG_RST_CPU BIT(27)
#define CFG_RST_CPN BIT(26)
#define CFG_RST_VPN BIT(24)
#define CFG_DSI_PHY_RST BIT(23)
#define CFG_VPN_TX_EN BIT(8)
#define CFG_VPN_SLV BIT(4)
#define CFG_CPN_EN BIT(2)
#define CFG_VPN_EN BIT(0)

//DSI_CTRL_1 0x4
#define CFG_EOTP_EN BIT(8)
#define CFG_CPN_VCH_NO_SHIFT 6
#define CFG_VPN_VCH_NO_SHIFT 0


//DSI_IRQ_ST 0x10
#define IRQ_RX_ERR BIT(25)
#define IRQ_PHY_FIFO_UNDERRUN BIT(23)
#define IRQ_REQ_CNT_ERR BIT(22)
#define IRQ_VPN_REQ_PHY_DLY_ERR BIT(20)
#define IRQ_VPN_BF_UNDERRUN_ERR BIT(19)
#define IRQ_VPN_REQ_ARB_DLY_ERR BIT(18)
#define IRQ_VPN_BF_OVERRUN_ERR BIT(17)
#define IRQ_VPN_TIMING_ERR BIT(16)
#define IRQ_VPN_BF_FULL BIT(14)
#define IRQ_CPN_BF_FULL BIT(13)
#define IRQ_RX_TRG3 BIT(7)
#define IRQ_RX_TRG2 BIT(6)
#define IRQ_RX_TRG1 BIT(5)
#define IRQ_RX_TRG0 BIT(4)
#define IRQ_RX_PKT BIT(2)

//DSI_CPU_CMD_0 0x20
#define CFG_CPU_CMD_REQ BIT(31)
#define CFG_CPU_SP BIT(30)
#define CFG_CPU_TURN BIT(29)
#define CFG_CPU_TXLP BIT(27)
#define CFG_CPU_WC_SHIFT 0

//DSI_CPU_CMD_1 0x24
#define CFG_TXLP_LPDT_SHIFT 20

#define CFG_TXLP_LPDT_MASK (0xF)

//DSI_CPU_CMD_3 0x2C
#define CFG_CPU_DAT_REQ BIT(31)
#define CFG_CPU_DAT_RW BIT(30)
#define CFG_CPU_DAT_ADDR_SHIFT 16

//DSI_CPN_CMD 0x50
#define CFG_CPN_TE_EN_SHIFT 28
#define CFG_CPN_RGB_TYPE_SHIFT 24
#define CFG_CPN_BURST_MODE_SHIFT 3
#define CFG_CPN_FIRSTP_SEL_SHIFT 2
#define CFG_CPN_DMA_DIS_SHIFT 1
#define CFG_CPN_ADDR0_EN_SHIFT 0

//DSI_CPN_CTRL_1 0X58
#define CFG_CPN_PKT_CNT_SHIFT   16
#define CFG_CPN_FIFO_FULL_LEVEL_SHIFT 0

//DSI_RX_PKT_ST_0 0x60
#define CFG_RX_PKT0_PTR_SHIFT 16
#define CFG_RX_PKT0_PTR_MASK (0x3F << CFG_RX_PKT0_PTR_SHIFT)

//DSI_RX_PKT_CTRL 0x70
#define CFG_RX_PKT_RD_REQ BIT(31)
#define CFG_RX_PKT_RD_PTR_SHIFT 16
#define CFG_RX_PKT_RD_PTR_MASK (0x3F << CFG_RX_PKT_RD_PTR_SHIFT)
#define CFG_RX_PKT_RD_DATA_SHIFT 0
#define CFG_RX_PKT_RD_DATA_MASK (0x3F << CFG_RX_PKT_RD_DATA_SHIFT)

//DSI_RX_PKT_CTRL_1 0x74
#define CFG_RX_PKT_BCNT_SHIFT 0
#define CFG_RX_PKT_BCNT_MASK (0xff << CFG_RX_PKT_BCNT_SHIFT)

//DSI_LCD_BDG_CTRL0 0x84
#define CFG_VPN_FIFO_AFULL_CNT_SHIT 16
#define CFG_VPN_FIFO_AFULL_CNT_MASK (0xfff)
#define CFG_VPN_FIFO_AFULL_BYPASS BIT(6)
#define CFG_CPN_VSYNC_EDGE_SHIFT 5
#define CFG_CPN_VSYNC_EDGE_MASK (1)
#define CFG_CPN_TE_EDGE_SHIFT 4
#define CFG_CPN_TE_EDGE_MASK (1)
#define CFG_CPN_TE_MODE_SHIFT 2
#define CFG_CPN_TE_MODE_MASK (3)
#define CFG_PIXEL_SWAP BIT(1)
#define CFG_SPLIT_EN BIT(0)

//DSI_LCD_BDG_CTRL1 0x88
#define CFG_CPN_TE_DLY_CNT_SHIFT 16
#define CFG_CPN_TE_LINE_CNT_SHIFT 0

//DSI_VPN_CTRL_1 0x104
#define CFG_VPN_VSYNC_RST_EN_SHIFT 31
#define CFG_VPN_AUTO_WC_DIS_SHIFT 27
#define CFG_VPN_HACT_WC_EN_SHIFT 26
#define CFG_VPN_TIMING_CHECK_DIS_SHIFT 25
#define CFG_VPN_AUTO_DLY_DIS_SHIFT 24
#define CFG_VPN_HLP_PKT_EN_SHIFT 22
#define CFG_VPN_HEX_PKT_EN_SHIFT 21
#define CFG_VPN_HFP_PKT_EN_SHIFT 20
#define CFG_VPN_HBP_PKT_EN_SHIFT 18
#define CFG_VPN_HSE_PKT_EN_SHIFT 17
#define CFG_VPN_HSA_PKT_EN_SHIFT 16
#define CFG_VPN_HEX_SLOT_EN_SHIFT 14
#define CFG_VPN_LAST_LINE_TURN_SHIFT 10
#define CFG_VPN_LPM_FRAME_EN_SHIFT 9
#define CFG_VPN_BURST_MODE_SHIFT 2
#define CFG_VPN_BURST_MODE_MASK (0x3)
#define CFG_VPN_RGB_TYPE_SHIFT 0
#define CFG_VPN_RGB_TYPE_MASK (0x3)

//DSI_PHY_CTRL_1 0x184
#define CFG_DPHY_ADD_VALID BIT(17)
#define CFG_DPHY_VDD_VALID BIT(16)
#define CFG_DPHY_ULPS_DATA BIT(2)
#define CFG_DPHY_ULPS_CLK BIT(1)
#define CFG_DPHY_CONT_CLK BIT(0)

//DSI_PHY_CTRL_2 0x188
#define CFG_DPHY_HSTX_RX BIT(14)
#define CFG_DPHY_LANE_MAP_SHIFT 12
#define CFG_DPHY_LANE_EN_SHIFT 4
#define CFG_DPHY_FORCE_BTA BIT(0)

#define CFG_DPHY_LANE_MAP_MASK (0x3)
#define CFG_DPHY_LANE_EN_MASK (0xF)

//DSI_PHY_TIME_0 0x1C0
#define CFG_DPHY_TIME_HS_EXIT_SHIFT 24
#define CFG_DPHY_TIME_HS_TRAIL_SHIFT 16
#define CFG_DPHY_TIME_HS_ZERO_SHIFT 8
#define CFG_DPHY_TIME_HS_PREP_SHIFT 0

#define CFG_DPHY_TIME_HS_EXIT_MASK (0xFF)
#define CFG_DPHY_TIME_HS_TRAIL_MASK (0xFF)
#define CFG_DPHY_TIME_HS_ZERO_MASK (0xFF)
#define CFG_DPHY_TIME_HS_PREP_MASK (0xFF)

//DSI_PHY_TIME_1 0x1C4
#define CFG_DPHY_TIME_TA_GET_SHIFT 24
#define CFG_DPHY_TIME_TA_GO_SHIFT 16
#define CFG_DPHY_TIME_WAKEUP_SHIFT 0

#define CFG_DPHY_TIME_TA_GET_MASK (0xFF)
#define CFG_DPHY_TIME_TA_GO_MASK (0xFF)
#define CFG_DPHY_TIME_WAKEUP_MASK (0xFFFF)

//DSI_PHY_TIME_2 0x1C8
#define CFG_DPHY_TIME_CLK_EXIT_SHIFT 24
#define CFG_DPHY_TIME_CLK_TRAIL_SHIFT 16
#define CFG_DPHY_TIME_CLK_ZERO_SHIFT 8
#define CFG_DPHY_TIME_CLK_LPX_SHIFT 0

#define CFG_DPHY_TIME_CLK_EXIT_MASK (0xFF)
#define CFG_DPHY_TIME_CLK_TRAIL_MASK (0xFF)
#define CFG_DPHY_TIME_CLK_ZERO_MASK (0xFF)
#define CFG_DPHY_TIME_CLK_LPX_MASK (0xFF)

//DSI_PHY_TIME_3 0x1CC
#define CFG_DPHY_TIME_LPX_SHIFT 8
#define CFG_DPHY_TIME_REQRDY_SHIFT 0

#define CFG_DPHY_TIME_LPX_MASK (0xFF)
#define CFG_DPHY_TIME_REQRDY_MASK (0xFF)

//DSI_PHY_ANA_PWR_CTRL 0x1E0
#define CFG_DPHY_ANA_RESET BIT(8)
#define CFG_DPHY_ANA_PU BIT(0)

//DSI_PHY_ANA_CTRL1 0x1E8
#define CFG_CLK_SEL BIT(23)
#define CFG_CLK_DIV2 BIT(11)
#define CFG_SET_TEST BIT(15)
#define DFG_SET_TEST_LP BIT(14)
#define DFG_TEST_PATTERN_SHIFT 12
#define DFG_TEST_PATTERN_MASK (0x3)

/*--------------------dsi regs end------------------------*/

#define MAX_TX_CMD_COUNT 100
#define MAX_RX_DATA_COUNT 64

enum {
    DSI_ESC_CLK_52M = 0,
    DSI_ESC_CLK_48M = 1,
    DSI_ESC_CLK_26M = 2,
    DSI_ESC_CLK_78M = 3,
    DSI_ESC_CLK_LIMIT,
};

enum {
    DSI_BURST_MODE_NON_BURST_SYNC_PULSE = 0,
    DSI_BURST_MODE_NON_BURST_SYNC_EVENT = 1,
    DSI_BURST_MODE_BURST = 2,
    DSI_BURST_MODE_LIMIT
};

enum {
    DSI_INPUT_DATA_RGB_MODE_565 = 0,
    DSI_INPUT_DATA_RGB_MODE_666PACKET = 1,
    DSI_INPUT_DATA_RGB_MODE_666UNPACKET = 2,
    DSI_INPUT_DATA_RGB_MODE_888 = 3,
    DSI_INPUT_DATA_RGB_MODE_LIMIT
};

enum {
    DSI_MODE_VIDEO,
    DSI_MODE_CMD,
    DSI_MODE_LIMIT
};

enum {
    DSI_DCS_SWRITE = 0x5,
    DSI_DCS_SWRITE1 = 0x15,
    DSI_DCS_LWRITE = 0x39,
    DSI_DCS_READ = 0x6,
    DSI_GENERIC_LWRITE = 0x29,
    DSI_GENERIC_READ1 = 0x14,
    DSI_SET_MAX_PKT_SIZE = 0x37,
};

enum {
    DSI_HS_MODE = 0,
    DSI_LP_MODE = 1,
};

enum {
    DSI_ACK_ERR_RESP = 0x2,
    DSI_EOTP = 0x8,
    DSI_GEN_READ1_RESP = 0x11,
    DSI_GEN_READ2_RESP = 0x12,
    DSI_GEN_LREAD_RESP = 0x1A,
    DSI_DCS_READ1_RESP = 0x21,
    DSI_DCS_READ2_RESP = 0x22,
    DSI_DCS_LREAD_RESP = 0x1C,
};

enum {
    DSI_POLARITY_POS = 0,
    DSI_POLARITY_NEG,
    DSI_POLARITY_LIMIT
};

enum {
    DSI_TE_MODE_NO = 0,
    DSI_TE_MODE_A,
    DSI_TE_MODE_B,
    DSI_TE_MODE_C,
    DSI_TE_MODE_LIMIT,
};

enum {
    DSI_STATUS_UNINIT = 0,
    DSI_STATUS_INIT = 1,
    DSI_STATUS_LIMIT
};

struct s_dsi_adv_setting {
    uint32_t lpm_frame_en; /*return to LP mode every frame*/
    uint32_t last_line_turn;
    uint32_t hex_slot_en;
    uint32_t hsa_pkt_en;
    uint32_t hse_pkt_en;
    uint32_t hbp_pkt_en; /*bit:18*/
    uint32_t hfp_pkt_en; /*bit:20*/
    uint32_t hex_pkt_en;
    uint32_t hlp_pkt_en; /*bit:22*/
    uint32_t auto_dly_dis;
    uint32_t timing_check_dis;
    uint32_t hact_wc_en;
    uint32_t auto_wc_dis;
    uint32_t vsync_rst_en;
};

struct s_mipi_info {
    unsigned int width;
    unsigned int height;
    unsigned int hfp; /*pixel*/
    unsigned int hbp;
    unsigned int hsync;
    unsigned int vfp; /*line*/
    unsigned int vbp;
    unsigned int vsync;
    unsigned int fps;

    unsigned int work_mode; /*command_mode, video_mode*/
    unsigned int rgb_mode;
    unsigned int lane_number;
    unsigned int phy_freq; /*KHz*/
    unsigned int split_enable;
    unsigned int eotp_enable;

    /*for video mode*/
    unsigned int burst_mode;

    /*for cmd mode*/
    unsigned int te_enable;
    unsigned int vsync_pol;
    unsigned int te_pol;
    unsigned int te_mode;

    /*The following fields need not be set by panel*/
    unsigned int real_fps;
};

struct s_dsi_cmd_desc {
    int cmd_type;
    uint8_t  lp;    /*command tx through low power mode or hs mode */
    uint32_t delay; /* time to delay */
    uint32_t length;  /* cmds length */
    uint8_t data[MAX_TX_CMD_COUNT];
};

struct s_dsi_rx_buf {
    int data_type;
    uint32_t length; /* cmds length */
    uint8_t data[MAX_RX_DATA_COUNT];
};

struct s_dsi_ctx {
    uint32_t base_addr;
    uint32_t esc_clk_rate, bit_clk_rate; /*KHz*/
    struct s_dphy_ctx dphy_config;
    struct s_dsi_adv_setting adv_setting;
    struct s_mipi_info mipi_info;
    uint8_t work_mode;   /* 0: polling, 1: interrupt */
    uint8_t status;
    uint16_t reserved;
};

int lcd_dsi_init(const struct s_mipi_info *mipi_info, int32_t work_mode);
int lcd_dsi_uninit(void);
void lcd_dsi_reset(void);
int lcd_dsi_update(int32_t work_mode);
int lcd_dsi_update_mipi_info(uint16_t width, uint16_t height);
int lcd_dsi_before_refresh(void);
int lcd_dsi_after_refresh(void);

int lcd_dsi_write_cmds(struct s_dsi_cmd_desc *cmds, int count);
int lcd_dsi_read_cmds(struct s_dsi_rx_buf *dbuf,
                struct s_dsi_cmd_desc *cmds, int count);

void lcd_dump_dsi_registers(void);

void lcd_dsi_write(uint32_t reg, uint32_t val);
uint32_t lcd_dsi_read(uint32_t reg);
void lcd_dsi_set_bits(uint32_t reg, uint32_t bits);
void lcd_dsi_clear_bits(uint32_t reg, uint32_t bits);
void lcd_dsi_write_bits(uint32_t reg, uint32_t value, uint32_t mask, uint32_t shifts);

#endif /* USE_CRANE_LCD_MIPI */

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
