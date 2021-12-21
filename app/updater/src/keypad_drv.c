#include <string.h>
#include <stdlib.h>
//#include "plat_basic_api.h"
#include "keypad_drv.h"
#include "keypad.h"
//#include "log_api.h"
//#include "keypad_api.h"
//#include "intc_api.h"
//#include "gpio_cus.h"
#include "key_defs.h"
//#include "ui_os_message.h"
//#include "event.h"
//#include "base_prv.h"
#include "mfpr_api.h"
//#include "intc.h"
//#include "gpio_api.h"
//#include "bsp_hisr.h"



//add by yazhouren >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
#define      REGS_MFPR_BASE                                  0xD401E000
#define CONFIG_BOARD_CRANE_PHONE 1
//#define BU_REG_READ(x) (*(volatile UINT32 *)(x))
//#define BU_REG_WRITE(x,y) ((*(volatile UINT32 *)(x)) = (y) )
#define CHIP_REG_OR(reg_addr, value)    (*(volatile UINT32 *)(reg_addr) |= (UINT32)(value))
static UINT32 mfp_cfg_keypad[17];
#if 0
= {
        /* Keypad */
        MFP_REG(GPIO_00) | MFP_AF1 | MFP_DRIVE_MEDIUM | MFP_PULL_LOW | MFP_SLEEP_DIR | MFP_LPM_EDGE_NONE ,
#ifdef CONFIG_BOARD_CRANE_PHONE
        MFP_REG(GPIO_01) | MFP_AF0 | MFP_DRIVE_MEDIUM | MFP_PULL_LOW | MFP_SLEEP_DIR | MFP_LPM_EDGE_NONE ,
#else
        MFP_REG(GPIO_01) | MFP_AF1 | MFP_DRIVE_MEDIUM | MFP_PULL_LOW | MFP_SLEEP_DIR | MFP_LPM_EDGE_NONE ,
#endif
        MFP_REG(GPIO_02) | MFP_AF1 | MFP_DRIVE_MEDIUM | MFP_PULL_LOW | MFP_SLEEP_DIR | MFP_LPM_EDGE_NONE ,
        MFP_REG(GPIO_03) | MFP_AF1 | MFP_DRIVE_MEDIUM | MFP_PULL_LOW | MFP_SLEEP_DIR | MFP_LPM_EDGE_NONE ,
        MFP_REG(GPIO_04) | MFP_AF1 | MFP_DRIVE_MEDIUM | MFP_PULL_LOW | MFP_SLEEP_DIR | MFP_LPM_EDGE_NONE,
        MFP_REG(GPIO_05) | MFP_AF1 | MFP_DRIVE_MEDIUM | MFP_PULL_LOW | MFP_SLEEP_DIR | MFP_LPM_EDGE_NONE,
        MFP_REG(GPIO_06) | MFP_AF1 | MFP_DRIVE_MEDIUM | MFP_PULL_LOW | MFP_SLEEP_DIR | MFP_LPM_EDGE_NONE,
        MFP_REG(GPIO_07) | MFP_AF1 | MFP_DRIVE_MEDIUM | MFP_PULL_LOW | MFP_SLEEP_DIR | MFP_LPM_EDGE_NONE,
        MFP_REG(GPIO_08) | MFP_AF2 | MFP_DRIVE_MEDIUM | MFP_PULL_LOW | MFP_SLEEP_DIR | MFP_LPM_EDGE_NONE,
        MFP_REG(GPIO_09) | MFP_AF2 | MFP_DRIVE_MEDIUM | MFP_PULL_LOW | MFP_SLEEP_DIR | MFP_LPM_EDGE_NONE,
        MFP_EOC,                 /*End of configuration, must have */
};
#endif
/*void KeypadPinmuxCfg(void)
{
        //mfp_config(mfp_cfg_keypad);
}*/
void mfp_config(unsigned int * mfp_cfgs)
{
	volatile unsigned int p_mfpr = 0;
	volatile unsigned int mfpr_offset = 0;
	volatile unsigned int cfg_val, val;

	while (1) {
		cfg_val = *mfp_cfgs++;
		/* exit if End of configuration table detected */
		if (cfg_val == MFP_EOC)
			break;

		//mfpr_offset = MFP_REG_GET_OFFSET(cfg_val);
		mfpr_offset = cfg_val >> 16;
		p_mfpr = (unsigned int) (REGS_MFPR_BASE + mfpr_offset);
		
		/* Write a mfg register as per configuration */
		val = 0;
		if (cfg_val & MFP_VALUE_MASK)
			val = cfg_val & MFP_VALUE_MASK;

		//raw_uart_log("before mfpr register set:%#x\n", val);
		if(BU_REG_READ(p_mfpr)!=val)
			BU_REG_WRITE(p_mfpr, val);
	
		//raw_uart_log("mfpr register %#x = %#x\n",p_mfpr,REG32(p_mfpr));
	}
	return;
}

