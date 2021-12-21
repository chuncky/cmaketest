#include "Typedef.h"
#include "Errors.h"
//#include "bsp.h"
#include "PlatformConfig.h"
#include "NSSP.h"
#include <string.h>
#include "common.h"
#include "gpio.h"

//#include "global_types.h"
//#include "timer.h"
#include "ssp.h"
#include "ssp_dma.h"
#include "ssp_hw_access.h"
#include "ssp_host.h"
#include "spi_nor.h"
//#include "diag_API.h"

#define fatal_printf uart_printf

#define TS_TIMER_ID 2
#if 1
extern __align(32) UINT32 SlaveRxbuffer[512];
extern __align(32) UINT32 DummyTxbuffer[512];
extern __align(32) UINT32 Mux2SPITxBuffer[512];

#else
UINT32 Mux2SPITxBuffer[512]={0};
UINT32 SlaveRxbuffer[512]={0};
UINT32 DummyTxbuffer[512]={0};
#endif

/* timer function is common, move into updater/src/main.c */
extern unsigned long GetTimer0CNT(void);
#if 0
/* Timer0_0 is configured to free run @1MHz from BootROM. */
#define APBTIMER0_CNT_REG   0xD4014090
#define APBTIMER0_EN_REG    0xD4014000
#endif
unsigned long Timer0IntervalInMicro(unsigned long Before, unsigned long After)
{    
    unsigned long temp = (After - Before);    
	return temp;
}
#if 0
unsigned long GetTimer0CNT(void){
	return *(volatile unsigned long*)APBTIMER0_CNT_REG;
}
#endif
void DelayInMilliSecond(unsigned int us){
	unsigned long startTime, endTime;
	startTime = GetTimer0CNT();
	do    {
		endTime = GetTimer0CNT();
	}    while(Timer0IntervalInMicro(startTime, endTime) < us);
}
#if 0
void Timer0_Switch(unsigned char OnOff){
	if (OnOff)        
		*(volatile unsigned long*)APBTIMER0_EN_REG |= 0x1; //enable Timer0_0 (in free run)    
	else        
		*(volatile unsigned long*)APBTIMER0_EN_REG &= ~0x1; //disable Timer0_0 (in free run)
}
#endif
extern void Timer0_Switch(unsigned char OnOff); 
UINT32 timerCountRead(int tcr_number){
	//uart_printf("Counter read\r\n");
	return GetTimer0CNT();
}

typedef struct
{
	int					srdy_state;
	int					task_state;
	int					rx_handle_state;
	int					recv_int_count;
	int					useful_int_count;
	int					dma_done_count;
    UINT32              TxDMA_byte_count_reg_dump;
    UINT32              RxDMA_byte_count_reg_dump;
	UINT32				Spi_error_count;
	UINT32				spi_task_last_timestamp;
	UINT32              tx_mrdy_loss;

} SSP_STATUS;
SSP_STATUS	   ssp_status;
#define SSP0_BASE_ADDRESS			0xD401B000
static volatile SSPPortHWRegisters *SSPPortsRegistersArray[SSP_NUMBER_OF_PORTS] =
    {
        (volatile SSPPortHWRegisters *)(SSP0_BASE_ADDRESS), // start of SSP port no. 0 Registers Structure
        (volatile SSPPortHWRegisters *)(SSP_1_START_ADDRESS), // start of SSP port no. 1 Registers Structure
        (volatile SSPPortHWRegisters *)(SSP_2_START_ADDRESS), // start of SSP port no. 2 Registers Structure

    };


