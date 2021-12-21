#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "qspi_dma.h"
#include "qspi_flash.h"
#include "qspi_host.h"
#include "qspi_nor.h"

#define uart_printf(...)
extern void uudelay(int us);
extern void mdelay(int ms);


static struct spi_nor_info spi_nor_table[] = {
	/* Macronix */
	{"MX25U12835", 0xC2, 0x2538, 256, 256, 16*1024*1024, 0},
	{"MX25U25643G", 0xC2, 0x2539, 256, 256, 32*1024*1024, 0},
	{"MX25U3273", 0xC2, 0x2536, 256, 256, 4*1024*1024, 0},

	/* GigaDeivce */
	{"GD25LQ256C", 0xC8, 0x6019, 256, 256, 32*1024*1024, 0},
	{"GD25LQ128D", 0xC8, 0x6018, 256, 256, 16*1024*1024, 0},
	{"GD25LQ64C", 0xC8, 0x6017, 256, 256, 8*1024*1024, 0},

	/* Winbond */
	{"W25Q256JW", 0xEF, 0x6019, 256, 256, 32*1024*1024, 0},
	{"W25Q256JW-IM*", 0xEF, 0x8019, 256, 256, 32*1024*1024, 0},
	{"W25Q128JW", 0xEF, 0x6018, 256, 256, 16*1024*1024, 0},
	{"W25Q128JW-IM", 0xEF, 0x8018, 256, 256, 16*1024*1024, 0},
	{"W25Q64FW", 0xEF, 0x6017, 256, 256, 8*1024*1024, 0},

	/* Dosilicon */
	{"FM25M4AA", 0xF8, 0x4218, 256, 256, 16*1024*1024, 0},
	{"DS25M4AB", 0xE5, 0x4218, 256, 256, 16*1024*1024, 0},

	/* DouQi */
	{"DQ25Q128AL", 0x54, 0x6018, 256, 256, 16*1024*1024, 0},

	/* Puya */
	{"P25Q128L", 0x85, 0x6018, 256, 256, 16*1024*1024, 0},

	/* ZettaDevice */
	{"ZD25Q128", 0xBA, 0x4218, 256, 256, 16*1024*1024, 0},

	/* Wuhan XinXin */
	{"XM25QU64B", 0x20, 0x5017, 256, 256, 8*1024*1024, 0},
	{"XM25QU128B", 0x20, 0x5018, 256, 256, 16*1024*1024, 0},
	{"XM25QU256B", 0x20, 0x7019, 256, 256, 32*1024*1024, 0},
	{"XM25QU128C", 0x20, 0x4118, 256, 256, 16*1024*1024, 0},

    /* Fudan Microelectronics */
    {"FM25W128", 0xA1, 0x2818, 256, 256, 16*1024*1024, 0},
    /* Zbit Semiconductor */
    {"ZB25LQ128A", 0x5E, 0x5018, 256, 256, 16*1024*1024, 0},
	{NULL},
};

/**
 * spi_nor_scan_id_table - scan chip info in id table
 * @chip: SPI-NOR device structure
 * Description:
 *   If found in id table, config chip with table information.
 */
static int spi_nor_scan_id_table(struct spi_flash_chip *chip)
{
	struct spi_nor_info *type = spi_nor_table;

	for (; type->name; type++) {
		if (chip->mfr_id == type->mfr_id && chip->dev_id == type->dev_id) {
			chip->name = type->name;
			chip->size = type->total_size;
			chip->block_size = type->page_size * type->pages_per_blk;
			chip->page_size = type->page_size;
			chip->options = type->options;
			uart_printf("SPI-NOR: %s is found in table\r\n",type->name);
			return TRUE;
		}
	}

	return FALSE;
}

enum normal_cmd {
	ENABLE_4BYTE,
	DISABLE_4BYTE,
	READ_STATUS1,
	READ_STATUS2,
	READ_STATUS3,
	WRITE_STATUS,
	WRITE_STATUS2,
	READ_SLOW,
	READ_FAST,
	READ_FAST_X2,
	READ_FAST_X4,
	READ_FAST_DUAL,
	READ_FAST_QUAD,
	SECTOR_ERASE_4K,
	BLK_ERASE_32K,
	BLK_ERASE_64K,
	CHIP_ERASE,
	PROG,
	PROG_X4,
	WR_ENABLE,
	WR_DISABLE,
	READ_ID,
	READ_UID,
	ENABLE_QPI,
	PGM_ERS_SUSPEND,
	PGM_ERS_RESUME,
	EN_RST,
	RESET,
	READ_STATUS2_MXIC,
	PROG_X4_MXIC,
	ENABLE_QPI_MXIC,
	PGM_ERS_SUSPEND_MXIC,
	PGM_ERS_RESUME_MXIC,
	MAX_CMD,
};

enum qspi_cmd {
	ENABLE_4BYTE_QPI,
	DISABLE_4BYTE_QPI,
	READ_STATUS1_QPI,
	READ_STATUS2_QPI,
	READ_STATUS3_QPI,
	READ_STATUS2_MXIC_QPI,
	WRITE_STATUS_QPI,
	WRITE_STATUS2_QPI,
	READ_FAST_QPI,
	READ_FAST_QUAD_QPI,
	SECTOR_ERASE_4K_QPI,
	BLK_ERASE_32K_QPI,
	BLK_ERASE_64K_QPI,
	CHIP_ERASE_QPI,
	PROG_QPI,
	WR_ENABLE_QPI,
	WR_DISABLE_QPI,
	READ_ID_QPI,
	READ_QPIID,
	DISABLE_QPI,
	DISABLE_QPI_MXIC,
	SET_READ_PARA_QPI,
	PGM_ERS_SUSPEND_QPI,
	PGM_ERS_RESUME_QPI,
	PGM_ERS_SUSPEND_MXIC_QPI,
	PGM_ERS_RESUME_MXIC_QPI,
	EN_RST_QPI,
	RESET_QPI,
	MAX_CMD_QPI,
};

