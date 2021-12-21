#ifndef __CAM_PIPELINE_REG_H__
#define __CAM_PIPELINE_REG_H__

#include "../cam_pipeline.h"


/* macro for regs */
#define IRQ_CAMERA_IPE 58
#define ISP_REG_BASE 0xd420e000
#define REG_CAMERA_BASE 0xD420C000
#define AP_PMU_BASE 0xd4282800

#define REG_Y0_BASE 0x00
#define REG_Y1_BASE 0x04
#define REG_Y2_BASE 0x08
#define REG_U0_BASE 0x0c
#define REG_U1_BASE 0x10
#define REG_U2_BASE 0x14
#define REG_V0_BASE 0x18
#define REG_V1_BASE 0x1c
#define REG_V2_BASE 0x20


#define IRQ_CCIC_MASK_ALL 0xFF03F800 //mask the bit23 csi packet error is always report


#define REG_PIP0_CCIC_IMG_PITCH 0x24
#define REG_CCIC_IRQ_RAW_STATUS 0x28
#define REG_CCIC_IRQ_MASK 0x2c
#define REG_CCIC_IRQ_STATUS 0x30
#define IRQ_CSI2_GENERIC_SHORT_PACKET_ERROR (0x1 << 31)
#define IRQ_CSI2_GENERIC_SHORT_PACKET_VALID (0x1 << 30)
#define IRQ_CSI2_PARSE_ERROR (0x1 << 29)
#define IRQ_CSI2_LANE_FIFO_OVERRUN_ERROR (0x1 << 28)
#define IRQ_CSI2_ECC_CORRECTABLE_ERROR (0x1 << 27)
#define IRQ_CSI2_PARITY_ERROR (0x1 << 26)
#define IRQ_CSI2_ECC_ERROR (0x1 << 25)
#define IRQ_CSI2_CRC_ERROR (0x1 << 24)
#define IRQ_CSI2_PACKET_ERROR (0x1 << 23)
#define IRQ_MIPI_DPHY_LANE_START_OF_TRANSMISSION_ERROR (0x1 << 17)
#define IRQ_MIPI_DPHY_LANE_START_OF_TRANSMISSION_SYNCHRONIZATION_ERROR (0x1 << 16)
#define IRQ_MIPI_DPHY_LANE_ERROR_CONTROL (0x1 << 15)
#define IRQ_MIPI_DPHY_LANE_ULPS_ACTIVE (0x1 << 14)
#define IRQ_MIPI_DPHY_RX_CLK_ULPS (0x1 << 13)
#define IRQ_MIPI_DPHY_RX_CLK_ULPS_ACTIVE (0x1 << 12)
#define IRQ_AXI_WRITE_ERROR (0x1 << 11)
#define REG_IMG_SIZE_PIP0 0x34
#define REG_IMG_OFFSET 0x38
#define REG_CTRL_0 0x3c
#define REG_CTRL_1 0x40
#define REG_CTRL_2 0x44
#define REG_CAMERA_IRQ_RAW_STATUS 0x48
#define REG_CAMERA_IRQ_RAW_MASK 0x4c
#define REG_CAMERA_IRQ_STATUS 0x50

//irq bit
#define PIP1_STOP_DOWN_IRQ		   0x80000000 //(0x01 << 31)
#define PIP0_STOP_DOWN_IRQ		   (0x01 << 30)
#define AEC_UPDATE_IRQ             (0x01 << 24)
#define SPI_LINE_ERR_IRQ           (0x01 << 23)
#define SPI_CRC_ERR_IRQ            (0x01 << 22)
#define PIP1_SCL_FIFO_OVERRUN_IRQ  (0x01 << 21)
#define PIP0_SCL_FIFO_OVERRUN_IRQ  (0x01 << 20)
#define PIP2_DMA_FIFO_OVERRUN_IRQ  (0x01 << 19)
#define PIP1_DMA_FIFO_OVERRUN_IRQ  (0x01 << 18)
#define PIP0_DMA_FIFO_OVERRUN_IRQ  (0x01 << 17)
#define ISIM_IRQ                   (0x01 << 16)
#define PIP1_SHADOW_DONE_IRQ       (0x01 << 15)
#define PIP0_SHADOW_DONE_IRQ       (0x01 << 14)
#define PIP2_DMA_SOF_IRQ           (0x01 << 13)
#define PIP2_DMA_EOF_IRQ           (0x01 << 12)
#define PIP1_DMA_SOF_IRQ           (0x01 << 11)
#define PIP1_DMA_EOF_IRQ           (0x01 << 10)
#define PIP0_DMA_SOF_IRQ           (0x01 << 9)	
#define PIP0_DMA_EOF_IRQ           (0x01 << 8)	
#define PIP1_SCL_SOF_IRQ           (0x01 << 7)	
#define PIP1_SCL_EOF_IRQ           (0x01 << 6)	
#define PIP0_SCL_SOF_IRQ           (0x01 << 5)	
#define PIP0_SCL_EOF_IRQ           (0x01 << 4)	
#define ISP_SOF_IRQ                (0x01 << 3)	
#define ISP_EOF_IRQ                (0x01 << 2)	
#define CSI2_SOF_IRQ               (0x01 << 1)	
#define CSI2_EOF_IRQ               (0x01 << 0)	

