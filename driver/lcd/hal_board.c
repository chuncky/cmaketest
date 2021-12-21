#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bsp_common.h"
#include "hal_drv_conf.h"
#include "hal_board.h"
#include "hal_disp.h"


/* I2C MFP */
uint32_t i2c_pinmux_cfgs[4][4] = {
    {   /* CI2C 0 */
        MFP_REG(GPIO_49) | MFP_DRIVE_MEDIUM | MFP_PULL_HIGH | MFP_AF1,
        MFP_REG(GPIO_50) | MFP_DRIVE_MEDIUM | MFP_PULL_HIGH | MFP_AF1,
        MFP_EOC
    },
    {   /* CI2C 1 */
        MFP_REG(GPIO_10) | MFP_DRIVE_MEDIUM | MFP_PULL_HIGH | MFP_AF1,
        MFP_REG(GPIO_11) | MFP_DRIVE_MEDIUM | MFP_PULL_HIGH | MFP_AF1,
        MFP_EOC
    },
    {   /* CI2C 2 */
        MFP_REG(GPIO_72) | MFP_DRIVE_MEDIUM | MFP_PULL_HIGH | MFP_AF2,
        MFP_REG(GPIO_73) | MFP_DRIVE_MEDIUM | MFP_PULL_HIGH | MFP_AF2,
        MFP_EOC
    },
    {   /* CI2C 3 */
        MFP_REG(GPIO_USIM2_UCLK) | MFP_DRIVE_MEDIUM | MFP_PULL_HIGH | MFP_AF6,
        MFP_REG(GPIO_USIM2_UIO) | MFP_DRIVE_MEDIUM | MFP_PULL_HIGH | MFP_AF6,
        MFP_EOC
    },
};

void pmic_vibrator_on(void)
{
    NingboVibratorEnable();
}

void pmic_vibrator_off(void)
{
    NingboVibratorDisable();
}

uint32_t tick_to_ms(uint32_t tick)
{
    uint32_t delta = lv_get_ticks_per_second() / 1000;
    return tick / delta;
}


__attribute__((__used__)) static const uint32_t pm812_bg_mfp_cfgs[] = {
    MFP_REG(GPIO_77) | MFP_AF0 | MFP_DRIVE_MEDIUM | MFP_PULL_LOW | MFP_LPM_EDGE_NONE,
    MFP_EOC /*End of configuration, must have */
};

__attribute__((__used__)) static const uint32_t pm802_bg_mfp_cfgs[] = {
    MFP_REG(GPIO_84) | MFP_AF0 | MFP_DRIVE_MEDIUM | MFP_PULL_LOW | MFP_LPM_EDGE_NONE,
    MFP_EOC /*End of configuration, must have */
};

#if USE_PWM
/* CraneG evb backlight config */
#define LEDK_PWM                          GPIO_31
#define LEDK_PWM_GPIO                     GPIO31
#define PWM_DEV_BACKLIGHT                 PWM_DEV_0
#define PERIOD_NS                         33333 /* 30KHz */
#define LEVEL_MAX                         5

__attribute__((__used__)) static const uint32_t pwm_bg_mfp_cfgs[] = {
    MFP_REG(LEDK_PWM) | MFP_AF2 | MFP_DRIVE_MEDIUM | MFP_PULL_HIGH,
    MFP_EOC /*End of configuration, must have */
};

__attribute__((__used__)) static const uint32_t pwm_bgoff_mfp_cfgs[] = {
    MFP_REG(LEDK_PWM) | MFP_AF0 | MFP_DRIVE_MEDIUM | MFP_PULL_LOW,
    MFP_EOC /*End of configuration, must have */
};
#endif

int pmic_get_type(void)
{
    int type = PM_813;

    if(PMIC_IS_PM812()) {
        type = PM_812;
    } else if(PMIC_IS_PM813()) {
        type = PM_813;
    } else if(PMIC_IS_PM802()) {
        type = PM_802;
    } else if(PMIC_IS_PM803()) {
        type = PM_803;
    }
    return type;
}

void hal_pmic_init(void)
{
    int pmic_type = pmic_get_type();
    if(pmic_type == PM_812) {
        printf("PMIC: 812~\n");
    } else if(pmic_type == PM_813) {
        printf("PMIC: 813~\n");
    } else if(pmic_type == PM_802) {
        printf("PMIC: 802~\n");
    } else if(pmic_type == PM_803) {
        printf("PMIC: 803~\n");
    } else {
        printf("PMIC: unknown!! \n");
    }
}

