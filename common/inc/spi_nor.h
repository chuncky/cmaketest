#ifndef _SPI_NOR_H_
#define _SPI_NOR_H_


#define FALSE 0
#define TRUE 1
#define BIT(n) (1<<(n))




#define SSP_READ_TIME_OUT_MILLI		0x200
#define SSP_READ_DMA_DESC			0x10	//total RX descriptors
#ifdef FOTA_ASRSPI
#define SSP_READ_DMA_SIZE			0x100	//bytes per descriptor
#else
#define SSP_READ_DMA_SIZE			0x1000	//bytes per descriptor
#endif
#define PAGE_SIZE (256)
#define SZ_4K				0x00001000
#define SZ_32K				0x00008000
#define SZ_64K				0x00010000
#define SPINOR_OP_EN4B		0xb7	/* Enter 4-byte mode */
#define SPINOR_OP_EX4B		0xe9	/* Exit 4-byte mode */
#define SPINOR_OP_READ_4B	0x13	/* Read data bytes (up to 256 bytes) */
#define SPINOR_OP_PP_4B		0x12	/* Page program (up to 256 bytes) */
#define SPINOR_OP_BE_4K_4B	0x21	/* Erase 4KiB block (up to 256 bytes) */
#define SPINOR_OP_BE_32K_4B	0x5c	/* Erase 32KiB block (up to 256 bytes)*/
#define SPINOR_OP_SE_4B		0xdc	/* Sector erase (usually 64KiB, up to 256 bytes) */
#define	MAX_CMD_SIZE		6
#define BIT_0 (1 << 0)
#define BIT_1 (1 << 1)
#define BIT_2 (1 << 2)
#define BIT_3 (1 << 3)
#define BIT_4 (1 << 4)
#define BIT_5 (1 << 5)
#define BIT_6 (1 << 6)
#define BIT_7 (1 << 7)
#define BIT_8 (1 << 8)
#define BIT_9 (1 << 9)
#define BIT_10 (1 << 10)
#define BIT_11 (1 << 11)
#define BIT_12 (1 << 12)
#define BIT_13 (1 << 13)
#define BIT_14 (1 << 14)
#define BIT_15 (1 << 15)
#define BIT_16 (1 << 16)
#define BIT_17 (1 << 17)
#define BIT_18 (1 << 18)
#define BIT_19 (1 << 19)
#define BIT_20 (1 << 20)
#define BIT_21 (1 << 21)
#define BIT_22 (1 << 22)
#define BIT_23 (1 << 23)
#define BIT_24 (1 << 24)
#define BIT_25 (1 << 25)
#define BIT_26 (1 << 26)
#define BIT_27 (1 << 27)
#define BIT_28 (1 << 28)
#define BIT_29 (1 << 29)
#define BIT_30 (1 << 30)
#define BIT_31 ((unsigned)1 << 31)
typedef unsigned long long 			uint64_t;

typedef unsigned char				uint8_t;
typedef unsigned short				uint16_t;
typedef unsigned int				uint32_t;
#define ADDR_ALIGN(addr_, align_)         ((addr_+align_-1)&(~(align_-1)))
#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))

struct DMA_CMD_BITS 
{
    uint32_t Length         :13; //Bits [12:0]: Length of transfer in Bytes
    uint32_t Reserved4      :1; //Bit 13: reserved
    uint32_t Width          :2; //Bits [15:14]: Width of onchip peripheral - 0=reserved, 1=1B, 2=2B, 3=4B
    uint32_t MaxBurstSize   :2; //Bits [17:16]: Max Burst Size - 0=reserved, 1=8B, 2=16B, 3=32B
    uint32_t Reserved3      :3; //Bits [20:18]: reserved
    uint32_t EndIRQEn       :1; //Bit 21: end interrupt enable
    uint32_t StartIRQEn     :1; //Bit 22: start interrupt enable
    uint32_t AddrMode       :1; //Bit 23: Addressing mode for descriptor compare - 0=target has addr/1=target has data
    uint32_t Reserved2      :1; //Bit 24: reserved
    uint32_t CmpEn          :1; //Bit 25: Compare Enable - descriptor branching compare enable
    uint32_t Reserved1      :2; //Bits [27:26]: reserved
    uint32_t FlowTrg        :1; //Bit 28: Target flow control - 1=wait for request signal
    uint32_t FlowSrc        :1; //Bit 29: Source flow control - 1=wait for request signal
    uint32_t IncTrgAddr     :1; //Bit 30: Target increment - 0=no increment, 1=increment
    uint32_t IncSrcAddr     :1; //Bit 31: Source increment - 0=no increment, 1=increment
};
		
