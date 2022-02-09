/*------------------------------------------------------------
(C) Copyright [2006-2008] Marvell International Ltd.
All Rights Reserved
------------------------------------------------------------*/

#ifndef _TIMER_HW_H
#define _TIMER_HW_H   (1)

/*--------- Local defines ---------------------------------------------------*/
/* Manitoba VTCXO = 13M   ; fast_clk = 3,250,000 hz = 13M/4   ==>  1 Clk = 307.692307 nSec = 0.3077 MicroSec */
/* Full Cycle = 32bit * 307.692307 nSec =   ~21.9 Minutes. */
//#define MAX_MICRO_SEC_INTERVAL_IN_32_BIT_REGISTER  0xFFFFFFFFL/307*100

/*alex added*/
/*#define MAX_MICRO_SEC_INTERVAL_32kHz_IN_32_BIT_REGISTER (0xFFFFFFFFL-1)/327*10000*/ // why these actions can leav it fffff..ff
/*lena changed alex addings*/
#define MAX_MICRO_SEC_INTERVAL_32kHz_IN_32_BIT_REGISTER (0xFFFFFFFFUL)
#define MAX_MICRO_SEC_INTERVAL_FAST_IN_32_BIT_REGISTER  (TIMER_USECS_IN_SECOND*(0xFFFFFFFFUL/FAST_TIMER_CLOCK_RATE_IN_HZ))

//#define FAST_TIMER_CLOCK_RATE_IN_HZ                3250000
#define TIMER_USECS_IN_SECOND                      1000000
#define FAST_TIMER_CLOCK_RATE_IN_HZ                13000000
#define SLOW_TIMER_CLOCK_RATE_IN_HZ                32768
#define SECONDS_TO_MICRO_SEC_RATIO                 1000000
#define MILISEC_TO_MICRO_SEC_RATIO                 1000L

//Fast clock (13Mhz/4)
/*alex added*/
#define MICRO_SEC_TO_CLOCK_TICKS(uSec)             (uSec*13)

#define CLOCK_TICKS_TO_MICRO_SEC(clockTicks)       (clockTicks/13)
#define CLOCK_TICKS_TO_MICRO_SEC_CLK_32KHZ(x)  ((x)>(0xFFFFFFFF/15625)?((x/512)*15625+((x%512)*15625)/512):(((UINT32)(x)*15625)/512))
#define CLOCK_TICKS_TO_MICRO_SEC_32K             CLOCK_TICKS_TO_MICRO_SEC_CLK_32KHZ

#define MICRO_SEC_TO_CLOCK_TICKS_CLK_32KHZ(x)  ((x)>(0xFFFFFFFF/512)?((x/15625)*512+((x%15625)*512)/15625):(((UINT32)(x)*512)/15625))
#define MICRO_SEC_TO_CLOCK_TICKS_CLK_FAST               MICRO_SEC_TO_CLOCK_TICKS
#define MICRO_SEC_TO_CLOCK_TICKS_CLK_1KHZ(uSec)         ((uSec*1)/1000)    // 0.001 * Tick = 1USec, Min value = 100 Usec, resolution 100 Usec
/*#define MICRO_SEC_TO_CLOCK_TICKS_CLK_EXT(microSecInterval_4)        ((microSecInterval_4 * 1) / 6) */ /* There is no pin for ext. clk */


/************* Timers HW defenitions *****************************************/

#define HALF_MAX_TCR 3                  // First half is NDR, second - drowsy (if exists)
#define NON_DROWSY_TIMERS_BASE_ADDRESS 0x40A00000UL
#define DROWSY_TIMERS_BASE_ADDRESS     0x41A00000UL  // _HERMON_, not implemented in Manitoba


#define TIMER_BASE_ADDRESS          0xd4080000
#define TIMER_EXT_BASE_ADDRESS      0xd403a000


/******* 3 Match registers per TCR ******************************************/
typedef struct {
      UINT32  match_reg_0 ;    /* R/W */
      UINT32 match_reg_1 ;    /* R/W */
      UINT32 match_reg_2 ;    /* R/W */
} T_MATCH_STR;


/******* TCCR = Timer Clock Control Register  *******************************/
/*clock Source of tcr 0 */
/*#define C_TCCR_CS_0_MASK  0xfffffffc */

typedef enum
{
	C_TCCR_CS_3_25MHZ,
	C_TCCR_CS_1KHZ,
	C_TCCR_CS_32KHz,
	C_TCCR_CS_EXT
}Timer_TCCR_Clock_Type;



#define TIMER_HISR_PRIORITY 1





#define C_TCCR_1_CS_3_25MHZ   0x00
#define C_TCCR_2_CS_3_25MHZ   0x00
#define C_TCCR_3_CS_3_25MHZ   0x00

