#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "qspi_dma.h"
#include "qspi_flash.h"
#include "qspi_host.h"
#include "qspi_nor.h"




volatile P_XLLP_DMAC_T DMA=(P_XLLP_DMAC_T)(volatile unsigned int*)0xd4000000;





static DMA_STRESS_ARG arg_glob; //global variable for stress test.
static uint32_t dma_stress_iter;


//atomic operation to fill single descriptor.
static void _dma_config_desc(P_DMA_DESCRIPTOR p_des, 
							 P_DMA_DESCRIPTOR p_next_des, 
							 uint32_t stop, 
							 uint32_t branch_mode, 
							 uint32_t src, 
							 uint32_t trg, 
							 P_DMA_CMDx_T pCMD)
{
	uint32_t next_des_addr = (uint32_t)p_next_des;
	if(!stop){
		//clear the reserved bits.
		next_des_addr = next_des_addr & DMA_DDADR_RESERVED_MASK;
	}else{
		next_des_addr = 0;
	}

	next_des_addr |= ( (stop & 0x1) | ( (branch_mode&0x1) << 1) );

	//fill the descriptor entries.
	p_des->DDADR = (uint32_t)next_des_addr;
	p_des->DSADR = (uint32_t)src;
	p_des->DTADR = (uint32_t)trg;
	p_des->DCMD = (uint32_t)pCMD->value;

	return;
}

//no fetch mode.
void dma_set_reg_nf(uint32_t src, uint32_t trg, P_DMA_CMDx_T pCMD, uint32_t channel){
	DMA->DMA_DESC[channel].DSADR = src;
	DMA->DMA_DESC[channel].DTADR = trg;
	DMA->DMA_DESC[channel].DCMD = pCMD->value;

	return;
}

static void dma_irq_handler_default(){
	uint32_t t = 0;
	for(t=0; t<32; t++){
		//check irq channel
		if(DMA->DINT & (1<<t)){
			uint32_t dcsr_val = DMA->DCSR[t];
			DMA->DCSR[t] |= DMA_STATUS_INTERRUPTS_MASK;//clear all the interrupt.

			if(dcsr_val & CSR_BUSERRINTR){
				uart_printf("DMA: bus error on channel %d\r\n", t);
			} 
					   
			if(dcsr_val & CSR_STARTINTR){
				uart_printf("DMA: start int on channel %d\r\n", t);
			}
				
			if(dcsr_val & CSR_ENDINTR){
				uart_printf("DMA: end int on channel %d\r\n", t);
				DMA->DCSR[t] &= ~CSR_EORIRQEN;
			}

			if(dcsr_val & CSR_STOPINTR){
				uart_printf("DMA: stop int on channel %d\r\n", t);
				DMA->DCSR[t] &= ~CSR_STOPIRQEN;//disable the stop int.
			}
		}
	}

	return;	
}





/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Configure the descriptor entries.
 * p_des should be malloc ahead and the malloc size can be got by dma_get_des_num().
 * Descriptor, source and target address should align.
 */
