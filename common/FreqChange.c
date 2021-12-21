/******************************************************************************
**	(C) Copyright 2007 Marvell International Ltd. ?
**	All Rights Reserved
******************************************************************************/
#include "FreqChange.h"
#include "bsp.h"
//#include "acipc.h"
//#include "diag_API.h"
//#include <bsp.h>
//#include "utilities.h"
#include "pmic.h"
#include "Typedef.h"


extern UINT64 T32kHzBase;
extern UINT32 T32kCountLast;
extern int T32kCountInitialized;

extern UINT64 timerCountReadU64(void);
//extern rti_rt_t rti_rt;

#define FREQ_MODE	"[SYS ]"
#define RTI_CURRENT_TICK	timerCountReadU64()
//#define CHIP_IS_CRANE_Z1 (getLongChipId()==0XF06731)
/*
__inline void Delay_32k(UINT32 delay){
	UINT32 countBefore, countAfter, i;
	if(delay == 0)
		return;
	countBefore = timerCountRead(TS_TIMER_ID);
	while(1){
		countAfter = timerCountRead(TS_TIMER_ID);
		if(countAfter>countBefore){
			if(countAfter - countBefore >= delay)
				break;
		}else{
			countAfter += 0xFFFFFFFF;
			if(countAfter - countBefore >= delay)
				break;
		}
	}

}

__inline UINT32 Delta_32k_Tick(UINT32 initTick){
	UINT32 deltaTick = 0, currentTick = 0;

	currentTick = timerCountRead(TS_TIMER_ID);
	if(currentTick >= initTick){
		deltaTick = currentTick - initTick;
	}else{
		currentTick += 0xFFFFFFFF;
		deltaTick = currentTick - initTick;
	}
	return deltaTick;
}
*/

//#include "timer.h"
//#include "timer_config.h"
//#include "timer_hw.h"
//#include "osa.h"
//#include "global_types.h"
//#include "diag_API.h"
//#include "log.h"

#define CALC_DURATION 1

#define RUNNING_MODE_L	0
#define RUNNING_MODE_H	1
#define DDR_HIGH_TO_LOW_THRESHOLD (5)
#define RTI_ENABLE_THRESHOLD (10)
#define PS_DL_H  (3276800) 		//25mbps*1024*1024/8
#define PS_DL_L  (1310720)			//10mbps
#define PS_UL_H  (655360)			//5mbps
#define PS_UL_L  (262144)			//2mbps
extern UINT32 PsUploadDataSize;
extern UINT32 PsDownloadDataSize;
extern void GetRTISetting(int *enable, int *ht, int *lt);
extern void GetModeSetting(int *enable, int *ht, int *lt);
extern int CPUUsageInfo(UINT64 duration);
extern void DDRFrequencyChangeRequestForPlatform(DDR_FREQUENCY frequency);
extern void rti_set_enable(UINT32 enable);

UINT8 current_PP = 0x02;  //boot_PP

UINT32 RunningMode = RUNNING_MODE_L;

extern UINT32 DDRFreq_Id_Plat;
extern void tft_hash_table_clean(void);
extern UINT32 commpmdebuglog[36];

void FreqChangeUpdate(void)
{
#ifdef NEZHA3_1826
	UINT64 tick;
	UINT32 dummytick;//we need read 32K ts timer to update timer.last32kvalue when pmu is disable, wdt kick API need this value to be updated
	UINT32 usage;
	static UINT64 last_tick = 0;
	static UINT32 DDRHighToLowTriggerCount=0;
	static UINT32 RtiEnableTriggerCount=0;
	int rtiChange, rtiHT, rtiLT;
	int modeChange, modeHT, modeLT;

	GetRTISetting(&rtiChange, &rtiHT, &rtiLT);
	GetModeSetting(&modeChange, &modeHT, &modeLT);

	dummytick = timerCountRead(TS_TIMER_ID);
	tick = RTI_CURRENT_TICK;

	if (last_tick == 0)
	{
		last_tick = tick;
		return;
	}

	if (tick - last_tick > 13000000 * CALC_DURATION)
	{
#ifndef LWIP_IPNETBUF_SUPPORT
		tft_hash_table_clean();
#endif
		usage = CPUUsageInfo(tick - last_tick);
		last_tick = tick;

		/* change running mode */
		if (modeChange)
		{
            if ((usage == 0)||(usage > 100)) return; //neglect the case that TS read overflow.
			if ((RunningMode == RUNNING_MODE_L) && (usage <= modeHT))
			{
				/* switch to high speed */
				RunningMode = RUNNING_MODE_H;

				//log_printf(LOG_INFO FREQ_MODE "switch to high freq by CPU loading\n");
				DDRHighToLowTriggerCount=0;
				commpmdebuglog[32] = 12;

                CpCoreFreqChangeTo624();

				commpmdebuglog[32] = 13;
			}

			if (RunningMode == RUNNING_MODE_H)
			{
				if(usage >= modeLT)
				{
					DDRHighToLowTriggerCount++;

					if(DDRHighToLowTriggerCount>=DDR_HIGH_TO_LOW_THRESHOLD)
					{
						/* switch to low speed */
						RunningMode = RUNNING_MODE_L;
						DDRHighToLowTriggerCount=0;
						//log_printf(LOG_INFO FREQ_MODE "switch to low freq by CPU loading\n");
						
						commpmdebuglog[32] = 14;

                        CpCoreFreqChangeTo416();

						commpmdebuglog[32] = 15;
					}
				}
				else
				{
					DDRHighToLowTriggerCount=0;
				}
			}
		}

		/* change RTI */
		if (rtiChange)
		{
			if (RunningMode == RUNNING_MODE_H || !modeChange)
			{
				if (usage <= rtiHT)
				{
					RtiEnableTriggerCount=0;
					rti_set_enable(0);
				}

			}
			if (usage >= rtiLT)
			{
				RtiEnableTriggerCount++;
				if(RtiEnableTriggerCount>=RTI_ENABLE_THRESHOLD)
				{
					rti_set_enable(1);
					RtiEnableTriggerCount=0;
				}
			}else{
				RtiEnableTriggerCount=0;
			}
		}
	}
#endif
}

