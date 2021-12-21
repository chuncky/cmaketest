#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <errno.h>
//#include "xz.h"		//porting AliOS_OTA
#include "updater.h"

extern int ali_hal_partition_init(void);
extern void PMIC_SW_RESET(void);


unsigned short ali_ota_get_data_crc16(const unsigned char *buf, unsigned int len)
{
    CRC16_CTX ctx;
    unsigned short crc16 = 0xffff;
    if((buf != NULL) && (len > 0)) {
        crc16_init(&ctx);
        crc16_update(&ctx, buf, len);
        crc16_final(&ctx, &crc16);
    }
    return crc16;
}




int ota_updater(void)
{
    int ret = 0;
    ota_boot_param_t ota_param = {0};

	uart_printf("++++++++++++++ ota_updater start ++++++++++++++++++++++\r\n");

	//[1] init hal_partition
	ret = ali_hal_partition_init();
    if(ret < 0) {
        OTA_LOG_I("ali_hal_partition err.");
		ali_clear_fota_magic();
        return -1;
    }

	//debug
	dupm_ali_hal_partition();

	
	//[2] init heap start address and buffer len
	//ali_heap_init();
	OTA_LOG_I("XZ_HEAP_BASE:0x%x,XZ_HEAP_SIZE:0x%x",XZ_HEAP_BASE,XZ_HEAP_SIZE);
	
	
	
	//[3] read struct: ota_boot_param_t, 7 parameters
    ret = ota_patch_read_param(&ota_param);
    if(ret < 0) {
        OTA_LOG_I("read param err.\n");
		ali_clear_fota_magic();
        return -1;
    }
    OTA_LOG_I("ota upg_flag:0x%04x ;0x%04x \n", ota_param.upg_flag, ota_param.param_crc);
    if(ota_param.upg_flag == OTA_UPGRADE_DIFF) {
		OTA_LOG_I("Enter ota_nbpatch_main");
        ret = ota_nbpatch_main();
		OTA_LOG_I("Exit ota_nbpatch_main");
    }  else {
        OTA_LOG_I("No OTA upgrade.\n");
		ali_clear_fota_magic();
        return 0;
    }

EXIT:
    OTA_LOG_I("ota update complete ret:%d \n", ret);
    if(ret < 0 ) {	// OTA Fail
		ota_param.upg_status = ret;
    }else{	//OTA Success
		ota_param.upg_status = 0;
    }
	
    if(ota_param.upg_flag != 0) {
        ota_param.upg_flag = 0;
        ota_patch_write_param(&ota_param);
    }
	OTA_LOG_I("clean fota_flag!");
	ali_clear_fota_magic();
	OTA_LOG_I("reboot system!");
    PMIC_SW_RESET();

    return ret;
}




