#ifndef __APBCLK_H__
#define __APBCLK_H__
#define     __IO    volatile             /*!< defines 'read / write' permissions              */
typedef unsigned char				uint8_t;
typedef unsigned short				uint16_t;
typedef unsigned int				uint32_t;
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

typedef struct
{

   __IO uint32_t UART0_CLK_RST;
   __IO uint32_t UART1_CLK_RST;		   //0x0004 
   __IO uint32_t GPIO_CLK_RST;		//0x0008 
   __IO uint32_t PWM0_CLK_RST;		//0x000c 
   __IO uint32_t PWM1_CLK_RST;		//0x0010 
   __IO uint32_t PWM2_CLK_RST;		//0x0014 
   __IO uint32_t PWM3_CLK_RST;      //0x0018 
   __IO uint32_t SSP0_CLK_RST;		//0x001c 
   __IO uint32_t SSP1_CLK_RST;		//0x0020 
   __IO uint32_t IPC_CLK_RST;       //0x0024 
   __IO uint32_t RTC_CLK_RST;       //0x0028
   __IO uint32_t TWSI_CLK_RST;		//0x002c 
   __IO uint32_t KPC_CLK_RST;       //0x0030 
   __IO uint32_t TIMERS_CLK_RST;	   //0x0034 
   __IO uint32_t TB_ROTARY_CLK_RST;	//0x0038
   __IO uint32_t AIB_CLK_RST;		   //0x003C
   __IO uint32_t SW_JTAG_CLK_RST;	//0x0040
   __IO uint32_t TIMERS1_CLK_RST;	//0x0044 
   __IO uint32_t ONEWIRE_CLK_RST;	//0x0048
   __IO uint32_t SSP2_CLK_RST;		//0x004C 
   __IO uint32_t ASFAR;			      //0x0050
   __IO uint32_t ASSAR;			      //0x0054
   __IO uint32_t RESERVED1[2];      //0x0058-0x00060
   __IO uint32_t TWSI2_CLK_RST;     //0x60
   __IO uint32_t COUNTER_CLK_SEL;   //0x0064
   __IO uint32_t RESERVED2;         //0x0068
   __IO uint32_t TSEN_CLK_RST;		//0x006c 
   __IO uint32_t USCER1;            //0x0070
   __IO uint32_t USCER2;            //0x0074
   __IO uint32_t ICER;              //0x0078
   __IO uint32_t TWSI3_CLK_RST;     //0x007c
}APBClock_TypeDef;

#define  APBClock_BASE  0xD4015000

#define  APBClock  (( APBClock_TypeDef *) APBClock_BASE )


//0x0000 offset Clock/Reset Control Register for UART 0
#define UART0_CLK_RST_APBCLK   BIT_0
#define UART0_CLK_RST_FNCLK   BIT_1
#define UART0_CLK_RST_RST   BIT_2
#define UART0_CLK_RST_FNCLKSEL_MSK  SHIFT4(0x7)
#define UART0_CLK_RST_FNCLKSEL_BASE   4


//0x004 offset Clock/Reset Control Register for UART 1
#define UART1_CLK_RST_APBCLK   BIT_0
#define UART1_CLK_RST_FNCLK   BIT_1
#define UART1_CLK_RST_RST   BIT_2
#define UART1_CLK_RST_FNCLKSEL_MSK  SHIFT4(0x7)
#define UART1_CLK_RST_FNCLKSEL_BASE   4


//0x008 offset Clock/Reset Control Register for GPIO
#define GPIO_CLK_RST_APBCLK   BIT_0
#define GPIO_CLK_RST_FNCLK   BIT_1
#define GPIO_CLK_RST_RST   BIT_2
#define GPIO_CLK_RST_FNCLKSEL_MSK  SHIFT4(0x7)
#define GPIO_CLK_RST_FNCLKSEL_BASE   4



//0x00c offset Clock/Reset Control Register for PWM 0
#define PWM0_CLK_RST_APBCLK   BIT_0
#define PWM0_CLK_RST_FNCLK   BIT_1
#define PWM0_CLK_RST_RST   BIT_2
#define PWM0_CLK_RST_FNCLKSEL_MSK  SHIFT4(0x7)
#define PWM0_CLK_RST_FNCLKSEL_BASE   4