/* Standard SPI-NOR flash normal commands */
static struct spi_flash_cmd_cfg cmd_table[] =
{
	/*opcode  addr_bytes  addr_pins mode_bits  mode_pins  dummy_cycles
		dummy_pins   data_pins  seq_id  cmd_type */

{0xb7, 0, 0, 0, 0, 0, 0, 0, -1, 0},//		[ENABLE_4BYTE]		= SPI_CMD(0xb7, 0, 0, 0, 0, 0, 0, 0, -1, 0),
{0xe9, 0, 0, 0, 0, 0, 0, 0, -1, 0},//		[DISABLE_4BYTE] 	= SPI_CMD(0xe9, 0, 0, 0, 0, 0, 0, 0, -1, 0),
{0x05, 0, 0, 0, 0, 0, 0, 1,  2, 1},//		[READ_STATUS1]		= SPI_CMD(0x05, 0, 0, 0, 0, 0, 0, 1,  2, 1),
{0x35, 0, 0, 0, 0, 0, 0, 1, -1, 1},//		[READ_STATUS2]		= SPI_CMD(0x35, 0, 0, 0, 0, 0, 0, 1, -1, 1),
{0x15, 0, 0, 0, 0, 0, 0, 1, -1, 1},//		[READ_STATUS3]		= SPI_CMD(0x15, 0, 0, 0, 0, 0, 0, 1, -1, 1),
{0x01, 0, 0, 0, 0, 0, 0, 1, -1, 2},//		[WRITE_STATUS]		= SPI_CMD(0x01, 0, 0, 0, 0, 0, 0, 1, -1, 2),
{0x31, 0, 0, 0, 0, 0, 0, 1, -1, 2},//		[WRITE_STATUS2]		= SPI_CMD(0x31, 0, 0, 0, 0, 0, 0, 1, -1, 2),
{0x03, 3, 1, 0, 0, 0, 0, 1, -1, 1},//		[READ_SLOW] 	= SPI_CMD(0x03, 3, 1, 0, 0, 0, 0, 1, -1, 1),
{0x0b, 3, 1, 0, 0, 8, 1, 1,  3, 1},//		[READ_FAST] 	= SPI_CMD(0x0b, 3, 1, 0, 0, 8, 1, 1,  3, 1),
{0x3b, 3, 1, 0, 0, 8, 1, 2, -1, 1},//		[READ_FAST_X2]		= SPI_CMD(0x3b, 3, 1, 0, 0, 8, 1, 2, -1, 1),
{0x6b, 3, 1, 0, 0, 8, 1, 4,  4, 1},//		[READ_FAST_X4]		= SPI_CMD(0x6b, 3, 1, 0, 0, 8, 1, 4,  4, 1),
{0xbb, 3, 2, 8, 2, 0, 0, 2, -1, 1},//		[READ_FAST_DUAL]	= SPI_CMD(0xbb, 3, 2, 8, 2, 0, 0, 2, -1, 1),
{0xeb, 3, 4, 8, 4, 4, 4, 4,  5, 1},//		[READ_FAST_QUAD]	= SPI_CMD(0xeb, 3, 4, 8, 4, 4, 4, 4,  5, 1),
{0x20, 3, 1, 0, 0, 0, 0, 0, -1, 0},//		[SECTOR_ERASE_4K]	= SPI_CMD(0x20, 3, 1, 0, 0, 0, 0, 0, -1, 0),
{0x52, 3, 1, 0, 0, 0, 0, 0, -1, 0},//		[BLK_ERASE_32K] 	= SPI_CMD(0x52, 3, 1, 0, 0, 0, 0, 0, -1, 0),
{0xd8, 3, 1, 0, 0, 0, 0, 0, -1, 0},//		[BLK_ERASE_64K] 	= SPI_CMD(0xd8, 3, 1, 0, 0, 0, 0, 0, -1, 0),
{0xc7, 0, 0, 0, 0, 0, 0, 0, -1, 0},//		[CHIP_ERASE]		= SPI_CMD(0xc7, 0, 0, 0, 0, 0, 0, 0, -1, 0),
{0x02, 3, 1, 0, 0, 0, 0, 1,  6, 2},//		[PROG]			= SPI_CMD(0x02, 3, 1, 0, 0, 0, 0, 1,  6, 2),
{0x32, 3, 1, 0, 0, 0, 0, 4,  7, 2},//		[PROG_X4]		= SPI_CMD(0x32, 3, 1, 0, 0, 0, 0, 4,  7, 2),
{0x06, 0, 0, 0, 0, 0, 0, 0, -1, 0},//		[WR_ENABLE] 	= SPI_CMD(0x06, 0, 0, 0, 0, 0, 0, 0, -1, 0),
{0x04, 0, 0, 0, 0, 0, 0, 0, -1, 0},//		[WR_DISABLE]		= SPI_CMD(0x04, 0, 0, 0, 0, 0, 0, 0, -1, 0),
{0x9f, 0, 0, 0, 0, 0, 0, 1, -1, 1},//		[READ_ID]		= SPI_CMD(0x9f, 0, 0, 0, 0, 0, 0, 1, -1, 1),
{0x4b, 0, 0, 0, 0,32, 1, 1, -1, 1},//       [READ_UID]		= SPI_CMD(0x4b, 0, 0, 0, 0,32, 1, 1, -1, 1),
{0x38, 0, 0, 0, 0, 0, 0, 0, -1, 0},//		[ENABLE_QPI]		= SPI_CMD(0x38, 0, 0, 0, 0, 0, 0, 0, -1, 0),
{0x75, 0, 0, 0, 0, 0, 0, 0, -1, 0},//		[PGM_ERS_SUSPEND]	= SPI_CMD(0x75, 0, 0, 0, 0, 0, 0, 0, -1, 0),
{0x7a, 0, 0, 0, 0, 0, 0, 0, -1, 0},//		[PGM_ERS_RESUME]	= SPI_CMD(0x7a, 0, 0, 0, 0, 0, 0, 0, -1, 0),
{0x66, 0, 0, 0, 0, 0, 0, 0, -1, 0},//		[EN_RST]		= SPI_CMD(0x66, 0, 0, 0, 0, 0, 0, 0, -1, 0),
{0x99, 0, 0, 0, 0, 0, 0, 0, -1, 0},//		[RESET] 		= SPI_CMD(0x99, 0, 0, 0, 0, 0, 0, 0, -1, 0),

/* Vendor specific command */
{0x15, 0, 0, 0, 0, 0, 0, 1, -1, 1},//           [READ_STATUS2_MXIC] = SPI_CMD(0x15, 0, 0, 0, 0, 0, 0, 1, -1, 1),
{0x38, 3, 4, 0, 0, 0, 0, 4,  8, 2},//           [PROG_X4_MXIC]      = SPI_CMD(0x38, 3, 4, 0, 0, 0, 0, 4,  8, 2),
{0x35, 0, 0, 0, 0, 0, 0, 0, -1, 0},//           [ENABLE_QPI_MXIC]   = SPI_CMD(0x35, 0, 0, 0, 0, 0, 0, 0, -1, 0),
{0xb0, 0, 0, 0, 0, 0, 0, 0, -1, 0},//           [PGM_ERS_SUSPEND_MXIC]  = SPI_CMD(0xb0, 0, 0, 0, 0, 0, 0, 0, -1, 0),
{0x30, 0, 0, 0, 0, 0, 0, 0, -1, 0},//           [PGM_ERS_RESUME_MXIC]   = SPI_CMD(0x30, 0, 0, 0, 0, 0, 0, 0, -1, 0),

		/* END Mark */
{0x00, 0, 0, 0, 0, 0, 0, 0, -1, 0},//		[MAX_CMD]		= SPI_CMD(0x00, 0, 0, 0, 0, 0, 0, 0, -1, 0),

};