#define PIPE0_CTRL_SHADOW_MODE_MASK (0x1 << 12)
#define PIPE0_CTRL_SHADOW (0x1 << 12)
#define PIPE0_CTRL_DIRECT (0x0 << 12)

#define PIPE1_CTRL_SHADOW_MODE_MASK (0x1 << 12)
#define PIPE1_CTRL_SHADOW (0x1 << 12)
#define PIPE1_CTRL_DIRECT (0x0 << 12)

/* z1/z2 */
#define PIPE2_CTRL_RAW_DUMP (0x1 << 6)
#define PIPE2_CTRL_SHADOW_MODE_MASK (0x1 << 5) /* pip2_direct_mode '1' valid */
#define PIPE2_CTRL_SHADOW (0x0 << 5)
#define PIPE2_CTRL_DIRECT (0x1 << 5)
 /* z3 */
#define PIPE2_CTRL_RAW_DUMP_A0 (0x1 << 7)

#define REG_LNNUM 0x60
#define REG_CLOCK_CTRL 0x88
#define REG_SRAM_TC0_TEST_ONLY 0x8c
#define REG_SRAM_TC1_TEST_ONLY 0x90
#define REG_CSI2_CTRL_0 0x100
#define REG_CSI2_IRQ_RAW_STATUS 0x108
#define REG_CSI2_GSPFF 0x110
#define REG_CSI2_VCCTRL 0x114
#define REG_CSI2_DPHY1 0x124
#define REG_CSI2_DPHY2 0x128
#define REG_CSI2_DPHY3 0x12c
#define REG_CSI2_DPHY4 0x130
#define REG_CSI2_DPHY5 0x134
#define REG_CSI2_DPHY6 0x138
#define REG_CSI2_RESET 0x1fc
//#define REG_START_DESC_CHAIN_ADDR_Y 0x200
//#define REG_START_DESC_CHAIN_ADDR_U 0x204
//#define REG_START_DESC_CHAIN_ADDR_V 0x208
//#define REG_DESC_CHAIN_LEN_Y 0x20c
//#define REG_DESC_CHAIN_LEN_Y 0x210
//#define REG_DESC_CHAIN_LEN_Y 0x214
//#define REG_DESC_CHAIN_TADDR_Y 0x218
//#define REG_DESC_CHAIN_TADDR_Y 0x21c
//#define REG_DESC_CHAIN_TADDR_Y 0x220
//#define REG_DESC_CHAIN_CTRLY 0x224
//#define REG_DESC_CHAIN_CTRLU 0x228
//#define REG_DESC_CHAIN_CTRLV 0x22c
//#define REG_NEXT_DESC_CHAIN_ADDR_Y 0x230
//#define REG_NEXT_DESC_CHAIN_ADDR_U 0x234
//#define REG_NEXT_DESC_CHAIN_ADDR_V 0x238
#define REG_FRAME_BYTE_CNT 0x23c
#define REG_PIP1_CCIC_IMG_PITCH 0x240
#define REG_PIP2_CCIC_IMG_PITCH 0x244
#define REG_PIP0_CROP_ROI_X 0x248
#define REG_PIP0_CROP_ROI_Y 0x24c
#define REG_PIP1_CROP_ROI_X 0x250
#define REG_PIP1_CROP_ROI_Y 0x254
#define REG_PIP0_SCALER_IN_SIZE 0x258
#define REG_PIP0_SCALER_OUT_SIZE 0x25c
#define REG_PIP0_SCALER_H_PHASE 0x260
#define REG_PIP0_SCALER_V_PHASE 0x264
#define REG_PIP1_SCALER_IN_SIZE 0x268
#define REG_PIP1_SCALER_OUT_SIZE 0x26c
#define REG_PIP1_SCALER_H_PHASE 0x270
#define REG_PIP1_SCALER_V_PHASE 0x274
#define REG_SUBSAMPLE_SCALER_CTRL0 0x2a8
#define REG_SUBSAMPLE_SCALER_CTRL1 0x2ac
#define REG_SUBSAMPLE_SCALER_CTRL2 0x2b0
#define REG_DATA_FORMAT_CTRL 0x27c
#define REG_ISP_DVP_IF_CTRL 0x280
#define REG_IMG_SIZE_PIP1 0x284
#define REG_IMG_SIZE_PIP2 0x288
#define REG_ISP_IMG_SIZE 0x28c
#define REG_DATA_RANGE_CTRL0 0x290
#define REG_DATA_RANGE_CTRL1 0x294
#define REG_DATA_RANGE_CTRL2 0x298
#define REG_DATA_RANGE_CTRL3 0x29c
#define REG_DATA_RANGE_CTRL4 0x2a0
#define REG_DATA_RANGE_CTRL5 0x2a4