//0x0010 offset Clock/Reset Control Register for PWM 1
#define PWM1_CLK_RST_APBCLK   BIT_0
#define PWM1_CLK_RST_FNCLK   BIT_1
#define PWM1_CLK_RST_RST   BIT_2
#define PWM1_CLK_RST_FNCLKSEL_MSK  SHIFT4(0x7)
#define PWM1_CLK_RST_FNCLKSEL_BASE   4


//0x0014 offset Clock/Reset Control Register for PWM 2
#define PWM2_CLK_RST_APBCLK   BIT_0
#define PWM2_CLK_RST_FNCLK   BIT_1
#define PWM2_CLK_RST_RST   BIT_2
#define PWM2_CLK_RST_FNCLKSEL_MSK  SHIFT4(0x7)
#define PWM2_CLK_RST_FNCLKSEL_BASE   4


//0x0018 offset Clock/Reset Control Register for PWM 3
#define PWM3_CLK_RST_APBCLK   BIT_0
#define PWM3_CLK_RST_FNCLK   BIT_1
#define PWM3_CLK_RST_RST   BIT_2
#define PWM3_CLK_RST_FNCLKSEL_MSK  SHIFT4(0x7)
#define PWM3_CLK_RST_FNCLKSEL_BASE   4


//0x001c offset Clock/Reset Control Register for SSP 0
#define SSP0_CLK_RST_APBCLK   BIT_0
#define SSP0_CLK_RST_FNCLK   BIT_1
#define SSP0_CLK_RST_RST   BIT_2
#define SSP0_CLK_RST_FNCLKSEL_MSK  SHIFT4(0x7)
#define SSP0_CLK_RST_FNCLKSEL_BASE   4
#define SSP0_CLK_RST_SEL_SSP_FUNC_CLK  BIT_7

//0x0020 offset Clock/Reset Control Register for SSP 1
#define SSP1_CLK_RST_APBCLK   BIT_0
#define SSP1_CLK_RST_FNCLK   BIT_1
#define SSP1_CLK_RST_RST   BIT_2
#define SSP1_CLK_RST_FNCLKSEL_MSK  SHIFT4(0x7)
#define SSP1_CLK_RST_FNCLKSEL_BASE   4
#define SSP1_CLK_RST_SEL_SSP_FUNC_CLK  BIT_7


//0x0024 offset Clock/Reset Control Register for IPC
#define IPC_CLK_RST_APBCLK   BIT_0
#define IPC_CLK_RST_FNCLK   BIT_1
#define IPC_CLK_RST_RST   BIT_2
#define IPC_CLK_RST_FNCLKSEL_MSK  SHIFT4(0x7)
#define IPC_CLK_RST_FNCLKSEL_BASE   4

//0x0028 offset Clock/Reset Control Register for RTC
#define RTC_CLK_RST_APBCLK   BIT_0
#define RTC_CLK_RST_FNCLK   BIT_1
#define RTC_CLK_RST_RST   BIT_2
#define RTC_CLK_RST_PM_POWER_SENSOR   BIT_7
#define RTC_CLK_RST_FNCLKSEL_MSK     SHIFT4(0x7)
#define RTC_CLK_RST_FNCLKSEL_BASE      4

//0x002c offset Clock/Reset Control Register for TWSI1
#define TWSI1_CLK_RST_APBCLK   BIT_0
#define TWSI1_CLK_RST_FNCLK   BIT_1
#define TWSI1_CLK_RST_RST   BIT_2
#define TWSI1_CLK_RST_FNCLKSEL_MSK  SHIFT4(0x7)
#define TWSI1_CLK_RST_FNCLKSEL_BASE   4

//0x0030 offset Clock/Reset Control Register for Keypad Controller
#define KPC_CLK_RST_APBCLK   BIT_0
#define KPC_CLK_RST_FNCLK   BIT_1
#define KPC_CLK_RST_RST   BIT_2
#define KPC_CLK_RST_FNCLKSEL_MSK  SHIFT4(0x7)
#define KPC_CLK_RST_FNCLKSEL_BASE   4