/* Standard SPI-NOR flash QPI commands */
static struct spi_flash_cmd_cfg cmd_table_qpi[] = {
	/*opcode  addr_bytes  addr_pins mode_bits  mode_pins  dummy_cycles
		dummy_pins   data_pins  seq_id  cmd_type */
{0xb7, 0, 0, 0, 0, 0, 0, 0, -1, 0},//		[ENABLE_4BYTE_QPI]	= SPI_CMD(0xb7, 0, 0, 0, 0, 0, 0, 0, -1, 0),
{0xe9, 0, 0, 0, 0, 0, 0, 0, -1, 0},//		[DISABLE_4BYTE_QPI] = SPI_CMD(0xe9, 0, 0, 0, 0, 0, 0, 0, -1, 0),
{0x05, 0, 0, 0, 0, 0, 0, 4, -1, 1},//		[READ_STATUS1_QPI]	= SPI_CMD(0x05, 0, 0, 0, 0, 0, 0, 4,  2, 1),
{0x35, 0, 0, 0, 0, 0, 0, 4, -1, 1},//		[READ_STATUS2_QPI]	= SPI_CMD(0x35, 0, 0, 0, 0, 0, 0, 4, -1, 1),
{0x11, 0, 0, 0, 0, 0, 0, 4, -1, 1},//		[READ_STATUS3_QPI]	= SPI_CMD(0x11, 0, 0, 0, 0, 0, 0, 4, -1, 1),
{0x15, 0, 0, 0, 0, 0, 0, 4, -1, 1},//           [READ_STATUS2_MXIC_QPI] = SPI_CMD(0x15, 0, 0, 0, 0, 0, 0, 4, -1, 1),
{0x01, 0, 0, 0, 0, 0, 0, 4, -1, 2},//		[WRITE_STATUS_QPI]	= SPI_CMD(0x01, 0, 0, 0, 0, 0, 0, 4, -1, 2),
{0x31, 0, 0, 0, 0, 0, 0, 4, -1, 2},//		[WRITE_STATUS2_QPI]	= SPI_CMD(0x31, 0, 0, 0, 0, 0, 0, 4, -1, 2),
{0x0b, 3, 4, 0, 0, 8, 4, 4, -1, 1},//		[READ_FAST_QPI] 	= SPI_CMD(0x0b, 3, 4, 0, 0, 8, 4, 4,  3, 1),
{0xeb, 3, 4, 0, 0, 8, 4, 4, -1, 1},//		[READ_FAST_QUAD_QPI]	= SPI_CMD(0xeb, 3, 4, 0, 0, 8, 4, 4,  4, 1),
{0x20, 3, 4, 0, 0, 0, 0, 0, -1, 0},//		[SECTOR_ERASE_4K_QPI]	= SPI_CMD(0x20, 3, 4, 0, 0, 0, 0, 0, -1, 0),
{0x52, 3, 4, 0, 0, 0, 0, 0, -1, 0},//		[BLK_ERASE_32K_QPI] = SPI_CMD(0x52, 3, 4, 0, 0, 0, 0, 0, -1, 0),
{0xd8, 3, 4, 0, 0, 0, 0, 0, -1, 0},//		[BLK_ERASE_64K_QPI] = SPI_CMD(0xd8, 3, 4, 0, 0, 0, 0, 0, -1, 0),
{0xc7, 0, 0, 0, 0, 0, 0, 0, -1, 0},//		[CHIP_ERASE_QPI]	= SPI_CMD(0xc7, 0, 0, 0, 0, 0, 0, 0, -1, 0),
{0x02, 3, 4, 0, 0, 0, 0, 4, -1, 2},//		[PROG_QPI]		= SPI_CMD(0x02, 3, 4, 0, 0, 0, 0, 4,  5, 2),
{0x06, 0, 0, 0, 0, 0, 0, 0, -1, 0},//		[WR_ENABLE_QPI] 	= SPI_CMD(0x06, 0, 0, 0, 0, 0, 0, 0, -1, 0),
{0x04, 0, 0, 0, 0, 0, 0, 0, -1, 0},//		[WR_DISABLE_QPI]	= SPI_CMD(0x04, 0, 0, 0, 0, 0, 0, 0, -1, 0),
{0x9f, 0, 0, 0, 0, 0, 0, 4, -1, 1},//		[READ_ID_QPI]		= SPI_CMD(0x9f, 0, 0, 0, 0, 0, 0, 4, -1, 1),
{0xaf, 0, 0, 0, 0, 0, 0, 4, -1, 1},//		[READ_QPIID]		= SPI_CMD(0xaf, 0, 0, 0, 0, 0, 0, 4, -1, 1),
{0xff, 0, 0, 0, 0, 0, 0, 0, -1, 0},//		[DISABLE_QPI]		= SPI_CMD(0xff, 0, 0, 0, 0, 0, 0, 0, -1, 0),
{0xf5, 0, 0, 0, 0, 0, 0, 0, -1, 0},//           [DISABLE_QPI_MXIC]  = SPI_CMD(0xf5, 0, 0, 0, 0, 0, 0, 0, -1, 0),
{0xc0, 0, 0, 0, 0, 0, 0, 4, -1, 2},//		[SET_READ_PARA_QPI] = SPI_CMD(0xc0, 0, 0, 0, 0, 0, 0, 4, -1, 2),
{0x75, 0, 0, 0, 0, 0, 0, 0, -1, 0},//		[PGM_ERS_SUSPEND_QPI]	= SPI_CMD(0x75, 0, 0, 0, 0, 0, 0, 0, -1, 0),
{0x7a, 0, 0, 0, 0, 0, 0, 0, -1, 0},//		[PGM_ERS_RESUME_QPI]	= SPI_CMD(0x7a, 0, 0, 0, 0, 0, 0, 0, -1, 0),
{0xb0, 0, 0, 0, 0, 0, 0, 0, -1, 0},//           [PGM_ERS_SUSPEND_MXIC_QPI]= SPI_CMD(0xb0, 0, 0, 0, 0, 0, 0, 0, -1, 0),
{0x30, 0, 0, 0, 0, 0, 0, 0, -1, 0},//           [PGM_ERS_RESUME_MXIC_QPI]= SPI_CMD(0x30, 0, 0, 0, 0, 0, 0, 0, -1, 0),
{0x66, 0, 0, 0, 0, 0, 0, 0, -1, 0},//		[EN_RST_QPI]		= SPI_CMD(0x66, 0, 0, 0, 0, 0, 0, 0, -1, 0),
{0x99, 0, 0, 0, 0, 0, 0, 0, -1, 0},//		[RESET_QPI] 	= SPI_CMD(0x99, 0, 0, 0, 0, 0, 0, 0, -1, 0),

		/* END Mark */
{0x00, 0, 0, 0, 0, 0, 0, 0, -1, 0},//		[MAX_CMD_QPI]		= SPI_CMD(0x00, 0, 0, 0, 0, 0, 0, 0, -1, 0),

};

struct spi_flash_chip nor_chip;
struct spi_flash_chip nor_chip2;

static uint8_t ilog2(unsigned int v)
{
	uint8_t l = 0;
	while ((1UL << l) < v)
		l++;
	return l;
}

void *qspi_memset(void *str, int c, unsigned int count)
{
	void *s = str;

	while (count --) {
		*(char *) s = (char) c;
		s = (char *) s + 1;
	}

	return str;
}

/**
 * spi_nor_read_id - send 9Fh command to get ID
 * @chip: SPI_FLASH device structure
 * @buf: buffer to store id
 */