void camera_set_bit(uint_32 reg, uint_32 val);
void camera_clr_bit(uint_32 reg, uint_32 val);
uint_32 camera_reg_read(uint_32 reg);
void camera_reg_write(uint_32 reg, uint_32 val);
void isp_set_bit(uint_32 reg, uint_32 val);
void isp_clr_bit(uint_32 reg, uint_32 val);
void isp_write_mask(uint_32 reg, uint_32 val, uint_32 mask);
void cam_set_pipe_shadow_ready(uint_8 pipe_id);
void set_pipeline_smooth_zoom_cfg(struct cam_isp_pipeline * pipe_mgr ,uint_8 pipe_id);
int cam_set_addr(uint_8 pipe_id,struct isp_qbuf_buffer buf);
int  cam_set_pipe_hdl(struct pipeline_param* pipe_config,int enable);
int cam_set_pipe_shadow_mode(uint8_t pipeline_id, uint8_t shadow);
int cam_set_pitch_dma_size(struct pipeline_param* pipe_config);
int cam_set_pipe_outfmt(struct pipeline_param* pipe_config);
int cam_set_pipe_jpeg_mode(struct pipeline_param* pipe_config);
void set_pipeline_zoom_cfg(struct cam_isp_pipeline * pipe_mgr ,uint_8 pipe_id,uint_16 zoom_ratio);
uint_16 get_pipeline_cur_zoomratio(struct cam_isp_pipeline * pipe_mgr ,uint_8 pipe_id);
void set_pipeline_cur_zoomratio(struct cam_isp_pipeline * pipe_mgr ,uint_8 pipe_id,uint_16 value);
void set_pipeline_target_zoomratio(struct cam_isp_pipeline * pipe_mgr ,uint_8 pipe_id,uint_16 value);
uint_16 get_pipeline_target_zoomratio(struct cam_isp_pipeline * pipe_mgr ,uint_8 pipe_id);
int cam_config_data_range(struct pipeline_param *pipe_config);
void camera_write_mask(uint_32 reg, uint_32 val, uint_32 mask);
void set_pipeline_reg_reset(void);
uint_8 isp_reg_read(uint_32 reg);
void isp_reg_write(uint_32 reg, uint_32 val);
void isp_reg_tab_write( struct crane_isp_tuning_data *regs);
void isp_get_aec(uint_32 *exp_high, uint_32 *exp_low, uint_32 *gain, uint_32 *aec_en, uint_32 *exp_step);
int pipeline_update_mac_addr(uint8_t pipeline_id, uint32_t *addr, uint32_t plane);
void isp_calc_exp(uint_32 min_fps, uint_32 max_fps, CAM_BANDING banding);

#endif
