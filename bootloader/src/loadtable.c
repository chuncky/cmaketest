#include <stdio.h>
#include <string.h>
#include "loadtable.h"
#include "ptable.h"
#include "common.h"
#include "version_block.h"
#include "updater_table.h"

/*
*****************************************************************************************
*											Macro Used if CP is Erased
*****************************************************************************************
*/
#ifdef CP_ERASE    //OTA_SD will Erase cp.bin
//CP Partition Parametr
#define DEFAULT_CP_HEADER                 (0xE320F000)

//Updater Partition Parameter
#define DEFAULT_UPDATER_START_ADDRESS     (0x80FC0000)
#define DEFAULT_UPDATER_END_ADDRESS       (0x80FE0000)
#define DEFAULT_UPDATER_SIZE              (0x00020000)

//Fota Partition Parameter
#define DEFAULT_FOTA_PARAM_START_ADDRESS  (0x80FBF000)
#define DEFAULT_FOTA_PARAM_END_ADDRESS    (0x80FC0000)

extern volatile unsigned long CpFlashAddress;
#endif

//CRC-16 table
static UINT16 crc16_table[256] = {
0x0000, 0x1189, 0x2312, 0x329b, 0x4624, 0x57ad, 0x6536, 0x74bf,
0x8c48, 0x9dc1, 0xaf5a, 0xbed3, 0xca6c, 0xdbe5, 0xe97e, 0xf8f7,
0x1081, 0x0108, 0x3393, 0x221a, 0x56a5, 0x472c, 0x75b7, 0x643e,
0x9cc9, 0x8d40, 0xbfdb, 0xae52, 0xdaed, 0xcb64, 0xf9ff, 0xe876,
0x2102, 0x308b, 0x0210, 0x1399, 0x6726, 0x76af, 0x4434, 0x55bd,
0xad4a, 0xbcc3, 0x8e58, 0x9fd1, 0xeb6e, 0xfae7, 0xc87c, 0xd9f5,
0x3183, 0x200a, 0x1291, 0x0318, 0x77a7, 0x662e, 0x54b5, 0x453c,
0xbdcb, 0xac42, 0x9ed9, 0x8f50, 0xfbef, 0xea66, 0xd8fd, 0xc974,
0x4204, 0x538d, 0x6116, 0x709f, 0x0420, 0x15a9, 0x2732, 0x36bb,
0xce4c, 0xdfc5, 0xed5e, 0xfcd7, 0x8868, 0x99e1, 0xab7a, 0xbaf3,
0x5285, 0x430c, 0x7197, 0x601e, 0x14a1, 0x0528, 0x37b3, 0x263a,
0xdecd, 0xcf44, 0xfddf, 0xec56, 0x98e9, 0x8960, 0xbbfb, 0xaa72,
0x6306, 0x728f, 0x4014, 0x519d, 0x2522, 0x34ab, 0x0630, 0x17b9,
0xef4e, 0xfec7, 0xcc5c, 0xddd5, 0xa96a, 0xb8e3, 0x8a78, 0x9bf1,
0x7387, 0x620e, 0x5095, 0x411c, 0x35a3, 0x242a, 0x16b1, 0x0738,
0xffcf, 0xee46, 0xdcdd, 0xcd54, 0xb9eb, 0xa862, 0x9af9, 0x8b70,
0x8408, 0x9581, 0xa71a, 0xb693, 0xc22c, 0xd3a5, 0xe13e, 0xf0b7,
0x0840, 0x19c9, 0x2b52, 0x3adb, 0x4e64, 0x5fed, 0x6d76, 0x7cff,
0x9489, 0x8500, 0xb79b, 0xa612, 0xd2ad, 0xc324, 0xf1bf, 0xe036,
0x18c1, 0x0948, 0x3bd3, 0x2a5a, 0x5ee5, 0x4f6c, 0x7df7, 0x6c7e,
0xa50a, 0xb483, 0x8618, 0x9791, 0xe32e, 0xf2a7, 0xc03c, 0xd1b5,
0x2942, 0x38cb, 0x0a50, 0x1bd9, 0x6f66, 0x7eef, 0x4c74, 0x5dfd,
0xb58b, 0xa402, 0x9699, 0x8710, 0xf3af, 0xe226, 0xd0bd, 0xc134,
0x39c3, 0x284a, 0x1ad1, 0x0b58, 0x7fe7, 0x6e6e, 0x5cf5, 0x4d7c,
0xc60c, 0xd785, 0xe51e, 0xf497, 0x8028, 0x91a1, 0xa33a, 0xb2b3,
0x4a44, 0x5bcd, 0x6956, 0x78df, 0x0c60, 0x1de9, 0x2f72, 0x3efb,
0xd68d, 0xc704, 0xf59f, 0xe416, 0x90a9, 0x8120, 0xb3bb, 0xa232,
0x5ac5, 0x4b4c, 0x79d7, 0x685e, 0x1ce1, 0x0d68, 0x3ff3, 0x2e7a,
0xe70e, 0xf687, 0xc41c, 0xd595, 0xa12a, 0xb0a3, 0x8238, 0x93b1,
0x6b46, 0x7acf, 0x4854, 0x59dd, 0x2d62, 0x3ceb, 0x0e70, 0x1ff9,
0xf78f, 0xe606, 0xd49d, 0xc514, 0xb1ab, 0xa022, 0x92b9, 0x8330,
0x7bc7, 0x6a4e, 0x58d5, 0x495c, 0x3de3, 0x2c6a, 0x1ef1, 0x0f78
};

