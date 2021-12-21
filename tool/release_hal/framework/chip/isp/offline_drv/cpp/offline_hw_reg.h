#ifndef __OFFLINE_HW_REG_H__
#define __OFFLINE_HW_REG_H__

/*
 * SCALER_IN_SIZE
 * Offset: 0x000
 * Bits	Field	Field (Code)	Type	Init	Reset	Description
 * 31:29	Reserved		RSVD			Reserved for future use
 * 28:16	v_in_size					scaler input height
 * 15:13	Reserved		RSVD			Reserved for future use
 * 12:0	h_in_size					scaler input width
 */
#define REG_SCALER_IN_SIZE 0x00

/*
 * SCALER_OUT_SIZE
 * Offset: 0x04
 * Bits	Field	Field (Code)	Type	Init	Reset	Description
 * 31:29	Reserved		RSVD			Reserved for future use
 * 28:16	v_out_size					scaler output height
 * 15:13	Reserved		RSVD			Reserved for future use
 * 12:0	h_out_size					scaler output width
 */
#define REG_SCALER_OUT_SIZE	0x04						

/*
 * SCALER_SLICE_WIDTH
 * Offset: 0x08
 * Bits	Field	Field (Code)	Type	Init	Reset	Description
 * 31:29	Reserved		RSVD			Reserved for future use
 * 28:16	silice_width_out					silice_width_out
 * 15:13	Reserved		RSVD			Reserved for future use
 * 12:0	silice_width_in					silice_width_in
 */
#define REG_SCALER_SLICE_WIDTH 0x08						

/*
 * SCALER_H_PHASE
 * Offset: 0x0c
 * Bits	Field	Field (Code)	Type	Init	Reset	Description
 * 31	Reserved		RSVD			Reserved for future use
 * 30:24	h_init_ph					h initital phase
 * 23:21	Reserved		RSVD			Reserved for future use
 * 20:0	h_delta_ph_delta					h delta phase
 */
#define REG_SCALER_H_PHASE 0x0c						

/*
 * SCALER_V_PHASE
 * Offset: 0x10
 * Bits	Field	Field (Code)	Type	Init	Reset	Description
 * 31	Reserved		RSVD			Reserved for future use
 * 30:24	v_init_ph					v initital phase
 * 23:21	Reserved		RSVD			Reserved for future use
 * 20:0	v_delta_ph_delta					v delta phase
 */
#define REG_SCALER_V_PHASE 0x10						

/*
 * SCLAER_TOP_CTRL
 * Offset: 0x14
 * Bits	Field	Field (Code)	Type	Init	Reset	Description
 * 31:20	Reserved		RSVD			Reserved for future use
 * 19:16	coef_table_sel_v					"coef_table_sel_v
 * vertical direction coef table selection"
 * 15:14	yuv422_order		RSVD			"yuv422_order
 * // 0x0 yu yv (LSB to MSB)
 * // 0x1 yv yu
 * // 0x2 uy vy
 * // 0x3 vy uy
 * "
 * 13	Reserved		RSVD			Reserved for future use
 * 12	trigger_frame					"trigger_frame
 * '1' valid, hw clear "
 * 11:8	coef_table_sel_h					"coef_table_sel_h
 * horizontal direction coef table selection"
 * 7	scaler_ena					"scaler_ena
 * '1' valid"
 * 6:4	Subsample_mode					"0x0 1:1
 * 0x1 1:2
 * 0x2 1:4
 * 0x3 1:8
 * 0x4 1:16
 * 0x5 1:32"
 * 3	Reserved		RSVD			Reserved for future use
 * 2:0	data_fmt					"// 0x0 YUV422 1plane
 * // 0x1 YUV400 1plane
 * // 0x2 YUV420 2plane
 * // 0x3 YUV420 3plane Y/U/V
 * // 0x4 RGB888 1plane
 * // 0x5 RGB565 1plane
 * // 0x6 RGBA/ARGB 1plane
 * "
 */
#define REG_SCLAER_TOP_CTRL 0x14						

/*
 * SCALER_READ_PITCH
 * Offset: 0x18
 * Bits	Field	Field (Code)	Type	Init	Reset	Description
 * 31:18	Reserved		RSVD			Reserved for future use
 * 17:0	read_pitch					read_pitch
 */
#define REG_SCALER_READ_PITCH 0x18						


/*
 * SCALER_WRITE_PITCH
 * Offset: 0x1c
 * Bits	Field	Field (Code)	Type	Init	Reset	Description
 * 31:18	Reserved		RSVD			Reserved for future use
 * 17:0	write_pitch					read_pitch
 */
