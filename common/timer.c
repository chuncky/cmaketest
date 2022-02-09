#include "common.h"

#include "bsp.h"
#include "intc.h"
#include "timer_hw.h"

TIMER_CONFIG_STR   Timers[MAX_TCR_NUMBER];      //The timers array

UINT32* pTStimer_Last32kValue =  &(Timers[TS_TIMER_ID].last32kValue);

INTC_InterruptSources Timer_INTC[MAX_TCR_NUMBER] =
{
	INTC_SRC_TIMER_CP1,
    INTC_SRC_TIMER_CP2,
    INTC_SRC_TIMER_CP3,
    INTC_HW_Tmr1,
    INTC_HW_Tmr2,
    INTC_HW_Tmr3,
};

#define TIMER_NO_1  INTC_SRC_TIMER_CP1
#define TIMER_NO_2  INTC_SRC_TIMER_CP2
#define TIMER_NO_3  INTC_SRC_TIMER_CP3

#define TIMER_INTCID(tcr_number)   (Timer_INTC[(tcr_number)])
#define     TmrController    (* (volatile struct TIMER_HW_REGISTERS *) TIMER_BASE_ADDRESS)

#define     TmrController_EXT          (* (volatile struct TIMER_HW_REGISTERS *) TIMER_EXT_BASE_ADDRESS)

#define     TmrController_(timer_id)   (* (volatile struct TIMER_HW_REGISTERS *)((timer_id >= TCR_EXT) ? TIMER_EXT_BASE_ADDRESS : TIMER_BASE_ADDRESS))
#define     TIMER_HW_ID(timer_id)      ((timer_id >= TCR_EXT) ? (timer_id - TCR_EXT) : timer_id)




void CPTimer1ClockOnOff(CLK_OnOff OnOff)           // PRM_SRVC_TIMER2_13M_GB
{                                                  // RSRC_TIMER2_13M_GB (0x1 << 16)

	UINT32 value;  
	value = REG_READ(TICER);
	if(OnOff == CLK_ON)
	{
		TURN_BIT_OFF(value, 1 << 2);
		TURN_BIT_ON (value,	(1 << 1) | (1 << 0));
	}
	else
	{
		TURN_BIT_OFF(value, (1 << 1) | (1 << 0));
		TURN_BIT_ON (value,	1 << 2);
	}
	REG_WRITE(TICER, value);

}
UINT32 WDT_USE_CP_TIMER =0;

void timerEnableClock(void)
{
	volatile UINT32 *reg;
//	if(!WDT_USE_CP_TIMER)  // if  nezha 
	{
		reg = (volatile UINT32 *)0xd403b030; //TICER
		*reg = 0;
	       reg = (volatile UINT32 *)0xd403b030; //TICER
		*reg = 7;
	       reg = (volatile UINT32 *)0xd403b030; //TICER
		*reg = 3;
		if(!WDT_USE_CP_TIMER)
        {
            reg = (volatile UINT32 *)0xD4050200;
            *reg = 4;
        }
		reg = (volatile UINT32 *)0xD4050200;
        *reg = 3;
	}
/*	else   //if 1920
	{
		reg = (volatile UINT32 *)0xd4080084; //TMR_CER  disable counter
		*reg = 0;
	}
*/
}





/******************************************************************************
* Function: timerPhase1Init
*******************************************************************************
* Description: System & Application commitment to Group/Handset manager
*
* Parameters:  None.
*
* Return value: None.
*
* Notes:
******************************************************************************/
void timerPhase1Init(void)
{
	UINT8 tcr_number;
	UINT8  tm_hw_id;

	uart_printf("file:%s,function:%s,line:%d\r\n", __FILE__,__func__,__LINE__);

	CPTimer1ClockOnOff(CLK_ON);

	timerEnableClock();
	
	uart_printf("file:%s,function:%s,line:%d\r\n", __FILE__,__func__,__LINE__);

	//********** initializing tcr data base ***********
	for (tcr_number=0; tcr_number < MAX_TCR_NUMBER; tcr_number++)
	{
    //configure timers interrupts to INTC package.

	//CP will use the timer 2 to replace the WDT timer and 
	//the interrupt number of the timer 1 and timer 2 is same.
	//So the interrupt routine of the timer 0 can only be set.
	
	uart_printf("file:%s,function:%s,line:%d\r\n", __FILE__,__func__,__LINE__);
	if(tcr_number == TCR_1 || tcr_number == TCR_2)
	    INTCConfigure(TIMER_INTCID(tcr_number), // configure TCRn - Match Timers 0,1,2, TimerNumber 0,1,2
	                  INTC_IRQ,
	                  INTC_HIGH_LEVEL); //INTC_RISING_EDGE
	uart_printf("file:%s,function:%s,line:%d\r\n", __FILE__,__func__,__LINE__);

    Timers[tcr_number].timerAddingValue = 0;
    Timers[tcr_number].timerNotificationPtr = NULL;
    Timers[tcr_number].busy            = FALSE;
    Timers[tcr_number].clock_mode      = CLK_32KHZ;  // default
	
    tm_hw_id = TIMER_HW_ID(tcr_number);
	// clear the match reg.
    TmrController_(tcr_number).tmr[tm_hw_id].match_reg_0 = 0;
    TmrController_(tcr_number).tmr[tm_hw_id].match_reg_1 = 0;
    TmrController_(tcr_number).tmr[tm_hw_id].match_reg_2 = 0;

	//clear interrupt
    TmrController_(tcr_number).ticr[tm_hw_id].tclr_0 = C_TICR_TIMER_CLEAR_INTERRUPT_AND_STATUS;
    TmrController_(tcr_number).ticr[tm_hw_id].tclr_1 = C_TICR_TIMER_CLEAR_INTERRUPT_AND_STATUS;
    TmrController_(tcr_number).ticr[tm_hw_id].tclr_2 = C_TICR_TIMER_CLEAR_INTERRUPT_AND_STATUS;
    
    //disabling relevant bit in Interrupt Enable register
    TmrController_(tcr_number).tier[tm_hw_id].tier_bits.ie_0 = C_TIER_TIMER_DISABLE_MATCH;
    TmrController_(tcr_number).tier[tm_hw_id].tier_bits.ie_1 = C_TIER_TIMER_DISABLE_MATCH;
    TmrController_(tcr_number).tier[tm_hw_id].tier_bits.ie_2 = C_TIER_TIMER_DISABLE_MATCH;
	
	}
	uart_printf("file:%s,function:%s,line:%d\r\n", __FILE__,__func__,__LINE__);	
    TmrController.tcmr.reg      = 0x0;
    TmrController.tcer          = 0x0;

} /* End of <timerPhase1Init> */
#define MAX_TICK_REGISTER		5

typedef unsigned long   TimeIn32KhzUnit;


typedef void (*TickCallbackPtr)(UINT32);
typedef TimeIn32KhzUnit (*SuspendCallbackPtr)(void);
typedef void (*PrepareTimeCallbackPtr)(void);



/*
*		Put all global variables in global struct - easier to control  less code
*		produced.
*/



