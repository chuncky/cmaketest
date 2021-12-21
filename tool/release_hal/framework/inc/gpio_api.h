#ifndef __GPIO_API_H__
#define __GPIO_API_H__
typedef void (*func)(void);

#define HAL_GPIO_NONE 0xFFFFFFFF
#define HAL_GPIO_TYPE_NONE HAL_GPIO_NONE
#define HAL_GPIO_TYPE_INVALID 0xFFFFFFFF

typedef enum
{
    HAL_GPIO_0,
    HAL_GPIO_1,
    HAL_GPIO_2,
    HAL_GPIO_3,
    HAL_GPIO_4,
    HAL_GPIO_5,
    HAL_GPIO_6,
    HAL_GPIO_7,
    HAL_GPIO_8,
    HAL_GPIO_9,
    HAL_GPIO_10,
    HAL_GPIO_11,
    HAL_GPIO_12,
    HAL_GPIO_13,
    HAL_GPIO_14,
    HAL_GPIO_15,
    HAL_GPIO_16,
    HAL_GPIO_17,
    HAL_GPIO_18,
    HAL_GPIO_19,
    HAL_GPIO_20,
    HAL_GPIO_21,
    HAL_GPIO_22,
    HAL_GPIO_23,
    HAL_GPIO_24,
    HAL_GPIO_25,
    HAL_GPIO_26,
    HAL_GPIO_27,
    HAL_GPIO_28,
    HAL_GPIO_29,
    HAL_GPIO_30,
    HAL_GPIO_31,
    HAL_GPIO_32,
    HAL_GPIO_33,
    HAL_GPIO_34,
    HAL_GPIO_35,
    HAL_GPIO_36,
    HAL_GPIO_37,
    HAL_GPIO_38,
    HAL_GPIO_39,
    HAL_GPIO_40,
    HAL_GPIO_41,
    HAL_GPIO_42,
    HAL_GPIO_43,
    HAL_GPIO_44,
    HAL_GPIO_45,
    HAL_GPIO_46,
    HAL_GPIO_47,
    HAL_GPIO_48,
    HAL_GPIO_49,
    HAL_GPIO_50,
    HAL_GPIO_51,
    HAL_GPIO_52,
    HAL_GPIO_53,
    HAL_GPIO_54,
    HAL_GPIO_55,
    HAL_GPIO_56,
    HAL_GPIO_57,
    HAL_GPIO_58,
    HAL_GPIO_59,
    HAL_GPIO_60,
    HAL_GPIO_61,
    HAL_GPIO_62,
    HAL_GPIO_63,
    HAL_GPIO_64,
    HAL_GPIO_65,
    HAL_GPIO_66,
    HAL_GPIO_67,
    HAL_GPIO_68,
    HAL_GPIO_69,
    HAL_GPIO_70,
    HAL_GPIO_71,
    HAL_GPIO_72,
    HAL_GPIO_73,
    HAL_GPIO_74,
    HAL_GPIO_75,
    HAL_GPIO_76,
    HAL_GPIO_77,
    HAL_GPIO_78,
    HAL_GPIO_79,
    HAL_GPIO_80,
    HAL_GPIO_81,
    HAL_GPIO_82,
    HAL_GPIO_83,
    HAL_GPIO_84,
    HAL_GPIO_85,
    HAL_GPIO_86,
    HAL_GPIO_87,
    HAL_GPIO_88,
    HAL_GPIO_89,
    HAL_GPIO_90,
    HAL_GPIO_91,
    HAL_GPIO_92,
    HAL_GPIO_93,
    HAL_GPIO_94,
    HAL_GPIO_95,
    HAL_GPIO_96,
    HAL_GPIO_97,
    HAL_GPIO_98,
    HAL_GPIO_99,
    HAL_GPIO_100,
    HAL_GPIO_101,
    HAL_GPIO_102,
    HAL_GPIO_103,
    HAL_GPIO_104,
    HAL_GPIO_105,
    HAL_GPIO_106,
    HAL_GPIO_107,
    HAL_GPIO_108,
    HAL_GPIO_109,
    HAL_GPIO_110,
    HAL_GPIO_111,
    HAL_GPIO_112,
    HAL_GPIO_113,
    HAL_GPIO_114,
    HAL_GPIO_115,
    HAL_GPIO_116,
    HAL_GPIO_117,
    HAL_GPIO_118,
    HAL_GPIO_119,
    HAL_GPIO_120,
    HAL_GPIO_121,
    HAL_GPIO_122,
    HAL_GPIO_123,
    HAL_GPIO_124,
    HAL_GPIO_125,
    HAL_GPIO_126,
    HAL_GPIO_127,
    HAL_GPIO_QTY = 128

} HAL_GPIO_GPIO_ID_T;


typedef enum
{
	GPIORC_FALSE = 0,
	GPIORC_TRUE = 1,
	GPIORC_LOW = 0,
	GPIORC_HIGH = 1,

	GPIORC_OK = 0,
    GPIORC_INVALID_PORT_HANDLE = -100,
    GPIORC_NOT_OUTPUT_PORT,
    GPIORC_NO_TIMER,
    GPIORC_NO_FREE_HANDLE,
    GPIORC_AMOUNT_OUT_OF_RANGE,
    GPIORC_INCORRECT_PORT_SIZE,
    GPIORC_PORT_NOT_ON_ONE_REG,
    GPIORC_INVALID_PIN_NUM,
    GPIORC_PIN_USED_IN_PORT,
    GPIORC_PIN_NOT_FREE,
    GPIORC_PIN_NOT_LOCKED,
    GPIORC_NULL_POINTER,
    GPIORC_PULLED_AND_OUTPUT,
	GPIORC_INCORRECT_PORT_TYPE,
	GPIORC_INCORRECT_TRANSITION_TYPE,
	GPIORC_INCORRECT_DEBOUNCE,
	GPIORC_INCORRECT_DIRECTION,
	GPIORC_INCORRECT_PULL,	
	GPIORC_INCORRECT_INIT_VALUE,
	GPIORC_WRITE_TO_INPUT
}GPIOReturnCode;

typedef enum
{
    GPIO_IN_PIN = 0,
    GPIO_OUT_PIN = 1
}GPIOPinDirection;

typedef enum
{
    GPIO_PULL_DISABLE = 0,
    GPIO_PULLUP_ENABLE,
    GPIO_PULLDN_ENABLE
}GPIOPullUpDown;

typedef enum
{
    GPIO_NO_EDGE = 0,
    GPIO_RISE_EDGE,
    GPIO_FALL_EDGE,
    GPIO_TWO_EDGE,
}GPIOTransitionType;

void gpio_direction_output(unsigned int gpio);
void gpio_direction_input(unsigned int gpio);
void gpio_set_value(unsigned int gpio,unsigned int value);
unsigned int gpio_get_value(unsigned int gpio);
void gpio_set_irq_handler(void);
void  gpio_irq_config(unsigned int  gpio,GPIOTransitionType  type,func handler);
void gpio_irq_handler(unsigned int interrupt);
void gpio_hal_config(unsigned int *gpio_cfg);
void GpioCameraFlashPowerOnOff(unsigned char on_off);
void gpio_enable_edge_detection(unsigned int  gpio, GPIOTransitionType  type);
void gpio_disable_edge_detection(unsigned int  gpio, GPIOTransitionType  type);

#endif