typedef struct DMA_DESCRIPTOR_TAG{
	uint32_t	DDADR;	// descriptor address reg
	uint32_t	DSADR;	// source address register
	uint32_t	DTADR;	// target address register
	uint32_t	DCMD;	// command address register
}DMA_DESCRIPTOR, *P_DMA_DESCRIPTOR;

typedef union
{
	uint32_t value;
	struct DMA_CMD_BITS bits;	
} DMA_CMDx_T, *P_DMA_CMDx_T;


#ifndef SPINOR_MAX_ID_LEN
#define SPINOR_MAX_ID_LEN		4
#endif

#define INFO(_jedec_id, _ext_id, _sector_size, _n_sectors, _flags)	\
		.id = {                         \
            ((_jedec_id) >> 24) & 0xff,         \
            ((_jedec_id) >> 16) & 0xff,          \
            ((_jedec_id) >> 8) & 0xff,          \
            (_jedec_id) & 0xff             \
            },						\
		.id_len = 4,	\
		.sector_size = (_sector_size),				\
		.n_sectors = (_n_sectors),				\
		.page_size = 256,					\
		.flags = (_flags),
    
#define SPI_NOR_CMD(_jedec_id, _enable_4byte, _disable_4byte, _read_status, _write_status,	\
            _sector_erase_4k, _blk_erase_32k, _blk_erase_64k, _chip_erase, _prog, _wr_enable, \
            _wr_disable, _read_id, _en_rst, _reset, _release_powerdown, _cache_read, _max_cmd)   \
            .id = {                         \
                ((_jedec_id) >> 24) & 0xff,         \
                ((_jedec_id) >> 16) & 0xff,          \
                ((_jedec_id) >> 8) & 0xff,          \
                (_jedec_id) & 0xff             \
                },             \
            .enable_4byte = (_enable_4byte),   \
            .disable_4byte = (_disable_4byte),             \
            .read_status = (_read_status),             \
            .write_status = (_write_status),                   \
            .sector_erase_4k = (_sector_erase_4k),             \
            .blk_erase_32k = (_blk_erase_32k),             \
            .blk_erase_64k = (_blk_erase_64k),             \
            .chip_erase = (_chip_erase),               \
            .prog = (_prog),                   \
            .wr_enable = (_wr_enable),             \
            .wr_disable = (_wr_disable),               \
            .read_id = (_read_id),             \
            .en_rst = (_en_rst),                   \
            .reset = (_reset),             \
            .release_powerdown = (_release_powerdown),             \
            .cache_read = (_cache_read),             \
            .max_cmd = (_max_cmd), 
            



struct nor_info {
	char		*name;

	/*
	 * This array stores the ID bytes.
	 * The first three bytes are the JEDIC ID.
	 * JEDEC ID zero means "no ID" (mostly older chips).
	 */
	uint8_t		id[SPINOR_MAX_ID_LEN];
	uint8_t		id_len;

	/* The size listed here is what works with SPINOR_OP_SE, which isn't
	 * necessarily called a "sector" by the vendor.
	 */
	unsigned	sector_size;
	uint16_t		n_sectors;

	uint16_t		page_size;
	uint16_t		addr_width;