static volatile UINT32 timerDebugLog[8]={0};
void dumpTimerReg(TCR_NUMBER tcr_number)
{
	memset((void *)timerDebugLog, 0xA5, sizeof(timerDebugLog));
	timerDebugLog[0] = TmrController_(tcr_number).tmr[TIMER_HW_ID(tcr_number)].match_reg_0;
	timerDebugLog[1] = TmrController_(tcr_number).tcr[TIMER_HW_ID(tcr_number)];
	timerDebugLog[2] = TmrController_(tcr_number).tsr[TIMER_HW_ID(tcr_number)].reg;
	timerDebugLog[3] = TmrController_(tcr_number).tplvr[TIMER_HW_ID(tcr_number)];
	timerDebugLog[4] = TmrController_(tcr_number).tccr.reg;
	timerDebugLog[5] = TmrController_(tcr_number).tcer;
	timerDebugLog[6] = TmrController_(tcr_number).tcmr.reg;
	timerDebugLog[7] = TmrController_(tcr_number).tilr[TIMER_HW_ID(tcr_number)].reg;

    uart_printf("-------dumpTimerReg %d--------\r\n",tcr_number);
    int i;
    for (i=0;i<8;i++)
    uart_printf("timerDebugLog[%d]=0x%x\r\n",i,timerDebugLog[i]);
    uart_printf("-------dumpTimerReg end --------\r\n");
}


volatile UINT32 os_timer_start_count = 0;
static UINT8 ts_timer_active = 0;

static void timerDelayTS(UINT32 ticks)
{
	volatile UINT32 ts;
	if(ts_timer_active)
	{
		ts = timerCountRead(TS_TIMER_ID);
		while((timerCountRead(TS_TIMER_ID) - ts) <= ticks);
	}

}






static void timerTCMRSet(TCR_NUMBER tcr_number , TCMR_COUNT_MODE tcmr_mode)
{
	UINT32 tcmr_val ;
    UINT8  tm_hw_id;

    ASSERT(tcr_number < MAX_TCR_NUMBER);

    tm_hw_id = TIMER_HW_ID(tcr_number);
    
	/* Read */
    tcmr_val  = TmrController_(tcr_number).tcmr.reg;

    switch (tcmr_mode)
    {
       case TCMR_DISABLE_MODE:
	    TmrController_(tcr_number).tplcr[tm_hw_id] = 0;
	    TmrController_(tcr_number).tcer &= ~(1 << tm_hw_id);
	    if(Timers[tcr_number].clock_mode == CLK_32KHZ)
	    {
			timerDelayTS(2);
	    }
		break;
		
       case TCMR_EN_FREERUN_MODE:
       case TCMR_EN_ONESHOT_MODE:	   	
	   	//TmrController.tplcr[tcr_number] = 0;

		tcmr_val |= (1 << tm_hw_id);
		TmrController_(tcr_number).tcmr.reg = tcmr_val;
		
	   	TmrController_(tcr_number).tcer |= (1 << tm_hw_id);
		if(Timers[tcr_number].clock_mode == CLK_32KHZ)
		{
			os_timer_start_count = timerGetCount(tcr_number);
		}
	   	
		break;
		
       case TCMR_EN_PERIOD_MODE:
	   	//TmrController.tplcr[tcr_number] = 1; 

		tcmr_val &= ~(1 << tm_hw_id);
		TmrController_(tcr_number).tcmr.reg  =  tcmr_val;
		
	   	TmrController_(tcr_number).tcer |= (1 << tm_hw_id);
		if(Timers[tcr_number].clock_mode == CLK_32KHZ)
		{
			os_timer_start_count = timerGetCount(tcr_number);
		}
		break;
		
       case TCMR_EN_SW_PERIOD_MODE:
	   	ASSERT(0);   //not support one shot mode.
		break;
       default:
            ASSERT(0);
            return;
    	}

}


UINT32  timerGetCount(TCR_NUMBER tcr_number)
{
	 volatile UINT32 reg_, reg; 

     reg_=TmrController_(tcr_number).tcvwr[TIMER_HW_ID(tcr_number)];
     while((reg=TmrController_(tcr_number).tcvwr[TIMER_HW_ID(tcr_number)])==reg_)
     {
        TmrController_(tcr_number).tcvwr[TIMER_HW_ID(tcr_number)] = 1;      //irits - write "1" for latch timer value before read.
        reg=TmrController_(tcr_number).tcvwr[TIMER_HW_ID(tcr_number)];
     }
	return reg; 
}

/******************************************************************************
* Function name:   timerManageResource
*******************************************************************************
*
* Description  : allocate or release the timer resource - 13Mhz clock
*
* Input args   : tcrNumber - the timer number
*								 action    - allocate or release the resource
*
* Output args  : none.
*
* Return value : none.
*
* Notes        : despite we check all timers, access to to GB timers should not occur via arbel timer driver.
*
*	Author       : Eli Levy IntelID# 10689543
******************************************************************************/
static void timerManageResource(TCR_NUMBER tcrNumber,PRM_AllocFreeE action)
{

}


static char hisrName[]="Timerx";

OS_HISR _timerHisrRefs  [MAX_TCR_NUMBER];

static void timerClearAndRestart(TCR_NUMBER tcr_number)
{
   TCMR_MODES real_mode ;
   UINT32 loopCnt = 0;
   ASSERT(tcr_number < MAX_HW_TCR_NUMBER);
   
   //Clearing the interrupt source / status
   // TmrController.tsr.reg  = (1<<tcr_number);   //elevy - not needed from HW perspective so we save GPB write access
   INTCClrSrc(Timer_INTC[tcr_number]);    /* clearing the interrupt */

   // Raviv debug 21/2/6
   //PM_EXT_DBG_EVENT_SEND(PM_EXT_DBG_EVENT_TIMER_STATUS_CLEAR);

   // The MSR register is not always updated when match happens (m.b. it does NOT when APB clock is off)
   // Therefore use only m_0 it caused the interrupt
   // Clear TSR (status) for the match
   TmrController_(tcr_number).ticr[TIMER_HW_ID(tcr_number)].tclr_0 = C_TICR_TIMER_CLEAR_INTERRUPT_AND_STATUS;  // clear Level interrupt and Status bit
   for(loopCnt = 0; loopCnt < 1000; loopCnt++)
   {
   		 /*coverity[overrun-local]*/
   		 if(TmrController_(tcr_number).tsr[tcr_number].reg == 0)
		 	break;
   }
   if(loopCnt == 1000)
   {
   		extern 	UINT32 get_int_status_3(void);
		if((get_int_status_3() & 0x80)!=0)
		{
   			read_arb_regs();
		}
		dumpTimerReg(tcr_number);
   }

   real_mode = Timers[tcr_number].real_mode;

	if(C_TCMR_ONE_SHOT_MODE == real_mode)
  {
		if (Timers[tcr_number].repetative_mode == C_TCMR_ONE_SHOT_MODE)
		{
				timerTCMRSet(tcr_number , TCMR_DISABLE_MODE);
        		Timers[tcr_number].busy = FALSE;        /* change by elevy from TCR_1 to tcr_number*/
        		timerManageResource(tcr_number,PRM_RSRC_FREE); /* if the timer use 13Mhz, send release notification to the RM */ /*add by elevy 09-07-05 18:01 */
    }
   	    else if (Timers[tcr_number].repetative_mode == C_TCMR_SW_PERIODIC_TCMR_MODE)
   	 		{
				timerTCMRSet(tcr_number , TCMR_DISABLE_MODE);
    			timerTCMRSet(tcr_number , (TCMR_COUNT_MODE)real_mode);
     		}//End C_TCMR_SW_PERIODIC_TCMR_MODE
  }//End C_TPLCR_FREE_RUNNING

} //timerClearAndRestart


typedef void (*SISR_Function)(void);
static UINT32   Sisr_3_Count = 0 ;


/******************************************************************************
* Function: timerNotify
*******************************************************************************
* Description:  This function is called by the INTC upon a TCR 0 experation
*               interrupt (match_timer#0 or match_timer#1 or match_timer#2).
* Parameters:
*
* Return value:  None.
*
* Notes:
******************************************************************************/
void timerNotify(TCR_NUMBER tcr_number)
{
	//uart_printf("%s-01\r\n",__func__);
   if (Timers[tcr_number].timerNotificationPtr != (void *)NULL)
   {
     Timers[tcr_number].timerNotificationPtr(Timers[tcr_number].cookie);  // send notification to the client...
   }
   //uart_printf("%s-02\r\n",__func__);
} //timerNotify