/************************** Added frequency change module code for Crane below. *********************************/
typedef struct
{
    volatile unsigned int  CC_CP;                //0x0
    volatile unsigned int  CC_AP;                //0x4
    volatile unsigned int  DM_CC_CP;             //0x8
    volatile unsigned int  DM_CC_AP;             //0xC
    volatile unsigned int  FC_TIMER;             //0x10
    volatile unsigned int  CP_IDLE_CFG;          //0x14
    volatile unsigned int  AP_IDLE_CFG;          //0x18
    volatile unsigned int  SQU_CLK_GATE_CTRL;    //0x1C
    volatile unsigned int  IRE_CLK_GATE_CTRL;    //0x20
    volatile unsigned int  RESERVED0;            //0x24
    volatile unsigned int  CCIC_CLK_GATE_CTRL;   //0x28
    volatile unsigned int  FBRC0_CLK_GATE_CTRL;  //0x2C
    volatile unsigned int  FBRC1_CLK_GATE_CTRL;  //0x30
    volatile unsigned int  USB_CLK_GATE_CTRL;    //0x34
    volatile unsigned int  RESERVED1[2];            //0x38-0x3C
    volatile unsigned int  PMU_CLK_GATE_CTRL;    //0x40
    volatile unsigned int  LTE_DMA_AXI2MC0_CLK_RES_CTRL;     //0x44
    volatile unsigned int  AXI_CLK_RES_CTRL;     //0x48
    volatile unsigned int  HSI_CLK_RES_CTRL;     //0x4C
    volatile unsigned int  CCIC_CLK_RES_CTRL;    //0x50
    volatile unsigned int  SDH0_CLK_RES_CTRL;    //0x54
    volatile unsigned int  SDH1_CLK_RES_CTRL;    //0x58
    volatile unsigned int  USB_CLK_RES_CTRL;     //0x5C
    volatile unsigned int  QSPI_CLK_RES_CTRL;      //0x60
    volatile unsigned int  DMA_CLK_RES_CTRL;     //0x64
    volatile unsigned int  AES_CLK_RES_CTRL;     //0x68
    volatile unsigned int  MCB_CLK_RES_CTRL;     //0X6C
    volatile unsigned int  CP_IMR;           //0x70
    volatile unsigned int  CP_IRWC;          //0x74
    volatile unsigned int  CP_ISR;           //0x78
    volatile unsigned int  SD_ROT_WAKE_CLR;      //0X7C
    volatile unsigned int  PMU_FBRC_CLK;         //0x80
    volatile unsigned int  PWR_STBL_TIMER;       //0X84
    volatile unsigned int  DEBUG_REG;            //0x88
    volatile unsigned int  SRAM_PWR_DWN;         //0x8C
    volatile unsigned int  CORE_STATUS;          //0x90
    volatile unsigned int  RES_FRM_SLP_CLR;      //0x94
    volatile unsigned int  AP_IMR;               //0x98
    volatile unsigned int  AP_IRWC;              //0x9C
    volatile unsigned int  AP_ISR;               //0xA0
    volatile unsigned int  DX8_CLK_RES_CTRL;     //0XA4
    volatile unsigned int  PMU_VPRO_PWRDWN;      //0xA8
    volatile unsigned int  DTC_CLK_RES_CTRL;     //0xAC
    volatile unsigned int  MC_HW_SLP_TYPE;       //0XB0
    volatile unsigned int  MC_SLP_REQ_AP;        //0XB4
    volatile unsigned int  MC_SLP_REQ_CP;        //0xb8
    volatile unsigned int  MC_SLP_REQ_MSA;       //0xbc
    volatile unsigned int  MC_MC_SLP_TYPE;       //0XC0
    volatile unsigned int  PLL_SEL_STATUS;       //0xc4
    volatile unsigned int  SYNC_MODE_BYPASS;     //0xC8
    volatile unsigned int  GC_CLK_RES_CTRL;      //0xCC
    volatile unsigned int  GC_PWRDWN;            //0xd0  GPU_3D_PWRDWN
    volatile unsigned int  SMC_CLK_RES_CTRL;     //0xD4
    volatile unsigned int  PWR_CTRL_REG;         //0xD8
    volatile unsigned int  PWR_BLK_TMR_REG;      //0xDC
    volatile unsigned int  SDH2_CLK_RES_CTRL;    //0xE0
    volatile unsigned int  SDH_NOM_DENOM_CTRL;      //0xE4
    volatile unsigned int  RESERVED2[2];            //0xE8-0XEC
    volatile unsigned int  PWR_STATUS_REG;       //0xF0
    volatile unsigned int  MCK4_CTRL;    //0xF4
    volatile unsigned int  RESERVED3[(0x100-0xF4)/4-1];
    volatile unsigned int  CC2_AP;               //0x100
    volatile unsigned int  CC2_CP;               //0x104
    volatile unsigned int  TRACE_CONFIG;         //0x108
    volatile unsigned int  CP_CLK_CTRL;          //0x10C
    volatile unsigned int  AP_CLK_CTRL;          //0x110
    volatile unsigned int  RESERVED4[(0x120-0x110)/4-1];
    volatile unsigned int  CA7MP_IDLE_CFG0;      //0x120
    volatile unsigned int  CA7_CORE0_IDLE_CFG;   //0x124
    volatile unsigned int  RESERVED5[(0x140-0x124)/4-1];
    volatile unsigned int  DVC_DEBUG;           //0x140
	volatile unsigned int  ACLK_CTL;            //0x144
	volatile unsigned int  DDR_CKPHY_PLL1_CTRL1;  //0x148
	volatile unsigned int  DDR_CKPHY_PLL1_CTRL2; //0x14c
    volatile unsigned int  RESERVED6[(0x15c-0x14c)/4-1];
	volatile unsigned int  CKPHY_FC_CTRL;         //0x15c
} PMUA_TypeDef;

