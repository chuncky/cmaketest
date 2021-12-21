#ifndef _PIMC_H_
#define _PIMC_H_
#include "common.h"
/************************** MACROS ***************************************/
#define SET     1
#define CLEAR   0
#if 0
typedef signed char     INT8;         /* Signed 8-bit quantity          */
typedef signed short    INT16;        /* Signed 16-bit quantity         */
typedef signed long     INT32;        /* Signed 32-bit quantity         */
typedef unsigned char   UINT8;        /* Unsigned 8-bit quantity        */
typedef unsigned short  UINT16;       /* Unsigned 16-bit quantity       */
typedef unsigned long   UINT32;       /* Unsigned 32-bit quantity       */
typedef unsigned long   BU_U32;       /* Unsigned 32-bit quantity       */

/* Standard typedefs */
typedef unsigned char   BOOL;         /* Boolean                        */

#ifndef TRUE
  #define FALSE   0
  #define TRUE	  1
#endif	/* TRUE */
//gh_zh
#define BU_REG_READ(x) (*(volatile unsigned long *)(x))
#define BU_REG_WRITE(x,y) ((*(volatile unsigned long *)(x)) = y )


#endif
#define NULL 0 
void usticaInit(void);

#define PMIC_UNAVAILABLE 0xFF //UNAVAILABLE
#define PMIC_812    0xC /* PM812: 0x64 */
#define PMIC_813    0x6 /* PM813: 0x30(A0), 0x31(A1)... */
#define PMIC_813A3  0x7
    #define PMIC_813_A0 0x30 
    #define PMIC_813_A1 0x31 
    #define PMIC_813_A2 0x32
    #define PMIC_813_A3 0x3B
#define PMIC_802    0x2 /* PM802: 0x12(B0), 0x13(B1)... */
#define PMIC_803    0x3 /* PM803: 0x18(A0) */ 
#define PMIC_813S   0x4 /* PM813S: 0x20(A0) */ 
    #define PMIC_813S_A0 0x20 


// The I2C registers offset
#define I2C_ISAR_REG                0x0020  // Slave Address Register
#define I2C_ISR_REG                 0x0018  // Status Register
#define I2C_ICR_REG                 0x0010  // Control Register
#define I2C_IDBR_REG                0x0008  // Data Byte Register
#define I2C_IBMR_REG                0x0000  // Bus Monitor Register

#define I2C_OWN_SLAVE_ADDRESS                     0xFF        /* response */

/********************* Sub-Package Definitions ********************/
#define I2C_REG_CLEAR_VALUE         	0x0000
#define I2C_ICR_CLEAR_ALL_CONTROL_BITS  0xfff0	// Clear the TB, STOP, START, ACK bits in the ICR register

/** bit map of the ICR register **/
#define I2C_ICR_BUS_RESET_BIT                       28
#define I2C_ICR_FAST_MODE                           15
#define I2C_ICR_UNIT_RESET_BIT                      14
#define I2C_ICR_IDBR_RECEIVE_FULL_INT_ENABLE_BIT    9
#define I2C_ICR_UNIT_ENABLE_BIT                     6
#define I2C_ICR_SCLEA_ENABLE_BIT                    5
#define I2C_ICR_MASTER_ABORT_BIT                    4
#define I2C_ICR_TRANSFER_BYTE_BIT                   3
#define I2C_ICR_ACK_NACK_CONTROL_BIT                2
#define I2C_ICR_STOP_BIT                            1
#define I2C_ICR_START_BIT                           0

#define I2C_ICR_BUS_RESET                             (1 << I2C_ICR_BUS_RESET_BIT)
#define I2C_ICR_UNIT_RESET                            (1 << I2C_ICR_UNIT_RESET_BIT)
#define I2C_ICR_IDBR_RECEIVE_FULL_INT_ENABLE          (1 << I2C_ICR_IDBR_RECEIVE_FULL_INT_ENABLE_BIT)
#define I2C_ICR_UNIT_ENABLE                           (1 << I2C_ICR_UNIT_ENABLE_BIT)
#define I2C_ICR_SCLEA_ENABLE                          (1 << I2C_ICR_SCLEA_ENABLE_BIT)
#define I2C_ICR_MASTER_ABORT                          (1 << I2C_ICR_MASTER_ABORT_BIT)
#define I2C_ICR_TRANSFER_BYTE                         (1 << I2C_ICR_TRANSFER_BYTE_BIT)
#define I2C_ICR_ACK_NACK_CONTROL                      (1 << I2C_ICR_ACK_NACK_CONTROL_BIT)
#define I2C_ICR_STOP                                  (1 << I2C_ICR_STOP_BIT)
#define I2C_ICR_START                                 (1 << I2C_ICR_START_BIT)

/** bit map of the ISR register **/
#define I2C_ISR_UNEXPECTED_INTERRUPTS_REPORT			0xff
#define I2C_ISR_UNEXPECTED_INTERRUPTS					0x00000310
#define I2C_ISR_CLEAR_ALL_INTERRUPTS					0x000007f0

#define I2C_ISR_EARLY_IC_BUS_BUSY_BIT					11
#define I2C_ISR_BUS_ERROR_DETECTED_BIT                  10
#define I2C_ISR_SLAVE_ADDRESS_DETECTED_BIT              9
#define I2C_ISR_GENERAL_CALL_ADDRESS_DETECTED_BIT       8
#define I2C_ISR_IDBR_RECEIVE_FULL_BIT                   7
#define I2C_ISR_IDBR_TRANSMIT_EMPTY_BIT                 6
#define I2C_ISR_ARBITRATION_LOSS_DETECTED_BIT           5
#define I2C_ISR_SLAVE_STOP_DETECTED_BIT                 4
#define I2C_ISR_I2C_BUS_BUSY_BIT                        3
#define I2C_ISR_UNIT_BUSY_BIT                           2
#define I2C_ISR_ACK_NACK_STATUS_BIT                     1
#define I2C_ISR_READ_WRITE_MODE_BIT                     0