static UINT32 Crc_Check_16(UINT8* data, UINT32 length,UINT16 crc_reg)
{
	UINT32 count = 0;
	while (length--){
		crc_reg = (crc_reg >> 8) ^ crc16_table[(crc_reg ^ *data++) & 0xff];
		count++;
		if(count == 0x10000)
		{
			count = 0;
			crc_reg = ~crc_reg;
		}
	}
	return (UINT32)(~crc_reg) & 0x0000FFFF;
}

/*
_ptentry *apnEntry, *rdEntry, *nvmEntry;
_ptentry *factory_a_Entry, *factory_b_Entry;
*/
_ptentry *fota_param_Entry ,*fota_pkg_Entry ,*updater_Entry, *updater_backup_Entry, *cp_Entry;
_ptentry *logo_entry;



unsigned char updater_available = 0;
unsigned char updater_backup_available = 0;

void check_updater()
{
	UINT16 check_crc, calc_crc = 0xFFFF;
	UINT32 updater_vsize_from_table;

	if(updater_Entry)
	{
		updater_vsize_from_table = updater_get_ext_combine_bin_size() + updater_get_ext_combine_crc_size();
		uart_printf("updater address : 0x%x, size : 0x%x\r\n", 
			updater_Entry->vstart, 
			updater_vsize_from_table);
		calc_crc = Crc_Check_16(updater_Entry->vstart, updater_vsize_from_table - 4, calc_crc);
		check_crc = *(UINT16 *)(updater_Entry->vstart + updater_vsize_from_table - 4);
		uart_printf("updater CRC: 0x%x(check), 0x%x(calc)\r\n", check_crc, calc_crc);
		if(check_crc == calc_crc)
			updater_available = 1;
	}

	check_crc = 0xFFFF;
	calc_crc  = 0xFFFF;
	if(updater_backup_Entry)
	{
		uart_printf("updater backup address : 0x%x, size : 0x%x\r\n", 
			updater_backup_Entry->vstart, 
			updater_backup_Entry->vsize);
		calc_crc = Crc_Check_16(updater_backup_Entry->vstart, updater_backup_Entry->vsize - 4, calc_crc);
		check_crc = *(UINT16 *)(updater_backup_Entry->vstart + updater_backup_Entry->vsize - 4);
		uart_printf("updater backup CRC: 0x%x(check), 0x%x(calc)\r\n", check_crc, calc_crc);
		if(check_crc == calc_crc)
			updater_backup_available = 1;
	}

}

LoadTableType *pLoadTable = (LoadTableType*)INVALID_ADDRESS;