UINT32 ap2cpwrlen;
UINT8  ap2cpbuf[1024 * 2];
UINT32 ap2cpwrflag = 0;
#define  BUFFER_SIZE 			512
#define  SSP_PACKET_TAIL_CHECK  (0x7f7f7f7f)
BOOL INT_FROM_MASTER= FALSE;
BOOL SLAVE_SEND= FALSE;
BOOL DMA_DONE= FALSE;
UINT32 ssp_state = 0;
UINT32 SRDY_GPIO_NUM = 1/*79*/;
//static UINT32 MRDY_GPIO_NUM = 0/*80*/;
int srdy_int_count = 0;
void mrdy_srdy_gpio_init(void)
{
       //cs,mrdy
    //BU_REG_WRITE(0xd401e110, 0x1081);//pad int, cs
    //srdy
    BU_REG_WRITE(0xD401E0E0, 0x1080);//gpio1
    cgpio_set_output(SRDY_GPIO_NUM);
	uart_printf("L\r\n");
    cgpio_set_value(SRDY_GPIO_NUM, 0);
	Timer0_Switch(1);
}
typedef struct
{
	//UINT32				configurationSSCR0;
	//UINT32				configurationSSCR1;
	
	UINT32				configurationTOPCTRL;
	UINT32				configurationFIFOCTRL;
	UINT32				configurationINTEN;
	UINT32              configurationRWOT_CTRL;

    UINT32              DMADescriptorSize;

    UINT32              *dma_tx_descriptor;
    UINT32              *dma_rx_descriptor;
    UINT32              num_of_tx_descriptor;
    UINT32              num_of_rx_descriptor;
	UINT32				*pRxBuffer;
	int					rxBufferLen;
	int					rxBufferIndex;

	UINT32				*pTxBuffer;
	int					txBufferLen;    // total length
	int					txBufferIndex; //send index, send index++ when send 1, if sendindex==txBufferLen, send doen

	UINT32				*pRxPoolBase;			// pointer to Rx pool
	int					rxPoolIndex;			// maintain index of Rx buffer in use
	int					rxPoolNumberOfBuffers;	// maintain index of Rx buffer in use
	int					rxPoolLastReadIndex;	// maintain index of Rx buffer read by client
	BOOL				enabled;
} SSPDataBase;
static SSPDataBase				ssp_data[SSP_NUMBER_OF_PORTS];
typedef struct
{
	UINT8		Master_Got_MRDY;				 		 	
    UINT8      	Master_RTS;                   
    UINT8      	Master_have_more;                  
    UINT32      Master_payload_len;             
} SSP_Master;

SSP_Master ssp_master;
void ProcessRecvHeader_assert(UINT32* Header)			
{
	UINT32 i, SPIHeader = 0;
	SPIHeader = *Header;
	char* ptr;

	ssp_master.Master_RTS = (SPIHeader&0x40000000) >> 30;
	ssp_master.Master_have_more = (SPIHeader&0x00001000) >> 12;
	ssp_master.Master_payload_len = (SPIHeader&0x00000fff);
    //uart_printf("ProcessRecvHeader_assert :%d:%d:%:%d:%d\n", ssp_master.Master_payload_len, *(Header+1),*(Header+2),*(Header+3));
    if(*(Header+511) != SSP_PACKET_TAIL_CHECK) {
		uart_printf("spi recv data err, reinit spi\n");
		//spi_rw_test_init();
		return;
    }
	if(ssp_master.Master_payload_len)
	{
		//ssp_print("recv_data=%s\r\n", (Header + 1));	
		//MuxHalRxTransactionCompleted(Header+1,ssp_master.Master_payload_len);
		//if(assert_flag == 1)
		//	SspEeSendEehHalRcvEvent();
		ap2cpwrlen = ssp_master.Master_payload_len;
		memcpy(ap2cpbuf, Header + 1, ssp_master.Master_payload_len);
		Header  = ap2cpbuf;
        //uart_printf("ProcessRecvHeader_assert :%d:%d:%:%d:%d\n", ssp_master.Master_payload_len, *(Header),*(Header+1),*(Header+2));
	}
	//ssp_print("prh=%d\r\n",ssp_master.Master_payload_len);

}


void spiSRDYActive(void)
{
    cgpio_set_value(SRDY_GPIO_NUM, 1);

	//SSP_TRACE(SSP,__FUNCTION__,__LINE__,SSP_DEBUG_LEVEL,"SRDY is Active\r\n");
	ssp_status.srdy_state=TRUE;
    srdy_int_count++;
}
                                                            