static int spi_nor_read_id(struct spi_flash_chip *chip, uint8_t *buf)
{
	struct spi_flash_cmd cmd;

	qspi_memset(&cmd, 0, sizeof(struct spi_flash_cmd));
	if (chip->qpi_enabled)
	{
		cmd.cmd_cfg = chip->table + READ_ID_QPI;
		uart_printf("READ_ID_QPI=%d\r\n",READ_ID_QPI);
	}
	else
	{
		cmd.cmd_cfg = chip->table + READ_ID;
		uart_printf("READ_ID=%d\r\n",READ_ID);
	}
	cmd.n_rx = 3;
	cmd.rx_buf = buf;

	return qspi_start_cmd(chip, &cmd);
}

static int spi_nor_read_uid(struct spi_flash_chip *chip, uint8_t *buf)
{
	struct spi_flash_cmd cmd;

	qspi_memset(&cmd, 0, sizeof(struct spi_flash_cmd));
	cmd.cmd_cfg = chip->table + READ_UID;
	cmd.n_rx = 8;
	cmd.rx_buf = buf;

	return qspi_start_cmd(chip, &cmd);
}

static int spi_nor_read_qpiid(struct spi_flash_chip *chip, uint8_t *buf)
{
	struct spi_flash_cmd cmd;

	if (!chip->qpi_enabled) {
		uart_printf("err: not in qpi mode\r\n");
		return -1;		
	}

	qspi_memset(&cmd, 0, sizeof(struct spi_flash_cmd));
	cmd.cmd_cfg = chip->table + READ_QPIID;
	cmd.n_rx = 3;
	cmd.rx_buf = buf;

	return qspi_start_cmd(chip, &cmd);
}

/**
 * spi_nor_reset - send command to reset chip.
 * @chip: SPI_FLASH device structure
 */
static int spi_nor_reset(struct spi_flash_chip *chip)
{
	struct spi_flash_cmd cmd;

	qspi_memset(&cmd, 0, sizeof(struct spi_flash_cmd));
	if (chip->qpi_enabled)
		cmd.cmd_cfg = chip->table + EN_RST_QPI;
	else
		cmd.cmd_cfg = chip->table + EN_RST;
	if (qspi_start_cmd(chip, &cmd) != 0)
		uart_printf("spi_nor enable reset failed!\n");

	if (chip->qpi_enabled)
		cmd.cmd_cfg = chip->table + RESET_QPI;
	else
		cmd.cmd_cfg = chip->table + RESET;
	if (qspi_start_cmd(chip, &cmd) != 0)
		uart_printf("spi_nor reset failed!\n");

	/* elapse 2ms before issuing any other command */
	udelay(2000);
	return 0;
}

/**
 * spi_nor_write_enable - send command 06h to enable write or erase the
 * Nand cells
 * @chip: SPI_FLASH device structure
 * Description:
 *   Before write and erase the Nand cells, the write enable has to be set.
 *   After the write or erase, the write enable bit is automatically
 *   cleared (status register bit 2)
 *   Set the bit 2 of the status register has the same effect
 */
static int spi_nor_write_enable(struct spi_flash_chip *chip)
{
	struct spi_flash_cmd cmd;

	qspi_memset(&cmd, 0, sizeof(struct spi_flash_cmd));
	if (chip->qpi_enabled)
		cmd.cmd_cfg = chip->table + WR_ENABLE_QPI;
	else
		cmd.cmd_cfg = chip->table + WR_ENABLE;

	return qspi_start_cmd(chip, &cmd);
}

/**
 * spi_nor_read_status - get status register value
 * @chip: SPI_FLASH device structure
 * @status: buffer to store value
 * Description:
 *   After read, write, or erase, the Nand device is expected to set the
 *   busy status.
 *   This function is to allow reading the status of the command: read,
 *   write, and erase.
 *   Once the status turns to be ready, the other status bits also are
 *   valid status bits.
 */
static int spi_nor_read_status(struct spi_flash_chip *chip, uint8_t index,
				uint8_t *status)
{
	struct spi_flash_cmd cmd;
	int ret;

	qspi_memset(&cmd, 0, sizeof(struct spi_flash_cmd));
	cmd.cmd_cfg = chip->table + index;
	cmd.n_rx = 1;
	cmd.rx_buf = status;

	ret = qspi_start_cmd(chip, &cmd);
	if (ret != 0)
		uart_printf("err: read register %d\n", ret);

	return ret;
}

static int spi_nor_read_status1(struct spi_flash_chip *chip, uint8_t *status)
{
	uint8_t cmd_index;

	if (chip->qpi_enabled)
		cmd_index = READ_STATUS1_QPI;
	else
		cmd_index = READ_STATUS1;

	return spi_nor_read_status(chip, cmd_index, status);
}

static int spi_nor_read_status2(struct spi_flash_chip *chip, uint8_t *status)
{
	uint8_t cmd_index;

	if (chip->mfr_id == SPIFLASH_MFR_MXIC) {
		/* Not support SPINOR_CMD_READ_STATUS2 for 32MB spi-nor */
		if (chip->dev_id == 0x2536)
			return 0;

		if (chip->qpi_enabled)
			cmd_index = READ_STATUS2_MXIC_QPI;
		else
			cmd_index = READ_STATUS2_MXIC;
	} else {
		if (chip->qpi_enabled)
			cmd_index = READ_STATUS2_QPI;
		else
			cmd_index = READ_STATUS2;
	}

	return spi_nor_read_status(chip, cmd_index, status);
}

static int spi_nor_read_status3(struct spi_flash_chip *chip, uint8_t *status)
{
	uint8_t cmd_index;

	if (chip->qpi_enabled)
		cmd_index = READ_STATUS3_QPI;
	else
		cmd_index = READ_STATUS3;

	return spi_nor_read_status(chip, cmd_index, status);
}

/**
 * spi_nor_write_reg - send command 1Fh to write register
 * @chip: SPI_FLASH device structure
 * @reg; register to write
 * @buf: buffer stored value
 */
static int spi_nor_write_status1(struct spi_flash_chip *chip, uint8_t *buf,
				uint8_t count)
{
	struct spi_flash_cmd cmd;
	int ret;

	qspi_memset(&cmd, 0, sizeof(struct spi_flash_cmd));
	if (chip->qpi_enabled)
		cmd.cmd_cfg = chip->table + WRITE_STATUS_QPI;
	else
		cmd.cmd_cfg = chip->table + WRITE_STATUS;

	cmd.n_tx = count;
	cmd.tx_buf = buf;

	ret = qspi_start_cmd(chip, &cmd);
	if (ret != 0)
		uart_printf("err: %d write status1\n", ret);

	return ret;
}

static int spi_nor_write_status2(struct spi_flash_chip *chip, uint8_t *buf)
{
	struct spi_flash_cmd cmd;
	int ret;

	memset(&cmd, 0, sizeof(struct spi_flash_cmd));
	if (chip->qpi_enabled)
		cmd.cmd_cfg = chip->table + WRITE_STATUS2_QPI;
	else
		cmd.cmd_cfg = chip->table + WRITE_STATUS2;

	cmd.n_tx = 1;
	cmd.tx_buf = buf;

	ret = qspi_start_cmd(chip, &cmd);
	if (ret != 0)
		uart_printf("err: %d write status2\n", ret);

	return ret;
}

/**
 * spi_nor_wait - wait until the command is done
 * @chip: SPI_FLASH device structure
 * @s: buffer to store status register(can be NULL)
 */