/** bit map of the IBMR register **/
#define I2C_IBMR_SDA_BIT                     		0
#define I2C_IBMR_SCL_BIT                     		1
#define I2C_IBMR_SCL_AND_SDA_LINES_HIGH				0x3
#define I2C_IBMR_SDA_STATUS_BIT                 (1 << I2C_IBMR_SDA_BIT)
#define I2C_IBMR_SCL_STATUS_BIT                 (1 << I2C_IBMR_SCL_BIT)

/***************** macros ***********************/

#define I2C_REG_WRITE(reg,wval) \
        ( (* ( (volatile UINT32*)(I2C_REGISTER_BASE_ADDR + (reg)) ) ) = wval);

#define I2C_REG_READ(reg,rval) \
        rval = (* ( (volatile UINT32*)(I2C_REGISTER_BASE_ADDR + (reg)) ) );

#define I2C_REG_BIT_WRITE(reg,btNm,sc) \
            {   UINT32 regVl; \
                I2C_REG_READ(reg,regVl); \
                sc ? ( regVl = (1 << btNm) | regVl ) : (regVl = ((regVl) & (~(1 << btNm)))); \
                I2C_REG_WRITE(reg,regVl); \
            }


#define I2C_STATUS_REG_CLEAR_BIT(bitNum)   			\
            {   UINT32 regVal; 						\
                regVal = (1 << bitNum); 			\
                I2C_REG_WRITE(I2C_ISR_REG,regVal); 	\
			}

#define I2C_REG_BIT_READ(regVl,btNm)  ( (regVl >> btNm) & 0x00000001 )
#define I2C_REG_BIT(btNm) (((UINT32)1)<<(btNm))
#define I2C_SLAVE_WRITE(slv)        ( (slv) | 0x00000000 )			/* Master is writing to the slave */
#define I2C_SLAVE_READ(slv)         ( (slv) | 0x00000001 )      	/* Master is reading from the slave */
#define TIME_TO_SAMPLE  	                    15  //MicroSec

/*********************** Sub-Package Parameters *************************/
typedef enum
{
    I2C_RC_OK,
    I2C_RC_NOT_OK,
    I2C_RC_INVALID_DATA_SIZE,
    I2C_RC_INVALID_DATA_PTR,
    I2C_RC_TOO_MANY_REGISTERS,
    I2C_RC_TIMEOUT_ERROR,                               // 5
    I2C_RC_CHIP_BUSY,                                   // 6
    I2C_RC_INVALID_GENERAL_CALL_SLAVE_ADDRESS,          // 7
	I2C_RC_UNREGISTER_ERR,                              // 8
	I2C_RC_MESSAGE_QUEUE_IS_FULL,                       // 9
    I2C_ISR_UNEXPECTED_INTERRUPT,                       // 0xA
	I2C_ISR_BUS_ERROR,                                  // 0xB
	I2C_ISR_BUS_BUSY,                                   // 0xC
	I2C_ISR_EARLY_BUS_BUSY,                             // 0xD
	I2C_ISR_CALL_BACK_FUNCTION_ERR,                     // 0xE
	I2C_ISR_ARBITRATION_LOSS,                           // 0xF
	I2C_RC_ILLEGAL_USE_OF_API
}I2C_ReturnCode;


typedef enum
{
    SLOW_MODE_ENABLE = 0x7fff,
    FAST_MODE_ENABLE = 0x8000,
    HIGH_MODE_ENABLE = 0x18000,
}I2C_FAST_MODE;

#define I2C_ICR_REG_INIT_VALUE      0x014E0   //all interrupts are disabled, except of the Bus Error and arbitration lost
#define LOOP_COUNTER_LIMIT              4000L



#define CHECK_IF_CHIP_BUSY(st) \
    {   UINT32  rv; \
        I2C_REG_READ(I2C_ISR_REG,rv); \
        st = I2C_REG_BIT_READ(rv,I2C_ISR_UNIT_BUSY_BIT) ? TRUE : FALSE; \
    }

/******  USTICA BASE REGISTER    *********/
#define USTICA_ID_REG 0x00
#define USTICA_STATUS_1_REG 0x01
#define USTICA_STATUS_2_REG 0x02
#define USTICA_INT_STATUS_1_REG 0x05
#define USTICA_INT_STATUS_2_REG 0x06
#define USTICA_INT_STATUS_3_REG 0x07
#define USTICA_INT_STATUS_4_REG 0x08
#define USTICA_INT_ENABLE_1_REG 0x09
#define USTICA_INT_ENABLE_2_REG 0x0a
#define USTICA_INT_ENABLE_3_REG 0x0b
#define USTICA_INT_ENABLE_4_REG 0x0c
#define USTICA_WAKEUP_1_REG 0x0d
#define USTICA_WAKEUP_2_REG 0x0e
#define USTICA_WATCHDOG_REG 0x1d
#define USTICA_OSC_CONTROL_1_REG 0x50
#define USTICA_OSC_CONTROL_2_REG 0x51
#define USTICA_OSC_CONTROL_3_REG 0x52
#define USTICA_OSC_CONTROL_4_REG 0x53
#define USTICA_OSC_CONTROL_5_REG 0x54
#define USTICA_OSC_CONTROL_6_REG 0x55
#define USTICA_OSC_CONTROL_7_REG 0x56
#define USTICA_OSC_CONTROL_8_REG 0x57
#define USTICA_OSC_CONTROL_9_REG 0x58
#define USTICA_OSC_CONTROL_10_REG 0x59
#define USTICA_OSC_CONTROL_11_REG 0x5a
#define USTICA_OSC_CONTROL_12_REG 0x5b
#define USTICA_OSC_CONTROL_13_REG 0x5c
#define USTICA_OSC_CONTROL_14_REG 0x5d
#define USTICA_RTC_CONTROL_1_REG 0xd0
#define USTICA_RTC_COUNT_1_REG 0xd1
#define USTICA_RTC_COUNT_2_REG 0xd2
#define USTICA_RTC_COUNT_3_REG 0xd3
#define USTICA_RTC_COUNT_4_REG 0xd4
#define USTICA_RTC_EXPIRE1_1_REG 0xd5
#define USTICA_RTC_EXPIRE1_2_REG 0xd6
#define USTICA_RTC_EXPIRE1_3_REG 0xd7
#define USTICA_RTC_EXPIRE1_4_REG 0xd8
#define USTICA_RTC_EXPIRE2_1_REG 0xdd
#define USTICA_RTC_EXPIRE2_2_REG 0xde
#define USTICA_RTC_EXPIRE2_3_REG 0xdf
#define USTICA_RTC_EXPIRE2_4_REG 0xe0
#define USTICA_RTC_MISC_1_REG 0xe1
#define USTICA_RTC_MISC_2_REG 0xe2
#define USTICA_RTC_MISC_3_REG 0xe3
#define USTICA_RTC_MISC_4_REG 0xe4
#define USTICA_RTC_MISC_5_REG 0xe7
#define USTICA_RTC_MISC_6_REG 0xe8
/******  USTICA POWER REGISTER    *********/

