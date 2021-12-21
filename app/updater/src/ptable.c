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
        uart_printf("ptn[%2d]start=%08x size=%08x vsize=%08x vstart=%08x type=%-5s name='%s'\n", i,
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
