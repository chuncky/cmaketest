
#include "hal_drv_conf.h"
#include "halsoc_lcd_mipi.h"
#if USE_CRANE_LCD_MIPI

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bsp_common.h"
#include "halsoc_lcd_mipi_dsi.h"
#include "halsoc_lcd_mipi_dphy.h"

int lcd_mipi_init(uint32_t sclk, int32_t work_mode, const struct s_mipi_info *mipi_info)
{
    if(NULL == mipi_info) {
        printf("lcd_mipi_init : Invalid param\n");
        return -1;
    }

    return lcd_dsi_init(mipi_info, work_mode);
}

int lcd_mipi_uninit(void)
{
    printf("LCD mipi uninit\n");

    return lcd_dsi_uninit();
}

int lcd_mipi_reset(void)
{
    printf("LCD mipi reset\n");

    lcd_dsi_reset();
    return 0;
}

#if 0
int lcd_panel_mipi_update(struct s_dsi_ctx *dsi_ctx, int32_t work_mode)
{
  int ret;

  printf("lcd_panel_mipi_update\n");
  if(NULL == dsi_ctx){
    printf("Invalid param\n");
    return -1;
  }

  ret = lcd_dsi_update(work_mode);
  return ret;
}
#endif
#endif