void spiSRDYInactive(void)
{

    cgpio_set_value(SRDY_GPIO_NUM, 0);	
	//SSP_TRACE(SSP,__FUNCTION__,__LINE__,SSP_DEBUG_LEVEL,"SRDY is InActive\r\n");
	ssp_status.srdy_state=FALSE;


	

}
void SSP_GPIO_clear_wakeup(void)
{
	//ryz*(volatile UINT32 *)GPIO34_FRM_ADDRESS = (DRV_MED_C | AF0_C| Raising_Edge_Detect | (1<<6));//mrdy
	//ryz*(volatile UINT32 *)GPIO34_FRM_ADDRESS = (DRV_MED_C | AF0_C| Raising_Edge_Detect);//mrdy
	UINT32 reg = *(volatile UINT32 *)GPIO13_ADDRESS;
	//uart_printf("cs irq2:%x\n", reg);
    *(volatile UINT32 *)GPIO13_ADDRESS= reg | (1<<6);//mrdy
    *(volatile UINT32 *)GPIO13_ADDRESS= reg & (~(1<<6));
	reg = *(volatile UINT32 *)GPIO13_ADDRESS;
	//uart_printf("cs irq3:%x\n", reg);
	//*(volatile UINT32 *)GPIO13_ADDRESS= (DRV_MED_C | AF0_C| Falling_Edge_Detect);//mrdy


	
	//*(volatile UINT32 *)GPIO34_FRM_ADDRESS = (DRV_MED_C | AF0_C| Raising_Edge_Detect | (1<<6));//mrdy

}
//extern UINT32 dma_spi_send_buf[512];
//extern UINT32 dma_spi_recv_buf[512];

