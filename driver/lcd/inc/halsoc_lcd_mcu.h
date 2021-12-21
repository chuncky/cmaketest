#ifndef _CRANE_LCD_MCU_H_
#define _CRANE_LCD_MCU_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "hal_drv_conf.h"

#if USE_CRANE_LCD_MCU

#include <stdint.h>

enum{
  MCU_BUS_8080 = 0,
  MCU_BUS_6800 = 1,
  MCU_BUS_LIMIT
};

enum{
  MCU_ENDIAN_LSB = 0,
  MCU_ENDIAN_MSB = 1,
  MCU_ENDIAN_LIMIT
};

enum{
  MCU_PATH_IMAGE = 0,
  MCU_PATH_REGISTER = 1,
  MCU_PATH_LIMIT
};

enum{
  MCU_STATUS_UNINIT = 0,
  MCU_STATUS_INIT = 1,
  MCU_STATUS_LIMIT
};

/* MCU LCD specific properties */
struct timing_mcu {
  uint16_t rlpw; /*ns*/
  uint16_t rhpw; /*ns*/
  uint16_t wlpw; /*ns*/
  uint16_t whpw; /*ns*/
};

struct mcu_info {
  uint16_t bus_mode; /*8080, 6800*/
  uint16_t format;
  uint16_t endian;
  uint16_t device_id; /*0, 1*/
  uint16_t force_cs;  /*0-disable, 1-enable*/
  uint16_t reserved;
  struct timing_mcu *timing;
};

int lcd_mcu_init(uint32_t sclk, struct mcu_info* info);
int lcd_mcu_write_cmd(uint32_t cmd, uint32_t bits);
int lcd_mcu_write_data(uint32_t data, uint32_t bits);
int lcd_mcu_read_data(uint32_t cmd, uint32_t cmd_bits, uint32_t* data, uint32_t data_bits);
int lcd_mcu_write_cmd_data(uint32_t cmd, uint32_t cmd_bits, uint32_t data, uint32_t data_bits);
int lcd_mcu_before_refresh(void);
int lcd_mcu_after_refresh(void);
void lcd_mcu_uninit(void);

#endif

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*_CRANE_LCD_MCU_H_ */
