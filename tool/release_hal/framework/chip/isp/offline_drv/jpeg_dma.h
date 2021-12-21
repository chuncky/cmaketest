#ifndef __jdma_H__
#define __jdma_H__

int jdma_regs_dump(void);
int jdma_regs_check(void);
void jdma_trigger(void);
int jdma_cfg(uint32_t width, uint32_t height, uint32_t pitch,
	     uint32_t rot, uint32_t y_base_addr, uint32_t u_base_addr);

#endif