#define     PMUA_BASE      0xD4282800
#define     PMUA ((  PMUA_TypeDef *)  PMUA_BASE )

typedef struct
{
   volatile unsigned int       APB_SPARE0;
   volatile unsigned int       RESERVED0[2];
   volatile unsigned int       SENSOR_V18;
   volatile unsigned int       RECERVED1[(0x100-0x0C)/4-1];
   volatile unsigned int       PLL1_SW_CTRL;//0X100
   volatile unsigned int       PLL1_SW_CTRL2;
   volatile unsigned int       PLL2_SW_CTRL2;
   volatile unsigned int       APB_spare4_reg;
   volatile unsigned int       ANA_GRP_SW;
   volatile unsigned int       APB_spare6_reg; //0x114
   volatile unsigned int       APB_spare7_reg; //0x118

}APBSPARE_TypeDef;

#define  APBSPARE_BASE  0xD4090000
#define  APBSPARE  (( APBSPARE_TypeDef *) APBSPARE_BASE )

unsigned int exp_cpcore_pclk_freq=0;
unsigned int exp_cpcore_bus_freq=0;
unsigned int exp_cpcore_mc_freq=0;
unsigned int exp_axi_clk_freq=0;
unsigned int fc_config_fix = 0;
unsigned int fc_config_fix_wifi = 0;
/* PP Semaphore*/
OSSemaRef PPSysRef = NULL;


void PP_Mutex_Lock(void)
{
    //OSA_STATUS status;
    if (PPSysRef != NULL) {
        //status = OSASemaphoreAcquire(PPSysRef, OS_SUSPEND);
        //ASSERT(status == OS_SUCCESS);    
    }
}


void PP_Mutex_Unlock(void)
{
    //OSA_STATUS status;
    if (PPSysRef != NULL) {
        //status = OSASemaphoreRelease(PPSysRef);
        //ASSERT(status == OS_SUCCESS);
    }
}


void PP_init(void)
{
    //OSA_STATUS status;
    //status = OSASemaphoreCreate (&PPSysRef, 1, OSA_FIFO);
    //ASSERT(status == OS_SUCCESS);

}