static int spi_nor_wait(struct spi_flash_chip *chip, uint8_t *s)
{
	uint8_t status;
	unsigned long ret = ETIMEDOUT;
	int cnt;

	do{
		spi_nor_read_status1(chip, &status);
		if ((status & STATUS_OIP_MASK) == STATUS_READY) {
			ret = 0;
			goto out;
		}
		uudelay(1);
		cnt++;
	}while(cnt<0x0fffffff);
out:
	if (s)
		*s = status;
	return ret;
}

/**
 * spi_nor_erase_block_erase - send command D8h to erase a block
 * @chip: SPI_FLASH device structure
 * @addr: the flash addr to erase.
 * Description:
 *   Need to wait for tERS.
 */
static int spi_nor_erase_sector(struct spi_flash_chip *chip,
				uint32_t addr)
{
	struct spi_flash_cmd cmd;

	qspi_memset(&cmd, 0, sizeof(struct spi_flash_cmd));
	if (chip->qpi_enabled)
		cmd.cmd_cfg = chip->table + SECTOR_ERASE_4K_QPI;
	else
		cmd.cmd_cfg = chip->table + SECTOR_ERASE_4K;

	if (chip->en_addr_4byte) {
		cmd.n_addr = 4;
		cmd.addr[0] = (uint8_t)(addr >> 24);
		cmd.addr[1] = (uint8_t)(addr >> 16);
		cmd.addr[2] = (uint8_t)(addr >> 8);
		cmd.addr[3] = (uint8_t)addr;
	} else {
		cmd.n_addr = 3;
		cmd.addr[0] = (uint8_t)(addr >> 16);
		cmd.addr[1] = (uint8_t)(addr >> 8);
		cmd.addr[2] = (uint8_t)addr;
	}
	cmd.flag = RST_AHB_DOMAIN;

	return qspi_start_cmd(chip, &cmd);
}

/**
 * spi_nor_erase_block_erase - send command D8h to erase a block
 * @chip: SPI_FLASH device structure
 * @addr: the flash addr to erase.
 * Description:
 *   Need to wait for tERS.
 */
static int spi_nor_erase_block(struct spi_flash_chip *chip,
				uint32_t addr)
{
	struct spi_flash_cmd cmd;

	qspi_memset(&cmd, 0, sizeof(struct spi_flash_cmd));
	if (chip->block_size < 64*1024) {
		if (chip->qpi_enabled)
			cmd.cmd_cfg = chip->table + BLK_ERASE_32K_QPI;
		else
			cmd.cmd_cfg = chip->table + BLK_ERASE_32K;
	} else {
		if (chip->qpi_enabled)
			cmd.cmd_cfg = chip->table + BLK_ERASE_64K_QPI;
		else
			cmd.cmd_cfg = chip->table + BLK_ERASE_64K;
	}

	if (chip->en_addr_4byte) {
		cmd.n_addr = 4;
		cmd.addr[0] = (uint8_t)(addr >> 24);
		cmd.addr[1] = (uint8_t)(addr >> 16);
		cmd.addr[2] = (uint8_t)(addr >> 8);
		cmd.addr[3] = (uint8_t)addr;
	} else {
		cmd.n_addr = 3;
		cmd.addr[0] = (uint8_t)(addr >> 16);
		cmd.addr[1] = (uint8_t)(addr >> 8);
		cmd.addr[2] = (uint8_t)addr;
	}
	cmd.flag = RST_AHB_DOMAIN | SW_PROG_ERASE;

	return qspi_start_cmd(chip, &cmd);
}

static int spi_nor_erase_chip(struct spi_flash_chip *chip)
{
	struct spi_flash_cmd cmd;

	qspi_memset(&cmd, 0, sizeof(struct spi_flash_cmd));
	if (chip->qpi_enabled)
		cmd.cmd_cfg = chip->table + CHIP_ERASE_QPI;
	else
		cmd.cmd_cfg = chip->table + CHIP_ERASE;
	cmd.flag = RST_AHB_DOMAIN;

	return qspi_start_cmd(chip, &cmd);
}

static int spi_nor_erase_all(struct spi_flash_chip *chip)
{
	uint8_t status;
	int ret = 0;

	spi_nor_write_enable(chip);
	spi_nor_erase_chip(chip);
	ret = spi_nor_wait(chip, &status);
	if (ret != 0) {
		uart_printf("chip erase command wait failed\n");
		return ret;
	}

	return 0;
}

static int spi_nor_enable_4byte_mode(struct spi_flash_chip *chip)
{
	struct spi_flash_cmd cmd;
	uint8_t data[3];
	uint8_t status, en4b_shift;
	uint32_t sdata;
	int ret;

	qspi_memset(&cmd, 0, sizeof(struct spi_flash_cmd));
	if (chip->qpi_enabled)
		cmd.cmd_cfg = chip->table + ENABLE_4BYTE_QPI;
	else
		cmd.cmd_cfg = chip->table + ENABLE_4BYTE;

	ret = qspi_start_cmd(chip, &cmd);
	if (ret != 0) {
		uart_printf("enable_4byte mode failed\r\n");
		return ret;
	}

	chip->en_addr_4byte = 1;
	/*
	 * Macronix use status register bit S13 for EN4B
	 * Gigadevice use status register bit S11
	 * WinBond use status register bit S16
	 */
	if (chip->mfr_id == SPIFLASH_MFR_MXIC)
		en4b_shift = 13;
	else if (chip->mfr_id == SPIFLASH_MFR_GIGADEVICE)
		en4b_shift = 11;
	else if (chip->mfr_id == SPIFLASH_MFR_WINBOND)
		en4b_shift = 16;
	else {
		uart_printf("EN4B: lack vendor status bit info\r\n");
		return 0;
	}

	if (en4b_shift >= 16)
		spi_nor_read_status3(chip, &data[2]);
	else if (en4b_shift >= 8)
		spi_nor_read_status2(chip, &data[1]);
	else
		spi_nor_read_status1(chip, &data[0]);

	sdata = data[2] << 16 | data[1] << 8 | data[0];
	if (!(sdata & (1 << en4b_shift))) {
		uart_printf("Failed to enable 4byte mode: bit%d\r\n", en4b_shift);
		chip->en_addr_4byte = 0;
		return -1;
	}
	//uart_printf("Enter 4byte address mode, bit%d\r\n", en4b_shift);
	return 0;
}

static int spi_nor_disable_4byte_mode(struct spi_flash_chip *chip)
{
	struct spi_flash_cmd cmd;
	int ret;

	qspi_memset(&cmd, 0, sizeof(struct spi_flash_cmd));
	if (chip->qpi_enabled)
		cmd.cmd_cfg = chip->table + DISABLE_4BYTE_QPI;
	else
		cmd.cmd_cfg = chip->table + DISABLE_4BYTE;

	ret = qspi_start_cmd(chip, &cmd);
	if (ret != 0) {
		uart_printf("Failed to disable 4byte_mode\r\n");
		return ret;
	}

	chip->en_addr_4byte = 0;
	return 0;
}

