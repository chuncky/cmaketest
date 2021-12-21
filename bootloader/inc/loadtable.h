#ifndef _LOADTABLE_H_
#define _LOADTABLE_H_
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

#include "common.h"

#define BINARY_LOAD_TABLE_OFFSET 0x04 //the size of region DDR_RO_INIT
#define FLASH_BASE_ADDR 0x80000000
#define INVALID_ADDRESS           0x4c4c554E //NULL
#define EMPTY_ADDRESS           0x00000000 //NULL
//++++++++++++++++++++++++++++++++++++++++++++++++++
//AREA_1: LOADTABLE_HEADER

#define LOADTABLE_HEADER_SIZE 28
typedef union{
	struct loadtable_init_routine{
		unsigned int b2init;                         /* branch to init routine */
		unsigned int init;                           /* image init routine */
	}init_routine;

	unsigned char filer[LOADTABLE_HEADER_SIZE];            /* max size*/
}LOADTABLE_AREA_HEADER;

//++++++++++++++++++++++++++++++++++++++++++++++++++
//AREA_2: VERSION & SIGNATURE
typedef enum {
    XIP,
    PSRAM
}CP_EXECUTE_MODE;

typedef enum {
    NBIOT,
    CATM,
    LTEONLY,
    LTEGSM
}PS_MODE;


#define LOADTABLE_VER_AREA_SIZE 96
typedef union{
	struct loadtable_version_info{
		char anti_rollback_version[16];    /* anti-roll back version , co-work with boot33 */
		char execute_mode[8];              /* XIP or PSRAM*/
		char ps_mode[8];                   /* LTEONLY/LG/CAT1/NBIOT*/
		char image_info[50];              /* filled by external script ,default IMG_INFO as index*/
	}version_info;

	unsigned char filer[LOADTABLE_VER_AREA_SIZE];     /* max size*/
}LOADTABLE_AREA_VER_INFO;

//++++++++++++++++++++++++++++++++++++++++++++++++++
//AREA_3: RW COMPRESS REGION SYMBOL
#define RW_REGION_CPZ_NUM  5
#define RW_REGION_MARK_PRE_STRING "RW_CPZ_"
#define RW_REGION_CPZ_OFFSET  0xE8
#define RW_REGION_COMPRESSED_ADDR_NONE  0x00000000

typedef struct
{
	char    RW_REGION_MARK[7];
	char    RW_REGION_MARK_NUM;
	char    RW_REGION_NAME[8];
	unsigned int  RW_REGION_EXEC_ADDR;
	unsigned int  RW_REGION_LOAD_ADDR;
	unsigned int  RW_REGION_LENGTH;
	unsigned int  RW_REGION_COMPRESSED_ADDR;
}rw_region_item;

#define LOADTABLE_RW_CPZ_SIZE 224
typedef union{
	rw_region_item compress_rw_region_list[RW_REGION_CPZ_NUM];

	unsigned char filer[LOADTABLE_RW_CPZ_SIZE];       /* max size*/
}LOADTABLE_AREA_RW_CPZ_INFO;

//++++++++++++++++++++++++++++++++++++++++++++++++++
//AREA_4: ARMLINKE SYMBOL LIST
typedef struct{
	char	name[12];
	unsigned int	value;
}armlink_symbol_item;

typedef struct {
		//cp.bin
		armlink_symbol_item cp_exec_addr;
		armlink_symbol_item cp_load_addr;
		armlink_symbol_item image_end;
		armlink_symbol_item binary_size;
		//dsp.bin
		armlink_symbol_item dsp_begin_addr;
		armlink_symbol_item dsp_end_addr;
		//rf.bin
		armlink_symbol_item rf_z2_load_addr;
		armlink_symbol_item rf_a0_load_addr;
		//rd.bin
		armlink_symbol_item rd_begin_addr;
		armlink_symbol_item rd_end_addr;
		//apn.bin
		armlink_symbol_item apn_begin_addr;
		armlink_symbol_item apn_end_addr;
		//fota_param
		armlink_symbol_item fota_param_start_address;
		armlink_symbol_item fota_param_end_address;
		//updater
		armlink_symbol_item updater_start_address;
		armlink_symbol_item updater_end_address;
		//fota_pkg
		armlink_symbol_item fota_pkg_start_address;
		armlink_symbol_item fota_pkg_end_address;
		//nvm
		armlink_symbol_item nvm_begin_addr;
		armlink_symbol_item nvm_end_addr;
		//factory_a
		armlink_symbol_item factory_a_begin_addr;
		armlink_symbol_item factory_a_end_addr;
		//factory_b
		armlink_symbol_item factory_b_begin_addr;
		armlink_symbol_item factory_b_end_addr;
		//mmipool
		armlink_symbol_item mmipool_start_address;
		armlink_symbol_item mmipool_size_address;	
		//logo binary length
		armlink_symbol_item logo_img_length;	
}loadtable_armlink_symbol_info;

#define LOADTABLE_ARMLINK_SYMBOL_SIZE 512
typedef union{
    loadtable_armlink_symbol_info armlink_symbol_info;

	unsigned char filer[LOADTABLE_ARMLINK_SYMBOL_SIZE];       /* max size*/
}LOADTABLE_AREA_ARMLINK_SYMBOL;

//++++++++++++++++++++++++++++++++++++++++++++++++++
//AREA_5: FUNCTIONAL VAL
typedef struct{
	char	name[12];
	unsigned int	value;
}functional_item;