#define REG_SCALER_WRITE_PITCH 0x1c						

/*
 * SCALER_RD_Y_BASE_ADDR
 * Offset: 0x20
 * Bits	Field	Field (Code)	Type	Init	Reset	Description
 * 31:18	Reserved		RSVD			Reserved for future use
 * 31:0	RD_Y_BASE_ADDR					Read DMA Y-Plane base address
 */
#define REG_SCALER_RD_Y_BASE_ADDR 0x20						

/*
 * SCALER_RD_U_BASE_ADDR
 * Offset: 0x24
 * Bits	Field	Field (Code)	Type	Init	Reset	Description
 * 31:18	Reserved		RSVD			Reserved for future use
 * 31:0	RD_U_BASE_ADDR					Read DMA U-Plane base address
 */
#define REG_SCALER_RD_U_BASE_ADDR 0x24						

/*
 * SCALER_RD_V_BASE_ADDR
 * Offset: 0x28
 * Bits	Field	Field (Code)	Type	Init	Reset	Description
 * 31:18	Reserved		RSVD			Reserved for future use
 * 31:0	RD_V_BASE_ADDR					Read DMA V-Plane base address
 */
#define REG_SCALER_RD_V_BASE_ADDR 0x28						

/*
 * SCALER_WR_Y_BASE_ADDR
 * Offset: 0x2c
 * Bits	Field	Field (Code)	Type	Init	Reset	Description
 * 31:18	Reserved		RSVD			Reserved for future use
 * 31:0	WR_Y_BASE_ADDR					Write DMA Y-Plane base address
 */
#define REG_SCALER_WR_Y_BASE_ADDR 0x2c						

/*
 * SCALER_WR_U_BASE_ADDR
 * Offset: 0x30
 * Bits	Field	Field (Code)	Type	Init	Reset	Description
 * 31:18	Reserved		RSVD			Reserved for future use
 * 31:0	WR_U_BASE_ADDR					Write DMA U-Plane base address
 */
#define REG_SCALER_WR_U_BASE_ADDR 0x30						

/*
 * SCALER_WR_V_BASE_ADDR
 * Offset: 0x34
 * Bits	Field	Field (Code)	Type	Init	Reset	Description
 * 31:18	Reserved		RSVD			Reserved for future use
 * 31:0	WR_V_BASE_ADDR					Write DMA V-Plane base address
 */
#define REG_SCALER_WR_V_BASE_ADDR 0x34						

/*
 * OFFLINE_IRQ_RAW
 * Offset: 0x38
 * Bits	Field	Field (Code)	Type	Init	Reset	Description
 * 0	ire_eof_irq		read-only			ire_eof_irq
 * 1	scl_eof_irq		read-only			scl_eof_irq,  write '1' to clear
 * 2	dmac_rd_chnl0_err		read-only			dmac_rd_chnl0_err,  write '1' to clear
 * 3	dmac_rd_chnl1_err		read-only			dmac_rd_chnl1_err,  write '1' to clear
 * 4	dmac_wr_chnl0_err		read-only			dmac_wr_chnl0_err,  write '1' to clear
 * 5	dmac_wr_chnl1_err		read-only			dmac_wr_chnl1_err,  write '1' to clear
 * 6	scl_wfifo_overrun		read-only			scl_wfifo_overrun,  write '1' to clear
 * 31:7	reserved
 */
#define REG_OFFLINE_IRQ_RAW 0x38

/*
 * OFFLINE_IRQ_MASK
 * Offset: 0x3c
 * Bits	Field	Field (Code)	Type	Init	Reset	Description
 * 0	ire_eof_irq mask					ire_eof_irq mask
 * 1	scl_eof_irq mask					scl_eof_irq mask
 * 2	dmac_rd_chnl0_err_mask					dmac_rd_chnl0_err_mask
 * 3	dmac_rd_chnl1_err_mask					dmac_rd_chnl1_err_mask
 * 4	dmac_wr_chnl0_err_mask					dmac_wr_chnl0_err_mask
 * 5	dmac_wr_chnl1_err_mask					dmac_wr_chnl1_err_mask
 * 6	scl_wfifo_overrun_mask					scl_wfifo_overrun_mask
 * 31:7	reserved
 */
#define REG_OFFLINE_IRQ_MASK 0x3c						