/**
 * spi_nor_erase - [Interface] erase block(s)
 * @chip: spi flash device structure
 * @addr: address that erase start with, should be blocksize aligned
 * @len: length that want to be erased, should be blocksize aligned
 * Description:
 *   Erase one ore more blocks
 *   The command sequence for the BLOCK ERASE operation is as follows:
 *       06h (WRITE ENBALE command)
 *       D8h (BLOCK ERASE command)
 *       0Fh (GET FEATURES command to read the status register)
 */
int spi_nor_erase(struct spi_flash_chip *chip, uint32_t addr, uint32_t len)
{
	uint8_t status;
	int erase_len;
	int ret = 0;

	/* reset AHB buffer */
	{
		int dummy, dummy_addr;

		if (addr + len + chip->page_size <= chip->size)
			dummy_addr = addr + len;
		else if (addr > QSPI_AHB_BUFF_MAX_SIZE)
			dummy_addr = addr - QSPI_AHB_BUFF_MAX_SIZE;
		else
			dummy_addr = 0;

		if (dummy_addr != addr) {
			dummy_addr += chip->host->cs_addr[chip->cs];
			qspi_memcpy(&dummy, (void *)dummy_addr, 4);
		}
	}
	/* check address align on block boundary */
	if (addr & (4*1024 - 1)) {
		return EINVAL;
	}

	if (len & (4*1024 - 1)) {
		uart_printf("%s: Length not 4K-Sector aligned\n", __func__);
		return EINVAL;
	}

	/* Do not allow erase past end of device */
	if ((len + addr) > chip->size) {
		return EINVAL;
	}

	while (len > 0) {
		spi_nor_write_enable(chip);
		qspi_enable_ahb_preempt(1);
		if (len >= chip->block_size &&
		    !(addr & (chip->block_size - 1))) {
			erase_len = chip->block_size;
			spi_nor_erase_block(chip, addr);
		} else {
			erase_len = 4*1024;
			spi_nor_erase_sector(chip, addr);
		}
		ret = spi_nor_wait(chip, &status);
		
		qspi_enable_ahb_preempt(0);

		if (ret != 0) {
			uart_printf("block erase command wait failed\n");
			goto erase_exit;
		}

		/* Increment page address and decrement length */
		len -= erase_len;
		addr += erase_len;
		if (addr >= chip->size)
			break;
	}
erase_exit:
	return ret;
}

//To be compatible for MMI code so far.
int spi_nor_erase_4k(struct spi_flash_chip *chip, uint32_t addr, uint32_t len)
{
    spi_nor_erase(chip,addr,len);
}

static int spi_nor_read(struct spi_flash_chip *chip, int addr,
			int size, uint8_t *rbuf)
{
	struct spi_flash_cmd cmd;

	if (chip->host->xip_read) {
		addr += chip->host->cs_addr[chip->cs];
		qspi_memcpy(rbuf, (void *)addr, size);
		return 0;
	}

	qspi_memset(&cmd, 0, sizeof(struct spi_flash_cmd));
	cmd.cmd_cfg = chip->table + chip->read_op;
	if (chip->en_addr_4byte) {
		cmd.n_addr = 4;
		cmd.addr[0] = (uint8_t)((addr >> 24) & 0xff);
		cmd.addr[1] = (uint8_t)((addr >> 16) & 0xff);
		cmd.addr[2] = (uint8_t)((addr >> 8) & 0xff);
		cmd.addr[3] = (uint8_t)(addr & 0xff);
	} else {
		cmd.n_addr = 3;
		cmd.addr[0] = (uint8_t)((addr >> 16) & 0xff);
		cmd.addr[1] = (uint8_t)((addr >> 8) & 0xff);
		cmd.addr[2] = (uint8_t)(addr & 0xff);
	}
	cmd.n_rx = size;
	cmd.rx_buf = rbuf;
	cmd.mode = 0xff;

	return qspi_start_cmd(chip, &cmd);
}

/**
 * spi_nor_program_data_to_cache - write data to cache register
 * @chip: SPI_FLASH device structure
 * @page_addr: page to write
 * @column: the location to write to the cache
 * @len: number of bytes to write
 * @wrbuf: buffer held @len bytes
 * @clr_cache: clear cache register or not
 * Description:
 *   Command can be 02h, 32h, 84h, 34h
 *   02h and 32h will clear the cache with 0xff value first
 *   Since it is writing the data to cache, there is no tPROG time.
 */
static int spi_nor_program_data(struct spi_flash_chip *chip,
				uint32_t addr, int len, const uint8_t *wbuf)
{
	struct spi_flash_cmd cmd;

	qspi_memset(&cmd, 0, sizeof(struct spi_flash_cmd));
	cmd.cmd_cfg = chip->table + chip->write_op;
	if (chip->en_addr_4byte) {
		cmd.n_addr = 4;
		cmd.addr[0] = (uint8_t)((addr >> 24) & 0xff);
		cmd.addr[1] = (uint8_t)((addr >> 16) & 0xff);
		cmd.addr[2] = (uint8_t)((addr >> 8) & 0xff);
		cmd.addr[3] = (uint8_t)(addr & 0xff);
	} else {
		cmd.n_addr = 3;
		cmd.addr[0] = (uint8_t)((addr >> 16) & 0xff);
		cmd.addr[1] = (uint8_t)((addr >> 8) & 0xff);
		cmd.addr[2] = (uint8_t)(addr & 0xff);
	}
	cmd.n_tx = len;
	cmd.tx_buf = wbuf;
	cmd.flag = RST_AHB_DOMAIN | SW_PROG_ERASE;

	return qspi_start_cmd(chip, &cmd);
}

static int spi_nor_write_page(struct spi_flash_chip *chip, int addr,
				int size, uint8_t *buf)
{
	uint8_t status;
	int ret = 0;

	spi_nor_write_enable(chip);
	qspi_enable_ahb_preempt(1);

	spi_nor_program_data(chip, addr, size, buf);

	ret = spi_nor_wait(chip, &status);
	qspi_enable_ahb_preempt(0);
	if (ret != 0) {
		uart_printf("error %d reading page 0x%x from cache\n",ret, addr);
		return ret;
	}

	return ret;
}

static int spi_nor_write(struct spi_flash_chip *chip,
			    int addr, int size, uint8_t *buf)
{


	int len, ret;
	/* reset ahb buffer */
	{
		int dummy, dummy_addr;

		if (addr + size + chip->page_size <= chip->size)
			dummy_addr = addr + size;
		else if (addr > QSPI_AHB_BUFF_MAX_SIZE)
			dummy_addr = addr - QSPI_AHB_BUFF_MAX_SIZE;
		else
			dummy_addr = 0;

		if (dummy_addr != addr) {
			dummy_addr += chip->host->cs_addr[chip->cs];
			qspi_memcpy(&dummy, (void *)dummy_addr, 4);
		}
	}

	while (size) {
		len = chip->page_size - (addr & chip->page_mask);
		len = min(min(len, size), chip->tx_max_len);

		ret = spi_nor_write_page(chip, addr, len, buf);
		if (ret != 0) {
			uart_printf("page program failed\n");
			break;
		}

		/* Increment page address and decrement length */
		size -= len;
		addr += len;
		buf += len;
	}

	return ret;
}