SSP_ReturnCode SSPElementsSend_assert(SSP_Port portNumber,void *pRxBuffer, void *pTxBuffer, UINT32 numberOfElements)
{
	int i;
	/* Warning: Don't print log here. Otherwise, resulting in send and receving not sync */
	//uart_printf("SSPElementsSend len:%d\n", numberOfElements);
	#if 0
	for(i=0;i<512;i++)
	{
	    send[i] = i;
	}
    #endif

#if 0	
    uart_printf(" \r\n ",send[i]);
	for(i=0;i<512;i++)
	{
	    uart_printf(" %ld ",send[i]);
	}
	
    uart_printf(" \r\n ",send[i]);

	for(i=0;i<512;i++)
	{
	    dma_spi_recv_buf[i] = 0;
	}
#endif

	//uart_printf("SSPElementsSend data:%d,%d,%d,%d\n", send[0], send[1], send[2],send[3]);
	//uart_printf("send header info:0x%x\r\n",*(send));
	//uart_printf("send tail info:0x%x\r\n",*(send + 511));
	spi_dma_transfer(portNumber,pRxBuffer,pTxBuffer,numberOfElements);
	#if 0
    while(numberOfElements--) {
        spi_read_write_word(recv++, send++);
    }
    #endif
    #if 0
	memcpy(pRxBuffer,dma_spi_recv_buf,numberOfElements*4);
#if 0	

	for(i=0;i<512;i++)
	{
	    uart_printf(" %ld ",dma_spi_recv_buf[i]);
	}
#endif

	//uart_printf("SSPElementsSend Recv data:%d,%d,%d,%d\n", recv[0], recv[1], recv[2],recv[3]);
	ProcessRecvHeader_assert(pRxBuffer);
	//uart_printf("SSPElementsSendx len:%d\n", numberOfElements);
    #endif
}
void sspStateDump(void)
{
		UINT32		imageOfSSSR = 0;
  #if 0 //ryz
		volatile  SSPPortHWRegisters	*portRegister = SSPPortsRegistersArray[2];
		
		imageOfSSSRdump = portRegister->SSSR;
		imageOfSSSR = portRegister->SSSR;

		ssp_status.TxDMA_byte_count_reg_dump=DmaHwRegisters.ChannelRegister[ssp_dma_channel[2].rxChannel].DCMD&0x000001fff;
		ssp_status.RxDMA_byte_count_reg_dump=DmaHwRegisters.ChannelRegister[ssp_dma_channel[2].rxChannel].DCMD&0x000001fff;
		rxdma_len=ssp_status.RxDMA_byte_count_reg_dump;
		txdma_len=ssp_status.TxDMA_byte_count_reg_dump;
		
		//uart_enable=1;
		uart_printf("SSSR=0x%x\r\n",imageOfSSSR);
		uart_printf("srdy_state=%d\r\n",ssp_status.srdy_state);
		uart_printf("task_state=%d\r\n",ssp_status.task_state);
		uart_printf("recv_int_count=%d\r\n",ssp_status.recv_int_count);
		uart_printf("useful_int_count=%d\r\n",ssp_status.useful_int_count);
		uart_printf("dma_done_count=%d\r\n",ssp_status.dma_done_count);
		uart_printf("TxDMA_byte_count_reg_dump=%d\r\n",ssp_status.TxDMA_byte_count_reg_dump);
		uart_printf("RxDMA_byte_count_reg_dump=%d\r\n",	ssp_status.RxDMA_byte_count_reg_dump);

		uart_printf("TUR=%d\r\n",	ssp_counter[2].TUR);
		uart_printf("error_count=%d\r\n",	ssp_counter[2].error_count);
		//uart_enable=0;
		#if 0
		DIAG_FILTER(SSP, sspStateDump, srdy_state, DIAG_INFORMATION) 
		diagPrintf("srdy_state=%d\r\n",ssp_status.srdy_state);

		DIAG_FILTER(SSP, sspStateDump, task_state, DIAG_INFORMATION) 
		diagPrintf("task_state=%d\r\n",ssp_status.task_state);

		DIAG_FILTER(SSP, sspStateDump, recv_int_count, DIAG_INFORMATION) 
		diagPrintf("recv_int_count=%d\r\n",ssp_status.recv_int_count);

		DIAG_FILTER(SSP, sspStateDump, useful_int_count, DIAG_INFORMATION) 
		diagPrintf("useful_int_count=%d\r\n",ssp_status.useful_int_count);

		DIAG_FILTER(SSP, sspStateDump, dma_done_count, DIAG_INFORMATION) 
		diagPrintf("dma_done_count=%d\r\n",ssp_status.dma_done_count);

		DIAG_FILTER(SSP, sspStateDump, TxDMA_byte_count_reg_dump, DIAG_INFORMATION) 
		diagPrintf("TxDMA_byte_count_reg_dump=%d\r\n",ssp_status.TxDMA_byte_count_reg_dump);

		DIAG_FILTER(SSP, sspStateDump, RxDMA_byte_count_reg_dump, DIAG_INFORMATION) 
		diagPrintf("RxDMA_byte_count_reg_dump=%d\r\n",	ssp_status.RxDMA_byte_count_reg_dump);
		#endif
    #endif
}
void SSP_error_recovery(SSP_Port 		portNumber)
{
	#if 0
		// declare local pointer for each SSP port
		volatile  SSPPortHWRegisters 	*portRegister = SSPPortsRegistersArray[portNumber];
		UINT32 dcsrReg;
		UINT32 cpsr;
		fatal_printf("SSP_error_recovery\r\n");
		// disabling SSP Serial Port:
        TOP_CTRL_SSE_DISABLE(portRegister->TOP_CTRL);

        SSSR_ALL_CLEAR(portRegister->SSSR);
				// enabling SSP Serial Port. FIFO is empty
		TOP_CTRL_SSE_ENABLE(portRegister->TOP_CTRL);

		//clearing all interrupt bits:
		SSSR_ALL_CLEAR(portRegister->SSSR);

		// make sure SSP disabled
		TOP_CTRL_SSE_DISABLE(portRegister->TOP_CTRL);
		
		
		cpsr = disableInterrupts(); //lock
		
		dcsrReg = DmaHwRegisters.DCSR[ssp_dma_channel[portNumber].txChannel]; 	// Get existing value
		
		// Clear out all read-only bits and RUN bit - this also will clear the interrupts if exists...
		dcsrReg &= DCSR_BIT_STOP_CHANNEL_MASK;
		
		DmaHwRegisters.DCSR[ssp_dma_channel[portNumber].txChannel] = dcsrReg;  // Write new value to HW
		
		dcsrReg = DmaHwRegisters.DCSR[ssp_dma_channel[portNumber].rxChannel]; 	// Get existing value
		
		// Clear out all read-only bits and RUN bit - this also will clear the interrupts if exists...
		dcsrReg &= DCSR_BIT_STOP_CHANNEL_MASK;
		
		DmaHwRegisters.DCSR[ssp_dma_channel[portNumber].rxChannel] = dcsrReg;  // Write new value to HW
			
		
		mrdy_int_count=0;
		restoreInterrupts(cpsr); //unlock
		spiSRDYInactive();
	#else
		uart_printf("ssp error recovery need to do............\r\n");
	#endif
}


