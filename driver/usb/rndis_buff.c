/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                     RNDIS MODULE

  Copyright (c) 2011 - 2015 by MARVELL Incorporated.
  All Rights Reserved.
  MARVELL Confidential and Proprietary.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.


when       who          what, where, why
--------   -------      ----------------------------------------------------------
07/19/11   zhoujin      Initial version.

===========================================================================*/
#include "common.h"

#include "rndis.h"
//#include "ECM.h"
#include "platform.h"
#include "utilities.h"
#include "mvUsbDevPrv.h"
#include "mvUsbDevApi.h"
#include "mvUsbMemory.h"

#define MALLOC malloc
#define ETH_PAD_SIZE                   0   /*2*/
#define SIZEOF_ETH_HDR                 (14 + ETH_PAD_SIZE)
#define ETH_HEADER_LEN                 (14 + ETH_PAD_SIZE)
#define RNDIS_HEADER_LEN               44 
#define ECM_HEADER_LEN                 0
#define MBIM_HEADER_LEN                64 
#define PPP_HEADER_LEN                 0 
#define WIFI_HEADER_LEN                108
#define EXTEND_HEADER_LEN              128 /*max set, can adapter to rndis,wifi hedaer*/

#define PBUF_IP_HLEN                   40




BOOL usb_uplink_is_reach_hwm(void)
{
	
	return 1;
}


/*free func match malloc, alloc, MALLOC*/
static inline void pmsg_free_fun(pmsg *msg)
{
    if (!msg) {return;}
    free(msg->phdr);

}

UINT8 tcpip_ul_non_cpy_flag = 0;

UINT8 lwip_get_ul_non_cpy(void)
{
    return tcpip_ul_non_cpy_flag;
}







static UINT16 lwip_in_buf_offset [12] =
{
    PMSG_SIZE + EXTEND_HEADER_LEN + PBUF_LINK_HLEN + PBUF_IP_HLEN,  //case PBUF_TRANSPORT:
    PMSG_SIZE + EXTEND_HEADER_LEN + PBUF_LINK_HLEN,                 //case PBUF_IP:
    PMSG_SIZE + EXTEND_HEADER_LEN,                                  //case PBUF_LINK:
    PMSG_SIZE + EXTEND_HEADER_LEN,                                  //case PBUF_RAW:
    PMSG_SIZE + EXTEND_HEADER_LEN - RNDIS_HEADER_LEN,               //case PBUF_RNDIS:
    PMSG_SIZE + EXTEND_HEADER_LEN - ECM_HEADER_LEN,                 //case PBUF_ECM:
    PMSG_SIZE + EXTEND_HEADER_LEN - WIFI_HEADER_LEN,                //case PBUF_WIFI:
    PMSG_SIZE + EXTEND_HEADER_LEN - MBIM_HEADER_LEN,                //case PBUF_MBIM:
    PMSG_SIZE + EXTEND_HEADER_LEN - PPP_HEADER_LEN,                 //case PBUF_PPP:
    PMSG_SIZE,                                                      //case PBUF_EXTEND:
    0,                                                              //case PBUF_PMSG:
    0                                                               //end
};








void* lwip_in_buf_alloc(size_t size,
                                pbuf_type type,
                                pbuf_layer layer,
                                alloc_fn alloc_in,
                                free_fn free_in)
{
  pmsg* pmsg_hdr = NULL;
  void* ret_mem  = NULL;
  void* buf_hdr  = NULL;
  alloc_fn alloc_cb = NULL;
  free_fn free_cb = NULL;
  UINT16 offset = 0;
  UINT16 alloc_size = 0;

  /*pre check*/
  /*for ringbuffer, mabe no need free*/
  if ((PBUF_REF == type)
    && (alloc_in == NULL)) {
    return NULL;
  }

  switch (type) {
  case PBUF_RAM:
    alloc_cb = MALLOC;
    free_cb  = pmsg_free_fun;
    break;
#if 0
  case PBUF_POOL:
    alloc_cb = mem_malloc;
    free_cb  = pmsg_mem_free;
    break;
#endif
  case PBUF_REF:
    alloc_cb = alloc_in;
    free_cb  = free_in;
    break;
  case PBUF_MEM:
    //alloc_cb = lte_ul_buf_alloc2;
    //free_cb = lte_ul_buf_free;
    break;
  default:
    return NULL;
  }

  /* determine header offset */
  offset = lwip_in_buf_offset[layer];

  alloc_size = size + offset;
  buf_hdr = alloc_cb(alloc_size);

  if (buf_hdr) {
      ret_mem = (void *)((u8_t *)buf_hdr + offset);
      pmsg_hdr = (pmsg *)((u8_t *)ret_mem - PMSG_SIZE);

      pmsg_hdr->phdr = buf_hdr;
      pmsg_hdr->free = free_cb;
      pmsg_hdr->plen = alloc_size;
      pmsg_hdr->rnum = 1;
      pmsg_hdr->type = type;
      pmsg_hdr->layer = layer;
#if 0
      pmsg_hdr->pmsgpos  = 0;
      pmsg_hdr->pmsgtick = OSAGetTicks();
#endif

#if 0
      LWIP_DIAG(MEM_DEBUG, lwip_pbuf_1800, "lwipmem: lwip_in_buf_alloc, ret_mem=%lx,pmsg=%lx,free=%lx,alloc=%lx,type=%d,layer=%d,alloc_size=%d",
                                                                      ret_mem, pmsg_hdr, free_cb, alloc_cb, type, layer, alloc_size);
#endif
  }

  return ret_mem;
}

void lwip_in_buf_free(void* buf, pbuf_layer layer)
{
  pmsg* msg = NULL;

  if (NULL == buf) {
    return;
  }

  if (layer == PBUF_PMSG) {
    msg = (pmsg*)buf;
  } else {
    msg = (pmsg*)((u8_t *)buf - PMSG_SIZE);
  }

  if (msg->free) {
    msg->free(msg);
  }

  return;
}