#define USTICA_BUCK_POWER_GOOD_STATUS_REG 0x01
#define USTICA_LDO_POWER_GOOD_STATUS_1_REG 0x02
#define USTICA_LDO_POWER_GOOD_STATUS_2_REG 0x03
#define USTICA_LDO_POWER_GOOD_STATUS_3_REG 0x04
#define USTICA_LDO_POWER_LAST_GROUP_REG 0x05    ????

#define USTICA_LDO_1_VOUT_1_REG 0x08
#define USTICA_LDO_1_VOUT_2_REG 0x09
#define USTICA_LDO_1_VOUT_3_REG 0x0a
#define USTICA_LDO_2_VOUT_REG 0x0b
#define USTICA_LDO_3_VOUT_REG 0x0c
#define USTICA_LDO_4_VOUT_REG 0x0d
#define USTICA_LDO_5_VOUT_REG 0x0e
#define USTICA_LDO_6_VOUT_REG 0x0f
#define USTICA_LDO_7_VOUT_REG 0x10
#define USTICA_LDO_8_VOUT_REG 0x11
#define USTICA_LDO_9_VOUT_REG 0x12
#define USTICA_LDO_10_VOUT_REG 0x13
#define USTICA_LDO_11_VOUT_REG 0x14
#define USTICA_LDO_12_VOUT_REG 0x15
#define USTICA_LDO_13_VOUT_REG 0x16
#define USTICA_LDO_14_VOUT_REG 0x17
#define USTICA_LDO_15_VOUT_REG 0x18
#define USTICA_LDO_16_VOUT_REG 0x19
#define USTICA_LDO_17_VOUT_REG 0x1a
#define USTICA_LDO_18_VOUT_REG 0x1b
#define USTICA_LDO_19_VOUT_REG 0x1c
#define USTICA_LE_LDO_3_VOUT_REG 0x16
#define USTICA_LE_LDO_2_VOUT_REG 0x0e

#define USTICA_VBUCK_1_SET_SLEEP_REG 0x30
#define USTICA_VBUCK_2_SET_SLEEP_REG 0x31
#define USTICA_VBUCK_3_SET_SLEEP_REG 0x32
#define USTICA_VBUCK_4_SET_SLEEP_REG 0x33
#define USTICA_VBUCK_5_SET_SLEEP_REG 0x34
#define USTICA_VAUDIO_MODE_CONFIG_REG 0x38
#define USTICA_VBUCK_1_SET_0_REG 0x3c
#define USTICA_VBUCK_1_SET_1_REG 0x3d
#define USTICA_VBUCK_1_SET_2_REG 0x3e
#define USTICA_VBUCK_1_SET_3_REG 0x3f
#define USTICA_VBUCK_2_SET_REG 0x40
#define USTICA_VBUCK_3_SET_REG 0x41
#define USTICA_VBUCK_4_SET_REG 0x42
#define USTICA_VBUCK_5_SET_0_REG 0x43
#define USTICA_VBUCK_5_SET_1_REG 0x44
#define USTICA_VBUCK_5_SET_2_REG 0x45
#define USTICA_VBUCK_5_SET_3_REG 0x46

#define USTICA_BUCK_ENABLE_REG 0x50
#define USTICA_LDO_ENABLE_1_REG 0x51
#define USTICA_LDO_ENABLE_2_REG 0x52
#define USTICA_LDO_ENABLE_3_REG 0x53
#define USTICA_BUCK_ENABLE_2_REG 0x55
#define USTICA_LDO_ENABLE_2_1_REG 0x56
#define USTICA_LDO_ENABLE_2_2_REG 0x57
#define USTICA_LDO_ENABLE_2_3_REG 0x58
#define USTICA_BUCK_SLEEP_MODE_1_REG 0x5a
#define USTICA_BUCK_SLEEP_MODE_2_REG 0x5b
#define USTICA_LDO_SLEEP_MODE_1_REG 0x5c
#define USTICA_LDO_SLEEP_MODE_2_REG 0x5d
#define USTICA_LDO_SLEEP_MODE_3_REG 0x5e
#define USTICA_LDO_SLEEP_MODE_4_REG 0x5f
#define USTICA_LDO_SLEEP_MODE_5_REG 0x60
#define USTICA_BUCK_GROUP_1_REG 0x64
#define USTICA_BUCK_GROUP_2_REG 0x65
#define USTICA_BUCK_GROUP_3_REG 0x66
#define USTICA_LDO_GROUP_1_REG 0x68
#define USTICA_LDO_GROUP_2_REG 0x69
#define USTICA_LDO_GROUP_3_REG 0x6a
#define USTICA_LDO_GROUP_4_REG 0x6b
#define USTICA_LDO_GROUP_5_REG 0x6c
#define USTICA_LDO_GROUP_6_REG 0x6d
#define USTICA_LDO_GROUP_7_REG 0x6e
#define USTICA_LDO_GROUP_8_REG 0x6f
#define USTICA_LDO_GROUP_9_REG 0x70
#define USTICA_LDO_GROUP_10_REG 0x71