void mfp_config_keypad(KEYPAD_KP_MKOUT_E row[KEYPAD_MATRIX_LENGTH], KEYPAD_KP_MKIN_E col[KEYPAD_MATRIX_LENGTH])
{
    int i = 0;
	int count = 0;
	for(i = 0; i < KEYPAD_MATRIX_LENGTH; i++) {
		if(row[i] != KP_MKO_NO_VALID) {
			if(row[i]<4) {
				mfp_cfg_keypad[count] = (MFP_REG(MFPR_OFFSET(55+row[i]*2+1))| MFP_AF1 | MFP_DRIVE_MEDIUM | MFP_PULL_LOW | MFP_SLEEP_DIR | MFP_LPM_EDGE_NONE);
			}
			else {
				mfp_cfg_keypad[count] = (MFP_REG(MFPR_OFFSET(55+row[i]*2+1))| MFP_AF2 | MFP_DRIVE_MEDIUM | MFP_PULL_LOW | MFP_SLEEP_DIR | MFP_LPM_EDGE_NONE);
			}
			count++;
		}
	}
	for(i = 0; i < KEYPAD_MATRIX_LENGTH; i++) {
		if(col[i] != KP_MKI_NO_VALID) {
			if(col[i]<4) {
				mfp_cfg_keypad[count] = (MFP_REG(MFPR_OFFSET(55+col[i]*2))| MFP_AF1 | MFP_DRIVE_MEDIUM | MFP_PULL_LOW | MFP_SLEEP_DIR | MFP_LPM_EDGE_NONE);
			}
			else {
				mfp_cfg_keypad[count] = (MFP_REG(MFPR_OFFSET(55+col[i]*2))| MFP_AF2 | MFP_DRIVE_MEDIUM | MFP_PULL_LOW | MFP_SLEEP_DIR | MFP_LPM_EDGE_NONE);
			}
			count++;
		}
	}
	mfp_cfg_keypad[count] = MFP_EOC;
	mfp_config(mfp_cfg_keypad);
	
}
#if 0
void lcd_light_on_off(int on)
{
    static int i = 0;
	volatile UINT32 p_mfpr = 0;
    volatile UINT32 mfpr_offset = 0;
    volatile UINT32 cfg_val, val;
	if (i == 0) {
		/*cfg_val = (MFP_REG(MFPR_OFFSET(55+77))| MFP_AF1 | MFP_DRIVE_MEDIUM | MFP_PULL_LOW | MFP_SLEEP_DIR | MFP_LPM_EDGE_NONE) ;
		mfpr_offset = cfg_val >> 16;
		p_mfpr = (UINT32) (REGS_MFPR_BASE + mfpr_offset);
		
		val = 0;
		if (cfg_val & MFP_VALUE_MASK)
			val = cfg_val & MFP_VALUE_MASK;
		if(BU_REG_READ(p_mfpr)!=val)
			BU_REG_WRITE(p_mfpr, val);*/
		val = (MFP_REG(MFPR_OFFSET(183))| MFP_AF1 | MFP_DRIVE_MEDIUM | MFP_PULL_LOW | MFP_SLEEP_DIR | MFP_LPM_EDGE_NONE) ;
		BU_REG_WRITE(0xD401E2DC, val);
		gpio_direction_output(HAL_GPIO_77);
		i = 1;
	}
	if(on)
		gpio_set_value(HAL_GPIO_77, 1);
	else
		gpio_set_value(HAL_GPIO_77, 0);
}
#endif
/*  bit shift macros */
#define BIT_0 (1 << 0)
#define BIT_1 (1 << 1)
#define BIT_2 (1 << 2)
#define BIT_3 (1 << 3)
#define BIT_4 (1 << 4)
#define BIT_5 (1 << 5)
#define BIT_6 (1 << 6)
#define BIT_7 (1 << 7)
#define BIT_8 (1 << 8)
#define BIT_9 (1 << 9)
#define BIT_10 (1 << 10)
#define BIT_11 (1 << 11)
#define BIT_12 (1 << 12)
#define BIT_13 (1 << 13)
#define BIT_14 (1 << 14)
#define BIT_15 (1 << 15)
#define BIT_16 (1 << 16)
#define BIT_17 (1 << 17)
#define BIT_18 (1 << 18)
#define BIT_19 (1 << 19)
#define BIT_20 (1 << 20)
#define BIT_21 (1 << 21)
#define BIT_22 (1 << 22)
#define BIT_23 (1 << 23)
#define BIT_24 (1 << 24)
#define BIT_25 (1 << 25)
#define BIT_26 (1 << 26)
#define BIT_27 (1 << 27)
#define BIT_28 (1 << 28)
#define BIT_29 (1 << 29)
#define BIT_30 (1 << 30)
#define BIT_31 ((unsigned)1 << 31)
#define BIT(n) (1<<(n))

#define SIMULATION_START_CHAR (KP_STAR)
#define SIMULATION_STOP_CHAR (KP_POUND)

typedef enum _e_cur_status
{
	IDEL = 0,
	START_FLAG,
	TRANSMITTING,
	STOP_FLAG,
	ENDING,
	ERR_STATUS
}CUR_STATUS;


//end yazhouren<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