void CommPMSwitch(unsigned char ProductPoint)
{
//	if (ProductPoint < 5)
    	cpcore_fc_config(ProductPoint);
}

void vccmain_switch(unsigned int idx)
{
    switch (idx)
    {
        case 2:
            PlatformVcoreConfigLow();
            //set_cpu_dvc(CP_DVC_LV0);//416
            return;
        case 3:
            PlatformVcoreConfigLow();
            //set_cpu_dvc(CP_DVC_LV1);//499
            return;
        case 4:
            PlatformVcoreConfigHigh();
            //set_cpu_dvc(CP_DVC_LV2);//624
            return;
        case 5:
            PlatformVcoreConfigTop();
            //set_cpu_dvc(CP_DVC_LV3);//832
            return;
        default:
            PlatformVcoreConfigLow();
            //set_cpu_dvc(CP_DVC_LV0);
            return;
    }
}

void cpcore_fc_fix_bywifi(UINT8 OnOff)
{
    fc_config_fix_wifi = (OnOff)?1:0;
}
void cpcore_fc_config(unsigned int idx)
{
	UINT32 ts_now;

    if( (idx>5) || fc_config_fix || (current_PP == idx) || fc_config_fix_wifi)
    {
//         fatal_printf("=== Abort cpcore_fc_config: idx=%d, current_PP=%d, fc_config_fix=%d", idx, current_PP, fc_config_fix);
         //DIAG_FILTER(PM, FreqChange, cpcore_fc_config1, DIAG_INFORMATION)
         uart_printf ("=== Abort cpcore_fc_config: idx=%d, current_PP=%d, fc_config_fix=%d, fc_config_fix_wifi=%d.\n\r", idx, current_PP, fc_config_fix, fc_config_fix_wifi);
         return;
    }

	PP_Mutex_Lock();

    //apb_spare2[5:0]=0x3F: enable PLL1 307M & 350M & 409M & 491M & 614M & 819M
    APBSPARE->PLL1_SW_CTRL2 |= 0x3F;

    if (idx > current_PP){
		vccmain_switch(idx);
		//uart_printf("cpcore_fc_config before\r\n");
	}

        
    switch(idx)
    {
        case 0:
//            fatal_printf("cpcore clock change to PLL1_208\n\r");

            PMUA->CP_CLK_CTRL = (0x1<<9) | (0x1<<6) | (0x1<<3) | (0x0<<0);  //bus_clk_div=0x1, mem_clk_div=0x1, pclk_div=0x1

            exp_cpcore_pclk_freq              = 208;
            exp_cpcore_mc_freq                = 104;
            exp_cpcore_bus_freq               = 104;

            RunningMode = RUNNING_MODE_L;
            break;

        case 1:
//            fatal_printf("cpcore clock change to PLL1_312\n\r");

            PMUA->CP_CLK_CTRL = (0x1<<9) | (0x1<<6) | (0x0<<3) | (0x1<<0);  //bus_clk_div=0x1, mem_clk_div=0x1, pclk_div=0x0

            exp_cpcore_pclk_freq              = 312;
            exp_cpcore_mc_freq                = 156;
            exp_cpcore_bus_freq               = 156;

            RunningMode = RUNNING_MODE_L;
            break;

        case 2:
//            fatal_printf("cpcore clock change to PLL1_416\n\r");

            PMUA->CP_CLK_CTRL = (0x1<<9) | (0x1<<6) | (0x0<<3) | (0x0<<0);  //bus_clk_div=0x1, mem_clk_div=0x1, pclk_div=0x0

            exp_cpcore_pclk_freq              = 416;
            exp_cpcore_mc_freq                = 208;
            exp_cpcore_bus_freq               = 208;

            RunningMode = RUNNING_MODE_L;
            break;

        case 3:
//            fatal_printf("cpcore clock change to PLL1_499\n\r");

            PMUA->CP_CLK_CTRL = (0x1<<9) | (0x1<<6) | (0x0<<3) | (0x2<<0);  //bus_clk_div=0x1, mem_clk_div=0x1, pclk_div=0x0

            exp_cpcore_pclk_freq              = 499;
            exp_cpcore_mc_freq                = 249;
            exp_cpcore_bus_freq               = 249;

            RunningMode = RUNNING_MODE_H;
            break;

        case 4:
//            fatal_printf("cpcore clock change to PLL1_624\n\r");

            PMUA->CP_CLK_CTRL = (0x1<<9) | (0x1<<6) | (0x0<<3) | (0x3<<0);  //bus_clk_div=0x1, mem_clk_div=0x1, pclk_div=0x0

            exp_cpcore_pclk_freq              = 624;
            exp_cpcore_mc_freq                = 312;
            exp_cpcore_bus_freq               = 312;

            RunningMode = RUNNING_MODE_H;
            break;

        case 5:
//            fatal_printf("cpcore clock change to PLL1_832\n\r");

            PMUA->CP_CLK_CTRL = (0x1<<9) | (0x1<<6) | (0x0<<3) | (0x4<<0);  //bus_clk_div=0x1, mem_clk_div=0x1, pclk_div=0x0

            exp_cpcore_pclk_freq              = 832;
            exp_cpcore_mc_freq                = 416;
            exp_cpcore_bus_freq               = 416;

            RunningMode = RUNNING_MODE_H;
            break;
    }

	PMUA->CP_CLK_CTRL |= (1 << 12);
	while((PMUA->CP_CLK_CTRL & 0x1000) != 0x0);

    if (idx < current_PP){
		vccmain_switch(idx);
		//uart_printf("cpcore_fc_config after\r\n");
	}

	#if 0
	/*update  13HMZ counter with 32K timer, begin, gsfan*/
	if(T32kCountInitialized == 1)
	{
		ts_now = timerCountRead(TS_TIMER_ID);
		enable_performance_count();/// clear the performance_count
		if(ts_now > T32kCountLast)
		{
			T32kHzBase = ((T32kHzBase>>32)<<32) + ts_now;
		}
		else
		{
			T32kHzBase = (((T32kHzBase + 0xffffffff)>>32)<<32) + ts_now;
		}
		T32kCountLast = ts_now;
	}
	/*update  13HMZ counter with 32K timer, end, gsfan*/
	#endif
    current_PP = idx;
    PP_Mutex_Unlock();
    
	//DIAG_FILTER(PM, FreqChange, cpcore_fc_config, DIAG_INFORMATION)
	CP_LOGD ("CpCoreFreqChangeTo %d.\n\r", exp_cpcore_pclk_freq);
}