/******************************************************************************
* Function: timer_n_SisrRoutine
*******************************************************************************
* Description:   These are the HISR routines for Timer 0-3
*
* Parameters:   None.
*
* Return value:  None.
*
* Notes:  This function's LISR is "timersHandler"
******************************************************************************/
void timer_1_SisrRoutine(void)
{
  timerNotify(TCR_1);
} // End of <timer_1_SisrRoutine>

void timer_2_SisrRoutine(void)
{
  timerNotify(TCR_2);
} // End of <timer_2_SisrRoutine>

void timer_3_SisrRoutine(void)
{
  Sisr_3_Count++;
  timerNotify(TCR_3);
} // < End of timer_3_SisrRoutine >

void timer_4_SisrRoutine(void)
{
   timerNotify(TCR_4);
}

void timer_5_SisrRoutine(void)
{
  timerNotify(TCR_5);
}

void timer_6_SisrRoutine(void)
{
  timerNotify(TCR_6);
}


static const SISR_Function sisrRoutines[]= {
 timer_1_SisrRoutine
,timer_2_SisrRoutine
,timer_3_SisrRoutine
,timer_4_SisrRoutine
,timer_5_SisrRoutine
,timer_6_SisrRoutine
};


void timersHandler(INTC_InterruptInfo sourceNum)
{
  TCR_NUMBER tcr_number=(TCR_NUMBER)0;


  //uart_printf("%s,%d\r\n",__func__,sourceNum);

  switch (sourceNum)                    // which Non-Drowsy TCR expired ?
  {
    //case INTC_HW_Tmr1:/* HW IRQ [36] Timer1 */ //INTC_SRC_TIMER_AP1:    /* ICU [13] Timer1 */
    case TIMER_NO_1:
	   tcr_number=TCR_1;
	   
  		//uart_printf("%s-02\r\n",__func__);
       break;

    //case INTC_HW_Tmr2:/* HW IRQ [37] Timer1 *///INTC_SRC_TIMER_AP2:    /* ICU [14] Timer2 */
    case TIMER_NO_2:
	//case INTC_HW_Tmr3: /* HW IRQ [39] Timer1 *///INTC_SRC_TIMER_AP3:/* ICU [15] Timer3 Backup */	
    //case TIMER_NO_3:
	   if(TmrController.tsr[TCR_2].reg){
//		timersHandler_ts = timerCountRead(TS_TIMER_ID);    //hsy add
//		timersHandler_match = TmrController_(TS_TIMER_ID).tmr[TIMER_HW_ID(TS_TIMER_ID)].match_reg_0;
//		timersHandler_tier= TmrController_(TS_TIMER_ID).tier[TIMER_HW_ID(TS_TIMER_ID)].tier_bits.ie_0;
//		timersHandler_timer_id = TS_TIMER_ID;
//		timersHandler_ticr = TmrController_(TS_TIMER_ID).ticr[TIMER_HW_ID(TS_TIMER_ID)].tclr_0;
	   	tcr_number=TCR_2;
	   	}
	   else if(TmrController.tsr[TCR_3].reg)
	   {
	   	tcr_number=TCR_3;
	   }
	   else{
		 dumpTimerReg(tcr_number);
	   	 ASSERT(FALSE);
	   	}
       break;
    default:
      ASSERT(FALSE);
    break;
  } /* switch */
    //uart_printf("%s-03,\r\n",__func__,tcr_number);
    //dumpTimerReg(tcr_number);
  // Clear interrupt and Restart
  timerClearAndRestart(tcr_number);        // Check and restart if configured as periodic
	//uart_printf("%s-04\r\n",__func__);
  if(Timers[tcr_number].interrupt_required&TO_LISR_NOTIFICATION)
  {
      timerNotify(tcr_number);
  }
  else
  {
       //OS_STATUS  _OSstatus = OS_Activate_HISR(&_timerHisrRefs[tcr_number]);

       //ASSERT(_OSstatus == OS_SUCCESS);
	   sisrRoutines[tcr_number]();
  }
	//uart_printf("%s-05\r\n",__func__);
    //dumpTimerReg(tcr_number);
} //End of <timersHandler>


static const UINT32 TCCR_ClockSet[MAX_TCR_NUMBER][4] = 
{   
    {C_TCCR_1_CS_3_25MHZ, C_TCCR_1_CS_1KHZ, C_TCCR_1_CS_32KHz, C_TCCR_1_CS_EXT},
	{C_TCCR_2_CS_3_25MHZ, C_TCCR_2_CS_1KHZ, C_TCCR_2_CS_32KHz, C_TCCR_2_CS_EXT},
  	{C_TCCR_3_CS_3_25MHZ, C_TCCR_3_CS_1KHZ, C_TCCR_3_CS_32KHz, C_TCCR_3_CS_EXT},
};



/******************************************************************************
* Function: timerPhase2Init
*******************************************************************************
* Description: Create all OS related Objects
*
* Parameters:  None.
*
* Return value:  None.
*
* Notes:
******************************************************************************/
void timerPhase2Init(void)
{
  TCR_NUMBER tcr_number;
  //STATUS         status;

  // lets register wirh RM for LPM mode - prepare and recovery
  // before the system is going to enter D2, we will register the
  // timer wakeup source and on exit we will remove it.
  // The wakeup function itself does nothing, so we can remove it on D2 recover,
  // as this is called BEFORE the wakeup callback itself is activated (so RM will not
  // call it at all!).

  //PRMRegisterForNonRetainState(PRM_NONRETAINED_SRVC_TIMER, timerD2Prepare, timerD2Recover);

  for (tcr_number=(TCR_NUMBER)0; tcr_number < MAX_TCR_NUMBER; tcr_number++)
  {
			hisrName[strlen(hisrName)-1]='0'+tcr_number;
		    //creating HISR for all TCRs
			OS_Create_HISR(&_timerHisrRefs[tcr_number], hisrName, sisrRoutines[tcr_number], TIMER_HISR_PRIORITY );

            if(tcr_number != TCR_3)
    			INTCBind(TIMER_INTCID(tcr_number), timersHandler);
  }

	/* if needed in the future, we should register the ARBEL RM call back functions here  */ /* change by elevy 06-08-05 10:50 */

} // End of <timerPhase2Init>

/* special program declaration */
#define ACC_TIMER_NODE_MAX 32		/* maximum number of nodes */

	typedef void(*ACC_TIMER_CALLBACK)(UINT32);	/* Function pointer */

	struct timer
	{
		unsigned int start_time;				/* time to configure match */
		unsigned int end_time;					/* interrupt trigger time */
		unsigned int flag;						/* flag: determine whether the timer executes once or periodically */
		unsigned int period;					/* delta_time unit is us */
		unsigned int period_tick;               /* delta_time unit is tick */
		unsigned int magic;						/* node status: create, active, stop, empty */
		ACC_TIMER_CALLBACK timerCallbackFunc;	/* callback function pointer */
		unsigned int timerParams;				/* function parameter */
		unsigned int timer_id;                  /* timer id*/       
	};

	typedef struct timer Item;

	struct _ACC_TIMER_NODE				/* node structure */
	{
		struct _ACC_TIMER_NODE *previous;	/* front pointer */
		struct _ACC_TIMER_NODE *next;		/* back pointer */
		Item item;
	};

typedef struct _ACC_TIMER_NODE ACC_TIMER_NODE;