static void backlight_pwm(unsigned level)
{
#if USE_PWM
    int duty_ns, period_ns;

    if (level > 5) {
        level = 5;
    }

    if (level == 0) {
        gpio_set_direction(LEDK_PWM_GPIO, GPIO_OUT);
        gpio_output_set(LEDK_PWM_GPIO, 0);
        hal_mfp_config((unsigned int *)pwm_bgoff_mfp_cfgs);

        hal_pwm_disable(PWM_DEV_BACKLIGHT);
    } else {
        period_ns = PERIOD_NS;
        duty_ns = period_ns / (LEVEL_MAX - level + 1);
        hal_pwm_enable(PWM_DEV_BACKLIGHT, duty_ns, period_ns);

        hal_mfp_config((unsigned int *)pwm_bg_mfp_cfgs);
    }
#endif
}

extern void GuilinLcdBackLightStatusRecord(UINT8 status);
extern void GuilinLcdBackLightLevelRecord(UINT8 level);
void lcd_backlight_ctrl(int level)
{
#if USE_LCD_PANEL_ICNA3310_MIPI
    extern void isca3310_set_display_brightness(int level);
    isca3310_set_display_brightness(level);
    return;
#endif

#if USE_LCD_PANEL_ILI9806E_MIPI
    backlight_pwm(level);
    return;
#endif

    int pm_type = pmic_get_type();

    if (pm_type == PM_812) { // PM812
        hal_mfp_config((unsigned int *)pm812_bg_mfp_cfgs);
        gpio_set_direction(GPIO77, GPIO_OUT);
        if (level > 0) {
            gpio_output_set(GPIO77, 1);
        }
        else {
            gpio_output_set(GPIO77, 0);
        }
    }
    else if (pm_type == PM_813) {  // PM813
        extern void NingboLcdBackLightCtrl(UINT8 level); //should be 0~5;
        NingboLcdBackLightCtrl(level);
    }
    else if (pm_type == PM_802) {  // PM802
        hal_mfp_config((unsigned int *)pm802_bg_mfp_cfgs);
        gpio_set_direction(GPIO84, GPIO_OUT);
        if (level > 0) {
            gpio_output_set(GPIO84, 1);
        }
        else {
            gpio_output_set(GPIO84, 0);
        }
        GuilinLcdBackLightStatusRecord(level>0?1:0);
    }
    else if (pm_type == PM_803) {  // PM803
        backlight_pwm(level);
        GuilinLcdBackLightStatusRecord(level>0?1:0);
        GuilinLcdBackLightLevelRecord(level);
    }
    else {
        printf("unknown PMIC, lcd_backlight_ctrl failed.\n");
    }
}

#define PMU_SD_ROT_WAKE_CLR 0xD428287C
#define VBUS_DETECT (1 << 15)
bool usb_is_connected(void)
{
    unsigned val = readl(PMU_SD_ROT_WAKE_CLR);
    return ((val & VBUS_DETECT) != 0);
}

int is_chip_3603(void)
{
    uint32_t chip_id = readl(0xD4282C00) & 0xFFFF;
    return ((chip_id == 0x3603) ? 1 : 0);
}

static lv_pm_info_t hal_pm_info;



int lv_pm_register(lv_pm_info_t *info)
{
    lv_pm_info_t *t;

    t = &hal_pm_info;
    if (!t) {
        printf("%s failed: no mem\n", __FUNCTION__);
        return -1;
    }

    memcpy(t, info, sizeof(lv_pm_info_t));
    return 0;
}


/**
 * Mark an area of an object as invalid.
 * This area will be redrawn by 'lv_refr_task'
 * @param obj pointer to an object
 * @param area the area to redraw
 */
void lv_obj_invalidate_area(const lv_obj_t * obj, const lv_area_t * area)
{

}

/**
 * Mark the object as invalid therefore its current position will be redrawn by 'lv_refr_task'
 * @param obj pointer to an object
 */
void lv_obj_invalidate(const lv_obj_t * obj)
{

}


#define CRANE_MFPR_BASE    0xD401E000
#define MFP_CONFIG_RETRY   3

#define HAL_MFP(_off, _pull, _drv, _slp, _edge, _sleep, _afn) (((_off))<<16)


