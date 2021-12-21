#ifndef _ADUPS_MAIN_H
#define _ADUPS_MAIN_H

#define __MULTI_PACKAGE__


typedef struct multi_head_info{
	adups_uint32 multi_bin_offset;
	adups_uint32 multi_bin_address;
	adups_uint8 multi_bin_method;
	adups_uint8 multi_bin_resever[7];
}multi_head_info_patch;



//Error code

#define ADUPS_FUDIFFNET_ERROR_NONE                                               (0)
#define ADUPS_FUDIFFNET_ERROR_INSUFFICIENT_WORKING_BUF                           (-100)

//Updating related
#define ADUPS_FUDIFFNET_ERROR_UPDATE_ERROR_START                                 (-600)
#define ADUPS_FUDIFFNET_ERROR_UPDATE_ERROR_END                                   (-699)


adups_extern void AUDPSProcedure(void);
adups_extern adups_int32 adups_bl_write_block(adups_uint8* src, adups_uint32 start, adups_uint32 size);
adups_extern adups_int32 adups_bl_read_block(adups_uint8* dest, adups_uint32 start, adups_uint32 size);
adups_extern void adups_bl_progress(adups_int32 percent);
//adups_extern void adups_bl_debug_print(void* ctx, const adups_char* fmt, ...);
adups_extern adups_uint8* adups_bl_get_working_buffer(void);
adups_extern adups_uint64 adups_bl_get_working_buffer_len(void);
adups_extern adups_uint32 adups_bl_get_app_base(void);
adups_extern adups_uint32 adups_bl_get_diff_param_size(void);
adups_extern adups_uint32 adups_bl_get_delta_base(void);


#endif