/*
 * OFFLINE_IRQ_STATUS
 * Offset: 0x40
 * Bits	Field	Field (Code)	Type	Init	Reset	Description
 * 0	ire_eof_irq status					ire_eof_irq status,  write '1' to clear
 * 1	scl_eof_irq status					scl_eof_irq status,  write '1' to clear
 * 2	dmac_rd_chnl0_err					dmac_rd_chnl0_err,  write '1' to clear
 * 3	dmac_rd_chnl1_err					dmac_rd_chnl1_err,  write '1' to clear
 * 4	dmac_wr_chnl0_err					dmac_wr_chnl0_err,  write '1' to clear
 * 5	dmac_wr_chnl1_err					dmac_wr_chnl1_err,  write '1' to clear
 * 6	scl_wfifo_overrun					scl_wfifo_overrun,  write '1' to clear
 * 31:7	reserved
 */
#define REG_OFFLINE_IRQ_STATUS 0x40						

/*
 * IRE_TOP_CTRL
 * Offset: 0x44
 * Bits	Field	Field (Code)	Type	Init	Reset	Description
 * 31:11	reserved					reserved
 * 11	ire_prefetch					IRE frame start signal,1 valid, hw clear
 * 10	cnfg_ire_422_edian					IRE YUV422 edianness                                         0x0 YUYV-LSB Y                                                     0x1 UYVY-LSB U/V
 * 9:8	cnfg_ire_ycbcr_fmt					IRE YCBCR FORMAT                                              0x0 YCBCR400                                             0x1 YCBCR420                                           0x2 YCBCR422                                           0x3 reserved
 * 7:6	cnfg_ire_rot_mod					IRE YUV format                                              0x0 YCBCR_PACKED                                                     0x1 YCBCR_2_PLANAR                                 0x2 YCBCR_3_PLANAR                                  0x3 reserved
 * 5:3	cnfg_ire_angle					IRE rotation angle                                              0x0 90-degree                                             0x1 270-degree                                            0x2 180-degree                                            0x3 mirror                                                   0x4 flip                                                       others reserved
 * 2:1	cnfg_ire_bpp					IRE bits per pixel                                              0x0 8-bit for YUV400/YUV420                                                     0x1 16-bit for YUV422/RGB565                     0x2 24-bit for RGB888                                  0x3 32-bit for ARGB8888
 * 0	cnfg_ire_en					IRE enable bit                                              0x0 ire disable                                             0x1 ire enable
 */
#define REG_IRE_TOP_CTRL 0x44						

/*
 * IRE_IN_SIZE
 * Offset: 0x48
 * Bits	Field	Field (Code)	Type	Init	Reset	Description
 * 31:28	Reserved					Reserved for future use
 * 27:16	cnfg_ire_ysize					ire input height
 * 15:13	Reserved					Reserved for future use
 * 12:0	cnfg_ire_xsize					ire input width
 */
#define REG_IRE_IN_SIZE 0x48						

/*
 * IRE_PRE_PITCH
 * Offset: 0x4c
 * Bits	Field	Field (Code)	Type	Init	Reset	Description
 * 31:0	cnfg_ire_pre_pitch					IRE Read DMA src pitch
 */
#define REG_IRE_PRE_PITCH 0x4c						

/*
 * IRE_PST_PITCH
 * Offset: 0x50
 * Bits	Field	Field (Code)	Type	Init	Reset	Description
 * 31:0	cnfg_ire_pst_pitch					IRE Write DMA pst pitch
 */
#define REG_IRE_PST_PITCH 0x50						

/*
 * IRE_RD_Y_BASE_ADDR
 * Offset: 0x54
 * Bits	Field	Field (Code)	Type	Init	Reset	Description
 * 31:0	cnfg_ire_src_y_addr					IRE Read DMA Y-Plane base address
 */
#define REG_IRE_RD_Y_BASE_ADDR 0x54						

/*
 * IRE_RD_U_BASE_ADDR
 * Offset: 0x58
 * Bits	Field	Field (Code)	Type	Init	Reset	Description
 * 31:0	cnfg_ire_src_u_addr					IRE Read DMA U-Plane base address
 */
#define REG_IRE_RD_U_BASE_ADDR 0x58						

/*
 * IRE_RD_V_BASE_ADDR
 * Offset: 0x5c
 * Bits	Field	Field (Code)	Type	Init	Reset	Description
 * 31:0	cnfg_ire_src_v_addr					IRE Read DMA V-Plane base address
 */
#define REG_IRE_RD_V_BASE_ADDR 0x5c						

/*
 * IRE_WR_Y_BASE_ADDR
 * Offset: 0x60
 * Bits	Field	Field (Code)	Type	Init	Reset	Description
 * 31:0	cnfg_ire_dst_y_addr					IRE Write DMA Y-Plane base address
 */
#define REG_IRE_WR_Y_BASE_ADDR 0x60						