#define INT_KEYPAD					(9)
#define INT_KEY_PRESS               (116) 

#define REG_PMU_BASE   			0xD4282800
#define REG_PMU_PMU_SD_ROT_WAKE_CLR (REG_PMU_BASE+0x07c)
#define BIT_KB_WK_CLR				(BIT(3))

#define PM812_ONKEY_INT				24


#define trace   uart_printf

#if 1
#define DEBUG(XX,...)   uart_printf("+++file:%s, line %d, func:%s\r\n",  __FILE__, __LINE__, __func__);
#define DEBUG_INFO 	uart_printf
#else
#define DEBUG(XX,...)		 do{}while(0)
#define DEBUG_INFO(XX,...)   do{}while(0)
#endif


/*Add multi-key support. multi-key is shorthanded as _MK.
 keypad layout  shuld match the enum Keypad_Num*/


/* hold the user defined function */

KeyRec key_rec;

char pressed_key[KEYPAD_ROW_NUM][KEYPAD_COL_NUM] = 
{
	{0, 0, 0, 0, 0,0,0,0},
	{0, 0, 0, 0, 0,0,0,0},
	{0, 0, 0, 0, 0,0,0,0},
	{0, 0, 0, 0, 0,0,0,0},
	{0, 0, 0, 0, 0,0,0,0},
	{0, 0, 0, 0, 0,0,0,0},
	{0, 0, 0, 0, 0,0,0,0},
	{0, 0, 0, 0, 0,0,0,0},
};
char pressed_key_bak[KEYPAD_ROW_NUM][KEYPAD_COL_NUM];

const unsigned char KeypadMatrix[KEYPAD_ROW_NUM][KEYPAD_COL_NUM]=
{
	{0, 1, 2, 3, 4, 5, 6, 7},
	{10,11,12,13,14,15,16,17},
	{20,21,22,23,24,25,26,27},
	{30,31,32,33,34,35,36,37},
	{40,41,42,43,44,45,46,47},
	{50,51,52,53,54,55,56,57},
	{60,61,62,63,64,65,66,67},
	{70,71,72,73,74,75,76,77}
};

#define	APBC_BASE	0xD4015000
#define	APBC_KPC_CLK_RST		(APBC_BASE+0x0030)	/* 32 bit	APBClock Clock/Reset Control
													 * Register for Keypad Controller
													 */
#define	APBC_KPC_CLK_RST_RST				BIT_2			/* Keypad Controller Reset Generation */
#define	APBC_KPC_CLK_RST_FNCLK				BIT_1			/* Keypad Controller Functional Clock Enable/Disable */
#define	APBC_KPC_CLK_RST_APBCLK				BIT_0			/* Keypad Controller APB Bus Clock Enable/Disable */
#define UPDATE_KEY_FLAG ("vol down")				/* Customer can modify according to their keypad */


void Keypad_init_clk(void)
{
	UINT32 value;
	DEBUG_INFO("Keypad_init_clk\n");
	value = BU_REG_READ(APBC_KPC_CLK_RST);
	value |=(APBC_KPC_CLK_RST_RST 	|
			APBC_KPC_CLK_RST_FNCLK 	|
			APBC_KPC_CLK_RST_APBCLK);
	BU_REG_WRITE (APBC_KPC_CLK_RST, value);

	value = BU_REG_READ(APBC_KPC_CLK_RST);
	value &= (~APBC_KPC_CLK_RST_RST);
	value |=(APBC_KPC_CLK_RST_FNCLK 	|
			APBC_KPC_CLK_RST_APBCLK);
	BU_REG_WRITE (APBC_KPC_CLK_RST, value);
	DEBUG_INFO("Keypad_init_clk done\n");
}

void set_debounce(int msecond) /* in msecond for both direct/matrix key */
{
	kpc_reg->kpc_kdi = ((msecond<<8) | msecond);
}
/*
 * select what kind of key you want to enable:
 * DIRECT_KEY	MATRIX_KEY	ROTARY_0	ROTARY_1
 */
void Keypad_enable(Keypad_Type key_type)
{
	UINT32 value ;
	switch(key_type){
		case MATRIX_KEY:
			value =  KPC_PC_ME | KPC_PC_MIE;
			break;
		case DIRECT_KEY:
			value = KPC_PC_DE | KPC_PC_DIE;
			break;
		case ROTARY_0:
			value = KPC_PC_REE0;
			break;
		case ROTARY_1:
			value = KPC_PC_REE1;
		default: break;
	}
	kpc_reg->kpc_pc |= value;

}

void Keypad_interrupt_enable(void)
{
	kpc_reg->kpc_pc |= (KPC_PC_MIE);
}


void Keypad_interrupt_disable(void)
{
	kpc_reg->kpc_pc &= ((~KPC_PC_MIE) & (~KPC_PC_DIE));
}

void Keypad_clear_wakup_interrupt(void)
{
	CHIP_REG_OR(REG_PMU_PMU_SD_ROT_WAKE_CLR,BIT_KB_WK_CLR);
}

