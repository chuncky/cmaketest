#include "common.h"
#include "rndis.h"
//#include "ECM.h"
#include "platform.h"
#include "utilities.h"
#include "mvUsbDevPrv.h"
#include "mvUsbDevApi.h"
#include "mvUsbMemory.h"


int mvUsbNetGetPacket(UINT8 *data, UINT32 len, pmsg *msg)
{
	
	
	
	
	
	
	
	
	
	
	
	
}

int netif_pc_get_status(void)
{
	
	return 0;
	
}


#if 0





#include "err.h"
#include "sys.h"
#include "stats.h"
#include "snmp.h"
#include "sys_arch.h"
#include "etharp.h"
#include "ethip6.h"
#include "ip6_addr.h"
#include "netifapi.h"
#include "dhcpd_api.h"
#include "lwip_stats.h"
#include "dialer_task.h"
#include "lwip_api.h"
#include "netif_td_api.h"

#ifndef MIN_SYS

#if LWIP_NETIF_PC

/**************************************************************************
 *                          Define local macro                            *
 **************************************************************************/


/**************************************************************************
 *                          Define local struct                           *
 **************************************************************************/


/**************************************************************************
 *                       Declare extern function                          *
 **************************************************************************/
err_t tcpip_input_eth_check(u8_t *data, u32_t len, struct netif *inp);
void mvUsbNetSendPacket(u8_t *data, u32_t len);
u16_t usb_get_dtd_nums(void);
int mvUsbGetTxNetQCnt(void);
int lwip_tcpip_unlock(void);
int OpUsbTxDropQCnt(int opType);
int OpUsbTxMaxQCnt(int opType);
int query_usbQCntMax(void);
void try_wakeup_host(void);
UINT8 IsMbimEnabled(void);
void netif_set_rx_error_info(statisticsData *ptr, u32_t len);
void netif_set_rx_info(statisticsData *ptr, u32_t len);
void netif_set_tx_error_info(statisticsData *ptr, u32_t len);
void netif_set_tx_info(statisticsData *ptr, u32_t len);

/**************************************************************************
 *                       Declare static function                          *
 **************************************************************************/
static err_t    pc_low_output(struct netif *netif, struct pbuf *p);

/**************************************************************************
 *                      Declare extern parameter                          *
 **************************************************************************/
extern GlobalParameter_Duster2Dialer gUI2DialerPara;
extern GlobalParameter_Dialer2Duster gDialer2UIPara;
extern u8_t current_PP;
//extern u8_t tcpip_ul_ack_sep_flag;

/**************************************************************************
 *                       Define global parameter                          *
 **************************************************************************/
struct netif netif_pc = {0};

/**************************************************************************
 *                       Define static parameter                          *
 **************************************************************************/
static statisticsData  *st_pc  = &(gDialer2UIPara.USB);
static int gUsbTxMaxQCnt  = 450;
static int gUsbTxDropQCnt = 0;

/**************************************************************************
 *                        Define static inline function                          *
 **************************************************************************/