#define EEH_SSP_DMA_TIMEOUT  (32*1024*5)  //1s read 32k timer

int Clear_For_MRDY()
{
		{
			/*clear intrrupt*/
		    SSP_GPIO_clear_wakeup();
			return 0;
		}
}

int Wait_For_MRDY()
{
	UINT32 t1=0,t2=0;
	t1= timerCountRead(TS_TIMER_ID);
    volatile UINT32 *addr = 0xD4019800;
	while(1)
	{
        //uart_printf("cs irq:%x:%x\n", *(volatile UINT32*) 0xD4019800, *(volatile UINT32*) 0xD4019804);
		if(*addr &(1<<13))
		{
			/*clear intrrupt*/
		   Clear_For_MRDY();
			//uart_printf("MRDY HISR\n");
			return 0;
		}
		t2=timerCountRead(TS_TIMER_ID);
		if(t2<t1){
			t1 = 0;
		}
		if((t2-t1)>=EEH_SSP_DMA_TIMEOUT) {
			//uart_printf("wait mrdy Timeout %lu:%lu\n", t1, t2);
			return -1;
		}
    }
}


int Wait_For_DMA_DONE()
{
	UINT32 pending;
	int i;
	UINT32 t1=0,t2=0;
	//ASSERT(0); //here need to do ryz
	t1=	timerCountRead(TS_TIMER_ID);
	//fatal_printf("Wait_For_DMA_DONEx\r\n");
	while(1)
	{
		pending = DmaHwRegisters.InterruptPendingRegister;
		for(i=0; i < 1; i++)
	    {
	        if( pending & 0x01 )
	        {
	            UINT32 dcsrReg = DmaHwRegisters.DCSR[i];
	            UINT32 clearBit = dcsrReg & DCSR_BIT_INTR_READ_MASK; //(DCSR_BIT_INTR_CLEAR_MASK | DCSR_BIT_RUN | DCSR_BIT_DESC_MASK);

	            if(dcsrReg & DCSR_BIT_END)
	            {
	                //uart_printf("DMA finish :%x::%x\n", DmaHwRegisters.DCSR[0], DmaHwRegisters.DCSR[1]);
					DmaHwRegisters.DCSR[i] = clearBit;
					return 0;
	            }
	           
	        }
	        pending >>= 1;
	    }
		t2=timerCountRead(TS_TIMER_ID);
		if((t2-t1)>=EEH_SSP_DMA_TIMEOUT) {
			uart_printf("Time %lu:%lu, %x:%x\n", t1, t2, DmaHwRegisters.DCSR[0],DmaHwRegisters.DCSR[1]);
			return -1;
		}
	}
}

void MakeSPIHeader(UINT32*Header,UINT32 len,BOOL more,BOOL flowctl)			
{
	UINT32	SPIHeader=0;
	{
		SPIHeader |=(flowctl<<30);
	}
	SPIHeader |=(more<<12);
	SPIHeader |=(len);

	

	*Header=SPIHeader;
}

void MakeSPIHeader_FlowCTS(UINT32*Header,int flowctl)			
{
    *Header &= ~(1<<30);
    *Header |=(flowctl<<30);
}
void ee_delay(int ms)
{
		volatile int i = 0;
		int j;
		for(j = 0; j < ms; j++) {
			for(i =0; i< 50000;i++)
			{
			}
		}
}