UINT32 TShandler_ts;
UINT64 TShandler_match_64;
UINT8 TShandler_timer_id;
UINT64 TShandler_ts_64;
UINT64 TShandler_end_time_64;
UINT32 TShandler_match;

   //add  global variables(more accurate timer)
ACC_TIMER_NODE ACCTimerNode[ACC_TIMER_NODE_MAX];		/* defines 32 nodes */

ACC_TIMER_NODE *active_list = NULL, *empty_list = NULL;	/* define two linked list and initialize them to NULL */

//2018-9-18 the new version
/******************************************************************************
* Function:TShandler
*******************************************************************************
* Description:  when match == count, execute the TShandler
*
* Parameters:   NONE
*                   
* Return value: NONE
*                                                
* Notes:  1.execute callback function untile end_time > ts +5 or the end of the lis       
*                 
*	                                    
******************************************************************************/	

void TShandler(void)
{		

	uart_printf("%s-01\r\n",__func__);

    unsigned int ts;                            /* declare a unsigned int variable in order to save the ts value*/
    UINT64 ts_64;                               //2018-8-24 compare in while loop
    UINT64 end_time_64;                         //2018-8-24  compare in while loop
    UINT64 scan_64;                             //2018-8-27

    UINT64 match_register_64;                   //2018-9-20 add to prevent overflow
    UINT32 match_register_32;                   //2018-10-26  save the match_register_value
    
    ACC_TIMER_NODE *qtimerlist;      /*save the header node's pointer, because when traverse the active_list will change the qtimerlist*/
    ACC_TIMER_NODE *save;                           /* when traverse the active_list, save the previous node */
    unsigned int cpsr;           
    ACC_TIMER_NODE *pTIMERREF;  //2018-8-20 test
    ACC_TIMER_NODE *scan,*save_periodic;

    cpsr = disableInterrupts();     //2018-10-31  close the interrupt to prevent interruptting
    
    ts = timerCountRead( TS_TIMER_ID );
    match_register_32 = TmrController_(TS_TIMER_ID).tmr[TIMER_HW_ID(TS_TIMER_ID)].match_reg_0;      //2018-10-26  save the match_register_value
    match_register_64 = (UINT64)TmrController_(TS_TIMER_ID).tmr[TIMER_HW_ID(TS_TIMER_ID)].match_reg_0; 

    if((ts > HALF_RANGE_32_BIT) && (match_register_64 < HALF_RANGE_32_BIT))    //2018-9-20 add to prevent overflow
        match_register_64 += RANGE_32_BIT; 
        
    TShandler_ts = ts;   //2018-9-20  add global variables to debug 
    TShandler_match_64 = match_register_64;  
    
    //1. prevent interruptions without calling functions;
    //2. if match_new > ts + 6 ,return
    //3. if match != active_list.item.end_time, return
    
    if(  ( active_list == NULL ) ||                                 //2018-9-20 add (UINT64) to prevent   match = 0xfffffffa and ts = 0xfffffffc     
         (  match_register_64 >  (UINT64)ts + TS_NODE_INTERVAL ) ||
         ( match_register_32 != active_list->item.end_time)          //2018-10-26  if enter TShandler match != active_list, return
       )
    {
        restoreInterrupts(cpsr);
        return;
    } 
           
    //old version   set ts_64 and end_time_64
    ts_64 = (UINT64)timerCountRead(TS_TIMER_ID) + 5;
    end_time_64 = (UINT64)active_list->item.start_time + (UINT64)active_list->item.period_tick;
    
    if((end_time_64 > HALF_RANGE_32_BIT) && (ts_64 < HALF_RANGE_32_BIT)) // avoid assert in the following. e.g. end_time = 0xFFFF_FFFFE, ts_64 = 5
        ts_64 += RANGE_32_BIT;  
        
    //2018-9-19 add global variable to debug
    TShandler_timer_id =  active_list->item.timer_id;   
    TShandler_ts_64 = ts_64;
    TShandler_end_time_64 = end_time_64;
    TShandler_match = TmrController_(TS_TIMER_ID).tmr[TIMER_HW_ID(TS_TIMER_ID)].match_reg_0;
    

	if( ts_64 < end_time_64 ){
	    restoreInterrupts(cpsr);
	    ASSERT(0);
	}   
	/* if ts >= count, execute callback function untile end_time > ts + 5 */
	//

    /* execute callback function until end_time > ts + 5 or the end of the list */
        
   /* traverse the active_list and find the end_time < ts + 5 node, then executing the node's callback and remove the node
    */
      /* 1.before traverse the active_list, read the current count value
              2. cyclic condition: 
                           (ptimerlist->item.end_time != 0) && (ptimerlist->item.end_time <= ts +2)
                            when the end_time is 0, meaning traversing the end of active_list
              3. loop body: 
                    1. execute the current node's callback                    
                    2. remove the current node(Caution: after removing the node, change the node's magic status )
                    3. refresh the pointer and ts
                    4. judge the node is periodic or auto_delete:
                        1.perioidc: add the node into active_list again;
                        2.auto_delete: delete this node, add the node into the empty_list

               4. after while loop, set match value again
             */ 
    for( qtimerlist = active_list; 
            ( qtimerlist != NULL ) && ( end_time_64 <= ts_64);       )         
	{
        //2018-10-31  after stop the timer, then execute callback function 
        //(*(qtimerlist->item.timerCallbackFunc))(qtimerlist->item.timerParams);	/*execute callback function*/
        
        save = qtimerlist;              /* save the previous node's pointer */
        qtimerlist = qtimerlist->next;  /* backward the qtimerlist */
        
        /* remove the node from the active_list*/
        if(save->previous == NULL)    /* meaning this node is header node*/
        {            
            if(save->next == NULL)    /* the active_list has only one node*/
            {
                //cpsr = disableInterrupts();     //2018-10-31  already closed the interruption
                            
               /* 2018-9-18 break the node chain from active_list*/
                active_list = NULL;         /* the active_list only has one node. After removing this node, active_list = NULL */

                save->item.magic = ACC_TIMER_STOP;  //  2018-10-31   need close the total interruption when execute this statement 
                
                TmrController_(TS_TIMER_ID).tier[TIMER_HW_ID(TS_TIMER_ID)].tier_bits.ie_0 = C_TIER_TIMER_DISABLE_MATCH;	//2018-8-19 when active_list == NULL, close timer interrupt
                           
                //restoreInterrupts(cpsr);      //2018-10-31
             }
             else    /*the active_list has more than one node*/
             {
                /* break the node chain from active_list*/
                //cpsr = disableInterrupts();       //2018-10-31  already closed the interruption
                //backward the active_list            
                active_list = save->next;                                            
                active_list->previous = NULL;

                //stop the save node from active_list
                save->previous = NULL;                             
                save->next = NULL;                                 

                save->item.magic = ACC_TIMER_STOP;  //  2018-10-31   need close the total interruption when execute this statement 
                
                //restoreInterrupts(cpsr);      //2018-10-31     
             }
                                    
         }


         restoreInterrupts(cpsr);      //2018-10-31
         
         (*(save->item.timerCallbackFunc))(save->item.timerParams);	//2018-10-31  after stop the timer, then execute callback function 
         
         cpsr = disableInterrupts();     //2018-10-31  close the interrupt to prevent interruptting

         //  2018-10-31   need close the total interruption when execute this statement 
         //save->item.magic = ACC_TIMER_STOP; /* after removing the node, change the node's magic status */
         
         /*   judge the node is periodic or auto_delete:
                         1.auto_delete: delete this node, add the node into the empty_list
                         2.perioidc: add the node into active_list again;
                         */
         if( save->item.flag & ACC_TIMER_AUTO_DELETE )     // 1.auto_delete: delete this node, add the node into the empty_list
         {
             /*After deleting the node, judge the active_list is NULL? yes, close the timer interrupt */
             if(active_list == NULL) 
                 TmrController_(TS_TIMER_ID).tier[TIMER_HW_ID(TS_TIMER_ID)].tier_bits.ie_0 = C_TIER_TIMER_DISABLE_MATCH;  
         }
         else                           
             if(save->item.flag & ACC_TIMER_PERIODIC)  // 2.perioidc: add the node into active_list again; do not set match value 
             {

                //if the node is not the header,add the node into the active_list
                //if(save != active_list)
                pTIMERREF = save; /* According to timer_id to find the node's pointer */

                pTIMERREF->item.magic = ACC_TIMER_ACTIVE;    //refresh the magic status
                
                // set start_time and end_time  2018-8-27; at this moment, node is excuted periodiclly
                pTIMERREF->item.start_time = pTIMERREF->item.end_time;
    	        pTIMERREF->item.end_time = pTIMERREF->item.start_time + pTIMERREF->item.period_tick;  /* set end_time*/
    	            
    	        /* if the active_list is empty, add the node into the list  */
    	        if(active_list == NULL)
    	        {
                    /* close the total interrupt*/
                    cpsr = disableInterrupts();
            
            		active_list =pTIMERREF;			/* update active_list */

                    restoreInterrupts(cpsr);        /* open the total interrupt*/

            	}

    	
                // if the active_list is not empty, sort by end_time from small to large. 
                         

            	else 
            	{
            	    //set value in order to sort active_list prevent count overflow
        	        //end_time_periodic_64 = (UINT64)pTIMERREF->item.start_time + (UINT64)pTIMERREF->item.period_tick; //2018-8-27
        	        scan = active_list;
        	        scan_64 = (UINT64)scan->item.start_time + (UINT64)scan->item.period_tick;
        	        if((scan_64 < HALF_RANGE_32_BIT) && ((UINT64)pTIMERREF->item.start_time + (UINT64)pTIMERREF->item.period_tick > HALF_RANGE_32_BIT))
        	            scan_64 += RANGE_32_BIT;	               
            		/* find the position to be inserted */     
            		while( ( scan != NULL ) && (scan_64 < (UINT64)pTIMERREF->item.start_time + (UINT64)pTIMERREF->item.period_tick))
            		{	
            				save_periodic = scan;					/* save_periodic the previous node pointer */
            				scan = scan->next;
        	                scan_64 = (UINT64)scan->item.start_time + (UINT64)scan->item.period_tick;
        	                if((scan_64 < HALF_RANGE_32_BIT) && ((UINT64)pTIMERREF->item.start_time + (UINT64)pTIMERREF->item.period_tick > HALF_RANGE_32_BIT))
        	                    scan_64 += RANGE_32_BIT;        				
            		}

            		/* whether the position is header */
            		if(scan == active_list)              /* position is header*/
            		{
                        cpsr = disableInterrupts();     /* close the total interruption */
                        /* insert the node and set match value */
                            /* 1. insert the node as the head node */            
            			active_list->previous = pTIMERREF;
            			pTIMERREF->next = active_list;
            			active_list = pTIMERREF;

                        restoreInterrupts(cpsr);                           			   
                    }
                    
                    /* The node is not inserted in head */
                    else
                    {
                        /* According to the scan to determine the position to insert the node*/						
            			/* insert in the central of the active_list*/
            			if( scan != NULL)					
            			{
                            cpsr = disableInterrupts();
                            
            				scan->previous->next = pTIMERREF;
            				pTIMERREF->previous = scan->previous;
            				scan->previous = pTIMERREF;
            				pTIMERREF->next = scan;

            				restoreInterrupts(cpsr);
            			}
            			else 							/* insert the node in the end of the active_list*/
            			{	
                            cpsr = disableInterrupts();
                
            				save_periodic->next = pTIMERREF;        				
            				pTIMERREF->previous = save_periodic;
                            pTIMERREF->next = NULL;
                            
            				restoreInterrupts(cpsr);
            			}
                        
                    }		
            	}

                /* if node is added in the front of qtimerlist pointed node, refresh the qtimelist */            
                if((qtimerlist == NULL)||((qtimerlist != NULL)&&(qtimerlist->previous == pTIMERREF)))
                    qtimerlist = pTIMERREF;
             }
       
        //2018-9-18 if qtimerlist is not NULL ,refresh the ts_64 and end_time_64
        if( qtimerlist != NULL )
        {
    	    ts_64 = (UINT64)timerCountRead(TS_TIMER_ID) + 5;
            end_time_64 = (UINT64)qtimerlist->item.start_time + (UINT64)qtimerlist->item.period_tick;

            // if the executing node's end_time_64 in the end of UINT32 and ts_64 in the front of UINT32, 
            // ts_64 add 0x100000000
            // e.g. ts_64 = 5, end_time_64 = 0xFFFFFFFF
            if( end_time_64 > 0xE0000000 && ts_64 < 0x20000000 )
                ts_64 += RANGE_32_BIT; 
         } 
         
    }

    restoreInterrupts(cpsr);      //2018-10-31

    if(active_list != NULL)
    {
        // after removing the header node, set match again
        TmrController_(TS_TIMER_ID).tier[TIMER_HW_ID(TS_TIMER_ID)].tier_bits.ie_0 = C_TIER_TIMER_DISABLE_MATCH;		
        TmrController_(TS_TIMER_ID).tmr[TIMER_HW_ID(TS_TIMER_ID)].match_reg_0 = active_list->item.end_time;              
	    asm_dsb();
	    asm_isb(); 
        TmrController_(TS_TIMER_ID).tier[TIMER_HW_ID(TS_TIMER_ID)].tier_bits.ie_0 = C_TIER_TIMER_ENABLE_MATCH;
      
      }
    uart_printf("%s-02\r\n",__func__);
}