void loadtable_init(unsigned int cp_flash_addr)
{
    pLoadTable = (LoadTableType*)(cp_flash_addr + BINARY_LOAD_TABLE_OFFSET);

	//cp_partition
	cp_Entry 		 = ptable_find_entry("cp");

    //fota_param
    fota_param_Entry = ptable_find_entry("fota_param");
    //fota_pkg
    fota_pkg_Entry   = ptable_find_entry("fota_pkg");
    //update updater
    updater_Entry    = ptable_find_entry("updater");

	logo_entry = ptable_find_entry("logo");

	updater_backup_Entry = ptable_find_entry("updater_bak");
	updater_header_table_init();
	if(updater_backup_Entry)
	{
		check_updater();
	}
}

//unused for boot33
unsigned int get_cp_load_addr(void)
{
	if(cp_Entry != NULL){
		return cp_Entry->vstart;
	}else{
		//TODO: once ptable block was ruined by fota erase and power shut, could not recovery
		uart_printf("[fatal_err][%s][%d]\r\n",__func__,__LINE__);
		return -1;
	}
}

//for PSRAM execute mode only
unsigned int get_cp_exec_addr(void)
{
    return pLoadTable->lt_area_armlink_symbol.armlink_symbol_info.cp_exec_addr.value;
}

//recorder inside loadtable, to detect DSP.bin start address
unsigned int get_cp_binary_size(void){
    return pLoadTable->lt_area_armlink_symbol.armlink_symbol_info.binary_size.value;
}

//for PSRAM execute mode only
unsigned int get_cp_copy_size(void)
{
    return pLoadTable->lt_area_armlink_symbol.armlink_symbol_info.image_end.value - pLoadTable->lt_area_armlink_symbol.armlink_symbol_info.cp_exec_addr.value + 0x04 ;
}

unsigned int get_dsp_load_addr(void)
{
    return pLoadTable->lt_area_armlink_symbol.armlink_symbol_info.dsp_begin_addr.value;
}

unsigned int get_dsp_copy_size(void)
{
    return pLoadTable->lt_area_armlink_symbol.armlink_symbol_info.dsp_end_addr.value - pLoadTable->lt_area_armlink_symbol.armlink_symbol_info.dsp_begin_addr.value + 0x04;
}

unsigned int get_fota_param_start_addr(void)
{
    if(fota_param_Entry != NULL){
    	return fota_param_Entry->start;
    }else{
		//TODO: once ptable block was ruined by fota erase and power shut, could not recovery
		uart_printf("[fatal_err][%s][%d]\r\n",__func__,__LINE__);
		return DEFAULT_FOTA_PARAM_START_ADDRESS;
    }
}

unsigned int get_fota_param_vstart_addr(void)
{
    if(fota_param_Entry != NULL){
    	return fota_param_Entry->vstart;
    }else{
		//TODO: once ptable block was ruined by fota erase and power shut, could not recovery
		uart_printf("[fatal_err][%s][%d]\r\n",__func__,__LINE__);
		return DEFAULT_FOTA_PARAM_START_ADDRESS;
    }
}

unsigned int is_fota_param_available(void)
{
    if(fota_param_Entry != NULL){
    	return fota_param_Entry->size;
    }else{
	    return 0;
    }
}

unsigned int get_updater_start_addr(void)
{
	if(updater_backup_Entry == NULL)
	{
	    if(updater_Entry != NULL){
	    	return updater_Entry->vstart;
	    }else{
		//TODO: once ptable block was ruined by fota erase and power shut, could not recovery
			uart_printf("[fatal_err][%s][%d]\r\n",__func__,__LINE__);
			return DEFAULT_UPDATER_START_ADDRESS;
	    }
	}else{
		if(updater_available)
			return updater_Entry->vstart;
		else if(updater_backup_available)
			return updater_backup_Entry->vstart;
		else {
			uart_printf("[fatal_err][%s][%d] should never be here\r\n",__func__,__LINE__);
			return -1;
		}
	}
}