void memcpy_workaround(UINT8 *dst,UINT8 *src,UINT32 len)
{
	UINT32 i;

	for(i = 0; i< len; i++)
	{
		*(UINT8 *)dst=*(UINT8 *)src;
		dst++;
		src++;
	}
}


void SSPSlaveTxRxFunc_assert(UINT32* buf, UINT32 len)
{
	UINT32     	reg;
	int ret=0;
	UINT32	error_triggered=TRUE;
    //uart_printf("SSPSlaveTxRxFunc_assert\r\n");
 
	if((buf != NULL) && (len != 0))
		SLAVE_SEND = TRUE;
	else
		SLAVE_SEND = FALSE;

	//uart_printf("func:%s,line:%d,slave_send:%d\r\n",__func__,__LINE__,SLAVE_SEND);

	while(error_triggered)
	{
		//ssp_print("wukong recv func start loop\r\n");
       
        UINT32 t1=0,t2=0, t3=0, t4=0, t5 = 0,t6 = 0;
	    t1= GetTimer0CNT();
		if(SLAVE_SEND == TRUE){
			memcpy((UINT8 *)(Mux2SPITxBuffer + 1), (UINT8 *)buf, len);
		}

		if(SLAVE_SEND == FALSE)		//recv ack from AP
		{
			//uart_printf("waiting for MRDY\r\n");
			while(1) {
			    ret = Wait_For_MRDY();
				if(ret == 0) {
					//uart_printf("Slave recv mrdy \n");
					break;
				}
				else{
					/* Note: remove the following uart_log to wait for mrdy */
					//uart_printf("Slave read mrdy \n");
					ee_delay(10);
					//return;
				}
				ee_delay(10);
			}
		}
		
        //uart_printf("before active srdy\n");
        t2= GetTimer0CNT();
		/*  If use DMA in spi, can remove the following software delay */
		//ee_delay(1);
		t3= GetTimer0CNT();
		if(SLAVE_SEND == TRUE)
		{
			//uart_printf("D:%d\r\n",len);
			MakeSPIHeader(Mux2SPITxBuffer, len, 0, 0);
			Mux2SPITxBuffer[511]=SSP_PACKET_TAIL_CHECK;
			ssp_data[SSP_PORT_2].pTxBuffer = Mux2SPITxBuffer;	
			ssp_data[SSP_PORT_2].pRxBuffer = SlaveRxbuffer;	
			SSPElementsSend_assert(SSP_PORT_2, ssp_data[SSP_PORT_2].pRxBuffer, ssp_data[SSP_PORT_2].pTxBuffer, BUFFER_SIZE);
		}
		else						//recv ack from AP
		{	
			MakeSPIHeader(DummyTxbuffer,0,0,0);
			//uart_printf("M\r\n");
			/* NOTE: before send dummy packet to AP, need memset except tail 4 Bytes */
			memset((UINT8 *)DummyTxbuffer,0,512 * sizeof(UINT32));
			DummyTxbuffer[511]=SSP_PACKET_TAIL_CHECK;
			ssp_data[SSP_PORT_2].pTxBuffer = DummyTxbuffer;
			ssp_data[SSP_PORT_2].pRxBuffer = SlaveRxbuffer;	
			/* [debug] before read, print all pTxBuffer */
			#if 0
			UINT32 i = 0;
			for(i = 0; i <512; i++)
				uart_printf("i:%d  ",*(ssp_data[SSP_PORT_2].pTxBuffer + i));
				
			#endif
			SSPElementsSend_assert(SSP_PORT_2, ssp_data[SSP_PORT_2].pRxBuffer, ssp_data[SSP_PORT_2].pTxBuffer, BUFFER_SIZE);
		}

		
		spiSRDYActive();			//AP will wait for this SRDY signal, otherwise AP will not provide SPI clk
		
		if(SLAVE_SEND == TRUE)
		{
			ret = Wait_For_MRDY();
			if(ret != 0) {
				uart_printf("Slave send mrdy timeout\n");
				spiSRDYInactive();
				break;
			}
		}
        t4= GetTimer0CNT();

        extern void dmaWaitDone(void);
        dmaWaitDone();
        t5= GetTimer0CNT();
        spiSRDYInactive();
	    //uart_printf("SSPElementsSend Recv data:%d,%d,%d,%d\n", recv[0], recv[1], recv[2],recv[3]);
	    ProcessRecvHeader_assert(ssp_data[SSP_PORT_2].pRxBuffer);		//parse spi receive package

		/* work around for master receive error package */    
        static int i = 0;
		static int j = 0;
		//uart_printf("MJ%d:%d:%x:%x\r\n",i, j,*(ssp_data[SSP_PORT_2].pRxBuffer),*(ssp_data[SSP_PORT_2].pRxBuffer)&0x00000fff);
		if(SLAVE_SEND==FALSE){
			if((*(ssp_data[SSP_PORT_2].pRxBuffer)&0x00000fff)==0){
				i++;
			}
			if((i== (j+1)) && (i>5)) {
				i = 0;
				j = 0;
				Clear_For_MRDY();
				uart_printf("SPI recv cycle\r\n");
			}
			if(i != (j+1)){
				i = 0; 
				j = 0;
			}
				
		}
		if(i > 0)
			j++;

		//ret=Wait_For_DMA_DONE();		//Rx is the same with Tx. When Rx, Tx is also working.
		ret = 1;
		if(ret<0)
		{
			sspStateDump();
			ASSERT(0); 
			//here need to debug ryz	
 			SSP_error_recovery(SSP_PORT_2);
			/*re send last error packet */
			if(*(ssp_data[SSP_PORT_2].pTxBuffer)&0x00000fff)
			{
				uart_printf("re_send last packet for recovery\r\n");
				error_triggered=TRUE;
				SLAVE_SEND = TRUE;
				continue;
			}
			else
			{
				uart_printf("re_send empty packet for recovery\r\n");
				Mux2SPITxBuffer[0]=0;
				error_triggered=TRUE;
				SLAVE_SEND = TRUE;
				continue;
				

			}
		
		}

		//ProcessRecvHeader_assert(ssp_data[SSP_PORT_2].pRxBuffer);

		
		error_triggered=FALSE;
		t6= GetTimer0CNT();
		//uart_printf("T:%d:%d:%d:%d:%d\n", (t2-t1), (t3-t1), (t4-t1),(t5-t1), (t6-t1));
	}
}