#define  TIMER_MIPS_TEST(eVENT)       /* */
#define  TIMER_MIPS_TEST_DATA(dATA)   /* */

void eeWdtDisable(void)
{

}


/******************************************************************************
* Function: AccTimerInitilize
*******************************************************************************
* Description:  This service initilizes the more accurate timer
*
* Parameters:   NONE
*
* Return value: NONE
*
* Notes:        1.declare a static structure array   
*                  2. initialize empty_list and active_list
*	
******************************************************************************/
void AccTimerInitilize(void)
{
	int i;
	
    active_list	= NULL;

    memset(ACCTimerNode, 0 , sizeof(ACC_TIMER_NODE)*ACC_TIMER_NODE_MAX);   

	empty_list = &ACCTimerNode[0];	/* Empty_list is the header pointer to the empty linked list */

	for(i=0; i < ACC_TIMER_NODE_MAX-1; i++)	/* Two-way linked list */
	{
			ACCTimerNode[i].next= &ACCTimerNode[i+1];
			ACCTimerNode[i+1].previous = &ACCTimerNode[i];
	}
	
	// The previous for the 0th node and the next pointer for the last node are assigned to NULL
	ACCTimerNode[0].previous= NULL;	
	ACCTimerNode[i].next= NULL;	

    /* Add ID to 32 nodes: 
            ID: 1-32*/
    for(i=0; i < ACC_TIMER_NODE_MAX; i++)
        ACCTimerNode[i].item.timer_id = i + 1;


    //set the count value into the count

}






