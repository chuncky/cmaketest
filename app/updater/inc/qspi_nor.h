
#ifndef _SPI_NOR_H
#define _SPI_NOR_H

#include "qspi_flash.h"

#define STATUS1_BUSY	(0x1)
#define STATUS1_WEL	(0x1 << 1)
#define STATUS1_BP0	(0x1 << 2)
#define STATUS1_BP1	(0x1 << 3)
#define STATUS1_BP2	(0x1 << 4)
#define STATUS1_TB	(0x1 << 5)
#define STATUS1_SEC	(0x1 << 6)
#define STATUS1_SRP0	(0x1 << 7)

#define STATUS2_SRP1	(0x1)
#define STATUS2_QE	(0x1 << 1)
#define STATUS2_LB1	(0x1 << 3)
#define STATUS2_LB2	(0x1 << 4)
#define STATUS2_LB3	(0x1 << 5)
#define STATUS2_CMP	(0x1 << 6)
#define STATUS2_SUS	(0x1 << 7)

/*SPI NAND chip options*/
#define SPINOR_NEED_PLANE_SELECT	(1 << 0)
#define SPINOR_NEED_DIE_SELECT	(1 << 1)

#define SPINOR_MFR_MICRON		0x2C
#define SPINOR_MAX_ID_LEN		3

struct spi_nor_info {
	char		*name;
	uint8_t		mfr_id;
	uint16_t	dev_id;
	uint32_t	page_size;
	uint32_t	pages_per_blk;
	uint32_t	total_size;
	uint32_t	options;
};

struct spi_flash_chip *spi_nor_init(struct qspi_host *host, int cs,
				    int rx_mode, int tx_mode, int qpi);
int spi_nor_test(void);

#endif
