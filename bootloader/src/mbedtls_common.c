/*
 * Copyright (c) 2015-2019, ARM Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <assert.h>
#include <stddef.h>

/* mbed TLS headers */
#include <memory_buffer_alloc.h>
#include <platform.h>

//#include <common/debug.h>
#include <mbedtls_common.h>
#include <mbedtls_config.h>
//#include <plat/common/platform.h>

#if 0
static void cleanup(void)
{
	ERROR("EXIT from BL2\n");
	panic();
}
#endif

/*
 * mbed TLS initialization function
 */
void mbedtls_init(void)
{
	static int ready;
	void *heap_addr;
	size_t heap_size = 0;
	int err;

	if (!ready) {
#if 0
		if (atexit(cleanup))
			panic();

		err = plat_get_mbedtls_heap(&heap_addr, &heap_size);

		/* Ensure heap setup is proper */
		if (err < 0) {
			//ERROR("Mbed TLS failed to get a heap\n");
			//panic();
		}
		assert(heap_size >= TF_MBEDTLS_HEAP_SIZE);

		/* Initialize the mbed TLS heap */
		mbedtls_memory_buffer_alloc_init(heap_addr, heap_size);

#ifdef MBEDTLS_PLATFORM_SNPRINTF_ALT
		mbedtls_platform_set_snprintf(snprintf);
#endif
#endif
		ready = 1;
	}
}

/*
 * The following helper function simply returns the default allocated heap.
 * It can be used by platforms for their plat_get_mbedtls_heap() implementation.
 */
int get_mbedtls_heap_helper(void **heap_addr, size_t *heap_size)
{
	static unsigned char heap[TF_MBEDTLS_HEAP_SIZE];

	assert(heap_addr != NULL);
	assert(heap_size != NULL);

	*heap_addr = heap;
	*heap_size = sizeof(heap);
	return 0;
}