#define C_TCCR_1_CS_1KHZ   NULL
#define C_TCCR_2_CS_1KHZ   0x02
#define C_TCCR_3_CS_1KHZ   0x01

#define C_TCCR_1_CS_32KHz   0x01
#define C_TCCR_2_CS_32KHz   0x01
#define C_TCCR_3_CS_32KHz   0x02

#define C_TCCR_1_CS_EXT   0x02
#define C_TCCR_2_CS_EXT   0x03
#define C_TCCR_3_CS_EXT   NULL




#define C_TCCR_CS_ERROR      -1

typedef union{
	UINT32 reg ;
	struct {
      unsigned
      cs_0       :2,            /* R/W */
      cs_1       :3,            /* R/W */
      cs_2       :2,            /* R/W */
      cs_wdt     :2,            /* R/W */
      null       :23;
	} tccr_bits;
} T_TCCR;



/******* TPLCR = Timer Preload Control Register  ****************************/
typedef struct {
      unsigned
      mcs        :2,            /* R/W    Match Comperator Select */
      null       :30;
} T_TPLCR;


/******* TSR = Timer Status Register  ***********************/
#define C_TSR_TIMER_NOT_MATCH_THE_COUNTER   0
#define C_TSR_TIMER_MATCH_THE_COUNTER       1

typedef union
{
	UINT32 reg;
	struct {
	      unsigned
	      m_1       :1,            /* R/w */
	      m_2       :1,            /* R/w */
	      m_3       :1,            /* R/w */
	      m_4       :1,            /* R/w */
	      null      :28;
	} tsr_bits;
}
T_TSR;


/******** TIER = Timer Interrupt Enable Register  ************/
#define C_TIER_TIMER_DISABLE_MATCH  0
#define C_TIER_TIMER_ENABLE_MATCH   1

#define C_TIER_MATCH_0_MASK 0x00000001
#define C_TIER_MATCH_1_MASK 0x00000002
#define C_TIER_MATCH_2_MASK 0x00000004

typedef union {
	UINT32 reg;
	struct {
    	  unsigned
      	  ie_0       :1,            /* R/W  interrupt enable for Match register 0 of TRCn */
          ie_1       :1,            /* R/W  interrupt enable for Match register 1 of TRCn */
          ie_2       :1,            /* R/W  interrupt enable for Match register 2 of TRCn */
          null       :29;
    } tier_bits;
}T_TIER;

/******** TICR = Timer Interrupt Clear Register  ************/
#define C_TICR_TIMER_NO_AFFECT 0
#define C_TICR_TIMER_CLEAR_INTERRUPT_AND_STATUS 1


/******** TCER = Timer Coiunt Enable Register  *******************************/
typedef struct {
      unsigned
      ten_0       :1,            /* R/W  TCR_0 count enable */
      ten_1       :1,            /* R/W  TCR_1 count enable */
      ten_2       :1,            /* R/W  TCR_2 count enable */
      null        :29;
} T_TCER;


/******** TCMR = Timer Count Mode Register  **********************************/

typedef union {
	UINT32 reg;
	struct {
      unsigned
      tmode_1     :1,            /* R/W   TCR_0 count mode */
      tmode_2     :1,            /* R/W   TCR_1 count mode */
      tmode_3     :1,            /* R/W   TCR_2 count mode */
      null        :29;
	} tcmr_bits;
} T_TCMR;



#define TCR_TCMR_MASK      (0x03)

/******* TILR = Timer Interrupt Mode (Length) Register  *********************/
typedef enum
{
    C_TILR_LEVEL,                       /* 0 */
    C_TILR_EDGE                         /* 1 */
}TILR_MODES;

typedef union
{
unsigned reg;
struct
    {
    unsigned til_1    : 1;   /* R/W  interrupt mode for Match register 1 of TRCn */
    unsigned til_2    : 1;   /* R/W  interrupt mode for Match register 2 of TRCn */
    unsigned til_3    : 1;   /* R/W  interrupt mode for Match register 3 of TRCn */
    unsigned null     : 29;
    } rcrd;
} TIMER_T_TILR;


typedef union
{
unsigned reg;
struct
    {
    unsigned WE       : 1;   /* R/W  watchdag enable counting */
    unsigned WMS      : 1;   /* R/W  0 - Generate interrupt , 1 - generate reset */
    unsigned null     : 30;
    } wer_str;
} T_WER_STR; /* watch dog enable reg */

typedef struct {
      unsigned
      tclr_0       :1,            /* W  clear interrupt and status bit, for Match register 0 of TRCn */
      tclr_1       :1,            /* W  clear interrupt and status bit, for Match register 1 of TRCn */
      tclr_2       :1,            /* W  clear interrupt and status bit, for Match register 2 of TRCn */
      null         :29;
} T_TICR;