unsigned int get_updater_end_addr(void)
{
	unsigned int updater_end_addr;
	
	if(updater_backup_Entry == NULL)
	{
	    if(updater_Entry != NULL){
			if(IsUpdaterSupportQueryExtImgInfo()){
				updater_end_addr = updater_Entry->vstart + updater_get_ext_combine_bin_size() + updater_get_ext_combine_crc_size();
			}else{
				updater_end_addr =  updater_Entry->vstart + updater_Entry->vsize;
			}
	    }else{
			//TODO: once ptable block was ruined by fota erase and power shut, could not recovery
			uart_printf("[fatal_err][%s][%d]\r\n",__func__,__LINE__);
			updater_end_addr =  DEFAULT_UPDATER_END_ADDRESS;
	    }
	}else{
		if(updater_available){
			if(IsUpdaterSupportQueryExtImgInfo()){
				updater_end_addr = updater_Entry->vstart + updater_get_ext_combine_bin_size() + updater_get_ext_combine_crc_size();
			}else{
				updater_end_addr = updater_Entry->vstart + updater_Entry->vsize;
			}
		}else if(updater_backup_available){
			updater_end_addr = updater_backup_Entry->vstart + updater_backup_Entry->vsize;
		}else {
			uart_printf("[fatal_err][%s][%d]\r\n",__func__,__LINE__);
			updater_end_addr =  -1;
		}
	}

	return updater_end_addr;
}

unsigned int get_updater_copy_size(void)
{
    return get_updater_end_addr() - get_updater_start_addr();
}

unsigned int get_fota_pkg_start_addr(void)
{
#ifdef AVOID_PTABLE
    return pLoadTable->lt_area_armlink_symbol.armlink_symbol_info.fota_pkg_start_address.value;
#else
    if(fota_pkg_Entry != NULL){
    	return fota_pkg_Entry->vstart;
    }else{
	//TODO: once ptable block was ruined by fota erase and power shut, could not recovery
	uart_printf("[fatal_err][%s][%d]\r\n",__func__,__LINE__);
	return -1;
    }
#endif
}

unsigned int get_fota_pkg_end_addr(void)
{
#ifdef AVOID_PTABLE
    return pLoadTable->lt_area_armlink_symbol.armlink_symbol_info.fota_pkg_end_address.value;
#else
    if(fota_pkg_Entry != NULL){
    	return fota_pkg_Entry->vstart + fota_pkg_Entry->size;
    }else{
	//TODO: once ptable block was ruined by fota erase and power shut, could not recovery
	uart_printf("[fatal_err][%s][%d]\r\n",__func__,__LINE__);
	return -1;
    }
#endif
}

unsigned int getLongChipId(void){
    return ((*(volatile unsigned int *)0xD4282C00)&0xffffffff);
}

unsigned int get_ps_mode(void)
{
    if(strstr(pLoadTable->lt_area_ver_info.version_info.ps_mode,"NBIOT")){
        return NBIOT;
    }else if (strstr(pLoadTable->lt_area_ver_info.version_info.ps_mode,"CATM")){
        return CATM;
    }else if (strstr(pLoadTable->lt_area_ver_info.version_info.ps_mode,"LTEONLY")){
        return LTEONLY;
    }else if (strstr(pLoadTable->lt_area_ver_info.version_info.ps_mode,"LTEGSM")){
        return LTEGSM;
    }
}

unsigned int get_rf_load_size(void)
{
    unsigned int psmode = get_ps_mode();
    unsigned int rf_load_size;

    // tmp wrokaround :load the front 20K rf.bin for all mode

    rf_load_size = 0x5000;//20k

#ifdef BOOT33_CP_DSP_RF
    switch(psmode)
    {
        case NBIOT:
            uart_printf("[RF ] PS_MODE=[%s] rf_load_size = [0x%08x]\r\n","NBIOT",rf_load_size);
            break;
        case LTEGSM:
            uart_printf("[RF ] PS_MODE=[%s] rf_load_size = [0x%08x]\r\n","LTEGSM",rf_load_size);
            break;
        case LTEONLY:
            uart_printf("[RF ] PS_MODE=[%s] rf_load_size = [0x%08x]\r\n","LTEONLY",rf_load_size);
            break;
        case CATM:
            uart_printf("[RF ] PS_MODE=[%s] rf_load_size = [0x%08x]\r\n","CATM",rf_load_size);
            break;
        default:
            break;
    }
#endif

    return rf_load_size;
}