void Deassert_all(void)
{
	UINT32 value;

	value = (~KPC_PC_MS7) & (~KPC_PC_MS6) 	\
			& (~KPC_PC_MS5)	& (~KPC_PC_MS4) & (~KPC_PC_MS3) 	\
			& (~KPC_PC_MS2) & (~KPC_PC_MS1) & (~KPC_PC_MS0) ;

	kpc_reg->kpc_pc &= value;

}

void Assert_all(void)
{
	UINT32 value;
	value = KPC_PC_MS7 | KPC_PC_MS6 | KPC_PC_MS5	\
		| KPC_PC_MS4 | KPC_PC_MS3 | KPC_PC_MS2 | KPC_PC_MS1 | KPC_PC_MS0;

	kpc_reg->kpc_pc |= value;
}


int is_mk()
{
	if ( kpc_reg->kpc_pc & KPC_PC_IMKP )
		return 0;
	else
		return 1;
}

int is_auto_mode(void)
{
	if ( kpc_reg->kpc_pc & KPC_PC_ASACT)
		return 1;
	else
		return 0;
}

/**************************************************************************
* Function Name:     Keypad_Init ()
*
* Description:Initialize the keypad driver
*
* Input:
*
*
* Output: none
*
*
* Return Value:
*     None.
***************************************************************************/
void key_press_isr(void);

void
Keypad_cfg(int is_auto, int is_mk, int debounce, Keypad_Type key_type, KEYPAD_KP_MKOUT_E row[KEYPAD_MATRIX_LENGTH],
               KEYPAD_KP_MKIN_E col[KEYPAD_MATRIX_LENGTH])
{
	UINT32 value;
	volatile int i;
	//INTC_ReturnCode intcStatus;
	//INTC_InterruptSources INT_NUM;

	/* reset and enbale APB KPD clock */
	//KeypadPinmuxCfg();
	mfp_config_keypad(row, col);
	Keypad_init_clk();
	/* Clear the auto scan bit */
	/* set/reset auto-scan-on-activity bit according to is_auto */
	value = KPC_PC_MKRN_MSK | KPC_PC_MKCN_MSK | KPC_PC_MS0 | KPC_PC_MS1 |    \
			KPC_PC_MS2 | KPC_PC_MS3 | KPC_PC_MS4 | KPC_PC_MS5 | KPC_PC_MS6 | \
			KPC_PC_MS7 | KPC_PC_DN_MSK;

	if ( is_auto )
		value |= KPC_PC_ASACT ;
	else
		value &= ~KPC_PC_ASACT;

	/* set/reset multi-key according to is_mk */
	if ( is_mk )
		value &= ~KPC_PC_IMKP;
	else
		value |= KPC_PC_IMKP;


	kpc_reg->kpc_pc =  value;

	set_debounce(debounce);

    
	/* Enable keypad and interrupt for both Matrix and DirectKey*/
    Keypad_enable(key_type);
    #if 0
	//register interrupt handler
	//interrupt_handler_set(INT_KEYPAD,Keypad_ISR);
	INT_NUM = INTCGetIntVirtualNum(INT_KEYPAD);

	intcStatus = INTCConfigure(INT_NUM, INTC_IRQ, INTC_HIGH_LEVEL);
	ASSERT(intcStatus == INTC_RC_OK);
	intcStatus = INTCBind(INT_NUM,Keypad_ISR);
	ASSERT(intcStatus == INTC_RC_OK);


	INT_NUM = INTCGetIntVirtualNum(INT_KEY_PRESS);

	intcStatus = INTCConfigure(INT_NUM, INTC_IRQ, INTC_HIGH_LEVEL);
	ASSERT(intcStatus == INTC_RC_OK);
	intcStatus = INTCBind(INT_NUM,key_press_isr);
	ASSERT(intcStatus == INTC_RC_OK);
    

	
	//enable interrupt
	//int_enable(INT_KEYPAD);
	INTCEnable(INT_NUM);
    #endif
	for(i=0; i<50;i++)
	;
	/* Asseset_debouncert all column for manu mode */

}

#ifdef CONFIG_BOARD_CRANE_PHONE
void PmicOnKey_isr(void)
{
#if 0
	char key_val;
	COS_EVENT key_evt;
	DEBUG();
	key_val = KP_PWR;
	key_evt.nParam1 = key_evt.nParam3 = key_val;
	if (PM812_GET_ONKEY_STATUS()){
		DEBUG_INFO("\r\n*k:%c-%c*\r\n",key_val,'U');
		key_evt.nEventId = EV_KEY_UP;
	} else {
		DEBUG_INFO("\r\n*k:%c-%c*\r\n",key_val,'D');
		key_evt.nEventId = EV_KEY_DOWN;
	}
	dm_SendKeyMessage(&key_evt);
#endif
}
#endif
void Keypad_init(void)
{
	DEBUG_INFO("Keypad_init\n");
	//Keypad_cfg(AUTO, MULTI_KEY, KEDPAD_DEBOUNCE_TIMERINTERVAL, MATRIX_KEY);
#ifdef CONFIG_BOARD_CRANE_PHONE
	//PM812_ONKEY_BIND_INTC(PmicOnKey_isr);
	//PM812_ONKEY_INTC_ENABLE(TRUE);
#endif
	DEBUG_INFO("Keypad_init done\n");
}

