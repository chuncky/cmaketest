#ifndef _SPI_FLASH_H
#define _SPI_FLASH_H

#define false	0
#define true	1

#ifndef min
#define min(a, b)    ((a) < (b) ? (a) : (b))
#endif

#ifndef max
#define max(a, b)    ((a) > (b) ? (a) : (b))
#endif

/* Vendor Manufacture ID */
#define SPIFLASH_MFR_MICRON		    0x2C
#define SPIFLASH_MFR_GIGADEVICE		0xC8
#define SPIFLASH_MFR_WINBOND		0xEF
#define SPIFLASH_MFR_MXIC		    0xC2
#define SPIFLASH_MFR_DOSILICON		0xE5
#define SPIFLASH_MFR_ZETTADEVICE	0xBA
#define SPIFLASH_MFR_DOUQI		    0x54
#define SPIFLASH_MFR_DOSILICON_NOR	0xF8
#define SPIFLASH_MFR_PUYA		    0x85
#define SPIFLASH_MFR_XM			    0x20
#define SPIFLASH_MFR_ZBIT           0x5E
#define SPIFLASH_MFR_FMSH		    0xA1

struct spi_flash_chip;
struct spi_flash_ops {
	int (*reset)(struct spi_flash_chip *chip);
	int (*read_id)(struct spi_flash_chip *chip, uint8_t *buf);
	int (*read_uid)(struct spi_flash_chip *chip, uint8_t *buf);
	int (*read)(struct spi_flash_chip *chip, int addr, int size, uint8_t *rbuf);
	int (*write)(struct spi_flash_chip *chip, int addr, int size, uint8_t *wbuf);
	int (*erase)(struct spi_flash_chip *chip, uint32_t addr, int len);
	int (*erase_4k)(struct spi_flash_chip *chip, uint32_t addr, int len);
};




#define ONFI_CRC_BASE	0x4F4E

/**
 * struct spi_flash_chip - SPI-NAND Private Flash Chip Data
 * @name:		name of the chip
 * @spi:		[INTERN] point to spi device structure
 * @mfr_id:		[BOARDSPECIFIC] manufacture id
 * @dev_id:		[BOARDSPECIFIC] device id
 * @read_cache_op:	[REPLACEABLE] Opcode of read from cache
 * @write_cache_op:	[REPLACEABLE] Opcode of program load
 * @write_cache_rdm_op:	[REPLACEABLE] Opcode of program load random
 * @oobbuf:		[INTERN] buffer for read/write oob
 * @size:		[INTERN] the size of chip
 * @block_size:		[INTERN] the size of eraseblock
 * @page_size:		[INTERN] the size of page
 * @oob_size:	[INTERN] the size of page oob size
 * @block_shift:	[INTERN] number of address bits in a eraseblock
 * @page_shift:		[INTERN] number of address bits in a page (column
 *			address bits).
 * @pagemask:		[INTERN] page number mask = number of (pages / chip) - 1
 * @options:		[BOARDSPECIFIC] various chip options. They can partly
 *			be set to inform nand_scan about special functionality.
 * @ecc_strength:	[INTERN] ECC correctability
 * @refresh_threshold:	[INTERN] Bitflip threshold to return -EUCLEAN
 * @ecclayout:		[BOARDSPECIFIC] ECC layout control structure
 *			See the defines for further explanation.
 * @onfi_params:	[INTERN] holds the ONFI page parameter
 */
struct spi_flash_chip {
	char *name;
	struct spi_flash_cmd_cfg *table;
	struct spi_flash_ops *ops;
	struct qspi_host *host;

	uint8_t cs;
	uint8_t mfr_id;
	uint16_t dev_id;
	uint32_t	tx_max_len;
	uint8_t read_op;
	uint8_t read_cache_op;
	uint8_t write_op;
	uint8_t write_cache_op;
	uint8_t write_cache_rdm_op;
	uint8_t en_addr_4byte;
	uint8_t qpi_enabled;
	uint8_t qpi_dummy;