/******************************************************************************
* Function: timerDBModeSet
*******************************************************************************
* Description: this function update the timer mode in the timer data base
*
* Notes: SW periodic = one shot + reload
******************************************************************************/
void timerDBModeSet(TCR_NUMBER tcr_number, TCR_CONFIGURATION *tcr_configuration)
{
	UINT32 SetTcmrMode = tcr_configuration->tcmr_mode;

	if(C_TCMR_SW_PERIODIC_TCMR_MODE == SetTcmrMode)
	{
	  /* SW Periodic is not exist in Arbel */
	  /* It will be treated as oneShot mode with counter reload*/
	  SetTcmrMode = C_TCMR_ONE_SHOT_MODE;
	}

    //Timers[tcr_number].busy = TRUE;
    Timers[tcr_number].repetative_mode = tcr_configuration->tcmr_mode;
	Timers[tcr_number].real_mode       =  (TCMR_MODES)SetTcmrMode;
} /* End of <timerDBModeSet> */


/******************************************************************************
* Function: timerTCCRSet
*******************************************************************************
* Description: This service allows a client to set the clock rate
* of any timer.
*
* Notes:
******************************************************************************/
TIMER_RETURN_CODE timerTCCRSet(TCR_NUMBER tcr_number, TCR_CONFIGURATION *tcr_configuration)
{

  volatile UINT32 bitForTCCR;
		   UINT32 tccr_reg_val;
	uart_printf("%s-01,%d,%d\r\n",__func__,tcr_number,Timers[tcr_number].clock_mode);

  ASSERT(tcr_number<MAX_TCR_NUMBER);
  
    Timers[tcr_number].clock_mode = tcr_configuration->clock_mode;  /* update TCR DB */
    switch (tcr_configuration->clock_mode)
    {
       case CLK_FAST:

	   bitForTCCR = TCCR_ClockSet[TIMER_HW_ID(tcr_number)][C_TCCR_CS_3_25MHZ];

        TIMER_MIPS_TEST(1);
       break;

       case CLK_32KHZ:

	   bitForTCCR = TCCR_ClockSet[TIMER_HW_ID(tcr_number)][C_TCCR_CS_32KHz];

        TIMER_MIPS_TEST(2);
      break;

      default:
        /* ASSERT(0); */ /* the call func will judge the returned value so remove assert here */
            return (TIMER_RC_BAD_VALUE);
       //break;
	} /* switch */

    tccr_reg_val = TmrController_(tcr_number).tccr.reg;
    switch (tcr_number)
    {
       case TCR_1:
	   	tccr_reg_val &= ~0x03;
		tccr_reg_val |= bitForTCCR;
		break;
        case TCR_2:

	   	tccr_reg_val &= ~(0x07<<2);
		tccr_reg_val |= (bitForTCCR<<2);
		break;
        case TCR_3:
	   	tccr_reg_val &= ~(0x03<<5);
		tccr_reg_val |= (bitForTCCR<<5);
		break;
       default:
		   /* ASSERT(0); */ /* the call func will judge the returned value so remove assert here */
            return (TIMER_RC_BAD_VALUE);
	}	   
    TmrController_(tcr_number).tccr.reg = tccr_reg_val;

	uart_printf("reg(0x%x)=0x%x\r\n",(&(TmrController_(tcr_number).tccr)),tccr_reg_val);
	return (TIMER_RC_OK);
} /* End of <timerTCCRSet> */


/******************************************************************************
* Function: timerTMRset
*******************************************************************************
* Description: This service allows a client to set the current Match value
* of any timer counter.
*
* Parameters: TCR_NUMBER tcrNumber
*
* Return value:  UINT32 (specific timer counter value)
*
* Notes:
******************************************************************************/
static TIMER_RETURN_CODE timerTMRset(TCR_NUMBER tcr_number, TCR_CONFIGURATION *tcr_configuration)
{
    UINT32 delta_time;
	UINT32 fast_clock_freq;

	UINT64 delta_fast;
	uart_printf("%s-01,%d,%d\r\n",__func__,tcr_number,Timers[tcr_number].clock_mode);
    ASSERT(tcr_number < MAX_TCR_NUMBER);
    switch (Timers[tcr_number].clock_mode)
    {
         case CLK_FAST:

			  /* crane/craneG/craneM fast clock frequency : 614M/48 = 12.791667M */
			  fast_clock_freq = (CHIP_IS_CRANE || CHIP_IS_CRANEG || CHIP_IS_CRANEM) ? 12791667 : 13000000;
			  delta_fast = (UINT64)(tcr_configuration->microSecTimeInterval) * (UINT64)(fast_clock_freq) / (UINT64)(32768);
			  if(delta_fast > (UINT64)0xFFFFFFFF)
			  	delta_time = 0xFFFFFFFF;
			  else
			  	delta_time = (UINT32)delta_fast;
	
            TIMER_MIPS_TEST(5);
         break;

         case CLK_32KHZ:
	 	
			 delta_time = tcr_configuration->microSecTimeInterval;

            TIMER_MIPS_TEST(6);
         break;

         default:
             delta_time = 0; //no Warnings
             ASSERT(0);
         break;
      } /* switch */

      /* the following line need for SW periodic  */
      Timers[tcr_number].timerAddingValue = delta_time;  /* update timers DB with HW ticks value, according to src clk */
      TIMER_MIPS_TEST_DATA(delta_time);

      /*calculate the value to put in Match reg.*/
      TmrController_(tcr_number).tmr[TIMER_HW_ID(tcr_number)].match_reg_0 = delta_time;
      // Raviv debug 21/2/6
      //PM_EXT_DBG_EVENT_SEND(PM_EXT_DBG_EVENT_TIMER_TMR_SET);
	  uart_printf("reg(0x%x)=0x%x\r\n",(&(TmrController_(tcr_number).tmr[TIMER_HW_ID(tcr_number)])),delta_time);

	return TIMER_RC_OK;
} /* End of <timerTMRset> */

void timerTPLCRSet(TCR_NUMBER tcr_number, TCR_CONFIGURATION *tcr_configuration)
{
   UINT8 tm_hw_id;
   
   ASSERT(tcr_number < MAX_TCR_NUMBER);

   tm_hw_id = TIMER_HW_ID(tcr_number);
    
   if (tcr_configuration->tplcr_mode == C_TPLCR_ENABLE_PRELOAD_WITH_MATCH_O)  // while using preload, other two matches are obsolete.
   {
      TmrController_(tcr_number).tplcr[tm_hw_id] = tcr_configuration->tplcr_mode;
      TmrController_(tcr_number).tplvr[tm_hw_id] = tcr_configuration->tplvr_value;  // usually 0 
   }
   else // since it is Free-Running timer
   {
      TmrController_(tcr_number).tplcr[tm_hw_id] = tcr_configuration->tplcr_mode; //irits timer fix bug no 1571106
      TmrController_(tcr_number).tplvr[tm_hw_id] = tcr_configuration->tplvr_value;  // usually 0  //irits timer fix bug no 1571106
	  Timers[tcr_number].busy = TRUE;
   }
} // <End of timerTPLCRSet>



/******************************************************************************
* Function: timerActivate
*******************************************************************************
* Description:  This service activates a specific timer
*
* Parameters:   tcr_number - the number of the timer to be configure*
*
* Return value: TIMER_RETURN_CODE : TIMER_RC_OK        =  0
*                                   TIMER_RC_NOT_VALID = -1
*                                   TIMER_RC_BAD_VALUE = -2
*
* Notes:
*
******************************************************************************/