static err_t pc_netif_init(struct netif *netif)
{
    LWIP_ASSERT_NOW((netif != NULL));

#if LWIP_NETIF_HOSTNAME
    /* Initialize interface hostname */
    netif->hostname = "pc";
#endif /* LWIP_NETIF_HOSTNAME */

    /*
    * Initialize the snmp variables and counters inside the struct netif.
    * The last argument should be replaced with your link speed, in units
    * of bits per second.
    */
    NETIF_INIT_SNMP(netif, snmp_ifType_ethernet_csmacd, 100 * 1024 * 1024);

    netif->name[0] = 'p';
    netif->name[1] = 'c';
    /* We directly use etharp_output() here to save a function call.
    * You can instead declare your own function an call etharp_output()
    * from it if you have to do some checks before sending (e.g. if link
    * is available...) */
    netif->output     = etharp_output;
    netif->linkoutput = pc_low_output;
    netif->output_ip6 = ethip6_output;

    /* initialize the hardware */
    u8_t s1 = sys_gen_rand() & 0xFF;
    u8_t s2 = sys_gen_rand() & 0xFF;
    netif->hwaddr_len = ETHARP_HWADDR_LEN;
    netif->hwaddr[0] = 0x00;
    netif->hwaddr[1] = s1;
    netif->hwaddr[2] = s2;
    netif->hwaddr[3] = s2;
    netif->hwaddr[4] = s2;
    netif->hwaddr[5] = s2;

    netif->mtu = MTU_MAX_SET;
    netif->mtu6 = MTU_MAX_SET;
    netif->if_apin_pkts = 0;
    netif->if_apout_pkts = 0;
    netif->if_in_pkts  = 0;
    netif->if_out_pkts = 0;
    netif->if_in_discards  = 0;
    netif->if_out_discards = 0;
    netif->if_lte_pkts = 0;

    netif_set_ip6_linklocal(netif, 0, htonl(0x1234));

    /* don't set NETIF_FLAG_ETHARP if this device is not an ethernet one */
    netif->flags = NETIF_FLAG_BROADCAST |
                   NETIF_FLAG_ETHARP    |
                   NETIF_FLAG_LINK_UP   |
                   NETIF_FLAG_INTER     |
                   NETIF_FLAG_INIT_DONE |
                   NETIF_FLAG_ETHERNET  |
                   NETIF_FLAG_IF_USB    |
                   NETIF_FLAG_STATIC_MEM;

    return ERR_OK;
}


/**************************************************************************
 *                        Define static function                          *
 **************************************************************************/
 #if IS_LWIP_PLATFORM_1802_1802S
 #pragma arm section code="netifpc"
 #endif