/*******************************************************************/

struct TIMER_HW_REGISTERS
    {
      T_TCCR       tccr;                 /* R/W           Timer Clock Control Reg */
      T_MATCH_STR  tmr   [MAX_HW_TCR_NUMBER];  /* R/W           3-match Reg per timer */
      UINT32       tcr   [MAX_HW_TCR_NUMBER];  /* Read Only     Timer Count Reg */
      T_TSR        tsr   [MAX_HW_TCR_NUMBER];  /* Read only     Timer Status Register */
      T_TIER       tier  [MAX_HW_TCR_NUMBER];  /* R/W           Timer Interrupt Enable Reg */
      UINT32       tplvr [MAX_HW_TCR_NUMBER];  /* R/W           Timer Preload Value Reg */
      UINT32       tplcr [MAX_HW_TCR_NUMBER]; /* R/W           Timer Preload Control Reg */
      UINT32       wd_regs[4];           /*               Implemented in saperate WD driver */
      T_TICR       ticr  [MAX_HW_TCR_NUMBER];  /* Write only    Timer Interrupt Clear Reg */
      UINT32       wd_twicr;
      UINT32       tcer;                 /* R/W           Timer Count Enable Reg */
      T_TCMR       tcmr;                 /* R/W           Timer Count Mode Reg */
      TIMER_T_TILR tilr  [MAX_HW_TCR_NUMBER];  /* R/W           Timer Interrupt Mode (Level, Edge) */
      UINT32       dummy_twcr;
      UINT32       dummy_twfar;
      UINT32       dummy_twsar;
      UINT32       tcvwr [MAX_HW_TCR_NUMBER];  /* R/W           Timer value wr-rd request */
    };

typedef enum
{
  C_TCMR_HW_DISABLED_MODE = 0x0,
  C_TCMR_HW_FREE_RUN_MODE = 0x01,
  C_TCMR_HW_PERIODIC_TCMR_MODE = 0x2,   // Accurated AutoLoad is done automatically by HW, using HW Timer Preload Value Register
  C_TCMR_ONE_SHOT_MODE = 0x3,
  C_TCMR_SW_PERIODIC_TCMR_MODE = 0x4   // Autoload is done by SW (= One Shot mode + SW periodic reload);
                                       // TPLCR value should be C_TPLCR_FREE_RUNNING (Used only by SW)
} TCMR_MODES;
//typedef  TCMR_COUNT_MODE TCMR_MODES ;

typedef enum
{
   CLK_FAST,
   CLK_32KHZ,
   CLK_1KHZ,                  // accuracy  30% @ 1mSec, 6% @ 30mSec, 2% @ 100mSec
   //CLK_EXT,                 // Not implemented as external pin on Manitoba chip
   CLK_256HZ
} CLOCK_MODES;


/* The following values can be OR-combined to set the desired options in the interrupt_required field below */
typedef enum
{
  NONE = 0,                  /* "NONE" causes enum collision. It is OBSOLET, use TMR_INTERRUPT_NONE instead.*/
  TMR_INTERRUPT_NONE = 0,    /* used if no interrupt is required*/
  TO_INTERRUPT_REQUIRED = 1, /* for compatibility with old definition of the field - BOOL */
  TO_LISR_NOTIFICATION  = 2  /* if set, the notification callback will be called in ISR rather than in HISR */
}TimerOptions;

typedef enum
{
	TCMR_DISABLE_MODE       = 0 ,
	TCMR_EN_FREERUN_MODE    = 0x01,   /* Enable , free running (wraps at max value) */
	TCMR_EN_PERIOD_MODE     = 0x02,   /* Enable , periodic timer mode               */
	TCMR_EN_ONESHOT_MODE    = 0x03 ,  /* Enable , one-shot timer mode               */
	TCMR_EN_SW_PERIOD_MODE  = 0x04    /* Used only by S/W */
}
TCMR_COUNT_MODE;
typedef enum
{
    PRM_RSRC_FREE=0,
    PRM_RSRC_ALLOC
} PRM_AllocFreeE;



typedef void (*TIMER_CALLBACK_FUNCTION)(UINT8) ; //pointer to a callback function to be called upon expiration

/* Setting physical addresses for Timers HW registers */
#define NON_DROWSY_HW_REG  (* (volatile struct TIMER_HW_REGISTERS *) NON_DROWSY_TIMERS_BASE_ADDRESS)
#define DROWSY_HW_REG      (* (volatile struct TIMER_HW_REGISTERS *) DROWSY_TIMERS_BASE_ADDRESS)
//#define TIMER_USE_TCVWR //TCVWR latching time is too long, therefore not recommended


