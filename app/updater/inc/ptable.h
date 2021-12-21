#ifndef __PTABLE_H__
#define __PTABLE_H__

#include <stddef.h>

//#define AVOID_PTABLE //independence from ptable for psrm or T32 load build

#define CP_BASE_ADDR (0x80040000)
#define PTABLE_ALIGN (1<<12) //4K
#define ADDR_ALIGN(addr) (((addr)+(PTABLE_ALIGN)-1) & ~((PTABLE_ALIGN)-1))

#define MAX_PTABLE_ENTRY_NAME   32
#define MAX_PTABLE_ENTRY_TYPE   16
#define MAX_PTABLE_PARTS        32
#define PTABLE_MAGIC            0x41505442
#define QSPI_FLASH_BASE         0x80000000

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

/*
 * @magic: ptable magic id
 * @count: partition numbers
 * @parts: info for each partition entry
 */
typedef struct ptable {
    unsigned magic;
    unsigned count;
    struct ptentry parts[MAX_PTABLE_PARTS];
} _ptable;

/* tools to populate and query the partition table */
int ptable_init(void);
_ptentry *ptable_find_entry(const char *name);
void ptable_dump(void);

#endif /* __PTABLE_H__ */