//unsigned int get_rf_load_addr(void)
//{
//    unsigned int chipID = getLongChipId();
//    unsigned int rf_load_addr;

//    switch(chipID)
//    {
//        case CRANE_Z2:
//            uart_printf("[CP ] CHIP_ID=[%08x] CRANE_Z2 \r\n",chipID);
//            rf_load_addr = pLoadTable->lt_area_armlink_symbol.armlink_symbol_info.rf_z2_load_addr.value;
//            break;
//        case CRANE_A0:
//            uart_printf("[CP ] CHIP_ID=[%08x] CRANE_A0 \r\n",chipID);
//            rf_load_addr = pLoadTable->lt_area_armlink_symbol.armlink_symbol_info.rf_a0_load_addr.value;
//            break;
//        default:
//            uart_printf("\r\n ** ERROR: UNKNOW CHIP TYPE!");
//            while(1){};
//    }

//    return rf_load_addr;
//}

unsigned int get_rw_cpz_struct_addr(void)
{
    return (unsigned int)pLoadTable->lt_area_rw_cpz_info.compress_rw_region_list;
}

UINT32 get_rw_zi_start_addr(void)
{
	return pLoadTable->lt_area_zi_info.zi_info.DDR_ZI_Base.value;
}

UINT32 get_rw_zi_end_addr( void )
{
	return pLoadTable->lt_area_zi_info.zi_info.DDR_ZI_Limit.value;
}

UINT32 get_rw_zi_length(void)
{
	return pLoadTable->lt_area_zi_info.zi_info.DDR_ZI_Limit.value - pLoadTable->lt_area_zi_info.zi_info.DDR_ZI_Base.value;
}


UINT32 get_dsp_backup_size(void)
{
	return pLoadTable->lt_area_temp_info.temp_info.DSP_BACKUP_SIZE.value;
}

UINT32 get_ptable_exec_start_addr(void)
{
	return pLoadTable->lt_area_temp_info.temp_info.PTABLE_EXEC_START_ADDR.value;
}

UINT32 get_ptable_exec_end_addr(void)
{
	return pLoadTable->lt_area_temp_info.temp_info.PTABLE_EXEC_END_ADDR.value;
}

UINT32 get_heap_guard_addr(void)
{
	return pLoadTable->lt_area_temp_info.temp_info.HEAP_GUARD_ADDR.value;
}

UINT32 get_heap_endmark_addr(void)
{
	return pLoadTable->lt_area_temp_info.temp_info.HEAP_ENDMARK_ADDR.value;
}

UINT32 get_heap_length( void )
{
	return pLoadTable->lt_area_temp_info.temp_info.HEAP_ENDMARK_ADDR.value - pLoadTable->lt_area_temp_info.temp_info.HEAP_GUARD_ADDR.value;
}


UINT32 get_logo_length_in_cp(void)
{
	return pLoadTable->lt_area_armlink_symbol.armlink_symbol_info.logo_img_length.value;
}

#if 0
//apn
void set_apn_begin_addr(unsigned int begin_addr){
    uart_printf("[APN] apn_begin_addr      = [%.08x]\r\n",begin_addr);
    crane_qspi_write_uint(&(pLoadTable->lt_area_armlink_symbol.armlink_symbol_info.apn_begin_addr.value),begin_addr);
}

void set_apn_end_addr(unsigned int end_addr){
    uart_printf("[APN] apn_end_addr        = [%.08x]\r\n",end_addr);
    crane_qspi_write_uint(&(pLoadTable->lt_area_armlink_symbol.armlink_symbol_info.apn_end_addr.value),end_addr);
}
//rd
void set_rd_begin_addr(unsigned int begin_addr){
    uart_printf("[RD ] rd_begin_addr       = [%.08x]\r\n",begin_addr);
    crane_qspi_write_uint(&(pLoadTable->lt_area_armlink_symbol.armlink_symbol_info.rd_begin_addr.value),begin_addr);
}
void set_rd_end_addr(unsigned int end_addr){
    uart_printf("[RD ] rd_end_addr         = [%.08x]\r\n",end_addr);
    crane_qspi_write_uint(&(pLoadTable->lt_area_armlink_symbol.armlink_symbol_info.rd_end_addr.value),end_addr);
}
//nvm
void set_nvm_end_addr(unsigned int end_addr){
    uart_printf("[NVM] nvm_end_addr        = [%.08x]\r\n",end_addr);
    crane_qspi_write_uint(&(pLoadTable->lt_area_armlink_symbol.armlink_symbol_info.nvm_end_addr.value),end_addr);
}