UINT32 get_cr5_cpu_freq(void)
{
	int currentCPFreq, clk_sel, divider;

    if (0 == exp_cpcore_pclk_freq)
    {
        divider = (PMUA->CP_CLK_CTRL >> 3) & 0x7;
        clk_sel = PMUA->CP_CLK_CTRL & 0x7;
        switch (clk_sel)
        {
            case 0:
                exp_cpcore_pclk_freq = 416 / (divider+1);
                break;
            case 1:
                exp_cpcore_pclk_freq = 312 / (divider+1);
                break;
            case 2:
                exp_cpcore_pclk_freq = 499 / (divider+1);
                break;
            case 3:
                exp_cpcore_pclk_freq = 624 / (divider+1);
                break;
            case 4:
                exp_cpcore_pclk_freq = 832;
                break;
            default:
                exp_cpcore_pclk_freq = 1; //not existing
                break;
        }
    }

	switch (exp_cpcore_pclk_freq)
    { //The actual signoff freq values are different.
       	case 208:
			currentCPFreq=204;
			break;
		case 312 :
			currentCPFreq=307;
			break;
		case 416 :
			currentCPFreq=409;
			break;
       	case 499:
			currentCPFreq=491;
			break;
		case 624 :
			currentCPFreq=614;
			break;
		case 832 :
			currentCPFreq=818;
			break;
		default:
			currentCPFreq=1; //not existing
			break;
	}
	return currentCPFreq;
}

UINT32 get_cpu_freq(void)
{
	return exp_cpcore_pclk_freq;
}

struct PsramPP_t
{
    const char *label;
    int pp;  //clock
    int pll; //clock selection
    int fcp; //FC point
    int dvc; //dvc level
};

static struct PsramPP_t pphy_pp[] = {
   /*label  pp,  pll,  fcp, dvc*/
    {"OP0", 156, 0x5,   0,   0},
    {"OP1", 208, 0x4,   0,   0},
    {"OP2", 312, 0x0,   1,   0},
    {"OP3", 350, 0x1,   1,   0},
    {"OP4", 416, 0x2,   1,   0},
    {"OP5", 26,  0x3,   1,   0},
};
#pragma arm section code="PSRAM_FC"