void dump_reg(void)
{
	trace("KPC_PC:0x%0x = 0x%0x\r\n",&kpc_reg->kpc_pc,kpc_reg->kpc_pc );
	trace("KPC_DK:0x%0x = 0x%0x\r\n", &kpc_reg->kpc_dk, kpc_reg->kpc_dk);
	trace("KPC_REC:0x%0x = 0x%0x\r\n",&kpc_reg->kpc_rec, kpc_reg->kpc_rec);
	trace("KPC_MK:0x%0x = 0x%0x\r\n",&kpc_reg->kpc_mk, kpc_reg->kpc_mk);
	trace("KPC_AS:0x%0x = 0x%0x\r\n",&kpc_reg->kpc_as, kpc_reg->kpc_as);
	trace("KPC_ASMKP0:0x%0x = 0x%0x\r\n",&kpc_reg->kpc_asmkp0, kpc_reg->kpc_asmkp0);
	trace("KPC_ASMKP1:0x%0x = 0x%0x\r\n",&kpc_reg->kpc_asmkp1, kpc_reg->kpc_asmkp1);
	trace("KPC_ASMKP2:0x%0x = 0x%0x\r\n",&kpc_reg->kpc_asmkp2, kpc_reg->kpc_asmkp2);
	trace("KPC_ASMKP3:0x%0x = 0x%0x\r\n",&kpc_reg->kpc_asmkp3, kpc_reg->kpc_asmkp3);
	trace("KPC_KDI:0x%0x = 0x%0x\r\n", &kpc_reg->kpc_kdi,kpc_reg->kpc_kdi);
}

/**************************************************************************
* Function Name:     Keypad_ISR()
*
* Description: Keypad Interrupt Service Routine
*			   Every Key press generate interrupt, the ISR for the Keypad will 
*			   get valid key press/release and disable more interrupts from the keypad 
* Input:
*     
*
* Output:
*     
*
* Return Value:
*     None.
***************************************************************************/
ValEventFunc keypad_callback_func = NULL;
static KEYPAD_KP_MKIN_E k_row_g[KEYPAD_MATRIX_LENGTH];
static KEYPAD_KP_MKOUT_E k_col_g[KEYPAD_MATRIX_LENGTH];

#if 0 
extern void txchk_all_task_stack(void);
extern void send_satrt_alarm_on_HW_req(void);
static OS_HISR	keypad_hisr_ref;


void key_press_isr(void)
{
	INTC_InterruptSources INT_NUM;
	COS_EVENT key_evt;

	Keypad_clear_wakup_interrupt();

	INT_NUM = INTCGetIntVirtualNum(INT_KEY_PRESS);	
	INTCDisable(INT_NUM);

	INT_NUM = INTCGetIntVirtualNum(INT_KEYPAD);	
	INTCEnable(INT_NUM);

	key_evt.nEventId = EV_KEY_DOWN; 
	key_evt.nParam1 = 10;
	key_evt.nParam2 = 10;

	if (keypad_callback_func != NULL)
		keypad_callback_func(key_evt);	
}

void keypad_d2_prepare_callback(void)
{
	INTC_InterruptSources INT_NUM;

	Keypad_clear_wakup_interrupt();

	INT_NUM = INTCGetIntVirtualNum(INT_KEYPAD);	
	INTCDisable(INT_NUM);

	INT_NUM = INTCGetIntVirtualNum(INT_KEY_PRESS);	
	INTCEnable(INT_NUM);
}
#endif
static void keypad_hisr(void);