/*
 * IRE_WR_U_BASE_ADDR
 * Offset: 0x64
 * Bits	Field	Field (Code)	Type	Init	Reset	Description
 * 31:0	cnfg_ire_dst_u_addr					IRE Write DMA U-Plane base address
 */
#define REG_IRE_WR_U_BASE_ADDR 0x64						

/*
 * IRE_WR_V_BASE_ADDR
 * Offset: 0x68
 * Bits	Field	Field (Code)	Type	Init	Reset	Description
 * 31:0	cnfg_ire_dst_v_addr					IRE Write DMA V-Plane base address
 */
#define REG_IRE_WR_V_BASE_ADDR 0x68						

/*
 * IRE_DMAC_CTRL
 * Offset: 0x6c
 * Bits	Field	Field (Code)	Type	Init	Reset	Description
 * 3:0	dmac_awqos					dmac_awqos
 * 7:4	dmac_arqos					dmac_arqos
 * 8	dmac_axi_sec					dmac_axi_sec
 * 12:9	dmac_user_id					dmac_user_id
 * 13	dmac_rst_n_pwr					dmac_rst_n_pwr
 * 14	dmac_rst_req					dmac_rst_req
 * 15	dmac_wr_int_clr					dmac_wr_int_clr
 * 16	dmac_rd_int_clr					dmac_rd_int_clr
 * 17	dmac_scan_enable					dmac_scan_enable
 * 18	dmac_scan_mode					dmac_scan_mode
 * 21:19	dmac_max_req_num					dmac_max_req_num
 * 23:22	reserved					reserved
 * 29:24	dmac_post_wr_en					dmac_post_wr_en
 */
#define REG_IRE_DMAC_CTRL 0x6c						

/*
 * IRE_DEBUG_INFO_LSB
 * Offset: 0x70
 * Bits	Field	Field (Code)	Type	Init	Reset	Description
 * 31:0	ire_debug_info-lsb		Read-only			IRE DEBUG INFO LSB
 */
#define REG_IRE_DEBUG_INFO_LSB 0x70						

/*
 * IRE_DEBUG_INFO_MSB
 * Offset: 0x74
 * Bits	Field	Field (Code)	Type	Init	Reset	Description
 * 31:0	ire_debug_info-msb		Read-only			IRE DEBUG INFO MSB
 */
#define REG_IRE_DEBUG_INFO_MSB 0x74						

/*
 * IRE_DMAC_DEBUG_INFO
 * Offset: 0x78
 * Bits	Field	Field (Code)	Type	Init	Reset	Description
 * 0	dmac_busy		Read-only			dmac busy flag
 * [3:1]	dmac_rd_err		Read-only			dmac rd error flag
 * [9:4]	dmac_wr_err		Read-only			dmac wr error flag
 */
#define REG_IRE_DMAC_DEBUG_INFO 0x78						

/*
 * SCL_DEBUG_INFO_0
 * Offset: 0x7c
 * Bits	Field	Field (Code)	Type	Init	Reset	Description
 * 31:10	Reserved		RSVD			Reserved for future use
 * 9:8	yuv_plane_status		RO			"yuv_plane_status,
 * <p>=0, Y-Plane status
 * <p>=1, U-Plane status
 * <p>=2, V-Plane status"
 * 7:0	slice_number		RO			slice_number
 */
#define REG_SCL_DEBUG_INFO_0 0x7c

/*
 * SCL_DEBUG_INFO_1
 * Offset: 0x80
 * Bits	Field	Field (Code)	Type	Init	Reset	Description
 * 31:0	SCL_IN_PIX_NUM		RO			SCL_IN_PIX_NUM
 */
#define REG_SCL_DEBUG_INFO_1 0x80

/*
 * SCL_DEBUG_INFO_2
 * Offset: 0x84
 * Bits	Field	Field (Code)	Type	Init	Reset	Description
 * 31:0	SCL_OUT_PIX_NUM		RO			SCL_OUT_PIX_NUM
 */
#define REG_SCL_DEBUG_INFO_2 0x84

#define IRQ_SCL_EOF		(0x1 << 0)
#define IRQ_IRE_EOF		(0x1 << 1)
#define IRQ_DMAC_RD_CH0_ERR	(0x1 << 2)
#define IRQ_DMAC_RD_CH1_ERR	(0x1 << 3)
#define IRQ_DMAC_WR_CH0_ERR	(0x1 << 4)
#define IRQ_DMAC_WR_CH1_ERR	(0x1 << 5)
#define IRQ_SCL_WF_OVERRUN	(0x1 << 6)