void update_nvm_end_addr(unsigned int available_end_addr){
    uart_printf("[NVM] available_end_addr  = [%.08x]\r\n",available_end_addr);
    available_end_addr |= FLASH_BLOCK_MASK;
    available_end_addr -= FLASH_BLOCK_MASK; 
    set_nvm_end_addr(available_end_addr);
}

void set_nvm_begin_addr(unsigned int begin_addr){
    uart_printf("[NVM] nvm_begin_addr      = [%.08x]\r\n",begin_addr);
    crane_qspi_write_uint(&(pLoadTable->lt_area_armlink_symbol.armlink_symbol_info.nvm_begin_addr.value),begin_addr);
}

void update_nvm_begin_addr(unsigned int available_base_addr){
    uart_printf("[NVM] available_base_addr = [%.08x]\r\n",available_base_addr);
    available_base_addr += FLASH_BLOCK_MASK; 
    available_base_addr &= ~(FLASH_BLOCK_MASK);
    set_nvm_begin_addr(available_base_addr);
}
//factory
void set_factory_a_begin_addr(unsigned int begin_addr){
    uart_printf("[F_A] factory_a_begin_addr= [%.08x]\r\n",begin_addr);
    crane_qspi_write_uint(&(pLoadTable->lt_area_armlink_symbol.armlink_symbol_info.factory_a_begin_addr.value),begin_addr);
}

void set_factory_a_end_addr(unsigned int end_addr){
    uart_printf("[F_A] factory_a_end_addr  = [%.08x]\r\n",end_addr);
    crane_qspi_write_uint(&(pLoadTable->lt_area_armlink_symbol.armlink_symbol_info.factory_a_end_addr.value),end_addr);
}

void set_factory_b_begin_addr(unsigned int begin_addr){
    uart_printf("[F_B] factory_b_begin_addr= [%.08x]\r\n",begin_addr);
    crane_qspi_write_uint(&(pLoadTable->lt_area_armlink_symbol.armlink_symbol_info.factory_b_begin_addr.value),begin_addr);
}

void set_factory_b_end_addr(unsigned int end_addr){
    uart_printf("[F_B] factory_b_end_addr  = [%.08x]\r\n",end_addr);
    crane_qspi_write_uint(&(pLoadTable->lt_area_armlink_symbol.armlink_symbol_info.factory_b_end_addr.value),end_addr);
}

int vergin_status_fetch(void)
{
    unsigned int is_vergin = pLoadTable->lt_area_func_val.func_val.vergin.value;
    if(is_vergin){
        crane_qspi_write_uint(&(pLoadTable->lt_area_func_val.func_val.vergin.value),(unsigned int)0);
        return 1;
    }else{
        return 0;
    }
}

void number_of_life_update(void)
{
    unsigned int bootcount = pLoadTable->lt_area_func_val.func_val.number_of_life.value;
    crane_qspi_write_uint(&(pLoadTable->lt_area_func_val.func_val.number_of_life.value),(bootcount+1));
}
#endif

CP_EXECUTE_MODE get_cp_execute_mode(void)
{
    return strncmp(pLoadTable->lt_area_ver_info.version_info.execute_mode,"XIP",3)?PSRAM:XIP;
}