static void  keypad_hisr(void)
{
    KeyRec *keyp = &key_rec;
    UINT32 key_pressed_num = 0,pressed_col=0, pressed_row=0,i = 0;
	UINT32 key_pressed0;
	UINT32 timeout_cnt;
	int key_found = 0;
	UINT8 key_val;
	COS_EVENT key_evt;
	int temp, temp2;
	
	    /* dump registers */
		//dump_reg();
		memcpy(&pressed_key_bak,&pressed_key,sizeof(pressed_key));
		memset(&pressed_key,0,sizeof(pressed_key));
	
		/* single key; auto mode */
		timeout_cnt = 0;
		/* wait for KPC_AS_SO off for valid data */
		while ( kpc_reg->kpc_as & KPC_AS_SO )
		{
			if((timeout_cnt++)> 0xfffff)
			{
				DEBUG_INFO("timeout Error -- scan on with interrupt!\r\n");
				return;
			}
		}
		key_pressed0 = kpc_reg->kpc_as;
		key_pressed_num = (key_pressed0 & KPC_AS_MUKP_MSK) >> KPC_AS_MUKP_BASE;
		keyp->kcnt = key_pressed_num;
		DEBUG_INFO("multikey:%d\n", key_pressed_num);
		if ( key_pressed_num == 1 )
		{
			pressed_col = (key_pressed0 & KPC_AS_CP_MSK) >> KPC_AS_CP_BASE;
			pressed_row = (key_pressed0 & KPC_AS_RP_MSK) >> KPC_AS_RP_BASE;
			DEBUG_INFO("Func:%s,p_col:%d,p_row:%d\r\n",__func__,pressed_col,pressed_row);
			if (pressed_col != 0xf && pressed_row != 0xf){
					keyp->keys[0] = (pressed_row<<8)+pressed_col;
					pressed_key[pressed_row][pressed_col] = 1;
			}
		}
		else if(key_pressed_num > 1)	/* more than one key */
		{
			/* Auto mode multi-key. */
			for (i = 0; i < 4; i++)
			{
				timeout_cnt = 0;
				/* if scan-on, the data is invalid */
				while ( BU_REG_READ(KPC_ASMKP0+i*8) & KPC_ASMKP0_SO)
				{
					if((timeout_cnt++)> 0xfffff)
					{
						DEBUG_INFO("timeout Error -- scan on with interrupt!\r\n");
						return;
					}
				}
				key_pressed0 = BU_REG_READ(KPC_ASMKP0+i*8);
	
				pressed_col = (key_pressed0 & KPC_ASMKP0_MKC0_MSK) >> KPC_ASMKP0_MKC0_BASE;
				if ( pressed_col )
				{
					for (pressed_row=0; pressed_row<8; pressed_row++)
					{
						if ( pressed_col & BIT(pressed_row))
						{
							keyp->keys[key_found++] = (pressed_row<<8)+(i*2);
							pressed_key[pressed_row][i*2] = 1;
						} 
					}
				}
	
				pressed_col = (key_pressed0 & KPC_ASMKP0_MKC1_MSK) >> KPC_ASMKP0_MKC1_BASE;
				if ( pressed_col )
				{
					for (pressed_row=0; pressed_row<8; pressed_row++)
					{
						if ( pressed_col & BIT(pressed_row))
						{
							keyp->keys[key_found++] = (pressed_row<<8)+(i*2+1);
							pressed_key[pressed_row][(i*2+1)] = 1;
						}
					}
				}
			}
		}
	
		/*	 */
		DEBUG();
		
		for (pressed_row = 0; pressed_row < KEYPAD_ROW_NUM; pressed_row++ ){
			for (pressed_col = 0; pressed_col < KEYPAD_COL_NUM; pressed_col++ ){
				if (pressed_key_bak[pressed_row][pressed_col] && pressed_key[pressed_row][pressed_col])
				{
					key_val = KeypadMatrix[pressed_row][pressed_col];
					//DEBUG_INFO("always pressed word: %c \r\n",KeypadMatrix[pressed_row][pressed_col]);
				} else if (pressed_key_bak[pressed_row][pressed_col] && (0 == pressed_key[pressed_row][pressed_col])) {
					key_val = KeypadMatrix[pressed_row][pressed_col];
					//DEBUG_INFO("\r\n*k:%c-%c*\r\n",key_val,'U');
					key_evt.nEventId = EV_KEY_UP;
					//key_evt.nParam1	=key_evt.nParam3 = key_val;
					temp2 = 0;
					for(temp =0; temp < KEYPAD_MATRIX_LENGTH; temp++) {
						if(k_row_g[temp] == pressed_col) {
							break;
						}
						if(k_row_g[temp] != KP_MKI_NO_VALID) {
							temp2++;
						}
					}
					key_evt.nParam1 = temp2;
					temp2 = 0;
					for(temp =0; temp < KEYPAD_MATRIX_LENGTH; temp++) {
						if(k_col_g[temp] == pressed_row) {
							break;
						}
						if(k_col_g[temp] != KP_MKO_NO_VALID) {
							temp2++;
						}
					}
					key_evt.nParam2 = temp2;
					if (keypad_callback_func != NULL)
						keypad_callback_func(key_evt);
					//dm_SendKeyMessage(&key_evt);
				//	send_satrt_alarm_on_HW_req();
				}else if ((0 == pressed_key_bak[pressed_row][pressed_col]) && pressed_key[pressed_row][pressed_col]) {
					key_val = KeypadMatrix[pressed_row][pressed_col];
					//DEBUG_INFO("\r\n*k:%c-%c*\r\n",key_val,'D');
					key_evt.nEventId = EV_KEY_DOWN; 
					//key_evt.nParam1	= key_evt.nParam3 = key_val;
					temp2 = 0;
					for(temp =0; temp < KEYPAD_MATRIX_LENGTH; temp++) {
						if(k_row_g[temp] == pressed_col) {
							break;
						}
						if(k_row_g[temp] != KP_MKI_NO_VALID) {
							temp2++;
						}
					}
					key_evt.nParam1 = temp2;
					temp2 = 0;
					for(temp =0; temp < KEYPAD_MATRIX_LENGTH; temp++) {
						if(k_col_g[temp] == pressed_row) {
							break;
						}
						if(k_col_g[temp] != KP_MKO_NO_VALID) {
							temp2++;
						}
					}
					key_evt.nParam2 = temp2;
					if (keypad_callback_func != NULL)
						keypad_callback_func(key_evt);
					//dm_SendKeyMessage(&key_evt);
					if(KP_POUND == key_val || KP_STAR == key_val){
						//txchk_all_task_stack();
					//	pmic_power_down();
					}
				}
			}
		}

}

