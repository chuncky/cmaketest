#ifndef _ADUPS_BL_FLASH_H
#define _ADUPS_BL_FLASH_H

#include "adups_typedef.h"

adups_uint32 adups_bl_flash_block_size(void);
adups_uint32 adups_bl_flash_backup_size(void);
adups_uint32 adups_bl_fota_get_block_size_length(adups_uint32 addr);
adups_uint32 adups_bl_flash_backup_base(void);
adups_uint32 adups_bl_flash_delta_base(void);
adups_uint32 adups_bl_flash_delta_size(void);
adups_uint32 AdupsGetFlashDiskSize(void);
adups_uint32 adups_bl_get_rtos_len(void);
adups_uint32 adups_bl_get_rtos_base(void);
adups_uint32 adups_bl_get_rom_base(void);
adups_int32 adups_bl_read_block(adups_uint8* dest, adups_uint32 start, adups_uint32 size);
adups_int32 adups_bl_write_block(adups_uint8* src, adups_uint32 start, adups_uint32 size);
adups_int32 adups_bl_read_flash(adups_uint32 offset, adups_uint8* dest,adups_uint32 size);

adups_int32 adups_bl_erase_block(adups_uint32 addr);
adups_uint32 adups_bl_erase_delata(void);
void adups_bl_erase_backup_region(void);
adups_uint32 adups_bl_init_flash(void);
adups_int32 adups_bl_write_backup_region(adups_uint8* data_ptr, adups_uint32 len);
adups_int32  adups_bl_read_backup_region(adups_uint8* data_ptr, adups_uint32 len);
ADUPS_BOOL AdupsUseFlash(void);
#endif 