void psram_phy_fc_config(int phy_idx)
{
    unsigned int rd_data;
    unsigned int fc_trigger_bit = 24;	// Note: from CraneZ2, PHYCLK Frequency Change Request is bit24.
    if(phy_idx>4)
    {
         uart_printf("=== Error: psram_phy_fc_config, idx=%d", phy_idx);
         return;
    }
    //apb_spare2[2:0]=0x7: enable PLL1 832M & 624M & 499M
    APBSPARE->PLL1_SW_CTRL2 |= 0x3F;
    rd_data = PMUA->MC_HW_SLP_TYPE;

    CP_LOGD("PSRAM PHY frequency changed to %d\n\r", pphy_pp[phy_idx].pp);

    rd_data &= ~(0x7 << 26);
    rd_data |= (0x1 << 26); //fix PSRAM ctrl @350M

    rd_data &= ~(0x7 << 18);
    rd_data |= (pphy_pp[phy_idx].pll << 18);

    rd_data &= ~(0x7 << 4);
    rd_data |= (pphy_pp[phy_idx].fcp << 4);

    if (CHIP_IS_CRANEG_AX)
        rd_data |= ((1 << 8)|(1 << 6)); //fc for controller and phy, clk mode 2x
    else    
        rd_data |= (1 << 5); //fc for controller and phy

    PMUA->MC_HW_SLP_TYPE = rd_data;
    PMUA->MC_HW_SLP_TYPE |= 0x1<<fc_trigger_bit ; //phy_clk fc request
    while(PMUA->MC_HW_SLP_TYPE & (0x1<<fc_trigger_bit));

    CP_LOGD("PSRAM PHY frequency change done\n\r");
}
#pragma arm section code

void axi_clk_fc_config(unsigned int idx)
{
    //apb_spare2[5:0]=0x3F: enable PLL1 307M & 350M & 409M & 491M & 614M & 819M
    APBSPARE->PLL1_SW_CTRL2 |= 0x3F;

    switch(idx)
    {
        case 0: //156M
            uart_printf("aclk clock change to 156\n\r");
            PMUA->ACLK_CTL = 0x0<<0;
            exp_axi_clk_freq               = 156;
            break;
        case 1: //208M
            uart_printf("aclk clock change to 208\n\r");
            PMUA->ACLK_CTL = 0x1<<0;
            exp_axi_clk_freq               = 208;
            break;
       default:
            return;
    }

	PMUA->ACLK_CTL |= (1 << 4);
	while((PMUA->ACLK_CTL & 0x10) != 0x0);

}

static CP_DVC_LEVEL  CPDVCValue  = CP_DVC_NO_SET;

unsigned char CRN_SVC_FP[];

void cpu_dvc_init(void)
{
    /* NOTE: the following three stable time registers should be updated 
    according to real core voltage once chip SVC data is ready for use. */
	BU_REG_WRITE(PMU_DVC_STBL_0_1, (CRN_SVC_FP[1]-CRN_SVC_FP[0])*26); //stable time 0_1
	BU_REG_WRITE(PMU_DVC_STBL_1_2, (CRN_SVC_FP[2]-CRN_SVC_FP[1])*26); //stable time 1_2
	BU_REG_WRITE(PMU_DVC_STBL_2_3, (CRN_SVC_FP[3]-CRN_SVC_FP[2])*26); //stable time 2_3
	
	BU_REG_WRITE(PMU_DVC_EX_STR, 0x1A0); //extra stable time
	BU_REG_WRITE(PMU_DVC_IMR, 0x7); //enable DVC int
	BU_REG_WRITE(PMU_DVC_CTRL, 0x3); //enable DVC
}

void set_cpu_dvc(CP_DVC_LEVEL cp_dvc_val)
{
	volatile UINT32 tmp;

	if(cp_dvc_val == CPDVCValue)
		return;
	CPDVCValue = cp_dvc_val;

//    uart_printf("set_cpu_dvc %d.\r\n", cp_dvc_val);

	tmp = BU_REG_READ(PMU_DVC_CP);
	tmp &= (~(0xf<<4));
	
	switch (cp_dvc_val)
    {
       	case CP_DVC_LV0:
             break;
		case CP_DVC_LV1 :
             tmp |= (0x1<<4);
			 break;
		case CP_DVC_LV2:
			 tmp |= (0x2<<4);
             break;
		case CP_DVC_LV3 :
             tmp |= (0x3<<4);
             break;
		default:
			 return;
	}
	
	BU_REG_WRITE(PMU_DVC_CP, (tmp |(0x1<<7)));
	while (!(BU_REG_READ(PMU_DVC_ISR) & (0x1<<1))) ;// wait until DVC change is done
	BU_REG_WRITE(PMU_DVC_ISR, 0x5);	//only clear bit1 of CP DVC done	
}
//ICAT EXPORTED FUNCTION - PM, FreqChange, CpCoreGetFreq
void CpCoreGetFreq(void)
{
	uart_printf("CpCoreFreq = %d.\r\n", exp_cpcore_pclk_freq);

	//DIAG_FILTER(PM, FreqChange, CpCoreGetFreq, DIAG_INFORMATION)
	uart_printf ("CpCoreFreq = %d.\n\r", exp_cpcore_pclk_freq);
}

