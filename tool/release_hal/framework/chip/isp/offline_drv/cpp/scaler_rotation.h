#ifndef __SCL_IRE_H__
#define __SCL_IRE_H__
#include "../cam_offline.h"

extern u8 g_offline_interrupt_flag;

int scl_ire_regs_dump(void);

void offline_s_reset(void);

int scl_ctrl_cfg(uint32_t w_in, uint32_t h_in, uint32_t w_ot, uint32_t h_ot,
		 uint32_t pitch_in, uint32_t pitch_ot, SCALER_INPUT_FMT fmt);

void scl_set_addr(uint32_t addr_rd_y, uint32_t addr_rd_u, uint32_t addr_rd_v,
		  uint32_t addr_wr_y, uint32_t addr_wr_u, uint32_t addr_wr_v);

void scl_trigger(void);

void ire_set_addr(uint32_t addr_rd_y, uint32_t addr_rd_u, uint32_t addr_rd_v,
		  uint32_t addr_wr_y, uint32_t addr_wr_u, uint32_t addr_wr_v);

int ire_ctrl_cfg(uint32_t w_in, uint32_t h_in, uint32_t pitch_in, uint32_t pitch_ot,
		 SCALER_INPUT_FMT fmt, enum offline_rot rot);

int ire_ctrl_cfg_v2(uint32_t w_in, uint32_t h_in, uint32_t pitch_in, uint32_t pitch_ot,
		 SCALER_INPUT_FMT fmt, enum offline_rot rot, int dma_mode);

void ire_trigger(void);

void offline_irq_init(void);

void offline_irq_deinit(void);

int offline_power_on(int module_id);

int offline_power_off(int module_id);
#endif
