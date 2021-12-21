#ifndef BT_MGR_H
#define BT_MGR_H

#define MAX_BT_NAME                 48

#define BT_MGR_ACL_STATUS       (1 << 0)
#define BT_MGR_A2DP_STATUS      (1 << 1)
#define BT_MGR_AVRCP_STATUS     (1 << 2)
#define BT_MGR_HFP_STATUS       (1 << 3)
#define BT_MGR_PAIRED_STATUS    (1 << 4)

#ifdef __cplusplus
extern "C"{
#endif // __cplusplus

// API: connect to remote device. 
int appbt_connect_headset(struct bt_addr addr, int call_status);
// API: disconnect specified device for all profile
int appbt_disconnect_headset(struct bt_addr *addr);
// API: get current a2dp connection status
int appbt_get_a2dp_status(void);
// API: get specified device status, user should provie buffer for name store 
unsigned int appbt_get_device_status(struct bt_addr addr, 
                                               char name[MAX_BT_NAME]);
// API: cancel a connect
void appbt_connect_cancel(void);

int appbt_connect_phone(struct bt_addr addr);

int appbt_disconnect_phone(struct bt_addr *addr);

int appbt_acl_role(void);

// cleanup mgr context, system code call this API when bt close complete
void appbt_mgr_cleanup(void);

#ifdef __cplusplus
}
#endif // __cplusplus 

#endif