#define USTICA_BUCK1_MISC_2_REG 0x79
#define USTICA_BUCK4_MISC_2_REG 0x82

#define USTICA_GPADC_MEAS_ENABLE_1_REG 0x01
#define USTICA_GPADC_MEAS_ENABLE_2_REG 0x02
#define USTICA_GPADC_MISC_CONGIG_2_REG 0x06
#define USTICA_GP_BIAS_ENABLE_1_REG 0x14

#define USTICA_GPADC_MEAS_VBAT_1_REG 0x42
#define USTICA_GPADC_MEAS_VBAT_2_REG 0x43
#define USTICA_GPADC_MEAS_GPADC0_1_REG 0x54
#define USTICA_GPADC_MEAS_GPADC0_2_REG 0x55
#define USTICA_GPADC_MEAS_GPADC2_1_REG 0x58
#define USTICA_GPADC_MEAS_GPADC2_2_REG 0x59

#define USTICA_BUCK5_MISC_1_REG 0x84
#define USTICA_BUCK5_MISC_2_REG 0x85
#define USTICA_BUCK5_MISC_3_REG 0x86
#define GPIO2_CONTROL_REG 0x32

#define PM801_LDO_3_VOUT_REG 0x16
#define PM801_LDO_6_VOUT_REG 0x1C
#define PM801_LDO_ENABLE_1_3_REG 0x53
#define PM801_LDO_ENABLE_2_2_REG 0x57
#define PM801_LDO_ENABLE_2_3_REG 0x58

#define PMC_I2C_WRITE_BASE_SLAVE_ADDRESS 			  0x60
#define PMC_I2C_READ_BASE_SLAVE_ADDRESS				  0x61
#define PMC_I2C_WRITE_POWER_SLAVE_ADDRESS 			  0x62
#define PMC_I2C_READ_POWER_SLAVE_ADDRESS			  0x63
#define PMC_I2C_WRITE_GPADC_SLAVE_ADDRESS 			  0x64
#define PMC_I2C_READ_GPADC_SLAVE_ADDRESS			  0x65
#define PMC_I2C_WRITE_TEST_SLAVE_ADDRESS 			  0x6E
#define PMC_I2C_READ_TEST_SLAVE_ADDRESS			      0x6F

#define PMC_I2C_WRITE_AUDIO_SLAVE_ADDRESS 			  0x70
#define PMC_I2C_READ_AUDIO_SLAVE_ADDRESS			  0x71


#define PM812_BASE_PAGE_WRITE_SLAVE_ADDR			0x60
#define PM812_BASE_PAGE_READ_SLAVE_ADDR				0x61
#define PM812_POWER_PAGE_WRITE_SLAVE_ADDR			0x62
#define PM812_POWER_PAGE_READ_SLAVE_ADDR			0x63
#define PM812_GPADC_PAGE_WRITE_SLAVE_ADDR			0x64
#define PM812_GPADC_PAGE_READ_SLAVE_ADDR			0x65

void ustica_I2CInit();
void ustica_I2CEnableclockandPin(void);
void ustica_I2CConfigureDi( void );
void USTICAI2CWriteDi_base(unsigned char I2CRegAddr, unsigned char I2CRegData);
unsigned char USTICAI2CReadDi_base(unsigned char I2CRegAddr);
void USTICAI2CWriteDi_power(unsigned char I2CRegAddr, unsigned char I2CRegData);
unsigned char USTICAI2CReadDi_power(unsigned char I2CRegAddr);
void USTICAI2CWriteDi_GPADC(unsigned char I2CRegAddr, unsigned char I2CRegData);
unsigned char USTICAI2CReadDi_GPADC(unsigned char I2CRegAddr);
void usticaPMICReadVBatTask(void *u);
void ustica_vbuck1_0_set(unsigned long value);
void ustica_USB_shutdown(void);
void usticaInit(void);
unsigned char GetPMICID(void);

//************ PM812 ******************
#define PM812_STATUS_1_REG	0x01
#define PM812_ONKEY_STATUS_BIT	(0x01<<0)
#define PM812_EXTON_STATUS_BIT	(0x01<<1)
#define PM812_VCHG_STATUS_BIT	(0x01<<2)
#define PM812_VBAT_STATUS_BIT	(0x01<<3)
#define PM812_VBUS_STATUS_BIT	(0x01<<4)
#define PM812_STATUS_2_REG	0x02
#define PM812_RTC_ALARM_STATUS_BIT	(0x01<<0)
#define PM812_INT_STATUS_1_REG	0x05
#define PM812_INT_STATUS_2_REG	0x06
#define PM812_INT_STATUS_3_REG	0x07
#define PM812_INT_STATUS_4_REG	0x08
#define PM812_INT_ENABLE_REG_1	0x09
#define PM812_INT_ENABLE_REG_2	0x0a
#define PM812_INT_ENABLE_REG_3	0x0b
#define PM812_INT_ENABLE_REG_4	0x0c
#define PM812_WAKEUP_1_REG	0x0d
#define PM812_RESET_PMIC_BIT (0x01<<6)
#define PM812_PWM_REG1          0x40
#define PM812_PWM_REG4          0x43
#define PM812_WD_MODE_BIT		(0x01<<0)
#define PM812_WD_RST_BIT		(0x01<<4)
#define PM812_SW_PDOWN_BIT	(0x01<<5)
#define PM812_WAKEUP_2_REG	0x0e

