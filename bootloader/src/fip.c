//#include <assert.h>
#include "fip.h"
//#include <fip_flash.h>
//#include "log.h"
#include <stdlib.h>
#include <string.h>
#include "tbbr_img_def.h"
#include "utils_def.h"

#define MAX_TOC_ENTRY_COUNT 12
fip_toc_entry_t m_fip_toc_entry_list[MAX_TOC_ENTRY_COUNT];
fip_info_t m_fip;

/* By default, ARM platforms load images from the FIP */
static const uuid_map_t image_uuid[] = {
#if 0
    { BL2_IMAGE_ID, UUID_TRUSTED_BOOT_FIRMWARE_BL2 },
    { SCP_BL2_IMAGE_ID, UUID_SCP_FIRMWARE_SCP_BL2 },
    { BL31_IMAGE_ID, UUID_EL3_RUNTIME_FIRMWARE_BL31 },
    { BL32_IMAGE_ID, UUID_SECURE_PAYLOAD_BL32 },
    { BL33_IMAGE_ID, UUID_NON_TRUSTED_FIRMWARE_BL33 },
    { TRUSTED_BOOT_FW_CERT_ID, UUID_TRUSTED_BOOT_FW_CERT },
    { TRUSTED_KEY_CERT_ID, UUID_TRUSTED_KEY_CERT },
    { SCP_FW_KEY_CERT_ID, UUID_SCP_FW_KEY_CERT },
    { SOC_FW_KEY_CERT_ID, UUID_SOC_FW_KEY_CERT },
    { TRUSTED_OS_FW_KEY_CERT_ID, UUID_TRUSTED_OS_FW_KEY_CERT },
    { NON_TRUSTED_FW_KEY_CERT_ID, UUID_NON_TRUSTED_FW_KEY_CERT },
    { SCP_FW_CONTENT_CERT_ID, UUID_SCP_FW_CONTENT_CERT },
    { SOC_FW_CONTENT_CERT_ID, UUID_SOC_FW_CONTENT_CERT },
    { TRUSTED_OS_FW_CONTENT_CERT_ID, UUID_TRUSTED_OS_FW_CONTENT_CERT },
    { NON_TRUSTED_FW_CONTENT_CERT_ID, UUID_NON_TRUSTED_FW_CONTENT_CERT },
    { BL32_EXTRA1_IMAGE_ID, UUID_SECURE_PAYLOAD_BL32_EXTRA1 },
    { BL32_EXTRA2_IMAGE_ID, UUID_SECURE_PAYLOAD_BL32_EXTRA2 },
#endif
    { ASR_OS_CP_IMAGE_ID, UUID_NON_TRUSTED_OS_CP },
    { ASR_OS_CP_KEY_CERT_ID, UUID_NON_TRUSTED_CP_CONTENT_CERT },
    { ASR_OS_DSP_IMAGE_ID, UUID_NON_TRUSTED_OS_DSP },
    { ASR_OS_DSP_KEY_CERT_ID, UUID_NON_TRUSTED_DSP_CONTENT_CERT },
    { ASR_OS_APP_IMAGE_ID, UUID_NON_TRUSTED_OS_APP },
    { ASR_OS_APP_KEY_CERT_ID, UUID_NON_TRUSTED_APP_CONTENT_CERT },
    { ASR_OS_USER1_IMAGE_ID, UUID_NON_TRUSTED_OS_USER1 },
    { ASR_OS_USER1_KEY_CERT_ID, UUID_NON_TRUSTED_USER1_CONTENT_CERT },
    { ASR_OS_USER2_IMAGE_ID, UUID_NON_TRUSTED_OS_USER2 },
    { ASR_OS_USER2_KEY_CERT_ID, UUID_NON_TRUSTED_USER2_CONTENT_CERT },
    { ASR_OS_USER3_IMAGE_ID, UUID_NON_TRUSTED_OS_USER3 },
    { ASR_OS_USER3_KEY_CERT_ID, UUID_NON_TRUSTED_USER3_CONTENT_CERT },
};

static const uuid_t uuid_null = {{0}, {0}, {0}, 0, 0, {0, 0, 0, 0, 0, 0}};

extern int uart_printf(const char *fmt, ...);

static inline int compare_uuids(const uuid_t *uuid1, const uuid_t *uuid2)
{
    return memcmp(uuid1, uuid2, sizeof(uuid_t));
}