TIMER_RETURN_CODE timerActivate (TCR_NUMBER tcr_number)
{

    UINT32 cpsrVal;
   
    cpsrVal = disableInterrupts();

    if  (tcr_number >= MAX_TCR_NUMBER)
    {
    	  restoreInterrupts(cpsrVal);
    	  return (TIMER_RC_NOT_VALID);
    }

      // Raviv debug 21/2/6
      //PM_EXT_DBG_EVENT_SEND(PM_EXT_DBG_EVENT_TIMER_TIER_SET);
    timerManageResource (tcr_number,PRM_RSRC_ALLOC);  /* if using 13M notify the RM */ /*add by elevy 09-07-05 18:02 */
	uart_printf("Timers[%d].realmode=%d\r\n",tcr_number,(TCMR_COUNT_MODE)Timers[tcr_number].real_mode);
    timerTCMRSet(tcr_number , (TCMR_COUNT_MODE)Timers[tcr_number].real_mode);

	if(tcr_number == TS_TIMER_ID)
		ts_timer_active = 1;

    restoreInterrupts(cpsrVal);
    return(TIMER_RC_OK);
} /* End of <timerActivate> */



/******************************************************************************
* Function: timerTCRconfigure
*******************************************************************************
* Description:  This function is called by Application in order to configure
*               selected TCR.
* Parameters:   tcr_number - the number of the timer to be configure
*								tcr_configuration - configration structure for the timer
*
* Return value:  TIMER_TCR_CONFIGURE_RC -
*								 TCR_CONFIGURE_OK   = configuration succeed,
*                TCR_CONFIGURE_FAIL = if the needed timer is already allocate,
*						     TCR_CONFIGURE_TCR_ERR_NUM = if no such timer number exist
*
* Notes:
******************************************************************************/
TIMER_TCR_CONFIGURE_RC timerTCRconfigure(TCR_NUMBER tcr_number, TCR_CONFIGURATION *tcr_configuration)
{
	 UINT32 cpsrVal;
   TIMER_RETURN_CODE status = TIMER_RC_OK;


   if(tcr_number == FAST_CAL_TIMER)
   {
	   extern void eeWdtDisable(void);
	   eeWdtDisable();
   }

   cpsrVal = disableInterrupts();

   if  (tcr_number >= MAX_TCR_NUMBER)
   {
		  restoreInterrupts(cpsrVal);
   		return (TCR_CONFIGURE_TCR_ERR_NUM);
   }

   if (Timers[tcr_number].busy == TRUE)                 /* ??? check all relevant configurations here. */
   {
	   restoreInterrupts(cpsrVal);
     return (TCR_CONFIGURE_FAIL);
   }

   Timers[tcr_number].busy = TRUE;
   Timers[tcr_number].timerNotificationPtr = tcr_configuration->timerNotificationPtr;
   Timers[tcr_number].microSec             = tcr_configuration->microSecTimeInterval;
   Timers[tcr_number].cookie    = tcr_configuration->cookie;
   Timers[tcr_number].interrupt_required = tcr_configuration->interrupt_required;


//   timerTCMRSet(tcr_number , TCMR_DISABLE_MODE); /* set TCMR - one shot, periodic */

   status = timerTCCRSet (tcr_number, tcr_configuration);                     /* set TCCR - clock source :fast, 32K, ext, 1K */
   if (status != TIMER_RC_OK)
   {
   	  Timers[tcr_number].busy = FALSE;
	    restoreInterrupts(cpsrVal);
      return (TCR_CONFIGURE_FAIL);
   }

   status = timerTMRset  (tcr_number, tcr_configuration);                     /* set Match Values */
   if (status != TIMER_RC_OK)
   {
      Timers[tcr_number].busy = FALSE;
      restoreInterrupts(cpsrVal);
	  	return (TCR_CONFIGURE_FAIL);
   }

  if (Timers[tcr_number].interrupt_required&TO_INTERRUPT_REQUIRED)
  {
    TmrController_(tcr_number).tier[TIMER_HW_ID(tcr_number)].reg = 1;

	uart_printf("reg(0x%x)=1\r\n",(&(TmrController_(tcr_number).tier[TIMER_HW_ID(tcr_number)])));
	uart_printf("timer  %d intcnum %d\r\n",tcr_number,TIMER_INTCID(tcr_number));
    INTCEnable(TIMER_INTCID(tcr_number));     /* Enable Interrupts for relevant TCR */

  }
   timerDBModeSet (tcr_number, tcr_configuration);            /* update the timer mode in the Timers DB   */
//   timerInterruptModeSet(tcr_number, C_TILR_LEVEL);                  /* set TILR (level or Edge) */
   timerTPLCRSet(tcr_number, tcr_configuration);                     /* set TPLCR & TPLVR - preload setting */

//   timerAPBClock(tcr_number);//,PMU_OFF);
	restoreInterrupts(cpsrVal);
   return (TCR_CONFIGURE_OK);
} /* <End of timerTCRconfigure> */

/******************************************************************************
* Function: timerTCRReConfigure
*******************************************************************************
* Description:  this function allow users to change ONLY the following timer parameters:
				clock_mode,microSec,timerNotificationPtr with minimal access to HW
*               for all other parameters call to timerDeactivate , timerTCRConfigure and timerActivate
* Parameters:   tcr_number - the number of the timer to be configure
*				tcr_configuration - configration structure for the timer
*
* Return value:  TIMER_TCR_CONFIGURE_RC -
*				 TCR_CONFIGURE_OK   = configuration succeed,
*                TCR_CONFIGURE_FAIL = if the needed timer is already allocate,
*			     TCR_CONFIGURE_TCR_ERR_NUM = if no such timer number exist
*
* Notes:
*    This API may be called some times with THE SAME TIMER CONFIGURATION.
*    In this case (TCMR_DISABLE_MODE & runningMODE) will bring it to timer restart
*    which is not wanted!!!.
*    So call HW APIs only if real change required!
******************************************************************************/
TIMER_TCR_CONFIGURE_RC timerTCRReConfigure(TCR_NUMBER tcr_number, TCR_CONFIGURATION *tcr_configuration)
{
    UINT32 cpsrVal;
    TIMER_RETURN_CODE status = TIMER_RC_OK;

    BOOL  changeTcmrMode;
    BOOL  changeFastSlow;
    BOOL  changeTime;

    ASSERT(tcr_number < MAX_TCR_NUMBER);

    cpsrVal = disableInterrupts();
    //ASSERT(Timers[tcr_number].busy == TRUE);  // BUSY is "Running". Call Re-Config for running only!

    //                                      SW Requested  !=  Currently HW Configured
    changeTcmrMode = (tcr_configuration->tcmr_mode            != Timers[tcr_number].real_mode);
    changeFastSlow = (tcr_configuration->clock_mode           != Timers[tcr_number].clock_mode);
    changeTime     = (tcr_configuration->microSecTimeInterval != Timers[tcr_number].microSec);

    if(changeTcmrMode || changeFastSlow || changeTime)
    {
        timerTCMRSet(tcr_number , TCMR_DISABLE_MODE);

        if(changeFastSlow)
        {
	        if (tcr_configuration->clock_mode == CLK_FAST)
	        		timerManageResource (tcr_number,PRM_RSRC_ALLOC);
	        else	timerManageResource (tcr_number,PRM_RSRC_FREE);

	        status = timerTCCRSet (tcr_number, tcr_configuration);  /* set TCCR - clock source :fast, 32K, ext, 1K */
	        ASSERT(status == TIMER_RC_OK); //if(!OK) {Timers[tcr_number].busy = FALSE; restoreInterrupts(cpsrVal);  return (TCR_CONFIGURE_FAIL); }
        }
        else  { TIMER_MIPS_TEST(4);  }


        if(changeFastSlow || changeTime)
        {
	        Timers[tcr_number].microSec             = tcr_configuration->microSecTimeInterval;
	        status = timerTMRset  (tcr_number, tcr_configuration);   /* set Match Values */
	        ASSERT(status == TIMER_RC_OK); //if(!OK) {Timers[tcr_number].busy = FALSE; restoreInterrupts(cpsrVal);  return (TCR_CONFIGURE_FAIL); }
        }
        else  { TIMER_MIPS_TEST(8);  }

        Timers[tcr_number].timerNotificationPtr = tcr_configuration->timerNotificationPtr;
        Timers[tcr_number].microSec             = tcr_configuration->microSecTimeInterval;

        timerDBModeSet (tcr_number, tcr_configuration);            /* update the timer mode in the Timers DB   */

		timerTPLCRSet(tcr_number, tcr_configuration); 
		
        timerTCMRSet(tcr_number , (TCMR_COUNT_MODE)Timers[tcr_number].real_mode);
    }
    restoreInterrupts(cpsrVal);
    return (TCR_CONFIGURE_OK);
} /* <End of timerTCRconfigure> */