//ICAT EXPORTED FUNCTION - PM, FreqChange, CpCoreFreqChangeTo416
void CpCoreFreqChangeTo416(void)
{
	cpcore_fc_config(2);

	//DIAG_FILTER(PM, FreqChange, CpCoreFreqChangeTo416, DIAG_INFORMATION)
	CP_LOGD("CpCoreFreqChangeTo416.\r\n");
}

//ICAT EXPORTED FUNCTION - PM, FreqChange, CpCoreFreqChangeTo624
void CpCoreFreqChangeTo624(void)
{
	cpcore_fc_config(4);

	//DIAG_FILTER(PM, FreqChange, CpCoreFreqChangeTo624, DIAG_INFORMATION)
//	uasrt_printf ("CpCoreFreqChangeTo624.\n\r");
}

//ICAT EXPORTED FUNCTION - PM, FreqChange, CpCoreFreqChangeTo832
void CpCoreFreqChangeTo832(void)
{
	cpcore_fc_config(5);

	//DIAG_FILTER(PM, FreqChange, CpCoreFreqChangeTo832, DIAG_INFORMATION)
	uart_printf ("CpCoreFreqChangeTo832.\n\r");
}

//ICAT EXPORTED FUNCTION - PM, FreqChange, PsramPhyFreqChangeTo156
void PsramPhyFreqChangeTo156(void)
{
	psram_phy_fc_config(0);

	//DIAG_FILTER(PM, FreqChange, PsramPhyFreqChangeTo156, DIAG_INFORMATION)
	uart_printf ("PsramPhyFreqChangeTo156.\n\r");
}

//ICAT EXPORTED FUNCTION - PM, FreqChange, PsramPhyFreqChangeTo208
void PsramPhyFreqChangeTo208(void)
{
	psram_phy_fc_config(1);

	//DIAG_FILTER(PM, FreqChange, PsramPhyFreqChangeTo208, DIAG_INFORMATION)
	uart_printf ("PsramPhyFreqChangeTo208.\n\r");
}

//ICAT EXPORTED FUNCTION - PM, FreqChange, PsramPhyFreqChangeTo312
void PsramPhyFreqChangeTo312(void)
{
	psram_phy_fc_config(2);

	//DIAG_FILTER(PM, FreqChange, PsramPhyFreqChangeTo312, DIAG_INFORMATION)
	uart_printf ("PsramPhyFreqChangeTo312.\n\r");
}

//ICAT EXPORTED FUNCTION - PM, FreqChange, PsramPhyFreqChangeTo350
void PsramPhyFreqChangeTo350(void)
{
	psram_phy_fc_config(3);

	//DIAG_FILTER(PM, FreqChange, PsramPhyFreqChangeTo350, DIAG_INFORMATION)
	uart_printf ("PsramPhyFreqChangeTo350.\n\r");
}

//ICAT EXPORTED FUNCTION - PM, FreqChange, PsramPhyFreqChangeTo416
void PsramPhyFreqChangeTo416(void)
{
	psram_phy_fc_config(4);

	//DIAG_FILTER(PM, FreqChange, PsramPhyFreqChangeTo416, DIAG_INFORMATION)
//	uart_printf ("PsramPhyFreqChangeTo416.\n\r");
}

//ICAT EXPORTED FUNCTION - PM, FreqChange, AxiFreqChangeTo208
void AxiFreqChangeTo208(void)
{
	axi_clk_fc_config(1);

	//DIAG_FILTER(PM, FreqChange, AxiFreqChangeTo208, DIAG_INFORMATION)
	uart_printf ("AxiFreqChangeTo208.\n\r");
}

static void se_clock_on(void)
{
    unsigned long reg;
    reg =  BU_REG_READ(REG_MAIN_CCGR);
    reg |= 0x1000;
    BU_REG_WRITE(REG_MAIN_CCGR, reg);

    reg =  BU_REG_READ(SE_CLOCK_REG);
    reg |= 0x09; // 0x08;  TODO check reset 
    BU_REG_WRITE(SE_CLOCK_REG, reg);
}

static void se_clock_off(void)
{
    BU_REG_WRITE(SE_CLOCK_REG, 0x0);
}

unsigned int efuse_dro( void)
{
    unsigned long buffer;
    static unsigned long dro_val = 0;//svt_val 

    se_clock_on();
    buffer = BU_REG_READ(REG_GEU_GEU_FUSE_BANK3_255_224);
    dro_val = (buffer >> 19) & 0x000000ff;
    se_clock_off();

    return dro_val;
}