static uint32_t read_data_from_memory(uint32_t from, uint32_t size, uint8_t *to)
{
    uint8_t *start = (uint8_t *)from;

    if (start + size < to || to + size < start) {
        memcpy(to, start, size);
    }
    else if (to != start) {
        memmove(to, start, size);
    }
    return (uint32_t)size;
}

/*
 * See if a Firmware Image Package is available,
 * by checking if TOC is valid or not.
 */
#if 0
int is_valid_fip(void *address)
{
    fip_toc_header_t header;

    read_data_from_memory((uint32_t)(uintptr_t)address, sizeof(header), (uint8_t *)&header);

    return header.name == TOC_HEADER_NAME;
}

int fip_get_plat_flag(void *address, uint16_t *flag)
{
    fip_toc_header_t header;

    read_data_from_memory((uint32_t)(uintptr_t)address, sizeof(header), (uint8_t *)&header);

    if (header.name == TOC_HEADER_NAME) {
        *flag = (header.flags >> 32) & 0xFFFF;
        return 0;
    }
    else {
        uart_printf("[SECBOOT]not a valid FIP");
        return -1;
    }
}
#endif

//==============================================
//DESC:
//	initial the static m_fip ,for FIP parse ToC struct
//
//PARAM:
//	address	- the address of fwcert.bin
//
//RETURN:
// fip_handle
//==============================================

fip_handle fip_open(void *address)
{
#if 0
    fip_handle fh = calloc(sizeof(*fh), 1);

    if (!fh) {
        uart_printf("[SECBOOT]error!out of memory!\n");
        return NULL;
    }
#endif

    fip_handle fh = &m_fip;

    fh->read = read_data_from_memory;
    fh->address = (uint32_t)address;

    if (fh->read(fh->address, sizeof(fh->toc), (uint8_t *)&fh->toc) != sizeof(fh->toc)) {
        uart_printf("[SECBOOT]error!read fail!\n");
        //free(fh);
        return NULL;
    }

#if 0
	uart_printf("[SECBOOT]fh->address          = [0x%0.8x]\r\n",fh->address);
	uart_printf("[SECBOOT]fh->toc.name         = [0x%0.8x]\r\n",fh->toc.name);
	uart_printf("[SECBOOT]fh->toc.serial_number= [0x%0.8x]\r\n",fh->toc.serial_number);
	uart_printf("[SECBOOT]fh->toc.flags        = [0x%0.8x]\r\n",fh->toc.flags);
#endif

    if (fh->toc.name != TOC_HEADER_NAME) {
        uart_printf("[SECBOOT]error!not a fip!\n");
        //free(fh);
        return NULL;
    }

    fip_toc_entry_t entry;
    uint32_t entry_address = fh->address + sizeof(fh->toc);

//      PARSER THE FIP STRUCT
//
//      ----------------- 
//      | ToC Header     |
//      |----------------|
//      | ToC Entry 0    |
//      |----------------|
//      | ToC Entry 1    |
//      |----------------|
//      | ToC End Marker |
//      |----------------|
//      | Data 0         |
//      |----------------|
//      | Data 1         |
//      |----------------|
//
 
	do {
        if (fh->read(entry_address, sizeof(entry), (uint8_t *)&entry) != sizeof(entry)) {
            uart_printf("[SECBOOT]error!read fail!\n");
            //free(fh);
            return NULL;
        }

        entry_address += sizeof(entry);
		if(!compare_uuids(&entry.uuid, &uuid_null) || fh->entry_count >= MAX_TOC_ENTRY_COUNT)
			break;

#if 0
	uart_printf("[SECBOOT]>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\r\n");
	uart_printf("[SECBOOT]entry.num              = [0x%0.8x]\r\n",fh->entry_count);
	uart_printf("[SECBOOT]entry.start            = [0x%0.8x]\r\n",(uint32_t)fh->address);
	uart_printf("[SECBOOT]entry.offset_address   = [0x%0.8x]\r\n",(uint32_t)entry.offset_address);
	uart_printf("[SECBOOT]entry.size             = [0x%0.8x]\r\n",(uint32_t)entry.size);
#endif


        fh->entry_count++;
    } while (1);

	uart_printf("[SECBOOT]fh->entry_count = [%d]\r\n",fh->entry_count);

#if 0
    fh->entry = malloc(fh->entry_count * sizeof(*fh->entry));
    if (!fh->entry) {
        uart_printf("[SECBOOT]error!out of memory\n");
        //free(fh);
        return NULL;
    }
#endif

	fh->entry = m_fip_toc_entry_list;

    entry_address = fh->address + sizeof(fh->toc);
    if (fh->read(entry_address, fh->entry_count * sizeof(entry), (uint8_t *)fh->entry) != fh->entry_count * sizeof(entry)) {
        uart_printf("[SECBOOT]error!read fail!\n");
        //free(fh->entry);
        //free(fh);
        return NULL;
    }

    return fh;
}