#define PM812_WD_TIMER_8s       (0x3<<3)
#define PM812_WD_TIMER_16s      (0x4<<3)
#define PM812_WD_TIMER_32s      (0x5<<3)
#define PM812_WD_TIMER_64s      (0x6<<3)
#define PM812_WD_TIMER_256s     (0x7<<3)

#define PM812_WD_TIMER_SLP_BITS	(0x1<<6|0x1<<7)
#define PM812_WD_TIMER_ACT_BITS	(0x1<<3|0x1<<4|0x1<<5)
#define PM812_WD_ENABLE_REG	0x1d
#define PM812_RTC_CONTROL_REG	0xd0
#define PM812_RTC_ALARM_WU_BIT	(0x01<<4)
#define PM812_RTC_ALARM_BIT	(0x01<<5)
#define PM812_RTC_USE_XO_BIT	(0x01<<7)
#define PM812_RTC_ALARM_SET1_BIT (0x01<<0)
#define PM812_RTC_ALARM_SET2_BIT (0x01<<1)
#define PM812_RTC_COUNT_1_REG	0xd1
#define PM812_RTC_COUNT_2_REG	0xd2
#define PM812_RTC_COUNT_3_REG	0xd3
#define PM812_RTC_COUNT_4_REG	0xd4
#define PM812_RTC_EXPIRE1_1_reg 0xd5
#define PM812_RTC_EXPIRE1_2_reg 0xd6
#define PM812_RTC_EXPIRE1_3_reg 0xd7
#define PM812_RTC_EXPIRE1_4_reg 0xd8
#define PM812_RTC_EXPIRE2_1_reg 0xdd
#define PM812_RTC_EXPIRE2_2_reg 0xde
#define PM812_RTC_EXPIRE2_3_reg 0xdf
#define PM812_RTC_EXPIRE2_4_reg 0xe0
#define PM812_RTC_MISC_3_REG 0xe3
#define PM812_RTC_MISC_4_REG 0xe4
#define PM812_LONGKEY_1	0
#define PM812_LONGKEY_2	1
#define PM812_RTC_LONGKEY1_EN_BIT (0x01<<PM812_LONGKEY_1)
#define PM812_RTC_LONGKEY2_EN_BIT (0x01<<PM812_LONGKEY_2)
#define PM812_RTC_MISC_5_REG 0xe7
#define PM812_RTC_FAULT_WU_EN_BIT (0x01<<2)
#define PM812_RTC_FAULT_WU_BIT (0x01<<3)
#define PM812_RTC_MISC_6_REG 0xe8


#define PM812_RTC_USER_DATA_1_REG 0xea
#define PM812_RTC_USER_DATA_2_REG 0xeb
#define PM812_RTC_USER_DATA_3_REG 0xec
#define PM812_RTC_USER_DATA_4_REG 0xed

//20190513@xiaokeweng : RTC_APP_OFFSET implement ++
#define PM812_RTC_SYS_OFFSET_REG PM812_RTC_USER_DATA_1_REG
#define PM812_RTC_APP_OFFSET_REG PM812_RTC_EXPIRE2_1_reg //reuse RTC_EXPIRE_2 as APP_OFFSET
//20190513@xiaokeweng : RTC_APP_OFFSET implement --

#define PM812_POWERUP_LOG_REG	0x10
#define PM812_POWERDOWN_LOG_REG	0xe5


#define PM812_LDO1_VOUT_REG (0x0a)
#define PM812_LDO2_VOUT_REG (0x0b)
//LDO3~LDO17 VOUT REG share the same VOL step
#define PM812_LDO3_VOUT_REG (0x0c)
#define PM812_LDO4_VOUT_REG (0x0d)
#define PM812_LDO5_VOUT_REG (0x0e)
#define PM812_LDO6_VOUT_REG (0x0f)
#define PM812_LDO7_VOUT_REG (0x10)
#define PM812_LDO8_VOUT_REG (0x11)
#define PM812_LDO9_VOUT_REG (0x12)
#define PM812_LDO10_VOUT_REG (0x13)
#define PM812_LDO11_VOUT_REG (0x14)
#define PM812_LDO12_VOUT_REG (0x15)
#define PM812_LDO13_VOUT_REG (0x16)
#define PM812_LDO14_VOUT_REG (0x17)
#define PM812_LDO15_VOUT_REG (0x18)
#define PM812_LDO16_VOUT_REG (0x19)
#define PM812_LDO17_VOUT_REG (0x1a)
//LDO18~LDO19 VOUT REG share the same VOL step
#define PM812_LDO18_VOUT_REG (0x1b)
#define PM812_LDO19_VOUT_REG (0x1c)

//LDO1 VOUT
#define PM812_LDO1_0V60_ACT (0x0)
#define PM812_LDO1_0V65_ACT (0x1)
#define PM812_LDO1_0V70_ACT (0x2)
#define PM812_LDO1_0V75_ACT (0x3)
#define PM812_LDO1_0V80_ACT (0x4)
#define PM812_LDO1_0V85_ACT (0x5)
#define PM812_LDO1_0V90_ACT (0x6)
#define PM812_LDO1_0V95_ACT (0x7)
#define PM812_LDO1_1V00_ACT (0x8)
#define PM812_LDO1_1V05_ACT (0x9)
#define PM812_LDO1_1V10_ACT (0xa)
#define PM812_LDO1_1V15_ACT (0xb)
#define PM812_LDO1_1V20_ACT (0xc)
#define PM812_LDO1_1V30_ACT (0xd)
#define PM812_LDO1_1V40_ACT (0xe)
#define PM812_LDO1_1V50_ACT (0xf)

