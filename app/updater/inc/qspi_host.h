#ifndef _QSPI_HOST_H
#define _QSPI_HOST_H
#include "common.h"
#include "qspi_flash.h"
#define QSPI_RX_BUFF_MAX 32
#define QSPI_TX_BUFF_MAX 32
#define QSPI_TX_BUFF_POP_MIN 16
#define QSPI_AHB_BUFF_MAX_SIZE (256*3)

#define QSPI_DMA_TX_CHANNEL 28
#define ENABLE_QSPI_INT
#define ENABLE_QSPI_DMA





struct spi_flash_cmd;

enum {
	QSPI_LUT_NOTSET = -1,
	QSPI_LUT_SEQID0 = 0,
	QSPI_LUT_SEQID1,
	QSPI_LUT_SEQID2,
	QSPI_LUT_SEQID3,
	QSPI_LUT_SEQID4,
	QSPI_LUT_SEQID5,
	QSPI_LUT_SEQID6,
	QSPI_LUT_SEQID7,
	QSPI_LUT_SEQID8,
	QSPI_LUT_SEQID9,
	QSPI_LUT_SEQID10,
	QSPI_LUT_SEQID11,
	QSPI_LUT_SEQID12,
	QSPI_LUT_SEQID13,
	QSPI_LUT_SEQID14,
	QSPI_LUT_SEQID15
};

enum {
	QSPI_NORMAL_MODE = 0,
	QSPI_DISABLE_MODE,
	QSPI_STOP_MODE
};

enum {
	QSPI_FUNC_CLK_26MHZ = 0,
	QSPI_FUNC_CLK_52MHZ,
	QSPI_FUNC_CLK_78MHZ,
	QSPI_FUNC_CLK_104MHZ,
	QSPI_FUNC_CLK_156MHZ,
	QSPI_FUNC_CLK_208MHZ,
	QSPI_FUNC_CLK_312MHZ,
	QSPI_FUNC_CLK_416MHZ
};

enum {
	QSPI_CS_A1 = 0,
	QSPI_CS_A2,
	QSPI_CS_B1,
	QSPI_CS_B2,
	QSPI_CS_MAX
};

enum {
	QSPI_CPU_XFER = 0,
	QSPI_DMA_XFER
};

enum QSPI_PAD_E {
	QSPI_PAD_1X = 0x0,
	QSPI_PAD_2X = 0x1,
	QSPI_PAD_4X = 0x2,
	QSPI_PAD_RSVD = 0x3
};

struct qspi_host
{
	struct spi_flash_cmd *cmd;
	int bytes_left;
	volatile int complete;
	int wmrk;
	int cs_addr[QSPI_CS_MAX];
	int use_intr;
	int	en_tx_dma;
	int		use_dma;
	int use_xip;
	int xip_read;
	int bus_clk;
	int lut_map;
};



int qspi_enable_xip(struct spi_flash_chip *chip,
			struct spi_flash_cmd_cfg *cmd_cfg);
int qspi_preinit_lookup_tbl(struct spi_flash_chip *chip);
struct qspi_host * qspi_host_init(int cs, int mhz, int use_xip);
int qspi_start_cmd(struct spi_flash_chip *chip, struct spi_flash_cmd *cmd);

#endif
