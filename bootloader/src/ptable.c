#include <stdio.h>
#include <string.h>

#include "ptable.h"

static _ptable *ptable_info = (_ptable *)0xB0020000; //BTCM area

int ptable_init(void)
{
    return (ptable_info->magic != PTABLE_MAGIC);    
}

void ptable_dump(void)
{
    _ptentry *ptn;
    unsigned i;

    uart_printf("============DUMP PARTITION ===========\r\n");
    for (i = 0; i < ptable_info->count; ++i) {
        ptn = &ptable_info->parts[i];
        uart_printf("ptn[%2d]start=%08x size=%08x vsize=%08x vstart=%08x type=%-5s name='%s'\r\n", i,
               ptn->start, ptn->size, ptn->vsize, ptn->vstart, ptn->type, ptn->name);
    }
}

_ptentry *ptable_find_entry(const char *name)
{
    _ptentry *ptn;
    unsigned i;

    for (i = 0; i < ptable_info->count; ++i) {
        ptn = &ptable_info->parts[i];
        if (!strcmp(ptn->name, name)) {
            return ptn;
        }
    }

    return NULL;
}

_ptentry *ptable_find_flash_entry(const char *name)
{
    _ptentry *flash_entry = NULL;
    _ptentry *ptn;
    unsigned i;

    for (i = 0; i < ptable_info->count; ++i) {
        ptn = &ptable_info->parts[i];
        if (!strcmp(ptn->type, PTABLE_TYPE_FLASH)) {
            flash_entry = ptn;
        }
        else if (!strcmp(ptn->name, name)) {
            return flash_entry;
        }
    }

    return flash_entry;
}


unsigned int ptable_is_flash_on_internal(const char *name)
{
	_ptentry *rtn;
	rtn = ptable_find_flash_entry(name);

	if(!strcmp(rtn->name , PTABLE_FLASH_INTERNAL))
		/* for "internal"*/
		return 1;
	else
		/* for "NULL" the case that no "flash" type partition or no matched partion
		 * for "external"*/
		return 0;
}
_ptflash ptable_get_flash_type(const char *name)
{
	_ptentry *rtn;
	rtn = ptable_find_flash_entry(name);

	if(!strcmp(rtn->name , PTABLE_FLASH_INTERNAL))
		/* for "internal"*/
		return pt_flash_internal;
	else
		/* for "NULL" the case that no "flash" type partition or no matched partion
		 * for "external"*/
		return pt_flash_external;
}