#define PM812_LDO1_0V60_SLP (0x0 << 4)
#define PM812_LDO1_0V65_SLP (0x1 << 4)
#define PM812_LDO1_0V70_SLP (0x2 << 4)
#define PM812_LDO1_0V75_SLP (0x3 << 4)
#define PM812_LDO1_0V80_SLP (0x4 << 4)
#define PM812_LDO1_0V85_SLP (0x5 << 4)
#define PM812_LDO1_0V90_SLP (0x6 << 4)
#define PM812_LDO1_0V95_SLP (0x7 << 4)
#define PM812_LDO1_1V00_SLP (0x8 << 4)
#define PM812_LDO1_1V05_SLP (0x9 << 4)
#define PM812_LDO1_1V10_SLP (0xa << 4)
#define PM812_LDO1_1V15_SLP (0xb << 4)
#define PM812_LDO1_1V20_SLP (0xc << 4)
#define PM812_LDO1_1V30_SLP (0xd << 4)
#define PM812_LDO1_1V40_SLP (0xe << 4)
#define PM812_LDO1_1V50_SLP (0xf << 4)

//LDO2 VOUT
#define PM812_LDO2_1V70_ACT (0x0)
#define PM812_LDO2_1V80_ACT (0x1)
#define PM812_LDO2_1V90_ACT (0x2)
#define PM812_LDO2_2V00_ACT (0x3)
#define PM812_LDO2_2V10_ACT (0x4)
#define PM812_LDO2_2V50_ACT (0x5)
#define PM812_LDO2_2V70_ACT (0x6)
#define PM812_LDO2_2V80_ACT (0x7)

#define PM812_LDO2_1V70_SLP (0x0 << 4)
#define PM812_LDO2_1V80_SLP (0x1 << 4)
#define PM812_LDO2_1V90_SLP (0x2 << 4)
#define PM812_LDO2_2V00_SLP (0x3 << 4)
#define PM812_LDO2_2V10_SLP (0x4 << 4)
#define PM812_LDO2_2V50_SLP (0x5 << 4)
#define PM812_LDO2_2V70_SLP (0x6 << 4)
#define PM812_LDO2_2V80_SLP (0x7 << 4)

//LDO3~LDO17
#define PM812_LDO3_TO_LDO17_1V20_ACT (0x0)
#define PM812_LDO3_TO_LDO17_1V25_ACT (0x1)
#define PM812_LDO3_TO_LDO17_1V70_ACT (0x2)
#define PM812_LDO3_TO_LDO17_1V80_ACT (0x3)
#define PM812_LDO3_TO_LDO17_1V85_ACT (0x4)
#define PM812_LDO3_TO_LDO17_1V90_ACT (0x5)
#define PM812_LDO3_TO_LDO17_2V50_ACT (0x6)
#define PM812_LDO3_TO_LDO17_2V60_ACT (0x7)
#define PM812_LDO3_TO_LDO17_2V70_ACT (0x8)
#define PM812_LDO3_TO_LDO17_2V75_ACT (0x9)
#define PM812_LDO3_TO_LDO17_2V80_ACT (0xa)
#define PM812_LDO3_TO_LDO17_2V85_ACT (0xb)
#define PM812_LDO3_TO_LDO17_2V90_ACT (0xc)
#define PM812_LDO3_TO_LDO17_3V00_ACT (0xd)
#define PM812_LDO3_TO_LDO17_3V10_ACT (0xe)
#define PM812_LDO3_TO_LDO17_3V30_ACT (0xf)

#define PM812_LDO3_TO_LDO17_1V20_SLP (0x0 << 4)
#define PM812_LDO3_TO_LDO17_1V25_SLP (0x1 << 4)
#define PM812_LDO3_TO_LDO17_1V70_SLP (0x2 << 4)
#define PM812_LDO3_TO_LDO17_1V80_SLP (0x3 << 4)
#define PM812_LDO3_TO_LDO17_1V85_SLP (0x4 << 4)
#define PM812_LDO3_TO_LDO17_1V90_SLP (0x5 << 4)
#define PM812_LDO3_TO_LDO17_2V50_SLP (0x6 << 4)
#define PM812_LDO3_TO_LDO17_2V60_SLP (0x7 << 4)
#define PM812_LDO3_TO_LDO17_2V70_SLP (0x8 << 4)
#define PM812_LDO3_TO_LDO17_2V75_SLP (0x9 << 4)
#define PM812_LDO3_TO_LDO17_2V80_SLP (0xa << 4)
#define PM812_LDO3_TO_LDO17_2V85_SLP (0xb << 4)
#define PM812_LDO3_TO_LDO17_2V90_SLP (0xc << 4)
#define PM812_LDO3_TO_LDO17_3V00_SLP (0xd << 4)
#define PM812_LDO3_TO_LDO17_3V10_SLP (0xe << 4)
#define PM812_LDO3_TO_LDO17_3V30_SLP (0xf << 4)

//LDO18~LDO19 VOUT
#define PM812_LDO18_TO_LDO19_1V70_ACT (0x0)
#define PM812_LDO18_TO_LDO19_1V80_ACT (0x1)
#define PM812_LDO18_TO_LDO19_1V90_ACT (0x2)
#define PM812_LDO18_TO_LDO19_2V50_ACT (0x3)
#define PM812_LDO18_TO_LDO19_2V80_ACT (0x4)
#define PM812_LDO18_TO_LDO19_2V90_ACT (0x5)
#define PM812_LDO18_TO_LDO19_3V10_ACT (0x6)
#define PM812_LDO18_TO_LDO19_3V30_ACT (0x7)

