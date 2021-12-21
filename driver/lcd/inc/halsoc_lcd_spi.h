#ifndef _CRANE_LCD_SPI_H_
#define _CRANE_LCD_SPI_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "hal_drv_conf.h"

#if USE_CRANE_LCD_SPI

#include <stdint.h>

enum {
    SPI_EDGE_RISING     = 0,
    SPI_EDGE_FALLING    = 1,
    SPI_EDGE_LIMIT
};

enum {
    SPI_ENDIAN_LSB  = 0,
    SPI_ENDIAN_MSB  = 1,
    SPI_ENDIAN_LIMIT
};

enum {
    SPI_PATH_IMAGE      = 1,
    SPI_PATH_REGISTER   = 2,
    SPI_PATH_LIMIT
};

enum {
    SPI_STATUS_UNINIT   = 0,
    SPI_STATUS_INIT     = 1,
    SPI_STATUS_LIMIT
};

struct spi_timing {
    uint32_t wclk; /*KHz*/
    uint32_t rclk; /*KHz*/
};

struct spi_info {
    uint16_t line_num; /*3 or 4*/
    uint16_t interface_id; /*1 or 2*/
    uint16_t data_lane_num; /*1 or 2*/
    uint16_t format;
    uint16_t device_id; /*0 or 1*/
    uint16_t sample_edge;
    uint16_t endian;
    uint16_t reserved;
    const struct spi_timing *timing;
};

int32_t lcd_spi_init(uint32_t sclk, const struct spi_info *info);
int32_t lcd_spi_write_cmd(uint32_t cmd, uint32_t bits);
int32_t lcd_spi_write_data(uint32_t data, uint32_t bits);
int32_t lcd_spi_read_data(uint32_t cmd, uint32_t cmd_bits, uint32_t *data, uint32_t data_bits, uint8_t read_gram);
int32_t lcd_spi_before_refresh(void);
int32_t lcd_spi_after_refresh(void);

#endif /* USE_CRANE_LCD_SPI */

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