#if 0
int fip_close(fip_handle fh)
{
    if (fh) {
        //free(fh->entry);
        //free(fh);
    }
    return 0;
}
#endif

static int find_uuid_by_id(unsigned int image_id)
{
	int i;
    for (i = 0; i < (int)ARRAY_SIZE(image_uuid); i++) {
        if (image_uuid[i].id == image_id) {
            return i;
        }
    }
    uart_printf("[SECBOOT]can't find uuid by id %u\n", image_id);
    return -1;
}

static int fip_get_image_info(fip_handle fh, unsigned int image_id, uint32_t *pstart, uint32_t *poffset, uint32_t *psize)
{
    if (!fh) {
        uart_printf("[SECBOOT]error!fip handle is empty\r\n");
        return FIP_ERR_EMPTY;
    }
    if (!fh->entry_count) {
        uart_printf("[SECBOOT]fip has no any images!\n");
        return FIP_ERR_NO_IMAGE;
    }

    int index = find_uuid_by_id(image_id);
	//uart_printf("[SECBOOT]queue index = [%d]\r\n", index);

    if (index < 0) {
        return FIP_ERR_UUID_MISS;
    }
    int entry_index = -1;
	int i;
    for (i = 0; i < fh->entry_count; i++) {
        if (!compare_uuids(&fh->entry[i].uuid, &image_uuid[index].uuid)) {
            entry_index = i;
			uart_printf("[SECBOOT]bingo entry_index = [%d]\r\n", entry_index);
            break;
        }
    }
    if (entry_index < 0) {
        uart_printf("[SECBOOT]fip doesn't contain image(id=%u)\n", image_id);
        return FIP_ERR_IMG_MISS;
    }

    if (pstart) {
        *pstart = fh->address;
//uart_printf("[SECBOOT]bingo *pstart   = [%x]\r\n", *pstart);
    }
    if (poffset) {
        *poffset = fh->entry[entry_index].offset_address;
//uart_printf("[SECBOOT]bingo *poffset  = [%x]\r\n", *poffset);
    }
    if (psize) {
        *psize = fh->entry[entry_index].size;
//uart_printf("[SECBOOT]bingo *psize    = [%x]\r\n", *psize);
    }
    return FIP_SUCCEED;
}



int fip_open_image(unsigned int image_id, fip_image_handle fih)
{
	int ret = -1;
#if 0
    fip_image_t *fih = malloc(sizeof(*fih));

    if (!fih) {
        uart_printf("[SECBOOT]error!out of memory!\n");
        return NULL;
    }
#endif

	fip_handle fh = &m_fip;

#if 0
	fip_image_t t_fip_image;
	fip_image_t *fih = &t_fip_image;
#endif

	ret = fip_get_image_info(fh, image_id, &fih->start, &fih->offset, &fih->size);

    if (ret != FIP_ERR_IMG_MISS ){
		if(ret == FIP_SUCCEED){
			fih->id   = image_id;
			fih->read = fh->read;
		}else{
			uart_printf("[SECBOOT]fip_open_image error ret=[%d]\r\n",ret);
		}
	}

    return ret;
}

#if 0
int fip_close_image(fip_image_handle fih)
{
    //free(fih);
    return 0;
}
#endif

uint32_t fip_read_image(fip_image_handle fih, uint8_t *data, uint32_t size)
{
    if (!fih) {
        uart_printf("[SECBOOT]error!can't read empty fip img\n");
        return 0;
    }
    if (!data) {
        uart_printf("[SECBOOT]error!can't store to empty buffer\n");
        return 0;
    }
    uint32_t real_size = MIN(size, fih->size);
    if (!real_size) {
        return 0;
    }
    uint32_t ret = fih->read(fih->start + fih->offset, real_size, data);
    if (ret < 0) {
        uart_printf("[SECBOOT]error!read fail!\n");
    }
    return ret;
}
