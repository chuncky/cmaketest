/*
 * Copyright (c) 2016-2017, ARM Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef FIP_H
#define FIP_H

#include "uuid.h"
#include "types.h"
#include "firmware_image_package.h"

typedef uint32_t (*fip_read_data)(uint32_t from, uint32_t size, uint8_t *to);

typedef struct fip_info {
    uint32_t address;
    fip_read_data read;
    fip_toc_header_t toc;
    fip_toc_entry_t *entry;
    int entry_count;
}fip_info_t;

typedef fip_info_t * fip_handle;

typedef struct fip_image {
    unsigned int id;
    uint32_t start;
    uint32_t offset;
    uint32_t size;
    fip_read_data read;
} fip_image_t;

typedef fip_image_t * fip_image_handle;


typedef struct {
    unsigned int id;
    uuid_t uuid;
} uuid_map_t;


//init the fip ToC from fwcert.bin
fip_handle fip_open(void *address);
//search out the target <image_id> fip_handle
int fip_open_image(unsigned int image_id, fip_image_handle fih);
//raw read and copy <size> of data from the begining of <fih> image
uint32_t fip_read_image(fip_image_handle fih, uint8_t *data, uint32_t size);

typedef enum error {
	FIP_SUCCEED,
	FIP_ERR_UUID_MISS,
	FIP_ERR_NO_IMAGE,
	FIP_ERR_EMPTY,
	FIP_ERR_IMG_MISS,
};


#if 0
#define NELEM(x) (sizeof (x) / sizeof *(x))

enum {
	DO_UNSPEC = 0,
	DO_PACK   = 1,
	DO_UNPACK = 2,
	DO_REMOVE = 3
};

enum {
	LOG_DBG,
	LOG_WARN,
	LOG_ERR
};

typedef struct image_desc {
	uuid_t             uuid;
	char              *name;
	char              *cmdline_name;
	int                action;
	char              *action_arg;
	struct image      *image;
	struct image_desc *next;
} image_desc_t;

typedef struct image {
	struct fip_toc_entry toc_e;
	void                *buffer;
} image_t;

typedef struct cmd {
	char              *name;
	int              (*handler)(int, char **);
	void             (*usage)(void);
} cmd_t;
#endif
#endif /* FIP_H */
