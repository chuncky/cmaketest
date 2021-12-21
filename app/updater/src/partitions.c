/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stddef.h>

//#include "aos/hal/flash.h"
#include "flash.h"
#include "loadtable.h"	//porting AliOS_OTA

/* Logic partition on flash devices */

/*TODO: get the information from fota_param
*@partition_start_addr
*@partition_length
*/
hal_logic_partition_t hal_partitions[] =
{
    [HAL_PARTITION_APPLICATION] =
    {
        .partition_owner            = HAL_FLASH_EMBEDDED,
        .partition_description      = "system",
        .partition_options          = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
    },
    [HAL_PARTITION_OTA_TEMP] =
    {
        .partition_owner           = HAL_FLASH_EMBEDDED,
        .partition_description     = "fota_pkg",
        .partition_options         = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
    },
    [HAL_PARTITION_PARAMETER_1] =
    {
        .partition_owner            = HAL_FLASH_EMBEDDED,
        .partition_description      = "fota_ali_param",
        .partition_options          = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
    },
};

const size_t hal_partitions_amount = (sizeof(hal_partitions)/sizeof(hal_logic_partition_t));


int ali_hal_partition_init(void)
{
	uart_printf("ali_hal_partition_init start ++++++++\r\n");
	unsigned int fota_pkg_offset_4K = 0x1000;	//4K
	unsigned int fota_pkg_offset_8K = 0x2000;	//8K
	int ret_val = 0;

	if(get_system_start_addr() == (-1) ||
	   get_system_end_addr() == (-1)  ||
	   get_fota_pkg_start_addr() == (-1) ||
	   get_fota_pkg_end_addr() == (-1) )
	{
	   ret_val = -1;
	   return ret_val;
	}
	//Note:addr must minus 0x80000000
	hal_partitions[HAL_PARTITION_APPLICATION].partition_start_addr = get_system_start_addr() ;
	hal_partitions[HAL_PARTITION_APPLICATION].partition_length = get_system_end_addr() - get_system_start_addr();

	hal_partitions[HAL_PARTITION_OTA_TEMP].partition_start_addr = get_fota_pkg_start_addr() + fota_pkg_offset_8K ;		//fota_pkg:
	hal_partitions[HAL_PARTITION_OTA_TEMP].partition_length = get_fota_pkg_end_addr() - get_fota_pkg_start_addr() - fota_pkg_offset_8K;

	hal_partitions[HAL_PARTITION_PARAMETER_1].partition_start_addr = get_fota_pkg_start_addr() ;
	hal_partitions[HAL_PARTITION_PARAMETER_1].partition_length = fota_pkg_offset_4K;
	uart_printf("ali_hal_partition_init end   --------\r\n");
	return ret_val;
}


void dupm_ali_hal_partition(void)
{
    unsigned i;

    uart_printf("============DUMP ali_hal_partition ===========\r\n");
	
    uart_printf("%-16s,start addr:0x%x,length:0x%x\r\n", 
               hal_partitions[HAL_PARTITION_APPLICATION].partition_description,
               hal_partitions[HAL_PARTITION_APPLICATION].partition_start_addr, hal_partitions[HAL_PARTITION_APPLICATION].partition_length);

	uart_printf("%-16s,start addr:0x%x,length:0x%x\r\n", 
			   hal_partitions[HAL_PARTITION_OTA_TEMP].partition_description,
			   hal_partitions[HAL_PARTITION_OTA_TEMP].partition_start_addr, hal_partitions[HAL_PARTITION_OTA_TEMP].partition_length);
	
	uart_printf("%-16s,start addr:0x%x,length:0x%x\r\n", 
			   hal_partitions[HAL_PARTITION_PARAMETER_1].partition_description,
			   hal_partitions[HAL_PARTITION_PARAMETER_1].partition_start_addr, hal_partitions[HAL_PARTITION_PARAMETER_1].partition_length);
}