int anti_rollback_check(void)
{
	extern Boot33VerBlockType boot33_vb;

    unsigned int cp_anti_rb_ver_dec = atoi((const char*)&pLoadTable->lt_area_ver_info.version_info.anti_rollback_version);
    unsigned int boot33_ver_dec = atoi((const char*)&boot33_vb.vb.version_block.version_date);
    
    if(boot33_ver_dec < cp_anti_rb_ver_dec ){
        CP_LOGE("\r\n ** ERROR: BOOT33 UPDATE EXPECTED, AT LEAST TO [%s] \r\n",pLoadTable->lt_area_ver_info.version_info.anti_rollback_version);
		CP_LOGE("BOOT33_VERSION : %d\r\n",boot33_ver_dec);
		CP_LOGE("CP_ANTI_RB_VER : %d\r\n",cp_anti_rb_ver_dec);
        return -1;
    }
    return 0;
}

void dump_loadtable(void)
{
    rw_region_item region_info;
	armlink_symbol_item * tmp_p ;
	zi_info_item *p_zi_info_temp;
	temp_info_item *p_temp_info_item;
    unsigned schedule_count = 0;
    unsigned count = 0;
    char * rw_cpz_struct_addr=(char*)get_rw_cpz_struct_addr();

//++++++++++++++++++++++++++++++++++++++++++++++++++
//AREA_1: LOADTABLE_HEADER
    CP_LOGD("[%57s]\r\n","---------------------------------------------------------");
    CP_LOGD("[%-15s][%-40s]\r\n","LOADTABLE AREA","LOADTABLE_AREA_HEADER");
    CP_LOGD("[%-15s][%-40x]\r\n","INIT_ROUTINE"  ,pLoadTable->lt_area_header.init_routine.init);

//++++++++++++++++++++++++++++++++++++++++++++++++++
//AREA_2: VERSION & SIGNATURE
    CP_LOGD("[%57s]\r\n","---------------------------------------------------------");
    CP_LOGD("[%-15s][%-40s]\r\n","LOADTABLE AREA","LOADTABLE_AREA_VER_INFO");
    CP_LOGD("[%-15s][%-40s]\r\n","ANTI_RB_VER"  ,pLoadTable->lt_area_ver_info.version_info.anti_rollback_version);
    CP_LOGD("[%-15s][%-40s]\r\n","EXECUTE_MODE" ,pLoadTable->lt_area_ver_info.version_info.execute_mode);
    CP_LOGI("%s:%s\r\n","PS"      ,pLoadTable->lt_area_ver_info.version_info.ps_mode);
    CP_LOGI("%s:%s\r\n","IMG"   ,pLoadTable->lt_area_ver_info.version_info.image_info);

//++++++++++++++++++++++++++++++++++++++++++++++++++
//AREA_3: RW COMPRESS REGION SYMBOL
    CP_LOGD("[%57s]\r\n","---------------------------------------------------------");
    CP_LOGI("%s\r\n","LDT_RW_CPZ_INFO");
    while(1){
        //read NEXT region cpz info struct
        memcpy(&region_info
                ,(void*)(rw_cpz_struct_addr + (sizeof(rw_region_item)*schedule_count++))
                ,sizeof(rw_region_item));

        if(strncmp(region_info.RW_REGION_MARK, RW_REGION_MARK_PRE_STRING ,strlen(RW_REGION_MARK_PRE_STRING)-1))
        { 
            break;
        }else if(region_info.RW_REGION_COMPRESSED_ADDR == RW_REGION_COMPRESSED_ADDR_NONE ){
            CP_LOGW("%s\r\n","NO COMPRESSED RW REGION");
            break;
        }

        //dump region info
        CP_LOGI("[%.7s%c][%7s][%.08x][%.08x][%.08x][%.08x]\r\n",
                region_info.RW_REGION_MARK,
                region_info.RW_REGION_MARK_NUM,
                region_info.RW_REGION_NAME,
                region_info.RW_REGION_EXEC_ADDR,
                region_info.RW_REGION_LOAD_ADDR,
                region_info.RW_REGION_LENGTH,
                region_info.RW_REGION_COMPRESSED_ADDR
                );
    }

//++++++++++++++++++++++++++++++++++++++++++++++++++
//AREA_4: ARMLINKE SYMBOL LIST
    CP_LOGD("[%57s]\r\n","---------------------------------------------------------");
    CP_LOGD("[%-15s][%-40s]\r\n","LOADTABLE AREA","LOADTABLE_AREA_ARMLINK_SYMBOL");
    schedule_count = sizeof(loadtable_armlink_symbol_info)/sizeof(armlink_symbol_item);
    
    tmp_p = (armlink_symbol_item *)&(pLoadTable->lt_area_armlink_symbol);
    do{
        CP_LOGD("[%02d.%-12s][0x%.08x][%-28s]\r\n",count,tmp_p->name, tmp_p->value
                ,(tmp_p->value==INVALID_ADDRESS?"INVALID":"AVAILABLE"));
		count++;
        tmp_p ++;
    }while(schedule_count > count && tmp_p->value!=EMPTY_ADDRESS);

//++++++++++++++++++++++++++++++++++++++++++++++++++
//AREA_5: FUNCTIONAL VAL
    CP_LOGD("[%57s]\r\n","---------------------------------------------------------");
    CP_LOGD("[%-15s][%-40s]\r\n","LOADTABLE AREA","LOADTABLE_AREA_FUNC_VAL");
    schedule_count = sizeof(loadtable_functional_info)/sizeof(functional_item);
    count = 0;
    tmp_p = (armlink_symbol_item * )&(pLoadTable->lt_area_func_val);

    do{
        CP_LOGD("[%02d.%-12s][%-40x]\r\n",count,tmp_p->name, tmp_p->value);
        tmp_p ++;
		count++;
    }while(schedule_count > count);

//++++++++++++++++++++++++++++++++++++++++++++++++++
//AREA_6: ZI INFO for LZMA
	CP_LOGD("[%57s]\r\n","---------------------------------------------------------");
	CP_LOGD("[%-15s][%-40s]\r\n","LOADTABLE AREA","LOADTABLE_AREA_ZI_INFO");
	schedule_count = sizeof(pLoadTable->lt_area_zi_info.zi_info)/sizeof(zi_info_item);
	count = 0;
	p_zi_info_temp = (zi_info_item * )&(pLoadTable->lt_area_zi_info.zi_info.DDR_ZI_Base);

	do{
		CP_LOGD("[%02d.%-12s][%-40x]\r\n",count,p_zi_info_temp->name, p_zi_info_temp->value);
		p_zi_info_temp ++;
		count++;
	}while(schedule_count > count); 

//++++++++++++++++++++++++++++++++++++++++++++++++++
//AREA_7: TEMP INFO for LZMA
	CP_LOGD("[%57s]\r\n","---------------------------------------------------------");
	CP_LOGD("[%-15s][%-40s]\r\n","LOADTABLE AREA","LOADTABLE_AREA_TEMP_INFO");
	schedule_count = sizeof(pLoadTable->lt_area_temp_info.temp_info)/sizeof(temp_info_item);
	count = 0;
	p_temp_info_item = (temp_info_item * )&(pLoadTable->lt_area_temp_info.temp_info.DSP_BACKUP_SIZE);

	do{
		CP_LOGD("[%02d.%-12s][%-40x]\r\n",count,p_temp_info_item->name, p_temp_info_item->value);
		p_temp_info_item ++;
		count++;
	}while(schedule_count > count); 

//++++++++++++++++++++++++++++++++++++++++++++++++++
	CP_LOGD("[%57s]\r\n","---------------------------------------------------------");
}

BOOL is_partition_internal_flash(const char *name)
{

	_ptflash tmp = ptable_get_flash_type(name);

	return (tmp == pt_flash_internal);
}

BOOL IsUpdaterbakExist(void)
{
	_ptentry *updater_bak_Entry;
	updater_bak_Entry = ptable_find_entry("updater_bak");
	if(updater_bak_Entry != NULL){
		return TRUE;
	}else{
		return FALSE;
	}	
}

unsigned int get_updater_backup_start_addr(void)
{
    if(updater_backup_Entry != NULL){
    	return updater_backup_Entry->vstart;
    }else{
	//TODO: once ptable block was ruined by fota erase and power shut, could not recovery
		uart_printf("[fatal_err][%s][%d]\r\n",__func__,__LINE__);
		return -1;
    }	
}