static void spi_nor_set_rd_wr_op(struct spi_flash_chip *chip,
				 uint32_t op_mode_rx, uint32_t op_mode_tx)
{
	if (op_mode_rx & SPI_OPM_RX_QUAD) {
		if (chip->qpi_enabled) {
			chip->read_op = READ_FAST_QPI;
		} else {
			if (chip->mfr_id == SPIFLASH_MFR_GIGADEVICE ||
			    chip->mfr_id == SPIFLASH_MFR_ZETTADEVICE ||
			    chip->mfr_id == SPIFLASH_MFR_WINBOND ||
                chip->mfr_id == SPIFLASH_MFR_PUYA ||
                chip->mfr_id == SPIFLASH_MFR_ZBIT) {
				chip->read_op = READ_FAST_QUAD;
			} else {
				chip->read_op = READ_FAST_X4;
			}
		}
	} else if (op_mode_rx & SPI_OPM_RX_DUAL) {
		chip->read_op = READ_FAST_DUAL;
	} else {
		chip->read_op = READ_FAST;
	}

	if (op_mode_tx & SPI_OPM_TX_QUAD) {
		if (chip->qpi_enabled) {
			chip->write_op = PROG_QPI;
		} else {
			if (chip->mfr_id == SPIFLASH_MFR_MXIC)
				chip->write_op = PROG_X4_MXIC;
			else
				chip->write_op = PROG_X4;
		}
	} else {
		chip->write_op = PROG;
	}

	//uart_printf("Set rx_pins: %d tx_pins: %d\r\n", op_mode_rx, op_mode_tx);
}

static int spi_set_read_parameters(struct spi_flash_chip *chip, uint8_t *buf)
{
	struct spi_flash_cmd cmd;
	int ret = 0;

	if (!chip->qpi_enabled)
		return 0;

	qspi_memset(&cmd, 0, sizeof(struct spi_flash_cmd));
	cmd.cmd_cfg = chip->table + SET_READ_PARA_QPI;
	cmd.n_tx = 1;
	cmd.tx_buf = buf;

	ret = qspi_start_cmd(chip, &cmd);
	if (ret != 0)
		uart_printf("err: %d set_read_parameters\n", ret);

	return ret;
}

static int spi_nor_enable_quad(struct spi_flash_chip *chip)
{
	uint8_t data[2];
	uint8_t status, qe_shift;
	uint16_t sdata;
	int ret;

	/*
	 * Macronix use status register bit s6 for QE enable
	 * Winbond/Gigadevice use status register bit S9
	 */
	if(chip->mfr_id == SPIFLASH_MFR_MXIC ||
	   chip->mfr_id == SPIFLASH_MFR_XM)
		qe_shift = 6;
	else
		qe_shift = 9;

	spi_nor_read_status1(chip, &data[0]);
	spi_nor_read_status2(chip, &data[1]);

	sdata = data[1] << 8 | data[0];
	if (sdata & (1 << qe_shift)) {
		//uart_printf("QE already set: bit%d\r\n", qe_shift);
		return 0;
	}
	sdata |= 1 << qe_shift;
	data[0] = sdata & 0xff;
	data[1] = sdata >> 8;

	spi_nor_write_enable(chip);
	if (chip->mfr_id == SPIFLASH_MFR_DOUQI ||
		chip->mfr_id == SPIFLASH_MFR_FMSH ||
		chip->mfr_id == SPIFLASH_MFR_PUYA) {
		spi_nor_write_status1(chip, &data[0], 1);
		ret = spi_nor_wait(chip, &status);
		if (ret != 0) {
			uart_printf("error %d write status register\n", ret);
			return ret;
		}

		spi_nor_write_enable(chip);
		spi_nor_write_status2(chip, &data[1]);
	} else if ((chip->mfr_id == SPIFLASH_MFR_MXIC &&
		    chip->dev_id == 0x2536) ||
		   chip->mfr_id == SPIFLASH_MFR_XM) {
		spi_nor_write_status1(chip, data, 1);
	} else {
		spi_nor_write_status1(chip, data, 2);
	}

	ret = spi_nor_wait(chip, &status);
	if (ret != 0) {
		uart_printf("error %d write status register\n", ret);
		return ret;
	}

	spi_nor_read_status1(chip, &data[0]);
	spi_nor_read_status2(chip, &data[1]);

	sdata = data[1] << 8 | data[0];
	if (!(sdata & (1 << qe_shift))) {
		uart_printf("Failed to enable quad, maybe not supported\r\n");
		return -1;
	}

	return 0;
}

static int spi_nor_disable_qpi(struct spi_flash_chip *chip)
{
	struct spi_flash_cmd cmd;
	int ret = 0;

	if (!chip->qpi_enabled)
		return 0;

	qspi_memset(&cmd, 0, sizeof(struct spi_flash_cmd));
	if (chip->mfr_id == SPIFLASH_MFR_MXIC)
		cmd.cmd_cfg = chip->table + DISABLE_QPI_MXIC;
	else
		cmd.cmd_cfg = chip->table + DISABLE_QPI;

	ret = qspi_start_cmd(chip, &cmd);
	if (ret != 0)
		return ret;

	chip->qpi_enabled = 0;
	chip->table = cmd_table;

	//uart_printf("QPI Disabled\r\n");
	return 0;
}

/*
 * QPI dummy clock depend on bus frequency
 *
 * Giga: {4, 6, 8, 8}, Winbond: {2, 4, 6, 8}
 * Macronix use a different way to set dummy clock
 */
static uint8_t qpi_dummy_wb[4][2] = { {2, 26}, {4, 50}, {6, 80}, {8, 104} };
static uint8_t qpi_dummy_gd[4][2] = { {4, 80}, {4, 80}, {6, 100}, {8, 133} };

static int spi_nor_enable_qpi(struct spi_flash_chip *chip)
{
	struct spi_flash_cmd cmd;
	int ret = 0, i;
	uint8_t cfg;
	uint8_t (*qpi_dummy)[2];

	if (chip->qpi_enabled)
		return 0;

	qspi_memset(&cmd, 0, sizeof(struct spi_flash_cmd));
	if (chip->mfr_id == SPIFLASH_MFR_MXIC)
		cmd.cmd_cfg = chip->table + ENABLE_QPI_MXIC;
	else
		cmd.cmd_cfg = chip->table + ENABLE_QPI;

	ret = qspi_start_cmd(chip, &cmd);
	if (ret != 0)
		return ret;

	chip->qpi_enabled = 1;
	chip->table = cmd_table_qpi;

	if(chip->mfr_id == SPIFLASH_MFR_MXIC) {
		chip->qpi_dummy = 6;
	} else {
		if (chip->mfr_id == SPIFLASH_MFR_GIGADEVICE)
			qpi_dummy = qpi_dummy_gd;
		else
			qpi_dummy = qpi_dummy_wb;

		for (i = 0; i < 4; i++) {
			if(chip->host->bus_clk <= qpi_dummy[i][1]) {
				cfg = i << 4;
				chip->qpi_dummy = qpi_dummy[i][0];
				break;
			}
		}

		ret = spi_set_read_parameters(chip, &cfg);
		if (ret != 0)
			return ret;
	}

	spi_nor_set_rd_wr_op(chip, SPI_OPM_RX_QUAD, SPI_OPM_TX_QUAD);
	//uart_printf("QPI Enabled, P5-4:%d dummy_cycles:%d\r\n", i, chip->qpi_dummy);
	return 0;
}