static err_t pc_low_output(struct netif *netif, struct pbuf *p)
{
    struct pbuf *q = NULL;
    pmsg *pmsg_hdr = NULL;
    u8_t *payload  = NULL;
    u8_t *buf  = NULL;
    u16_t len  = 0;
    u16_t size = 0;
    err_t ret  = 0;
    int mvUsbTxNetQCnt = 0;
    int atUsbTxMaxQCnt = 0;

    if (p) {
        size = p->tot_len;
    }

    if(IsMbimEnabled() == 1) {
        LWIP_DIAG(DIAG_ON_DEBUG, lwip_netif_pc_170, "lwiperr: %s, IsMbimEnabled == 1, p=%lx, netif=%lx", __FUNCTION__, p, netif);
        ret = ERR_IF;
        goto pc_low_ret;
    }

    if (NULL == netif || p == NULL)  {
        LWIP_DIAG(DIAG_ON_DEBUG, lwip_netif_pc_171, "lwiperr: %s, netif or pbuf is null, p=%lx, netif=%lx", __FUNCTION__, p, netif);
        ret = ERR_IF;
        goto pc_low_ret;
    }

    if (!netif_is_up(netif)) {
		try_wakeup_host();
		if (!netif_is_up(netif)) {
            LWIP_DIAG(DIAG_ON_DEBUG, lwip_netif_pc_179, "lwiperr: %s, netif is down after retry, pbuf=%lx, flowid=%lx", __FUNCTION__, p, p->flowid);
            ret = ERR_IF;
            goto pc_low_ret;
        }
    }

#if 0
    LWIP_DIAG(IF_USB_DEBUG, lwip_netif_pc_185, "lwip: netif_pc_output,pbuf=%lx,payload=%lx,pkt_mac=%lx,tot_len=%d,len=%d,type=%d,flowid=%lx,ifnum=%d",
                                        p, p->payload, p->pkt_mac, p->tot_len, p->len, p->type, p->flowid, p->ifnum);
#endif

    mvUsbTxNetQCnt = mvUsbGetTxNetQCnt();
    atUsbTxMaxQCnt = OpUsbTxMaxQCnt(1);
    if ((atUsbTxMaxQCnt != 0)
        && (mvUsbTxNetQCnt > atUsbTxMaxQCnt)) {
        OpUsbTxDropQCnt(2);
        LWIP_DIAG(DIAG_ON_DEBUG, lwip_netif_pc_194, "lwiperr: mvUsbTxNetQCnt %d beyond Max %d, pbuf=%lx, flowid=%lx", mvUsbTxNetQCnt, atUsbTxMaxQCnt, p, p->flowid);
        ret = ERR_MEM;
        goto pc_low_ret;
    }

    /* reshape pbuf for pc output */
    p = ethernet_dl_pbuf_reshape(netif, p);
    if (NULL == p) {
        LWIP_DIAG(DIAG_ON_DEBUG, lwip_netif_pc_202, "lwiperr: netif_pc, ethernet_dl_pbuf_reshape failed");
        ret = ERR_ARG;
        goto pc_low_ret;
    }

    /* calc len, need add RNDIS header size */
    len = p->tot_len + RNDIS_HEADER_LEN;

    /* for lte dl data path*/
    /*avoid: PBUF_MEM type pbuf from ipsec */
    /*avoid: PBUF_MEM type pbuf from UL netif_pc turnback, like icmpv6*/
    if (p->type == PBUF_MEM
        && !pbuf_is_for_ipsec(p)) {
        buf = (u8_t *)p->pkt_mac - RNDIS_HEADER_LEN;
        pmsg_hdr = (pmsg *)(buf - PMSG_SIZE);
        pmsg_pmsg_copy(pmsg_hdr, p->pkt_msg);
        p->next    = NULL; /*add protection, refer to urt 80379*/
        p->pkt_msg = NULL; /*set it for outer of lwip process to free pbuf, but not free lte data*/
    } else { /*for ethnet bridge or data from app*/
        buf = (u8_t *)MALLOC(len + PMSG_SIZE);
        if (buf == NULL) {
            ret = ERR_MEM;
            LWIP_DIAG(DIAG_ON_DEBUG, lwip_netif_pc_222, "lwiperr: lwip -> usb buf null, pbuf=%lx, flowid=%lx", p, p->flowid);
            goto pc_low_ret;
        }
		pmsg_hdr = (pmsg *)buf;
        pmsg_pmsg_set(pmsg_hdr, pmsg_free_fun, buf, 0, 0);

        buf += PMSG_SIZE;
        payload = buf + RNDIS_HEADER_LEN;
        for (q = p; q != NULL; q = q->next) {
            MEMCPY(payload, q->payload, q->len);
            payload += q->len;
        }
    }

    if ((current_PP <= 2) || (mvUsbTxNetQCnt >= 16)) {
        lwip_tcpip_unlock();
    }

#if 0
    LWIP_DIAG(IP_TRACE, lwip_netif_pc_245, "lwip: call mvUsbNetSendPacket,pbuf=%lx,buf=%lx,len=%d,flowid=%lx,ifnum=%d,tick=%lu",
                                        p, buf, len, p->flowid, p->ifnum, pbuf_get_crtick_diff(p));
#endif

	lwip_netif_tcpdump(netif, buf + RNDIS_HEADER_LEN, len - RNDIS_HEADER_LEN, 0);

    mvUsbNetSendPacket(buf, len);
    LINK_STATS_INC(link.xmit);

pc_low_ret:
    if (ret != ERR_OK) {
        netif_set_tx_error_info(st_pc, size);
    } else {
        netif_set_tx_info(st_pc, size);
    }
    return ret;
}


/**************************************************************************
 *                        Define global function                          *
 **************************************************************************/