#define SCL_CTRL_YUV422_ORDER_MASK (0x3 << 14)
#define YUV422_ORDER_YUYV (0x0 << 14)
#define YUV422_ORDER_YVYU (0x1 << 14)
#define YUV422_ORDER_UYVY (0x2 << 14)
#define YUV422_ORDER_VYUY (0x3 << 14)

#define SCL_CTRL_TRIGGER_FRAME (0x1 << 12)

#define SCL_CTRL_COEF_TAB_SEL_V_MASK (0xF << 16)
#define SCL_CTRL_COEF_TAB_SEL_H_MASK (0xF << 8)
#define SCL_25_COEFF (0x0 << 8)
#define SCL_30_COEFF (0x1 << 8)
#define SCL_35_COEFF (0x2 << 8)
#define SCL_40_COEFF (0x3 << 8)
#define SCL_45_COEFF (0x4 << 8)
#define SCL_50_COEFF (0x5 << 8)
#define SCL_55_COEFF (0x6 << 8)
#define SCL_60_COEFF (0x7 << 8)
#define SCL_65_COEFF (0x8 << 8)
#define SCL_70_COEFF (0x9 << 8)
#define SCL_75_COEFF (0xa << 8)
#define SCL_80_COEFF (0xb << 8)
#define SCL_85_COEFF (0xc << 8)
#define SCL_90_COEFF (0xd << 8)
#define SCL_95_COEFF (0xe << 8)
#define SCL_100_COEFF (0xf << 8)

#define SCL_CTRL_EN (0x1 << 7)

#define SCL_CTRL_SUBSAMLE_MASK (0x7 << 4)
#define SCL_1OVER1_SUBSAMPLE  (0x0 << 4)
#define SCL_1OVER2_SUBSAMPLE  (0x1 << 4)
#define SCL_1OVER4_SUBSAMPLE  (0x2 << 4)
#define SCL_1OVER8_SUBSAMPLE  (0x3 << 4)
#define SCL_1OVER16_SUBSAMPLE (0x4 << 4)
#define SCL_1OVER32_SUBSAMPLE (0x5 << 4)

#define SCL_CTRL_DATA_FMT_MASK (0x7 << 0)
#define SCL_FMT_YUV422 (0x0 << 0)
#define SCL_FMT_YUV400 (0x1 << 0)
#define SCL_FMT_YUV420_2PLANAR (0x2 << 0)
#define SCL_FMT_YUV420_3PLANAR (0x3 << 0)
#define SCL_FMT_RGB888 (0x4 << 0)
#define SCL_FMT_RGB565 (0x5 << 0)
#define SCL_FMT_ARGB   (0x6 << 0)

#define IRE_CTRL_DMA_MODE_MASK (0x3 << 12)
#define IRE_CTRL_DMA_MODE0 (0x0 << 12)
#define IRE_CTRL_DMA_MODE1 (0x1 << 12)
#define IRE_CTRL_DMA_MODE2 (0x2 << 12)
#define IRE_CTRL_DMA_MODE3 (0x3 << 12)

#define IRE_CTRL_PREFETCH (0x1 << 11)

#define IRE_CTRL_422_EDIAN_MASK (0x1 << 10)
#define IRE_CTRL_YUYV_EDIAN (0x0 << 10)
#define IRE_CTRL_YVYU_EDIAN (0x0 << 10)
#define IRE_CTRL_UYVY_EDIAN (0x1 << 10)
#define IRE_CTRL_VYUY_EDIAN (0x1 << 10)

#define IRE_CTRL_UV_SWAP_MASK (0x1 << 9)
#define IRE_UV_NO_SWAP (0x0 << 9)
#define IRE_UV_DO_SWAP (0x1 << 9)

#define IRE_CTRL_IMG_FMT_MASK (0x7 << 6)
#define IRE_FMT_YUV400 (0x0 << 6)
#define IRE_FMT_YUV420_2PLANAR (0x1 << 6)
#define IRE_FMT_YUV420_3PLANAR (0x2 << 6)
#define IRE_FMT_YUV422 (0x3 << 6)
#define IRE_FMT_RGB565 (0x4 << 6)
#define IRE_FMT_RGB888 (0x5 << 6)
#define IRE_FMT_ARGB   (0x6 << 6)

#define IRE_CTRL_ANGLE_MASK (0x7 << 3)
#define IRE_DEGREE_90  (0x0 << 3)
#define IRE_DEGREE_270 (0x1 << 3)
#define IRE_DEGREE_180 (0x2 << 3)
#define IRE_MIRROR     (0x3 << 3)
#define IRE_FLIP       (0x4 << 3)
#define IRE_ANGLE_MAX  (0x5 << 3)

#define IRE_CTRL_EN (0x1 << 0)

#endif