#define PM812_LDO18_TO_LDO19_1V70_SLP (0x0 << 4)
#define PM812_LDO18_TO_LDO19_1V80_SLP (0x1 << 4)
#define PM812_LDO18_TO_LDO19_1V90_SLP (0x2 << 4)
#define PM812_LDO18_TO_LDO19_2V50_SLP (0x3 << 4)
#define PM812_LDO18_TO_LDO19_2V80_SLP (0x4 << 4)
#define PM812_LDO18_TO_LDO19_2V90_SLP (0x5 << 4)
#define PM812_LDO18_TO_LDO19_3V10_SLP (0x6 << 4)
#define PM812_LDO18_TO_LDO19_3V30_SLP (0x7 << 4)

#define PM812_LDO1_TO_LDO8_ENABLE_REG_1   (0x51)
#define PM812_LDO1_TO_LDO8_ENABLE_REG_2   (0x56)
#define PM812_LDO9_TO_LDO16_ENABLE_REG_1  (0x52)
#define PM812_LDO9_TO_LDO16_ENABLE_REG_2  (0x57)
#define PM812_LDO17_TO_LDO19_ENABLE_REG_1 (0x53)
#define PM812_LDO17_TO_LDO19_ENABLE_REG_2 (0x58)

#define PM812_LDO_ENABLE_REG_1(LDO_X) ((LDO_X >= 17)?PM812_LDO17_TO_LDO19_ENABLE_REG_1:((LDO_X >= 9)?PM812_LDO9_TO_LDO16_ENABLE_REG_1:PM812_LDO1_TO_LDO8_ENABLE_REG_1))
#define PM812_LDO_ENABLE_REG_2(LDO_X) ((LDO_X >= 17)?PM812_LDO17_TO_LDO19_ENABLE_REG_2:((LDO_X >= 9)?PM812_LDO9_TO_LDO16_ENABLE_REG_2:PM812_LDO1_TO_LDO8_ENABLE_REG_2))
#define PM812_LDO_ENABLE_BIT(LDO_X)   (0x1 << ((LDO_X - 1)%8))   

//=============GPADC PAGE
#define PM812_GPADC_MEAS_ENABLE_1_REG 0x01
#define PM812_GPADC_MEAS_ENABLE_2_REG 0x02
#define PM812_GPADC_MISC_CONGIG_2_REG 0x06
#define PM812_GPADC_TIME_2_REG 0x08
#define PM812_GPADC_SW_TRIG_BIT       (0x01 << 2)
#define PM812_GPADC_NON_STOP          (0x01 << 1)            /* Enable NON_STOP GPADC operation mode.         */
#define PM812_GPADC_GPADC_EN          (0x01 << 0)            /* Enable the GPADC periodic modes.              */
#define PM812_GP_BIAS_ENABLE_1_REG 0x14
#define PM812_VCHG_LOW_TH (0x1a)
#define PM812_VCHG_HIGH_TH (0x2a)

#define PM812_GPADC_MEAS_VBAT_1_REG 0x42
#define PM812_GPADC_MEAS_VBAT_2_REG 0x43
#define PM812_GPADC_MEAS_VCHG_1_REG 0x46
#define PM812_GPADC_MEAS_VCHG_2_REG 0x47
#define PM812_GPADC_MEAS_GPADC1_1_REG 0x56
#define PM812_GPADC_MEAS_GPADC1_2_REG 0x57
#define PM812_GPADC_BAT_BD_GP3_EN_BIT (0x01<<7)
#define PM812_GPADC_BAT_BD_GP1_EN_BIT (0x01<<6)
#define PM812_GPADC_BAT_BD_EN_BIT (0x01<<5)

#define DIVIDE_5V6_12BIT_MV(meas) (meas*5600/4095)
#define DIVIDE_1V4_12BIT_MV(meas) (meas*1400/4095)

#define DIVIDE_5V6_MV_12BIT(vol) (vol*4095/5600)
#define DIVIDE_1V4_MV_12BIT(vol) (vol*4095/1400)


void PM812_bk4clkfll_set_2m(void);
void PM812_vbuck3_set_1_25(void);
void PM812_vbuck4_set_1_85(void);
void PM812_vbuck4_set_2_00(void);
void PM812_APT_enable(void);
void PM812_BUCK5_Enable(void);
static void PM812_LDO_Enable(int ldo_num,BOOL OnOff);
static void PM812_LDO_VOUT_CFG(int addr,int act_vol,int slp_vol);
void PM812_Ldo_4_set_1_8(void);
void PM812_Ldo_4_set_3_0(void);
void PM812_Ldo_4_set(BOOL OnOff);
void PM812_Ldo_10_set_1_8(void);
void PM812_Ldo_10_set_3_0(void);
void PM812_Ldo_10_set(BOOL OnOff);
void PM812_Ldo_12_set_1_8(void);
void PM812_Ldo_12_set_2_8(void);
void PM812_Ldo_12_set_3_0(void);
void PM812_Ldo_12_set(BOOL OnOff);
unsigned int PM812_REG_DUMP(unsigned char page, unsigned char addr);//page must be PM812_BASE_PAGE_READ_SLAVE_ADDR \ PM812_POWER_PAGE_READ_SLAVE_ADDR\ PM812_GPADC_PAGE_READ_SLAVE_ADDR
void PM812_Ldo_5_set_3_1(void);
void PM812_Ldo_5_set(BOOL OnOff);
void PM812_Ldo_6_set_1_8(void);
void PM812_Ldo_6_set_2_8(void);
void PM812_Ldo_6_set(BOOL OnOff);
void PM812_Ldo_8_set_2_8(void);
void PM812_Ldo_8_set(BOOL OnOff);
void PM812_Ldo_9_set_1_8(void);
void PM812_Ldo_9_set(BOOL OnOff);
void PM812_Ldo_11_set_2_8(void);
void PM812_Ldo_11_set(BOOL OnOff);
void PM812_Ldo_13_set_2_8(void);
void PM812_Ldo_13_set(BOOL OnOff);
void PM812_Ldo_15_set_2_8(void);
void PM812_Ldo_15_set(BOOL OnOff);
void PM812_Ldo_16_set_2_8(void);
void PM812_Ldo_16_set(BOOL OnOff);
void PM812_Ldo_17_set_2_8(void);
void PM812_Ldo_17_set(BOOL OnOff);
void PM812_Ldo_18_set_1_8(void);
void PM812_Ldo_18_set(BOOL OnOff);
void PM812SetLDO3OffOnSleep(void);
void PM812_32K_OUT2_Enable(void);