err_t mvUsbNetGetPacket(u8_t *data, u32_t len, pmsg *msg)
{
    struct pbuf *p      = NULL;
    struct netif* netif = &netif_pc;
    int free_flag       = 0;
    err_t ret           = 0;

    if (data == NULL || len == 0) {
        LWIP_DIAG(DIAG_ON_DEBUG, lwip_netif_pc_375, "lwiperr: mvUsbNetGetPacket, param error");
        return ERR_IF;
    }

#if 0
    ret = netif_ready(netif);
    if(ERR_OK != ret) {
        ret = netif_init_done(netif);
        if (ERR_OK == ret) {
            netif_set_up(netif);
        } else {
            LWIP_DIAG(DIAG_ON_DEBUG, lwip_netif_pc_376, "lwiperr: netif_pc is not ready, data=%lx, len=%d.", data, len);
            goto ethernet_in_ret;
        }
    }
#endif

    ret = netif_ready(netif);
    if(ERR_OK != ret) {
        LWIP_DIAG(DIAG_ON_DEBUG, lwip_netif_pc_377, "lwiperr: netif_pc is not ready, data=%lx, len=%d.", data, len);
        goto ethernet_in_ret;
    }

    /*do pre check*/
    ret = tcpip_input_eth_check(data, len, netif);
    if (ERR_OK != ret) {
        LWIP_DIAG(DIAG_ON_DEBUG, lwip_netif_pc_378, "lwiperr: netif_pc, tcpip_input_eth_check failed, data=%lx, len=%d.", data, len);
        goto ethernet_in_ret;
    }

    /*copy setting len to RAM buf*/
    p = ethernet_copy_data_ref(data, len, msg, &free_flag);
    if (p) {
#if 0
        struct eth_hdr *ethhdr = p->payload;
        LWIP_DIAG(DIAG_ON_DEBUG, lwip_netif_pc_379, "lwip: %s,from netif_%s,p=%lx,q=%lx,type=%x,data=%lx,len=%d,msg=%lx,crtick=%lu", 
                                            __FUNCTION__, netif->hostname, p, p->next, ethhdr->type, data, len, msg, p->crtick);
#endif

		lwip_netif_tcpdump(netif, data, len, 0);

        ret = netif->input(p, netif);       
    } else {
        ret = ERR_BUF;
    }

ethernet_in_ret:

    if (ERR_OK == ret) {
        netif_set_rx_info(st_pc, len);
    } else {
        netif_set_rx_error_info(st_pc, len);
    }

    /*change ret, to denote caller no need free mem, again*/
    if (free_flag) {
        ret = ERR_OK;
    }

    return ret;
}
#if IS_LWIP_PLATFORM_1802_1802S
#pragma arm section code
#endif


void netifapi_pc_rndis_setup(void)
{
    struct ip_addr ip, mask, gw;

    if (IS_LWIP_MIFI_MODE) {
        ip.addr = htonl(g_server_ip);
    } else {
        ip.addr = IP_169_254_ADDR;
    }

    IP4_ADDR(&mask, 255, 255, 255, 0);
    IP4_ADDR(&gw, 0, 0, 0, 0);

    netifapi_netif_add(&netif_pc, &ip, &mask, &gw, NULL, pc_netif_init, ethernet_netif_input);
    netifapi_netif_set_down(&netif_pc);
}

/*0: ok; other: failed.*/
int netifapi_set_pc_macaddr(u8_t *mac_addr)
{
    struct netif* inp;
    int ret;

    inp = netif_get_usb_if();
    if (NULL == inp) {
        return -1;
    }

    ret = (int)netifapi_netif_set_mac(inp, (char *)mac_addr);
    return ret;
}

int OpUsbTxMaxQCnt(int opType)
{
    /*
    Define:
    opType = 0: reset and disable
    opType = 1: query current maxQcnt
    opType = 2: query current usb drop cnt
    opType = 3: query the max usb pending pkt Cnt occured
    opType = other, set the value
    */
    int ret = 0;

    switch(opType)
    {
        case 0:
            gUsbTxMaxQCnt = 0;
            gUsbTxDropQCnt = 0;
            break;

        case 1:
            ret = gUsbTxMaxQCnt;
            break;

        case 2:
            ret = gUsbTxDropQCnt;
            break;

        case 3:
            ret = query_usbQCntMax();
            break;

        default: //for othervalue, set gUsbTxMaxQCnt
            gUsbTxMaxQCnt = opType;
            break;
    }

    return ret;

}

int OpUsbTxDropQCnt(int opType)
{
    /*
    Define:
    opType = 0: reset
    opType = 1: query
    opType = 2, add the value
    */
    int ret = 0;

    switch(opType)
    {
        case 0: //reset
            gUsbTxDropQCnt = 0;
            break;

        case 1: //query
            ret = gUsbTxDropQCnt;
            break;

        case 2: //add gUsbTxDropQCnt
            gUsbTxDropQCnt++;
            break;

       default:
            break;
    }

    return ret;
}

#else /*define position function if LWIP_NETIF_WF_UAP not set*/
void netifapi_pc_rndis_setup(void) {return;}
int netifapi_set_pc_macaddr(u8_t *mac_addr) {return 0;}/*denote success*/

#endif /* LWIP_NETIF_PC */


#endif
#endif