static const struct spi_flash_ops qspi_ops = {
	spi_nor_reset,
	spi_nor_read_id,
	spi_nor_read_uid,
	spi_nor_read,
	spi_nor_write,
	spi_nor_erase,
	spi_nor_erase_4k,
};

/**
 * spi_nor_init - [Interface] Init SPI_FLASH device driver
 * @spi: spi device structure
 * @chip_ptr: pointer point to spi nand device structure pointer
 */
struct spi_flash_chip *spi_nor_init(struct qspi_host *host, int cs,
				    int rx_mode, int tx_mode, int qpi)
{
	struct spi_flash_chip *chip = &nor_chip;
	uint8_t id[SPINOR_MAX_ID_LEN] = {0};

	if (chip->qpi_enabled)
		spi_nor_disable_qpi(chip);

	qspi_memset(chip, 0, sizeof(struct spi_flash_chip));
	chip->cs = cs >= QSPI_CS_MAX ? QSPI_CS_A1 : cs;
	chip->ops = (void *)&qspi_ops;
	chip->table = cmd_table;
	chip->host = host;
	chip->size = 16*1024*1024;
	chip->block_size = 1 << 16;
	chip->page_size = 1 << 7;

	spi_nor_reset(chip);
	spi_nor_read_id(chip, id);

	if((id[0]==0xff && id[1] ==0xff && id[2]==0xff) || (id[0]==0x0 && id[1] ==0x0 && id[2]==0x0))
		return NULL;
	
	chip->mfr_id = id[0];
	chip->dev_id = id[1] << 8 | id[2];
	spi_nor_scan_id_table(chip);
	chip->page_size = 1 << 7;
	chip->block_shift = ilog2(chip->block_size);
	chip->page_shift = ilog2(chip->page_size);
	chip->page_mask = chip->page_size - 1;
	uart_printf("SPI-NOR: mfr_id: 0x%x, dev_id: 0x%x\r\n",chip->mfr_id, chip->dev_id);
	uart_printf("chipsize: 0x%lx\r\n",chip->size);

	/*
	 * The Enter 4-Byte Address Mode instruction will allow 32-bit address
	 * (A31-A0) to be used to access the memory array beyond 128Mb
	 */
	if (chip->size > 16*1024*1024)
		spi_nor_enable_4byte_mode(chip);

	spi_nor_enable_quad(chip);
	if (qpi)
		spi_nor_enable_qpi(chip);

	spi_nor_set_rd_wr_op(chip, rx_mode, tx_mode);

	/*
	 * TODO:
	 * Maybe change due to different vendor
	 */
	if (chip->mfr_id == SPIFLASH_MFR_MXIC) {
		qspi_config_lookup_tbl(chip, chip->table + PGM_ERS_SUSPEND_MXIC,
					0x0, QSPI_LUT_SEQID14);
		qspi_config_lookup_tbl(chip, chip->table + PGM_ERS_RESUME_MXIC,
					0x0, QSPI_LUT_SEQID15);
	} else {
		qspi_config_lookup_tbl(chip, chip->table + PGM_ERS_SUSPEND,
					0x0, QSPI_LUT_SEQID14);
		qspi_config_lookup_tbl(chip, chip->table + PGM_ERS_RESUME,
					0x0, QSPI_LUT_SEQID15);
	}
	qspi_enable_xip(chip, chip->table + chip->read_op);

	if (qspi_preinit_lookup_tbl(chip) != 0) {
		uart_printf("preinit_lookup_tbl failed, check cmd table\r\n");
		return NULL;
	}
    if (!host->en_tx_dma)
        chip->tx_max_len = QSPI_TX_BUFF_MAX << 2;
    else
        chip->tx_max_len = chip->page_size;
        
	return chip;
}
/**
 * spi_nor_init - [Interface] Init SPI_FLASH device driver
 * @spi: spi device structure
 * @chip_ptr: pointer point to spi nand device structure pointer
 */
struct spi_flash_chip *spi_nor_init2(struct qspi_host *host, int cs,
				    int rx_mode, int tx_mode, int qpi)
{
	struct spi_flash_chip *chip = &nor_chip2;
	uint8_t id[SPINOR_MAX_ID_LEN] = {0};

	if (chip->qpi_enabled)
		spi_nor_disable_qpi(chip);

	qspi_memset(chip, 0, sizeof(struct spi_flash_chip));
	chip->cs = cs >= QSPI_CS_MAX ? QSPI_CS_A1 : cs;
	chip->ops = (void *)&qspi_ops;
	chip->table = cmd_table;
	chip->host = host;
	chip->size = 16*1024*1024;
	chip->block_size = 1 << 16;
	chip->page_size = 1 << 7;


	spi_nor_reset(chip);


	spi_nor_read_id(chip, id);

	if((id[0]==0xff && id[1] ==0xff && id[2]==0xff) || (id[0]==0x0 && id[1] ==0x0 && id[2]==0x0))
		return NULL;

	chip->mfr_id = id[0];
	chip->dev_id = id[1] << 8 | id[2];
	spi_nor_scan_id_table(chip);
	chip->page_size = 1 << 7;
	chip->block_shift = ilog2(chip->block_size);
	chip->page_shift = ilog2(chip->page_size);
	chip->page_mask = chip->page_size - 1;
	uart_printf("SPI-NOR: mfr_id: 0x%x, dev_id: 0x%x\r\n",chip->mfr_id, chip->dev_id);
	uart_printf("chip2size: 0x%lx\r\n",chip->size);

	/*
	 * The Enter 4-Byte Address Mode instruction will allow 32-bit address
	 * (A31-A0) to be used to access the memory array beyond 128Mb
	 */
	if (chip->size > 16*1024*1024)
		spi_nor_enable_4byte_mode(chip);

	spi_nor_enable_quad(chip);

	if (qpi)
		spi_nor_enable_qpi(chip);

	spi_nor_set_rd_wr_op(chip, rx_mode, tx_mode);

	/*
	 * TODO:
	 * Maybe change due to different vendor
	 */
	if (chip->mfr_id == SPIFLASH_MFR_MXIC) {
		qspi_config_lookup_tbl(chip, chip->table + PGM_ERS_SUSPEND_MXIC,
					0x0, QSPI_LUT_SEQID14);
		qspi_config_lookup_tbl(chip, chip->table + PGM_ERS_RESUME_MXIC,
					0x0, QSPI_LUT_SEQID15);
	} else {
		qspi_config_lookup_tbl(chip, chip->table + PGM_ERS_SUSPEND,
					0x0, QSPI_LUT_SEQID14);
		qspi_config_lookup_tbl(chip, chip->table + PGM_ERS_RESUME,
					0x0, QSPI_LUT_SEQID15);
	}
	qspi_enable_xip(chip, chip->table + chip->read_op);
	if (qspi_preinit_lookup_tbl(chip) != 0) {
		uart_printf("preinit_lookup_tbl failed, check cmd table\r\n");
		return NULL;
	}
    if (!host->en_tx_dma)
        chip->tx_max_len = QSPI_TX_BUFF_MAX << 2;
    else
        chip->tx_max_len = chip->page_size;

	return chip;
}