typedef union CNT_32{
    UINT8   cnt_8[4];
    UINT32  cnt_32;
} COUNTER_32;

typedef void (*PmicCallback)(void);

typedef struct {
	BOOL		pmic_intc_enabled;
	PmicCallback	pmic_isr;
}Pmic_Int_Handler;

typedef enum {
	//INT ENABLE REG 4 addr=0x0c
	PM812_GPIO0_INT=0,
	PM812_GPIO1_INT,
	PM812_GPIO2_INT,
	PM812_GPIO3_INT,
	PM812_GPIO4_INT,
	//INT ENABLE REG 3 addr=0x0b
	PM812_GPADC0_INT=8,
	PM812_GPADC1_INT,
	PM812_GPADC2_INT,
	PM812_GPADC3_INT,
	PM812_GPADC4_INT,
	//INT ENABLE REG 2 addr=0x0a
	PM812_VBAT_INT=16,
	PM812_VSYS_INT,
	PM812_VCHG_INT,
	PM812_TINT_INT,
	//INT ENABLE REG 1 addr=0x09
	PM812_ONKEY_INT	=24,
	PM812_EXTON_INT,
	PM812_CHG_INT,
	PM812_BAT_INT,
	PM812_RTC_INT,
	PM812_CLASSD_OC_INT
} PM812_INTC ;

#define PM812_INTC_MAX 31
#define PM812_INTC_TO_STATUS_BIT(intc) (0x01<<intc)
#define PM812_INTC_TO_ENABLE_BIT(intc) (1<<(intc%8))
#define PM812_INTC_TO_ENABLE_REG(intc) (PM812_INT_ENABLE_REG_4 - (intc/8))

typedef enum {
	//gpadc meas en reg 2 ,addr=0x02
	PM812_INT_TEMP_MEAS_EN,
	PM812_PMODE_MEAS_EN,
	PM812_GPADC0_MEAS_EN,
	PM812_GPADC1_MEAS_EN,
	PM812_GPADC2_MEAS_EN,
	PM812_GPADC3_MEAS_EN,
	PM812_GPADC4_MEAS_EN,
	//gpadc meas en reg 1 ,addr=0x01
	PM812_VBBAT_MEAS_EN=8,
	PM812_VBAT_MEAS_EN,
	PM812_VSYS_MEAS_EN,
	PM812_VCHG_MEAS_EN,
}PM812_MEAS_EN ;

typedef enum {
	//gpadc bias enable reg 1 ,addr=0x14
	PM812_GPADC0_BIAS_EN,
	PM812_GPADC1_BIAS_EN,
	PM812_GPADC2_BIAS_EN,
	PM812_GPADC3_BIAS_EN,
}PM812_BIAS_EN ;

typedef enum {
	PM812_GPADC0_BIAS_OUT=4,
	PM812_GPADC1_BIAS_OUT,
	PM812_GPADC2_BIAS_OUT,
	PM812_GPADC3_BIAS_OUT,
}PM812_BIAS_OUT ;

//ICAT EXPORTED ENUM
typedef enum
{
	BUCK_0_700 = 0x08, /* 0.700V */
	BUCK_0_750 = 0x0C,
	BUCK_0_800 = 0x10,
	BUCK_0_850 = 0x14,
	BUCK_0_900 = 0x18,
	BUCK_0_950 = 0x1C,
	BUCK_1_000 = 0x20,
	BUCK_1_025 = 0x22,
	BUCK_1_050 = 0x24,
    BUCK_1_075 = 0x26,
	BUCK_1_100 = 0x28,
	BUCK_1_150 = 0x2C,
	BUCK_1_200 = 0x30,
	BUCK_1_250 = 0x34,
	BUCK_1_300 = 0x38,
	BUCK_1_350 = 0x3C,	

	BUCK_1_600 = 0x50,
	BUCK_1_650 = 0x51,
	BUCK_1_700 = 0x52,
	BUCK_1_750 = 0x53,
	BUCK_1_800 = 0x54,
	BUCK_1_850 = 0x55,
	BUCK_1_900 = 0x56,
	BUCK_1_950 = 0x57,	
    BUCK_2_000 = 0x58,  
	
	BUCK_DEFAULT = 0xFF,
} PMIC_BUCK_T;


void PMIC_WRITE_REG_GPADC(UINT8 addr, UINT8 value);
UINT8 PMIC_READ_REG_GPADC(UINT8 addr);

void PMIC_WRITE_REG_POWER(UINT8 addr, UINT8 value);
UINT8 PMIC_READ_REG_POWER(UINT8 addr);

void PMIC_WRITE_REG_BASE(UINT8 addr, UINT8 value);
UINT8 PMIC_READ_REG_BASE(UINT8 addr);

void PMIC_WRITE_REG_AUDIO(UINT8 addr, UINT8 value);
UINT8 PMIC_READ_REG_AUDIO(UINT8 addr);

BOOL PMIC_IS_PM812(void);
BOOL PMIC_IS_PM813(void);
BOOL PMIC_IS_PM802(void);
BOOL PMIC_IS_PM813S(void);

#endif /* _PIMC_H_ */