/******************************************************************************
* Function: InitTimers
*******************************************************************************
* Description:  This function configures and activates the HW of the OS Timer
*
* Notes : Function is called from Phase2Inits function
*         Initialize only free running Timer
*******************************************************************************/
int TS_TIMER_INIT = 0;

#define FREQ_MUL   (1000)      // 5000 Usec = 5 mSec
#define OSA_TICK_FREQ_IN_MILLISEC   5

void InitTimers(void)
{

    TCR_CONFIGURATION	     timer_config;
    TIMER_TCR_CONFIGURE_RC   timerStatus;
	TIMER_RETURN_CODE time_stat;

	//   all  platforms - default
	memset(&timer_config,0,sizeof(timer_config));
	timer_config.clock_mode  = CLK_32KHZ;                                 // 32.768Khz ==> 1 hw tick clock = 30517.578125 nSec = 30.517 Usec
	timer_config.timerNotificationPtr = NULL;
	timer_config.interrupt_required = 0;

    //timer_config.tcmr_mode   = (TCMR_MODES)TCMR_EN_FREERUN_MODE;  //  One Shot mode + SW periodic reload
    timer_config.tcmr_mode   = (TCMR_MODES)TCMR_EN_PERIOD_MODE;  //  One Shot mode + SW periodic reload

	timer_config.tplcr_mode  = C_TPLCR_FREE_RUNNING ;                     // C_TPLCR_ENABLE_PRELOAD_WITH_MATCH_O;
	timer_config.tplvr_value = 0;                                         // after match occurs, pre-load value = 0; match#0 value - unchanged.
	timer_config.microSecTimeInterval = (OSA_TICK_FREQ_IN_MILLISEC*FREQ_MUL);
	//timer_config.microSecTimeInterval = 0xFFFFFFFF;
	//timer_config.microSecTimeInterval = (OSA_TICK_FREQ_IN_MILLISEC*1000000);
	timer_config.cookie = (UINT8)TS_TIMER_ID;
	timer_config.timerNotificationPtr = (TIMER_CALLBACK_FUNCTION)TShandler;  //hengshanyang 2018-7-24
	timer_config.interrupt_required = TO_INTERRUPT_REQUIRED;
	//more accurate timer init function
	AccTimerInitilize(); 
    dumpTimerReg(TS_TIMER_ID);
   	timerStatus = timerTCRconfigure(TS_TIMER_ID, &timer_config);
    dumpTimerReg(TS_TIMER_ID);   
    ASSERT(timerStatus == TCR_CONFIGURE_OK);
	time_stat = timerActivate(TS_TIMER_ID);
	ASSERT(time_stat == TIMER_RC_OK);
    dumpTimerReg(TS_TIMER_ID);
	TS_TIMER_INIT = 1;

}
UINT32 T32kCountLast = 0;
int T32kCountInitialized = 0;
extern UINT64 T32kHzBase ;



void initTimerCount13MHz(void)
{
	T32kCountLast = timerCountRead(TS_TIMER_ID);
	//enable_performance_count();//reset the performance count
	uart_printf("initTimerCount13MHz, T32kHzBase:%x\r\n", T32kCountLast);
	T32kHzBase = T32kCountLast;
	T32kCountInitialized = 1;
}




UINT32 timerCountRead(TCR_NUMBER tcr_number)
{
   UINT32 count_read,x0;
   UINT8  tm_hw_id;
   ASSERT(tcr_number < MAX_TCR_NUMBER);
   if((TS_TIMER_INIT == 0) && (tcr_number == TS_TIMER_ID))
		return 0;
   tm_hw_id = TIMER_HW_ID(tcr_number);
   //irits timer fix : addet 1Khz also
   {
	 // FAST CLOCK: use latching (TCVWR) since every tcr reading will return a value different from the previous one.
	 // Read the old latched value; latch; wait for the new latched value: different from the old latched value.
     x0=TmrController_(tcr_number).tcvwr[tm_hw_id];
     while((count_read=TmrController_(tcr_number).tcvwr[tm_hw_id])==x0)
     {
        TmrController_(tcr_number).tcvwr[tm_hw_id] = 1;      //irits - write "1" for latch timer value before read.
        count_read=TmrController_(tcr_number).tcvwr[tm_hw_id];
     }
   }
   return(count_read);  /* read to get real latched value */   

}


/******************************************************************************
* Function: timerDeActivate
*******************************************************************************
* Description: This service cancels the notification request.
* The timer is no longer allocated to the client.
*
* Parameters:  TIMER_NUMBER timerNumber: 0-3
*
* Return value: TIMER_RETURN_CODE : TIMER_RC_OK        =  0
*                                   TIMER_RC_NOT_VALID = -1
*                                   TIMER_RC_BAD_VALUE = -2
* Notes: after call to this function need to call timerTCRConfigure in order to use the timer again.
******************************************************************************/
TIMER_RETURN_CODE timerDeActivate (TCR_NUMBER tcr_number)
{

	UINT32 cpsrVal;
	UINT32 tier_val;
	uart_printf("file:%s,function:%s,line:%d\r\n", __FILE__,__func__,__LINE__);

    ASSERT(tcr_number < MAX_TCR_NUMBER);

	cpsrVal = disableInterrupts();
	
	if(tcr_number == TS_TIMER_ID)
		ts_timer_active = 0;
	
    timerTCMRSet(tcr_number , TCMR_DISABLE_MODE);
    timerManageResource(tcr_number,PRM_RSRC_FREE); /* if the timer use 13Mhz send release notification to the RM *//*add by elevy 09-07-05 18:00 */
    Timers[tcr_number].busy = FALSE;                    /* Update DataBase */
    Timers[tcr_number].clock_mode = CLK_32KHZ;          /* Update DataBase */

    /*timer_hw_ptr->tier[ireg].ie_0   = C_TIER_TIMER_DISABLE_MATCH;*/              /* disable timer interrupt */
	/* Read shadow tier reg*/

    tier_val = TmrController_(tcr_number).tier[TIMER_HW_ID(tcr_number)].reg;
	tier_val &= ~7;
    TmrController_(tcr_number).tier[TIMER_HW_ID(tcr_number)].reg = tier_val;

    /* Clear status bit by writing 1 */
   // TmrController.tsr.reg  |= (1<<tcr_number);   //elevy - not needed from HW perspective so we save GPB write access

    /*timer_hw_ptr->ticr[ireg].tclr_0 = C_TICR_TIMER_CLEAR_INTERRUPT_AND_STATUS;*/ /* clear Level interrupt and Status bit */

    /* Disable Interrupts for relevant TCR */
    INTCDisable(TIMER_INTCID(tcr_number));

  	restoreInterrupts(cpsrVal);


	//if(tcr_number == FAST_CAL_TIMER)
	//{
		//extern void eeWdtEnable(void);
		//eeWdtEnable();
	//}

    return(TIMER_RC_OK);
} /* End of <timerDeActivate> */


































