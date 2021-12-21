#ifndef BT_API_A2DP_H
#define BT_API_A2DP_H

#ifndef BT_UI_PRESENT
#define BT_SBC_TEST
#endif

#define SBC_48000_BIT       0
#define SBC_44100_BIT       1
#define SBC_32000_BIT       2
#define SBC_16000_BIT       3

// SBC encode used
/* sampling frequency */
#define SBC_FREQ_16000      0x00
#define SBC_FREQ_32000      0x01
#define SBC_FREQ_44100      0x02
#define SBC_FREQ_48000      0x03

/* blocks */
#define SBC_BLK_4           0x00
#define SBC_BLK_8           0x01
#define SBC_BLK_12          0x02
#define SBC_BLK_16          0x03

/* channel mode */
#define SBC_MODE_STEREO         0x02
#define SBC_MODE_JOINT_STEREO   0x03


/* allocation method */
#define SBC_AM_LOUDNESS     0x00
#define SBC_AM_SNR          0x01

/* subbands */
#define SBC_SB_4            0x00
#define SBC_SB_8            0x01

/* data endianess */
#define SBC_LE              0x00
#define SBC_BE              0x01

struct appbt_a2dp {
    int sample;
    int channel;
    void (*notify_upper_layer_send_data)(void);
};

#ifdef __cplusplus
extern "C"{
#endif // __cplusplus

// API: setup a a2dp connect
int appbt_a2dp_connect(struct bt_addr addr);
// API: disconnect a a2dp connection
int appbt_a2dp_disconnect(void);
// API: set a2dp volume
int appbt_a2dp_set_volume(unsigned char volume);
// API: send A2DP start command to target device, if application want paly music should call this API
int appbt_a2dp_send_start(struct appbt_a2dp *a2dp);
// API: send A2DP suspend command to target device, if application stop music should call this API
int appbt_a2dp_send_suspend(void);
// API: Multimedia use this API
int appbt_a2dp_get_buffer_size(int rate, int chan, int* min, int* max);
// API: send SBC data to target device
int appbt_a2dp_send_sbc_data(unsigned int length, unsigned char* data,
                                unsigned int timestamp, unsigned short int seq_num,
                                unsigned char payload_type,
                                unsigned char frames);
// API: send audio data to target device
int appbt_a2dp_send_pcm_data(unsigned char *data, int length);

int appbt_a2dp_media_init(void);

int send_a2dp_media_event(struct bt_task_event *event);

#ifdef __cplusplus
}
#endif // __cplusplus 

#endif