void PlatformVcoreConfigForDro(void)
{
	switch(PMIC_ID_GET())
	{
		case PMIC_812:
			break;
		case PMIC_813:
			break;
	}
	return;
}
unsigned int read_dro(void)
{
	unsigned int val;
	unsigned int lvt_cyc,svt_cyc;
    unsigned int svt_dro = 0;
	unsigned int dro_efuse;

	//read efuse dro
	dro_efuse = efuse_dro();

	//if efuse dro exists, only use efuse
	if(dro_efuse != 0)
	{
        svt_dro = dro_efuse;
        uart_printf("dro_efuse = %d\r\n", dro_efuse);
		return svt_dro;
	}

	//Set core voltage
	PlatformVcoreConfigForDro(); //get DRO under 1.05
	//trace("set vcc_main as [%6.2f]mV!!!\n", (double)start_vol/1000);

	//float lvt,hvt,svt;

	//Enable DRO clock
	*((volatile unsigned int *)REG_APBC_APBC_DRO_CLK_RST)=0x5;
	*((volatile unsigned int *)REG_APBC_APBC_DRO_CLK_RST)=0x1;

	//Enable DRO
	*((volatile unsigned int *)REG_DRO_CONTROLIS_DRO_CNT_TIME) = 0x1FFF;
	*((volatile unsigned int *)REG_DRO_CONTROLIS_DRO_CTRL) = 0x3;

	//Wait for dro timer done
	do{
		val=*((volatile unsigned int *)REG_DRO_CONTROLIS_DRO_CNT_TIME);
	}while((val&1<<31)!=(1<<31));
	*((volatile unsigned int *)REG_DRO_CONTROLIS_DRO_CTRL) = 0x2;

	//Get svt cycle number
	svt_cyc = *((volatile unsigned int *)REG_DRO_CONTROLIS_DRO_SVT);
	uart_printf("DRO_SVT_REG=0x%x\n",svt_cyc);
	svt_cyc = (((svt_cyc>>16)&0xffff) + (svt_cyc&0xffff))/2;

	//Calculate dro instric freq
    svt_dro = (int)(svt_cyc/315);
	uart_printf("SVT %d vs TYP 149\n",svt_dro);

    return svt_dro;

}

const unsigned char SVC_LVL_TBL_CRN[NUM_PROFILES_CRN][NUM_SVC_LVL_CRN] =
 {
	//Profile SVT_DRO           416MHz          499MHz          624MHz          832MHz          
	/*0       156-163*/	{  VOLTAGE_0_9375,  VOLTAGE_0_9375, VOLTAGE_0_9875, VOLTAGE_1_025   },
	/*1       148-155*/	{  VOLTAGE_0_9375,  VOLTAGE_0_95,   VOLTAGE_1_0125, VOLTAGE_1_0375  },
	/*2       140-147*/	{  VOLTAGE_0_95,    VOLTAGE_0_9625, VOLTAGE_1_025,  VOLTAGE_1_0625  },
	/*3       132-139*/	{  VOLTAGE_0_975,   VOLTAGE_0_975,  VOLTAGE_1_05,   VOLTAGE_1_0875  },
	/*4       124-131*/	{  VOLTAGE_0_9875,  VOLTAGE_1_00,   VOLTAGE_1_0625, VOLTAGE_1_10    },
	/*5       116-123*/	{  VOLTAGE_1_00,    VOLTAGE_1_0125, VOLTAGE_1_0875, VOLTAGE_1_125   },
	/*6       108-115*/	{  VOLTAGE_1_025,   VOLTAGE_1_0375, VOLTAGE_1_10,   VOLTAGE_1_15    },
	/*7       100-107*/	{  VOLTAGE_1_0375,  VOLTAGE_1_05,   VOLTAGE_1_1125, VOLTAGE_1_1625  },
};

unsigned char CRN_SVC_FP[NUM_SVC_LVL_CRN] = {0};

int get_prf_num_by_dro_crn(unsigned int dro)
{
    int prf_num = -1;

	if ((dro < 100)||(dro > 163))
	{
        uart_printf("ERROR: Invalid dro=%d vs exp[100~163]\r\n", dro);
        return -1;
	}

    /* profile num 0~14, vs DRO 100~163, 8 as stepping */
	prf_num = 7 - (dro - 100)/8;
	uart_printf("profile num = %d\r\n", prf_num);

    return prf_num;
}

void Crane_Get_Svc_Level_Values(void)
{
    int profile = NUM_PROFILES_CRN; //out of range
    unsigned int i = 0, dro = read_dro();

    profile = get_prf_num_by_dro_crn(dro);
    if ((profile >= NUM_PROFILES_CRN)||(profile < 0))
    {
        ASSERT(0);
    }

    for (i = 0; i < NUM_SVC_LVL_CRN; i++)
    {
        CRN_SVC_FP[i] = SVC_LVL_TBL_CRN[profile][i];
        uart_printf("CRN_SVC_FP[%d] = 0x%x\r\n", i, CRN_SVC_FP[i]);
    }
}