UINT32 eeh_io_read(UINT8* ptr)
{
	UINT32 len = 0;
	UINT32 EehHal_flags_received;
	OSA_STATUS OSAStatus;
    ap2cpwrlen = 0;

	SSPSlaveTxRxFunc_assert(NULL, 0);
	UINT32 *x = ap2cpbuf;
	#if 0
	/* NOTE: debug use */
	UINT32 i = 0;
	static UINT32 count = 0;
	
	/* SlaveRxbuffer is all receive data by spi_dma interface in updater */
	// address 2 channel:  channel = (address-1)/4
	uart_printf("\r\ncount:%d,channel:%d\r\n",count++,(*((UINT8 *)SlaveRxbuffer + 5)-1)/4);
	//if((*((UINT8 *)SlaveRxbuffer + 5)-1)/4 == 4){
	//	uart_printf("\r\nif check data is channel 4, print all data\r\n");
		for(i=0; i<512;i++)
			uart_printf("j:%d  ",*(SlaveRxbuffer+i));
	//}
	#endif
	len = ap2cpwrlen;
	ap2cpwrflag = 0;
	memcpy(ptr, ap2cpbuf, len);
	
	
	return len;
}
UINT32 eeh_io_write(UINT8* ptr, UINT32 len)
{
	//memcpy(ap2cpbuf, ptr, len);
	//via spi interface

	//uart_printf("func:%s,len:%d\r\n",__func__,len);
	
	SSPSlaveTxRxFunc_assert(ptr, len);
	
	ap2cpwrlen = len;
	ap2cpwrflag = 1;

	return len;
}