/*--------- Global constants definitions ------------------------------------*/

/*--------- Local constants definitions -------------------------------------*/


/*-------- Local macro definitions ------------------------------------------*/


/*-------- Local type definitions -------------------------------------------*/

/*  This structure holds all important information regarding a specific tcr */
typedef struct
{
    UINT32 microSec;                                   /*timer interval in microseconds */
    UINT32 timerAddingValue;                           /*time interval to add in SW periodic mode */
    TCMR_MODES repetative_mode;                        /*one shot or HW periodic or SW preiodic */
	TCMR_MODES real_mode ;
    TIMER_CALLBACK_FUNCTION timerNotificationPtr;      /*a pointer to the callback function at timer experation */
    UINT8 cookie;                                      /* this parameter is return uchanged from Callback routine */
    /* additions for whitesail */
    /*MATCH_NUMBER match_number; */                    /* range [0..2] */
    CLOCK_MODES  clock_mode;                           /* fast = vtcxo/4, 32Khz, 1Khz, 256 Hz and Ext. */
    BOOL   busy;
    UINT8   interrupt_required;                         /* cause an interrupt when timer expiered ? */
    UINT32  last32kValue;                               /* last read 32kHz value. May be used as "cached"*/
} TIMER_CONFIG_STR;

typedef struct
{
    volatile struct TIMER_HW_REGISTERS *timer_hw_ptr;  /* according to Drowsy Non-Drowsy regions */
	UINT8  ireg;                                       /* register index (0..2) inside the timer module */
    UINT8  intcid;                                     /* INTC interrupt ID */
	UINT8  apbclock;
	UINT8  pmuclock;
}TIMER_CONST_STR;

typedef enum
{
  C_TPLCR_FREE_RUNNING,
  C_TPLCR_ENABLE_PRELOAD_WITH_MATCH_O  // while using pre-load, the other two match registers are obsolete!!
  //C_TPLCR_ENABLE_PRELOAD_WITH_MATCH_1,
  //C_TPLCR_ENABLE_PRELOAD_WITH_MATCH_2
}TPLCR_MODES;

typedef enum
{
   TCR_CONFIGURE_OK   = 0,
   TCR_CONFIGURE_FAIL = -1,
   TCR_CONFIGURE_TCR_ERR_NUM = -2
}TIMER_TCR_CONFIGURE_RC;
typedef enum
{
  TIMER_RC_OK        =  0,
  TIMER_RC_NOT_VALID = -1,    // the specified timer NUMBER is  not valid
  TIMER_RC_BAD_VALUE = -2     // the specified timer VALUE is not valid
} TIMER_RETURN_CODE;



typedef struct   /* This is structure of the TCR Configuration  */
{
   CLOCK_MODES  clock_mode;      // TCCR, clock modes for selected TCR:fast = vtcxo/4, 32Khz, 1Khz, 256 Hz and Ext.
   TCMR_MODES   tcmr_mode;       // TCMR, timer modes: One-Shot or HW Periodic or SW Periodic
   TPLCR_MODES  tplcr_mode;      // TPLCR pre-load control mode: Free-Running or Pre-Load with Match#0 (other two match not relevant).
   UINT32       tplvr_value;     // Timer Pre-Load value register.
   UINT32       microSecTimeInterval;   // [Usec] interval
   TIMER_CALLBACK_FUNCTION timerNotificationPtr;  // callback function
   UINT8        cookie;
   UINT8        interrupt_required;    // interrupt when expired?, notification in ISR or in HISR?
}TCR_CONFIGURATION;


#define FAST_CAL_TIMER TCR_3


#define HALF_RANGE_32_BIT 0x7FFFFFFF
#define RANGE_32_BIT 0x100000000

typedef enum 	/* The value of enum member's value is sorted by value from small to large  */
{
	ACC_TIMER_EMPTY = 0x0,				/* the status of empty node */
	ACC_TIMER_ACTIVE = 0xAAAA,		    /* the status of active node */
	ACC_TIMER_CREATE = 0xCCCC,		    /* the status of active node */
	ACC_TIMER_STOP = 0xDDDD		        /* the status of stop node */	
}ACC_TIMER_MAGIC;

#define TS_NODE_INTERVAL 6      //judege the node's match during TS_handler 
#define STOP_NODE_INTERVAL 6    //judege the node's match during stop 

typedef enum   /* The meaning of the API flag*/         
{
	ACC_TIMER_PERIODIC = 0x1,		/* periodic execution */
	ACC_TIMER_AUTO_DELETE = 0x2	    /* one execution */
}ACC_TIMER_FLAG;


#endif