	uint8_t *oobbuf;
	uint32_t size;
	uint32_t block_size;
	uint16_t page_size;
	uint16_t oob_size;
	uint8_t lun_shift;
	uint8_t block_shift;
	uint8_t page_shift;
	uint16_t page_mask;
	uint32_t options;
	uint32_t ecc_strength;
	uint8_t refresh_threshold;
	uint8_t lun;
	//struct nand_ecclayout *ecclayout;

};

#define SPINAND_MAX_ADDR_LEN		4

enum {
	CMD_W_NO_DATA = 0,
	CMD_W_RX_DATA = 1,
	CMD_W_TX_DATA = 2
};

struct spi_flash_cmd_cfg {
	uint8_t opcode;
	uint8_t addr_bytes;
	uint8_t addr_pins;
	uint8_t mode_bits;
	uint8_t mode_pins;
	uint8_t dummy_cycles;
	uint8_t dummy_pins;
	uint8_t data_pins;
	char seq_id;
	char type;
};

#define SPI_CMD_ENTRY(_opcode,_addr_bytes,_addr_pins,_mode_bits, \
			_mode_pins,_dummy_cycles,_dummy_pins,_data_pins, \
			_seq_id, _type) \
	{	\
		.opcode = _opcode, \
		.addr_bytes = _addr_bytes, \
		.addr_pins = _addr_pins, \
		.mode_bits = _mode_bits, \
		.mode_pins = _mode_pins, \
		.dummy_cycles = _dummy_cycles, \
		.dummy_pins = _dummy_pins, \
		.data_pins = _data_pins, \
		.seq_id = _seq_id, \
		.type = _type, \
	}

struct test_config
{
	int rx_mode;
	int tx_mode;
	int pattern;
	int len;
};

#define QSPI_TEST_ITEM(_rx_mode, _tx_mode, _pattern, _len) \
	{	\
		.rx_mode = _rx_mode, \
		.tx_mode = _tx_mode, \
		.pattern = _pattern, \
		.len = _len, \
	}

enum {
	RST_AHB_DOMAIN	= 1,
	SW_PROG_ERASE	= 2
};

struct spi_flash_cmd {
	struct spi_flash_cmd_cfg *cmd_cfg;
	uint8_t n_addr;		/* Number of address */
	uint8_t addr[SPINAND_MAX_ADDR_LEN];	/* Reg Offset */
	uint32_t n_tx;		/* Number of tx bytes */
	const uint8_t *tx_buf;	/* Tx buf */
	uint32_t n_rx;		/* Number of rx bytes */
	uint8_t *rx_buf;	/* Rx buf */
	uint8_t mode;
	uint8_t flag;
};

/* feature registers */
#define REG_BLOCK_LOCK		0xa0
#define REG_CFG			0xb0
#define REG_STATUS		0xc0
#define REG_DIE_SELECT		0xd0

/* status */
#define STATUS_OIP_MASK		0x01
#define STATUS_CRBSY_MASK	0x80
#define STATUS_READY		(0 << 0)
#define STATUS_BUSY		(1 << 0)

#define STATUS_E_FAIL_MASK	0x04
#define STATUS_E_FAIL		(1 << 2)

#define STATUS_P_FAIL_MASK	0x08
#define STATUS_P_FAIL		(1 << 3)

enum {
	NORMAL_MODE,
	OTP_MODE,
	OTP_PROTECT_MODE,
	SNOR_READ_ENABLE_MODE
};

struct qspi_mode
{
	int mode;
	char *name;
};

/* Ability of SPI Controllers */
#define SPI_OPM_RX_QUAD		1 << 2 /* Support 4 pin RX */
#define SPI_OPM_RX_DUAL		1 << 1 /* Support 2 pin RX */
#define SPI_OPM_RX		1 << 0 /* Support 1 pin RX */
#define SPI_OPM_TX_QUAD		1 << 2 /* Support 4 pin TX */
#define SPI_OPM_TX		1 << 0 /* Support 1 pin TX */

extern struct qspi_mode qspi_tx_mode[];
extern struct qspi_mode qspi_rx_mode[];
char *qspi_tx_mode_name(int mode);
char *qspi_rx_mode_name(int mode);

#endif
