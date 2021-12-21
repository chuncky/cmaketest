#ifndef B33_SECBOOT
#define B33_SECBOOT

#define SECBOOT_DBG(fmt,...) uart_printf(fmt, ##__VA_ARGS__)
#include "tbbr_img_def.h"
#include "tbbr_oid.h"
#include "auth_common.h"

typedef enum {
	ITEM_CP,
	ITEM_DSP,
	ITEM_APP,
	ITEM_USER1,
	ITEM_USER2,
	ITEM_USER3,
	/*
	ITEM_CP_LOGO,
	ITEM_CP_1,
	ITEM_CP_2,
	*/
	ITEM_END
}sb_issue_item_id_t;

typedef struct{
	sb_issue_item_id_t item_id;
	const char * item_name;
	unsigned int item_cert_id;
	unsigned int item_img_id;
	const char * item_hash_oid;
}sb_issue_item_map_t;

#endif /*B33_SECBOOT*/