//0x0034 offset Clock/Reset Control Register for Timer 0 
#define TIMERS0_CLK_RST_APBCLK   BIT_0
#define TIMERS0_CLK_RST_FNCLK   BIT_1
#define TIMERS0_CLK_RST_RST   BIT_2
#define TIMERS0_CLK_RST_FNCLKSEL_MSK  SHIFT4(0x7)
#define TIMERS0_CLK_RST_FNCLKSEL_BASE   4



//0x0038 offset Clock/Reset Control Register for Trackball and Rotary
#define TB_ROTARY_CLK_RST_APBCLK   BIT_0
#define TB_ROTARY_CLK_RST_FNCLK   BIT_1
#define TB_ROTARY_CLK_RST_RST   BIT_2
#define TB_ROTARY_CLK_RST_FNCLKSEL_MSK  SHIFT4(0x7)
#define TB_ROTARY_CLK_RST_FNCLKSEL_BASE   4

//0x003c offset Clock/Reset Control Register for AIB
#define AIB_CLK_RST_APBCLK   BIT_0
#define AIB_CLK_RST_FNCLK   BIT_1
#define AIB_CLK_RST_RST   BIT_2
#define AIB_CLK_RST_FNCLKSEL_MSK  SHIFT4(0x7)
#define AIB_CLK_RST_FNCLKSEL_BASE   4


//0x0040 offset Clock/Reset Control Register for JTAG Software Emulation
#define SW_JTAG_CLK_RST_APBCLK   BIT_0
#define SW_JTAG_CLK_RST_FNCLK   BIT_1
#define SW_JTAG_CLK_RST_RST   BIT_2
#define SW_JTAG_CLK_RST_FNCLKSEL_MSK  SHIFT4(0x7)
#define SW_JTAG_CLK_RST_FNCLKSEL_BASE   4


//0x0044 offset Clock/Reset Control Register for Timer 1
#define TIMERS1_CLK_RST_APBCLK   BIT_0
#define TIMERS1_CLK_RST_FNCLK   BIT_1
#define TIMERS1_CLK_RST_RST   BIT_2
#define TIMERS1_CLK_RST_FNCLKSEL_MSK  SHIFT4(0x7)
#define TIMERS1_CLK_RST_FNCLKSEL_BASE   4


//0x0048 offset Clock/Reset Control Register for One-Wire
#define ONEWIRE_CLK_RST_APBCLK   BIT_0
#define ONEWIRE_CLK_RST_FNCLK   BIT_1
#define ONEWIRE_CLK_RST_RST   BIT_2
#define ONEWIRE_CLK_RST_FNCLKSEL_MSK  SHIFT4(0x7)
#define ONEWIRE_CLK_RST_FNCLKSEL_BASE   4


//0x004c offset Clock/Reset Control Register for SSP 2
#define SSP2_CLK_RST_APBCLK   BIT_0
#define SSP2_CLK_RST_FNCLK   BIT_1
#define SSP2_CLK_RST_RST   BIT_2
#define SSP2_CLK_RST_FNCLKSEL_MSK  SHIFT4(0x7)
#define SSP2_CLK_RST_FNCLKSEL_BASE   4
#define SSP2_CLK_RST_SEL_SSP_FUNC_CLK  BIT_7


//0x0050 AIB First  Access Register
#define APBC_ASFAR_MSK           SHIFT0(0xffff)
#define APBC_ASFAR_BASE          0

//0x0054 AIB Secure Second Access Register
#define APBC_ASSAR_MSK           SHIFT0(0xffff)
#define APBC_ASSAR_BASE          0

//0x0058 DRO_TS control Register
#define DRO_TS_dro_rst        BIT_2
#define DRO_TS_dro_func_clk_en        BIT_1
#define DRO_TS_dro_apb_clk_en       BIT_0

//0x005C I2C IO_control Access Register
#define I2C_IO_CTRL_i2c_control_MSK    SHIFT0(0xffff)
#define I2C_IO_CTRL_i2c_control_BASE     0

//0x0060 Clock/Reset Control Register for I2C1
#define APBC_I2C1_CLK_RST_APBCLK   BIT_0
#define APBC_I2C1_CLK_RST_FNCLK   BIT_1
#define APBC_I2C1_CLK_RST_RST   BIT_2
#define APBC_I2C1_CLK_RST_FNCLKSEL_MSK  SHIFT4(0x7)
#define APBC_I2C1_CLK_RST_FNCLKSEL_BASE   4




#endif