typedef struct {
	functional_item vergin;                    /* vergin */
	functional_item number_of_life;            /* number_of_life++ for each bootup,design for SIMPIN lock detect */
	functional_item uart_printf_enable;        /* enable uart printf after CP init*/
	functional_item fatal_printf_enable;       /* enable fatal printf after CP init */
	functional_item default_core_freq;         /* TODO:further porting */
	functional_item default_core_voltage;      /* TODO:further porting*/
}loadtable_functional_info;

#define LOADTABLE_FUNC_VAL_SIZE 256
typedef union{
    loadtable_functional_info func_val;

	unsigned char filer[LOADTABLE_FUNC_VAL_SIZE];     /* max size*/
}LOADTABLE_AREA_FUNC_VAL;

//++++++++++++++++++++++++++++++++++++++++++++++++++
//AREA_6: ZI_INFO for LZMA
#define LOADTABLE_AREA_ZI_INFO_SIZE 32
typedef struct{
	char	name[12];
	UINT32	value;
}zi_info_item;

typedef union{
	struct loadtable_zi_info{
		zi_info_item DDR_ZI_Base;
		zi_info_item DDR_ZI_Limit;
	}zi_info;

	UINT8 filer[LOADTABLE_AREA_ZI_INFO_SIZE];     /* max size*/
}LOADTABLE_AREA_ZI_INFO;
//++++++++++++++++++++++++++++++++++++++++++++++++++
//AREA_7: Temp info for lzma
#define LOADTABLE_AREA_TEMP_INFO_SIZE 80
typedef struct{
	char	name[12];
	UINT32	value;
}temp_info_item;
typedef union{
	struct loadtable_temp_info{
		temp_info_item DSP_BACKUP_SIZE;
		temp_info_item PTABLE_EXEC_START_ADDR;
		temp_info_item PTABLE_EXEC_END_ADDR;
		temp_info_item HEAP_GUARD_ADDR;
		temp_info_item HEAP_ENDMARK_ADDR;
	}temp_info;

	UINT8 filer[LOADTABLE_AREA_TEMP_INFO_SIZE];     /* max size*/
}LOADTABLE_AREA_TEMP_INFO;

typedef struct
{
	LOADTABLE_AREA_HEADER	      lt_area_header;
	LOADTABLE_AREA_VER_INFO       lt_area_ver_info;
	LOADTABLE_AREA_RW_CPZ_INFO    lt_area_rw_cpz_info;
	LOADTABLE_AREA_ARMLINK_SYMBOL lt_area_armlink_symbol;
	LOADTABLE_AREA_FUNC_VAL       lt_area_func_val;
	LOADTABLE_AREA_ZI_INFO        lt_area_zi_info;
	LOADTABLE_AREA_TEMP_INFO      lt_area_temp_info;
	/* TODO: explore the vendor area for customer's usage
	LOADTABLE_AREA_vendor         lt_area_vendor;
	*/
}LoadTableType;

void loadtable_init(unsigned int cp_flash_addr);
unsigned int get_cp_load_addr(void);
unsigned int get_cp_exec_addr(void);
unsigned int get_cp_binary_size(void);
unsigned int get_cp_copy_size(void);
unsigned int get_dsp_load_addr(void);
unsigned int get_dsp_copy_size(void);
//unsigned int get_rf_load_addr(void);
unsigned int get_rw_cpz_struct_addr(void);
unsigned int get_fota_param_start_addr(void);
unsigned int get_fota_param_vstart_addr(void);
unsigned int is_fota_param_available(void);
unsigned int get_fota_pkg_start_addr(void);
unsigned int get_updater_start_addr(void);
unsigned int get_updater_copy_size(void);
void set_rd_begin_addr(unsigned int begin_addr);
void set_apn_begin_addr(unsigned int begin_addr);
void set_nvm_begin_addr(unsigned int begin_addr);
void set_md_end_addr(unsigned int end_addr);
void set_apn_end_addr(unsigned int end_addr);
void set_nvm_end_addr(unsigned int end_addr);
unsigned int get_ps_mode(void);
unsigned int get_rf_load_size(void);

void update_nvm_begin_addr(unsigned int available_base_addr);
void dump_loadtable(void);
int anti_rollback_check(void);
CP_EXECUTE_MODE get_cp_execute_mode(void);
void number_of_life_update(void);
int vergin_status_fetch(void);

UINT32 get_rw_zi_start_addr(void);
UINT32 get_rw_zi_end_addr( void );

UINT32 get_rw_zi_length(void);

UINT32 get_dsp_backup_size(void);
UINT32 get_ptable_exec_start_addr(void);
UINT32 get_ptable_exec_end_addr(void);
UINT32 get_heap_guard_addr(void);

UINT32 get_heap_endmark_addr(void);
UINT32 get_heap_length( void );


UINT32 get_logo_length_in_cp(void);
BOOL IsUpdaterbakExist(void);
unsigned int get_updater_backup_start_addr(void);



typedef enum{
    CRANE_Z2 = 0xF16731,
    CRANE_A0 = 0xA06731
}CRANE_CHIP_TYPE;

BOOL is_partition_internal_flash(const char *name);

//unsigned int getLongChipId(void);

#ifdef __cplusplus
}
#endif
#endif // _LOADTABLE_H_