void dma_config_desc(P_DMA_DESCRIPTOR p_des,
					 P_DMA_CMDx_T pCMD,
					 uint32_t src, 
					 uint32_t trg,
					 uint32_t bytes_len){
	uint32_t bytes_step = (uint32_t)pCMD->bits.Length;
	uint32_t des_num = bytes_len / bytes_step;
	uint32_t bytes_tail = bytes_len % bytes_step;

	uint32_t i = 0;
	uint32_t p_cur_des;
	uint32_t p_next_des = 0;


	if(bytes_tail != 0){
		des_num++;
	}else{
		bytes_tail = bytes_step;
	}

	i=0;
	p_next_des=0;
	p_cur_des= (uint32_t)p_des;

	for(i=0; i<des_num-1; i++){
		p_cur_des = (uint32_t)p_des + i*sizeof(DMA_DESCRIPTOR);
		p_next_des = (uint32_t)p_des + (i+1)*sizeof(DMA_DESCRIPTOR);
		_dma_config_desc((P_DMA_DESCRIPTOR)p_cur_des, (P_DMA_DESCRIPTOR)p_next_des, 0, 0, src, trg, pCMD);

		// For some cases, such as ssp fifo. FIFO address should set src_inc but the address may not increase.
		if(pCMD->bits.IncSrcAddr) src = src + pCMD->bits.Length;
		if(pCMD->bits.IncTrgAddr) trg = trg + pCMD->bits.Length;
	}

	//handle the tail bytes.
	p_cur_des = (uint32_t)p_des + (des_num-1)*sizeof(DMA_DESCRIPTOR);
	p_next_des = (uint32_t)p_des + des_num*sizeof(DMA_DESCRIPTOR);	
	pCMD->bits.Length = bytes_tail;
	_dma_config_desc((P_DMA_DESCRIPTOR)p_cur_des, (P_DMA_DESCRIPTOR)p_next_des, 1, 0, src, trg, pCMD);

	if(des_num>1)
		pCMD->bits.Length = bytes_step;//reset the pCMD.

	return;
}

void dma_set_des(P_DMA_DESCRIPTOR p_des, uint32_t channel){
	DMA->DMA_DESC[channel].DDADR = (uint32_t)p_des;

	return;
}

void dma_xfer_start(uint32_t channel){
	if(DMA->DCSR[channel]&CSR_RUN){
		uart_printf("DMA: channel %d is busy\r\n", channel);
		return;
	}
	DMA->DCSR[channel] |= CSR_RUN;

	return;
}

void dma_xfer_stop(uint32_t channel){
	if(DMA->DCSR[channel]&CSR_RUN){
		DMA->DCSR[channel] &= ~CSR_RUN;
	}else{
		uart_printf("DMA: channel %d is not running\r\n", channel);
	}

	return;
}

void dma_set_mode(uint32_t mode, uint32_t channel){
	DMA->DCSR[channel] &= ~CSR_NODESCFETCH;
	DMA->DCSR[channel] |= mode<<30;

	return;
}


uint32_t dma_get_des_num(uint32_t bytes_step, uint32_t bytes_len){
	uint32_t ret = bytes_len / bytes_step;
	uint32_t bytes_tail = bytes_len % bytes_step;
	if(bytes_tail != 0)
		ret++;

	return ret;
}

void dma_map_device_to_channel(DMA_MAP_DEVICE device_id, uint32_t channel){
	if(device_id < DMA_DEVICE_PART1_NUM){
		DMA->DRCMR1[device_id] |= (RCMR_MAPVLD | channel);
	}else{
		DMA->DRCMR2[device_id-DMA_DEVICE_PART1_NUM] |= (RCMR_MAPVLD | channel);
	}

	return;
}

#if 0
void dma_unmap_device_to_channel(DMA_MAP_DEVICE device_id, uint32_t channel)
{

	(void)channel; //avoid warning.
	if(device_id < DMA_DEVICE_PART1_NUM){
		DMA->DRCMR1[device_id] &= RCMR_CHLNUM_MSK;
	}else{
		DMA->DRCMR2[device_id-DMA_DEVICE_PART1_NUM] &= RCMR_CHLNUM_MSK;
	}

	return;
}
#endif


void set_user_aligment(uint32_t channel){
	DMA->DALGN |= 1<<channel;

	return;
}

uint32_t dma_is_channel_busy(uint32_t channel){
	uint32_t ret = 0;
	if(DMA->DCSR[channel] & CSR_STOPINTR){
		ret = 0;
	}else{
		ret = 1;
	}

	return ret;
}

void dma_clr_irq(uint32_t channel){
	uint32_t t = dma_is_channel_busy(channel);
	if(t){
		uart_printf("DMA: channel %d is busy and can not clear the interrupt\r\n", channel);
		return;
	}

	DMA->DCSR[channel] |= DMA_STATUS_INTERRUPTS_MASK;
	
	return;
}

//Read DMA DCSR register.
uint32_t dma_read_status(uint32_t channel){
	return DMA->DCSR[channel];
}


