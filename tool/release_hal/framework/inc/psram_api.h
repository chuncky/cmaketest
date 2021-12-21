#ifndef __PSRAM_API_H__
#define __PSRAM_API_H__
#include "plat_basic_api.h"
#ifdef __cplusplus
extern "C"{
#endif
//#define DBG_MONITOR_ENABLE
#define MAX_MONITOR_SIZE 20
//performance catch type
#define		PERF_CATCH_TIME  (0x1UL<<0)
#define 	PERF_CATCH_BW	 (0x1UL<<1)
#define 	PERF_CATCH_MAX   (0x1UL<<15)
 	/*support 16 flag*/
#define		PERF_ALARM_TIME  (0x1UL<<16)
#define		PERF_ALARM_BW	 (0x1UL<<17)
#define		PERF_ALARM_MAX   (0x1UL<<31)

//psram cache regs
enum psram_cacheline_size{
    CACHE_64B = 0,
    CACHE_128B = 1,
    CACHE_256B = 2,
    CACHE_512B = 3,
};
#ifdef CONFIG_BOARD_CRANE_EVB_Z1
#define CHIP_IS_Z1 //tmp define ,need remove for z2 version
#endif
enum lock_ratio{
    LOCK_DISABLE,//0: disable
    LOCK_1_2,//1: 1/2 cache
    LOCK_1_4,//2: 1/4 cache
    LOCK_1_8,//3: 1/8 cache
    LOCK_1_16,//4: 1/16 cache
    LOCK_1_32,//5: 1/32 cache, valid for all cacheline size
    LOCK_1_64,//6: 1/64 cache, valid for cacheline size 64B, 128B, 256B
    LOCK_1_128,//7: 1/128 cache, valid for cacheline size 64B, 128B
    LOCK_1_256,//8: 1/256 cache, only valid for cacheline size 64B
};

enum region_size{
    REGION_64B=6,
    REGION_128B,
    REGION_256B,
    REGION_512B,
    REGION_1KB,
    REGION_2KB,
    REGION_4KB,
    REGION_8KB,
    REGION_16KB,
    REGION_32KB,
    REGION_64KB,
    REGION_128KB,
    REGION_256KB,
    REGION_512KB,
    REGION_1MB,
    REGION_2MB,
    REGION_4MB,
    REGION_8MB,
    REGION_16MB,
    REGION_32MB,
    REGION_64MB,
    REGION_128MB,
    REGION_256MB,
    REGION_512MB,
    REGION_1GB,
    REGION_2GB,
    REGION_4GB,
};
enum psram_attr{
    ATTR_NC = 0,        
    ATTR_WT = 2,
    ATTR_WB = 3,
};
//counter event:
enum cnt_event{
    CLOCK_CYCLES = 0x0,  //Clock cycles (divided by pc_clk_div)
    //<debug_event>
    LFQ_CACHE_RD_REQ = 0x20, // LFQ_CACHE_RD_REQ count
    LFQ_CACHE_WR_REQ = 0x21, // lfq cache wr req count
    MSTR_CACHE_RD_REQ = 0x22, // mstr cache rd req count
    MSTR_CACHE_WR_REQ = 0x23, // mstr cache wr req count
    MSTR_NC_RD_REQ = 0x24, // mstr nc rd req count
    MSTR_NC_WR_REQ = 0x25, // mstr nc wr req count
   // <debug_cache_tags>
    RD_AREQ = 0x40, //rd_areq count
    WR_AREQ = 0x41, //wr_areq count
    RD_DREQ = 0x42, //rd_dreq count
    WR_DREQ = 0x43, //wr_dreq count
    RD_AREQ_NC = 0x44, //rd_areq nc count
    WR_AREQ_NC = 0x45, //wr_areq nc count
    RD_AREQ_WT = 0x46, //rd_areq wt count
    WR_AREQ_WT = 0x47, //wr_areq wt count
    RD_AREQ_WT_HIT = 0x48, //rd_areq wt hit count
    WR_AREQ_WT_HIT = 0x49, //wr_areq wt hit count
    RD_AREQ_WB = 0x4a, //rd_areq wb count
    WR_AREQ_WB = 0x4b, //wr_areq wb count
    RD_AREQ_WB_HIT = 0x4c, //rd_areq wb hit count
    WR_AREQ_WB_HIT = 0x4d, //wr_areq wb hit count
    //<debug_lfev[1]>
    LFQ_REQ = 0x50, //lfq_req count
    LFQ_REQ_RD = 0x51, //lfq_req rd count
    LFQ_REQ_WR =0x52, //lfq_req wr count
    EVICT_REQ = 0x53, //evict_req count
    FLUSH_REQ = 0x54, //flush_req count
    LFQ_FULL_WR = 0x55, //lfq full wr count
    LFQ_PART_RD = 0x56, //lfq part rd count
    //<debug_lfq>
    LFQ_FULL = 0x58, //lfq_full cycle
    LFQ_FULL_PRE = 0x59, //lfq_full_pre cycle
    LFQ_EMPTY = 0x5a, //lfq_empty cycle
    /* 0x63 lfq half full cycle */
    LFQ_OVER_THRESHOLD = 0x5c, //lfq over threshold cycle
    LFQ_OVER_THRESHOLD_GRANT = 0x5d, //lfq over threshold granted count
    //<debug_cache_tags>
    HQ_FULL = 0x60, //hq_full cycle
    HQ_FULL_PRE = 0x61, //hq_full_pre cycle
    HQ_EMPTY = 0x62,
    NO_LRU_ENTRY = 0x63,
};
struct psram_perf_stat {
	uint32_t total_evt_num;
	uint32_t start_ts;
	uint32_t stop_ts;
	uint32_t last_ts;
};
struct psram_perf_cnt {
	uint32_t id;
	uint32_t evt_sel;
	uint32_t evt_num;
	struct psram_perf_stat stat;
};


#define AXI_PORT_MAX 4

struct axi_latency{
    uint32_t rd_lat[AXI_PORT_MAX];
    uint32_t wr_lat[AXI_PORT_MAX];
};

/*bw by axi monitor*/
struct axi_band_width{
    uint32_t bw_rd[AXI_PORT_MAX];
    uint32_t bw_wr[AXI_PORT_MAX];
    uint32_t bw_all[AXI_PORT_MAX];// all axi bw by axi monitor
};

/*bw by phy monitor*/
struct dev_band_width{
    uint32_t bw_all;
    uint32_t bw_rd;
    uint32_t bw_wr;
};

struct perf_data{
    struct axi_band_width axi_bw;
    struct dev_band_width dev_bw;
    struct axi_latency latency;
    uint32_t rw_ratio;
    uint32_t max_bw_port;
	uint32_t axi_bw_all;
};
struct peak_bw_data{
	uint64_t time; // time at which get the peak bw
	struct perf_data peak_data;
};

struct qos_mst{	
	uint32_t p3_x1_axi_qos;
	uint32_t p3_lte_axi_qos;
	uint32_t p2_ispoffline_axi_qos;
	uint32_t p2_usb_axi_qos;
	uint32_t p2_sdh_axi_qos;
	uint32_t p2_dma_axi_qos;
	uint32_t p1_cr5_axi_qos;
	uint32_t p0_lcdisp_axi_qos;
};
void qos_set(struct qos_mst* qos);


struct region_attr{
	uint32_t addr;
	uint8_t attr;
	uint8_t size;
	uint8_t en;
};

struct psram_info{
	uint32_t freq;//data rate
	struct region_attr attr[8];//8 region's configuration	
	uint8_t cache_enable;
	uint8_t cache_size;
};
int  get_psram_info(struct psram_info *info);
void dump_psram_info(struct psram_info *info );

/*psram cache apis*/
#ifdef CHIP_IS_Z1
__inline uint32_t get_cache_size(void){return 0;}
__inline void psram_cache_en(uint32_t en){}
__inline void psram_cacheline_set(uint32_t cache_size){}
__inline void psram_cache_lock(uint32_t region,uint32_t addr,uint32_t size, uint32_t lock_ratio){}
__inline void psram_invalidate_cache(uint32_t addr,uint32_t size,int all){}
__inline void psram_flush_cache(uint32_t addr,uint32_t size,int all){}

__inline void psram_attr_set(uint32_t region_n,uint32_t addr,uint32_t attr,uint32_t size){}
__inline uint32_t get_axi_bw(uint32_t id,uint32_t rw){return 0;}

#else
uint32_t get_cache_size(void);
void psram_cache_en(uint32_t en);
void psram_cacheline_set(uint32_t cache_size);
void psram_cache_lock(uint32_t region,uint32_t addr,uint32_t size, uint32_t lock_ratio);
void psram_invalidate_cache(uint32_t addr,uint32_t size,int all);
void psram_flush_cache(uint32_t addr,uint32_t size,int all);

void psram_attr_set(uint32_t region_n,uint32_t addr,uint32_t attr,uint32_t size);

uint32_t get_axi_bw(uint32_t id,uint32_t rw);

#endif


uint32_t get_psram_phy_freq(void);
uint32_t get_psram_controller_freq(void);
uint32_t get_r5_freq(void);

#ifdef DBG_MONITOR_ENABLE
//new interface for app use
void Performance_Entry(const char* name,uint32_t flag,uint32_t bw_limit,uint32_t time_limit);
void Performance_Exit(const char* name);
uint32_t perf_new_monitor(void);
void perf_free_monitor(uint32_t id);
void start_monitor(int id);
void stop_monitor(int id);
void show_monitor(int id);
int  psram_count_cfg_update(int phy_pc,struct psram_perf_cnt *counter,int num);
struct perf_data * get_perf_data(uint32_t id);
void dump_perf_data(struct perf_data * data);
uint32_t get_rw_ratio(void);

#else
//new interface for app use
static void Performance_Entry(const char* name,uint32_t flag,uint32_t bw_limit,uint32_t time_limit){}
static void Performance_Exit(const char* name){}
static uint32_t perf_new_monitor(void){return 0;}
static void perf_free_monitor(uint32_t id){}
static void start_monitor(int id){}
static void stop_monitor(int id){}
static void show_monitor(int id){}
static int  psram_count_cfg_update(int phy_pc,struct psram_perf_cnt *counter,int num){return 0;}
static struct perf_data * get_perf_data(uint32_t id){return NULL;}
static void dump_perf_data(struct perf_data * data){}
static uint32_t get_rw_ratio(void){return 0;}
#endif

#ifdef __cplusplus
}
#endif
#endif /* _ASR_PSRAM_PERF_H */