INT32 ValKeypadInit(BOOL MultiKey, UINT8 KeyHoldDuration, KEYPAD_KP_MKOUT_E row[KEYPAD_MATRIX_LENGTH],
	                   KEYPAD_KP_MKIN_E col[KEYPAD_MATRIX_LENGTH])
{
    //OS_Create_HISR(&keypad_hisr_ref, "uiKpdIsr", keypad_hisr, 2);
    memcpy(k_row_g, row, sizeof(k_row_g));
	memcpy(k_col_g, col, sizeof(k_col_g));
	//Keypad_cfg(AUTO, MultiKey, KEDPAD_DEBOUNCE_TIMERINTERVAL, MATRIX_KEY);
	Keypad_cfg(AUTO, MultiKey, KeyHoldDuration, MATRIX_KEY,row,col);
	return 0;
}
 
void ValKeypadRegister(ValEventFunc EventFuncP)
{
    keypad_callback_func = EventFuncP;
}

void ValKeypadMultikeyEn(BOOL En)
{
   if (En)
		kpc_reg->kpc_pc &= ~KPC_PC_IMKP;
	else
		kpc_reg->kpc_pc |= KPC_PC_IMKP;
}


char *key_string[5][5]= {"","camera","vol down","vol up","F right", 
                        // "up","F left","left","ok","right",
                         "ring off","down","call","1","2",
                         "3","4","5", "6","7",
                         "8","0","#","9","*"};
		
int key_quit = 0;


/*+++++++++++++++++++++++++++++++++*/
//Detect the OTA_Keypad_Flag pressed down or not?
static BOOL Is_Update_Key_Flag(const UINT32 row,const UINT32 col);	

//Delay for a period time
static void Keypad_Wait(const UINT32 count);

//After delaying a time, check the Keypad register value with the value before delaying
static BOOL Keyval_Check(const UINT32 val_before,const UINT32 val_after);
/*+++++++++++++++++++++++++++++++++*/



void Keypad_ISR(void)
{
	/*clean inter status*/
	static int index_print = 0;		//hsy
	if (!( kpc_reg->kpc_pc & KPC_PC_MI)){
		Keypad_clear_wakup_interrupt();
		if(index_print < 5)
			uart_printf("Keypad_ISR_cond_1\r\n,index_print:%d",index_print);
		index_print++;
		return;
	}

	if( !is_auto_mode()){ //not auto_mode
		if(index_print < 5)
			uart_printf("Keypad_ISR_cond_2\r\n,index_print:%d",index_print);
		index_print++;
		return;
	}
    keypad_hisr();
	
}

static void Keypad_Wait(const UINT32 count)
{
    UINT32 i = 0;
    for(i=0; i<count; i++)
    {;}
}


static BOOL Keyval_Check(const UINT32 val_before,const UINT32 val_after)
{
	return (val_before == val_after);
}

static BOOL Is_Update_Key_Flag(const UINT32 row,const UINT32 col)
{
	UINT32 temp, temp2 = 0;
	UINT32 row_custom_built,col_custom_built;

	uart_printf("Func:%s,row:%d;col:%d\r\n",__func__,row,col);
	for(temp =0; temp < KEYPAD_MATRIX_LENGTH; temp++) {
		if(k_row_g[temp] == col) {
			break;
		}
		if(k_row_g[temp] != KP_MKI_NO_VALID) {
			temp2++;
		}
	}
	row_custom_built = temp2;
	
	temp2 = 0;
	for(temp =0; temp < KEYPAD_MATRIX_LENGTH; temp++) {
		if(k_col_g[temp] == row) {
			break;
		}
		if(k_col_g[temp] != KP_MKO_NO_VALID) {
			temp2++;
		}
	}
	col_custom_built = temp2;

	DEBUG_INFO("[2_3]update_key_flag:[%s]; The pressed key value:[%s]\r\n",UPDATE_KEY_FLAG,
		key_string[row_custom_built][col_custom_built]);

	return (!strncmp(key_string[row_custom_built][col_custom_built],UPDATE_KEY_FLAG,strlen(UPDATE_KEY_FLAG)));	
}
/*+++++++++++++++++++++++++++++++++*/
INT32 callback(COS_EVENT event)
{
	DEBUG_INFO("\r\n%s row:%d, col:%d -%c*\r\n",key_string[event.nParam1][event.nParam2], event.nParam1,event.nParam2, event.nEventId==EV_KEY_DOWN?'D':'U');
	key_quit = 1;
}


