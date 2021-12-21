//#include <stdbool.h>
//#include <stddef.h>
#include "common.h"

int ta_init();
void *ta_alloc(size_t num);
void *ta_calloc(size_t num, size_t size);
int ta_free(void *ptr);

size_t ta_num_free();
size_t ta_num_used();
size_t ta_num_fresh();
int ta_check();