void hal_mfp_config(const uint32_t *mfp_cfgs)
{
    uint32_t *p_mfpr = NULL;
	uint32_t mfpr_offset = 0;
    uint32_t cfg_val, val, val_r;
    uint32_t i = 0;

    do {
        cfg_val = *mfp_cfgs++;
        /* exit if End of configuration table detected */
        if (cfg_val == MFP_EOC) {
            break;
        }
		mfpr_offset=cfg_val>>16;


        p_mfpr = (uint32_t *)(CRANE_MFPR_BASE + mfpr_offset);

        /* Write a mfg register as per configuration */
        val = 0;
        if (cfg_val & MFP_VALUE_MASK) {
            val |= cfg_val & MFP_VALUE_MASK;
        }

        for(i = 0; i< MFP_CONFIG_RETRY; i++) {
            writel(val, p_mfpr);
            val_r = readl(p_mfpr);
            if(val == val_r) {
              break;
            }
        }

        if(i == MFP_CONFIG_RETRY) {
            printf("mfp config (%p = 0x%x) failed, reg val = 0x%x\n", p_mfpr, val, val_r);
        }
    } while (1);
}

#define PMU_BASE_ADDR 0xd4282800

void pmu_write(uint32_t offset, uint32_t val)
{
    writel(val, PMU_BASE_ADDR + offset);
}

uint32_t pmu_read(uint32_t offset)
{
    return (uint32_t)readl(PMU_BASE_ADDR + offset);
}

void pmu_write_bits(uint32_t offset, uint32_t value, uint32_t bits, uint32_t shifts)
{
    uint32_t i = 0;
    uint32_t mask = 0;
    uint32_t reg_val = 0;

    if ((bits + shifts) > 32) {
        printf("%s: %d param (%d, %d) is error!!!! \n", __FUNCTION__, __LINE__, bits, shifts);
        return;
    }

    for (i = 0; i < bits; i++) {
        mask |= (1 << i);
    }

    reg_val = pmu_read(offset);
    reg_val &= ~(mask << shifts);
    reg_val |= (value << shifts);
    pmu_write(offset, reg_val);
}

void pmu_set_bits(uint32_t offset, uint32_t bits)
{
    pmu_write(offset, pmu_read(offset) | bits);
}

void pmu_clear_bits(uint32_t offset, uint32_t bits)
{
    pmu_write(offset, pmu_read(offset) & ~bits);
}

#if USE_CRANE_LCD || USE_CRANE_CAMERA

#define LCD_BASE_ADDR 0xd420a000

void hal_lcdc_write(uint32_t offset, uint32_t val)
{
    writel(val, LCD_BASE_ADDR + offset);
}

uint32_t hal_lcdc_read(uint32_t offset)
{
    return (uint32_t)readl(LCD_BASE_ADDR + offset);
}

void hal_lcdc_write_bits(uint32_t offset, uint32_t value, uint32_t bits, uint32_t shifts)
{
    uint32_t i = 0;
    uint32_t mask = 0;
    uint32_t reg_val = 0;

    if ((bits + shifts) > 32) {
        printf("%s: %d param (%d, %d) is error!!!! \n", __FUNCTION__, __LINE__, bits, shifts);
        return;
    }

    for (i = 0; i < bits; i++) {
        mask |= (1 << i);
    }

    reg_val = hal_lcdc_read(offset);
    reg_val &= ~(mask << shifts);
    reg_val |= (value << shifts);
    hal_lcdc_write(offset, reg_val);
}


void hal_lcdc_set_bits(uint32_t offset, uint32_t bits)
{
    hal_lcdc_write(offset, hal_lcdc_read(offset) | bits);
}

void hal_lcdc_clear_bits(uint32_t offset, uint32_t bits)
{
    hal_lcdc_write(offset, hal_lcdc_read(offset) & ~bits);
}

#define JPEG_BASE_ADDR 0xd420cc00
void jpu_write(uint32_t offset, uint32_t val)
{
    writel(val, JPEG_BASE_ADDR + offset);
}

uint32_t jpu_read(uint32_t offset)
{
    return (uint32_t)readl(JPEG_BASE_ADDR + offset);
}

void jpu_write_bits(uint32_t offset, uint32_t value, uint32_t bits, uint32_t shifts)
{
    uint32_t i = 0;
    uint32_t mask = 0;
    uint32_t reg_val = 0;

    if((bits + shifts) > 32){
        printf("%s: %d param (%d, %d) is error!!!! \n", __FUNCTION__, __LINE__, bits, shifts);
        return;
    }

    for(i = 0; i<bits; i++){
        mask |= (1 << i);
    }

    reg_val = jpu_read(offset);
    reg_val &= ~(mask << shifts);
    reg_val |= (value << shifts);
    jpu_write(offset, reg_val);
}

void jpu_set_bits(uint32_t offset, uint32_t bits)
{
    jpu_write(offset, jpu_read(offset) | bits);
}

void jpu_clear_bits(uint32_t offset, uint32_t bits)
{
    jpu_write(offset, jpu_read(offset) & ~bits);
}


#endif