void dma_stop_irq_en(uint32_t channel){
	DMA->DCSR[channel] |= CSR_STOPIRQEN;
	
	return;
}
void dma_stop_irq_dis(uint32_t channel){
	DMA->DCSR[channel] &= ~CSR_STOPIRQEN;
	
	return;
}

void dma_set_dcsr(uint32_t val, uint32_t mask, uint32_t channel){
	DMA->DCSR[channel] &= ~mask;
	DMA->DCSR[channel] |= val;

	return;
}

void dma_connect_irq_handler(){
//	ISR_Connect(IRQ_MMP_DMA1, dma_irq_handler_default, NULL);
//	INT_Enable(IRQ_MMP_DMA1, 0, 15);

	return;
}
void dma_disconnect_irq_handler(){
//	ISR_Disconnect(IRQ_MMP_DMA1);
//	INT_Disable(IRQ_MMP_DMA1);

	return;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
uint32_t _dma_m2m_xfer(uint32_t des, uint32_t src, uint32_t dst, uint32_t size, 
					   uint32_t byte_per_desc, uint32_t burst_size, uint32_t channel){
	uint32_t ret = 1;
	
	//set command register.
	DMA_CMDx_T cmd;
	P_DMA_DESCRIPTOR descriptor_addr;
	cmd.value = 0;
	cmd.bits.IncSrcAddr = 1;
	cmd.bits.IncTrgAddr = 1;
	cmd.bits.MaxBurstSize = burst_size;
	cmd.bits.Length = byte_per_desc;

	descriptor_addr = (P_DMA_DESCRIPTOR)des;

	set_user_aligment(channel);
	dma_config_desc(descriptor_addr, &cmd, src, dst, size);
	dma_set_des(descriptor_addr, channel);

	//dma_connect_irq_handler();
	//dma_stop_irq_en(channel);

	dma_xfer_start(channel);	//DMA transfer begin

	return ret;
}




void dma_config_descriptor(uint32_t* p_desc,
					   uint32_t* p_next_desc,
					   uint32_t p_src_addr,
					   uint32_t p_tag_addr,
					   uint32_t p_cmd,
					   uint32_t stop_bit){
	((P_DMA_DESCRIPTOR)p_desc)->DDADR = ((uint32_t)p_next_desc | (stop_bit & 0x1));
	((P_DMA_DESCRIPTOR)p_desc)->DSADR = p_src_addr;
	((P_DMA_DESCRIPTOR)p_desc)->DTADR = p_tag_addr;
	((P_DMA_DESCRIPTOR)p_desc)->DCMD = p_cmd;

	return;
}

static void dmac_descriptor_fetch(uint32_t channel){
	uint32_t temp_data;

	temp_data = DMA->DCSR[channel];
	temp_data &= ~(1<<30);
	DMA->DCSR[channel] = temp_data;

	return;
}

void dma_load_descriptor(P_DMA_DESCRIPTOR p_desc, uint32_t channel){
	DMA->DMA_DESC[channel].DDADR = (uint32_t)p_desc;
	dmac_descriptor_fetch(channel);

	return;
}

void dma_enable(uint32_t channel){
	dma_xfer_start(channel);
}

int dma_wait_done(uint32_t channel, uint32_t timeout, uint32_t flags){
	int flag = 0;
	uint64_t count = 0;
	int ret = 1;

	switch(flags){
	case DMA_STOP:
		flag = CSR_STOPINTR;
		break;
	case DMA_END:
		flag = CSR_ENDINTR;
		break;
	default:
		uart_printf("Unsupport DMA done flag!!!\r\n");
		return 0;
	}

	while(!(DMA->DCSR[channel] & flag))
	{
#if 1	
		count++;
		if(count==timeout)
		{
			ret = 0;
			uart_printf("DMA wait done TIMEOUT !![DCSR:%0.8x]\r\n",DMA->DCSR[channel]);

			break;
		}
#endif		
	}

	return ret;
}















