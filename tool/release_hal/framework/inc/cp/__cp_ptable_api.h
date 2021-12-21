#ifndef __CP_PTABLE_API_H__
#define __CP_PTABLE_API_H__

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_PTABLE_ENTRY_NAME   32
#define MAX_PTABLE_ENTRY_TYPE   16

#define PTABLE_TYPE_RAW         "raw"
#define PTABLE_TYPE_UBI         "ubi"
#define PTABLE_TYPE_PART        "part"
#define PTABLE_TYPE_FLASH       "flash"
#define PTABLE_TYPE_GROUP       "group"

#define PTABLE_FLASH_EXTERNAL   "external"
#define PTABLE_FLASH_INTERNAL   "internal"

typedef enum ptable_flash_type {
	pt_flash_external,
	pt_flash_internal,
}_ptflash;

/*
 * @name: partition name, e.g. bootloader, radio
 * @type: partition type, e.g. ubi or raw
 * @start: partition start address within flash
 * @size: partition size in bytes
 * @vstart: partition image load address at runtime
 * @vsize: partition image real size in bytes
 * @depth: partition nested depth
 */
typedef struct ptentry {
    char name[MAX_PTABLE_ENTRY_NAME];
    char type[MAX_PTABLE_ENTRY_TYPE];
    unsigned start;
    unsigned size;
    unsigned vstart;
    unsigned vsize;
    unsigned depth;
}_ptentry;

/* tools to populate and query the partition table */
_ptentry *ptable_find_entry(const char *name);
_ptflash ptable_get_flash_type(const char *name);


#ifdef __cplusplus
}
#endif

#endif /* __CP_PTABLE_API_H__ */