BOOL Keypad_OTA_Flag_Check_U(void)
{
	UINT32 key_pressed0;
	UINT32 pressed_row_before=0,pressed_col_before=0;
	UINT32 pressed_row_after=0,pressed_col_after=0;
	volatile int i = 0;
	
	uart_printf("Func:%s,Line:%d\r\n",__func__,__LINE__);
	uart_printf("[%57s]\r\n","---------------Keypad_OTA_Flag_Check_U---------------------");

	//Customer can modify according to their keypad
	KEYPAD_KP_MKOUT_E row[KEYPAD_MATRIX_LENGTH]= {KP_MKO0, KP_MKO2, KP_MKO3, KP_MKO4, KP_MKO_NO_VALID,
												   KP_MKO_NO_VALID,KP_MKO_NO_VALID, KP_MKO_NO_VALID};
	KEYPAD_KP_MKIN_E col[KEYPAD_MATRIX_LENGTH] ={KP_MKI0, KP_MKI1,KP_MKI2, KP_MKI3, KP_MKI4, KP_MKI_NO_VALID, 
												  KP_MKI_NO_VALID,KP_MKI_NO_VALID};
	//Init two global arrays: [k_row_g] and [k_col_g]
	memcpy(k_row_g, row, sizeof(k_row_g));
	memcpy(k_col_g, col, sizeof(k_col_g));
												  	

	//[2] Detec if key UPDATE_KEY_FLAG has pressed down or not?
	key_pressed0 = kpc_reg->kpc_as;
	pressed_row_before = (key_pressed0 & KPC_AS_RP_MSK) >> KPC_AS_RP_BASE;
	pressed_col_before = (key_pressed0 & KPC_AS_CP_MSK) >> KPC_AS_CP_BASE;
	if (pressed_row_before == 0xf || pressed_col_before == 0xf) { //[2_2] No key has pressed down
		DEBUG_INFO("[2_2]No key pressed down\r\n");
		return FALSE;		
	}
	
	DEBUG_INFO("[2_1]get a key value\r\n");
	DEBUG_INFO("[2_1]p_row_b:%d,p_col_b:%d\r\n",pressed_row_before,pressed_col_before);
	//Check key val is UPDATE_KEY_FLAG? 
	if(!Is_Update_Key_Flag(pressed_row_before,pressed_col_before)){
		DEBUG_INFO("[2_2] No [UPDATE_KEY_FLAG] pressed down\r\n");
		return FALSE;	
	}

	//[3] Delay 0.5s, detect the above key status: pressing down??
	Keypad_Wait(0x500000);
	DEBUG_INFO("[3]After Keypad_Wait 0.5s\r\n");

	//[4]After delaying a time, check the UPDATE_KEY_FLAG pressed down or not
	key_pressed0 = kpc_reg->kpc_as;		//Note: read register[kpc_as] again!		
	pressed_row_after = (key_pressed0 & KPC_AS_RP_MSK) >> KPC_AS_RP_BASE;
	pressed_col_after = (key_pressed0 & KPC_AS_CP_MSK) >> KPC_AS_CP_BASE;
	if (pressed_row_after != 0xf && pressed_col_after != 0xf){
		DEBUG_INFO("[4_1]Get a key value\r\n");
		DEBUG_INFO("[4_1]p_col:%d,p_row:%d\r\n",pressed_row_after,pressed_col_after);
	}else{
		DEBUG_INFO("[4_2]After delaying 0.5s no UPDATE_KEY_FLAG pressed down\r\n");
		return FALSE;		
	}

	if( Keyval_Check(pressed_row_before, pressed_row_after) &&	
		Keyval_Check(pressed_col_before, pressed_col_after)){
		//Have Detected the OTA_Sd_Keypad_Flag combination
		DEBUG_INFO("[4_1]Have Detected the OTA_Sd_Keypad_Flag combination\r\n");
		return TRUE;
	}else{
		DEBUG_INFO("[4_2]After delaying 0.5s, no OTA_Sd_Keypad_Flag pressed down\r\n");
		return FALSE;
	}
	
}



void keypad_test(void)
{
    uart_printf("keypad_test start\r\n");
	ValKeypadRegister(callback);
	KEYPAD_KP_MKOUT_E row[KEYPAD_MATRIX_LENGTH]= {KP_MKO0, KP_MKO2, KP_MKO3, KP_MKO4, KP_MKO_NO_VALID,
		                                           KP_MKO_NO_VALID,KP_MKO_NO_VALID, KP_MKO_NO_VALID};
	KEYPAD_KP_MKIN_E col[KEYPAD_MATRIX_LENGTH] ={KP_MKI0, KP_MKI1,KP_MKI2, KP_MKI3, KP_MKI4, KP_MKI_NO_VALID, 
		                                          KP_MKI_NO_VALID,KP_MKI_NO_VALID};
	
    ValKeypadInit(FALSE, KEDPAD_DEBOUNCE_TIMERINTERVAL, row, col);
	ValKeypadMultikeyEn(TRUE);
	volatile int i = 0;
	while(1) {
		while(1) {
		    if(key_quit)  {
				key_quit = 0;
			    break;
			}
	        Keypad_ISR();
			//dump_reg();
			while(i<1000)
				i++;
			i = 0;
	    }
		uart_printf("get a key value\r\n");
	}
	uart_printf("keypad_test end\r\n");
}