	uint16_t		flags;

#define SECT_4K			BIT(0)	/* SPINOR_OP_BE_4K works uniformly */
#define SPI_NOR_NO_ERASE	BIT(1)	/* No erase command needed */
#define SST_WRITE		BIT(2)	/* use SST byte programming */
#define SPI_NOR_NO_FR		BIT(3)	/* Can't do fastread */
#define SECT_4K_PMC		BIT(4)	/* SPINOR_OP_BE_4K_PMC works uniformly */
#define SPI_NOR_DUAL_READ	BIT(5)	/* Flash supports Dual Read */
#define SPI_NOR_QUAD_READ	BIT(6)	/* Flash supports Quad Read */
#define USE_FSR			BIT(7)	/* use flag status register */
#define SPI_NOR_HAS_LOCK	BIT(8)	/* Flash supports lock/unlock via SR */
#define SPI_NOR_HAS_TB		BIT(9)	/*
					 * Flash SR has Top/Bottom (TB) protect
					 * bit. Must be used with
					 * SPI_NOR_HAS_LOCK.
					 */

#define SPI_NOR_4B_OPCODES	BIT(11)	/*
					 * Use dedicated 4byte address op codes
					 * to support memory size above 128Mib.
					 */
};

struct spinor_chip_info;
struct spinor_ops {
	int (*reset)(struct spinor_chip_info *chip);
	int (*read_id)(struct spinor_chip_info *chip, uint8_t *buf);
	int (*read)(struct spinor_chip_info *chip, uint32_t FlashOffset, int Size, uint32_t Buffer);
	int (*write)(struct spinor_chip_info *chip, uint32_t addr, int Size, uint32_t Buffer);
	int (*erase)(struct spinor_chip_info *chip);
	int (*erase_sector)(struct spinor_chip_info *chip, int offset, int size);
	int (*erase_block)(struct spinor_chip_info *chip, int offset, int size);
};

struct spinor_cmd_cfg {
    char    *name;
    uint8_t		id[SPINOR_MAX_ID_LEN];
    
    uint8_t enable_4byte;
	uint8_t disable_4byte;
	uint8_t read_status;
	uint8_t write_status;
	uint8_t sector_erase_4k;
	uint8_t blk_erase_32k;
	uint8_t blk_erase_64k;
	uint8_t chip_erase;
	uint8_t prog;
	uint8_t wr_enable;
	uint8_t wr_disable;
	uint8_t read_id;
	uint8_t en_rst;
	uint8_t reset;
	uint8_t release_powerdown;
    uint8_t cache_read;
	uint8_t max_cmd;
};
typedef enum
{
	DMA_STOP = 0,
	DMA_END
}DMA_WAIT_DONE_FLAG;

struct spinor_chip_info {
    struct nor_info *info;
    struct spinor_cmd_cfg *table;
    struct spinor_ops *ops;
    uint8_t addr_width;
};

#if 0
struct test_config
{
	uint32_t pattern;
	int len;
};
#endif

struct spinor_cmd {
	uint8_t		n_addr;		/* Number of address */
	//uint32_t    cmdtmp;	
	const uint8_t		*cmdtmp;	
	uint32_t	n_tx;		/* Number of tx bytes */
	const uint8_t	*tx_buf;	/* Tx buf */
	uint32_t	n_rx;		/* Number of rx bytes */
	uint8_t		*rx_buf;	/* Rx buf */
	uint8_t		mode;
    uint32_t    command;
};

//int spinor_test(uint8_t argc, uint8_t **argv);

int asr3601s_spinor_flashinit(void);

unsigned int asr3601s_spinor_do_erase(unsigned int addr, unsigned int size);

unsigned int asr3601s_spinor_do_erase_4k(unsigned int addr, unsigned int size);
unsigned int asr3601s_spinor_do_erase_64k(unsigned int addr, unsigned int size);


unsigned int asr3601s_spinor_do_read(unsigned int addr, unsigned int buf_addr, unsigned int size);

unsigned int asr3601s_spinor_do_write(unsigned int addr, unsigned int buf_addr,unsigned int size);

#define SPINOR_BASEADDR 0x90000000U






#endif
