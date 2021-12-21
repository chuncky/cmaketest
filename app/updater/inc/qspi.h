#ifndef _QSPI_H_
#define _QSPI_H_

#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * QSPI register address offset definitions
 */
#define QSPI_MCR        0x000
#define QSPI_TCR        0x004
#define QSPI_IPCR       0x008
#define QSPI_FLSHCR     0x00C
#define QSPI_BUF0CR     0x010
#define QSPI_BUF1CR     0x014
#define QSPI_BUF2CR     0x018
#define QSPI_BUF3CR     0x01C
#define QSPI_BFGENCR    0x020
#define QSPI_SOCCR      0x024
#define QSPI_BUF0IND    0x030
#define QSPI_BUF1IND    0x034
#define QSPI_BUF2IND    0x038
#define QSPI_SFAR       0x100
#define QSPI_SFACR      0x104
#define QSPI_SMPR       0x108
#define QSPI_RBSR       0x10C
#define QSPI_RBCT       0x110
#define QSPI_TBSR       0x150
#define QSPI_TBDR       0x154
#define QSPI_TBCT       0x158
#define QSPI_SR         0x15C
#define QSPI_FR         0x160
#define QSPI_RSER       0x164
#define QSPI_SPNDST     0x168
#define QSPI_SPTRCLR    0x16C
#define QSPI_SFA1AD     0x180
#define QSPI_SFA2AD     0x184
#define QSPI_SFB1AD     0x188
#define QSPI_SFB2AD     0x18C
#define QSPI_DLPR       0x190
#define QSPI_RBDR0      0x200
#define QSPI_LUTKEY     0x300
#define QSPI_LCKCR      0x304
#define QSPI_LUT0       0x310

/*
 * QSPI clock freqency definitions
 */
#define QSPI_CLOCK_13MHZ    1
#define QSPI_CLOCK_26MHZ    3
#define QSPI_CLOCK_39MHZ    4
#define QSPI_CLOCK_52MHZ    5
#define QSPI_CLOCK_78MHZ    6
#define QSPI_CLOCK_104MHZ   7

/*
 * QSPI LUT instrcutions
 */
#define QSPI_INST_STOP        0x0
#define QSPI_INST_CMD         0x1
#define QSPI_INST_ADDR        0x2
#define QSPI_INST_DUMMY       0x3
#define QSPI_INST_MODE        0x4
#define QSPI_INST_MODE2       0x5
#define QSPI_INST_MODE4       0x6
#define QSPI_INST_READ        0x7
#define QSPI_INST_WRITE       0x8
#define QSPI_INST_JMP_ON_CS   0x9
#define QSPI_INST_ADDR_DDR    0xA
#define QSPI_INST_MODE_DDR    0xB
#define QSPI_INST_MODE2_DDR   0xC
#define QSPI_INST_MODE4_DDR   0xD
#define QSPI_INST_READ_DDR    0xE
#define QSPI_INST_WRITE_DDR   0xF
#define QSPI_INST_DATA_LEARN  0x10

/*
 * QSPI LUT pads
 */
#define QSPI_PAD_1X 0x0
#define QSPI_PAD_2X 0x1
#define QSPI_PAD_4X 0x2

/*
 * QSPI TX/RX fifo size
 */
#define QSPI_TX_FIFO_SIZE   128
#define QSPI_RX_FIFO_SIZE   128

/*
 * QSPI flash device address offset
 */
#define QSPI_FLASH_A1_OFFSET    0x00000000
#define QSPI_FLASH_A2_OFFSET    0x08000000
#define QSPI_FLASH_B1_OFFSET    0x10000000
#define QSPI_FLASH_B2_OFFSET    0x18000000

/*
 * QSPI TX FIFO watermark
 */
#define QSPI_TX_FIFO_WATERMARK_4_BYTE   0
#define QSPI_TX_FIFO_WATERMARK_8_BYTE   1
#define QSPI_TX_FIFO_WATERMARK_32_BYTE  7
#define QSPI_TX_FIFO_WATERMARK_64_BYTE  15

#define RSER_TBTDE          (1 << 25) /* TX Buffer Fill DMA Enable */

typedef struct {
    unsigned addr;  /* offset address */
    unsigned value; /* register value */
} qspi_register_config_t;

typedef union {
    struct {
        unsigned oprndi0 : 8;   /* operand for instr0 */
        unsigned pad0    : 2;   /* pad information for instr0 */
        unsigned instr0  : 6;   /* instruction 0 */
        unsigned oprndi1 : 8;   /* operand for instr1 */
        unsigned pad1    : 2;   /* pad information for instr1 */
        unsigned instr1  : 6;   /* instruction 1 */
    } s;
    unsigned value;
} qspi_lut_config_t;

int  qspi_init(int clock_freq, unsigned offset_addr);
void qspi_register_config(const qspi_register_config_t *config);
int  qspi_lut_config(const qspi_lut_config_t *config, unsigned index);
void qspi_cmd(unsigned seqid, unsigned addr);
int  qspi_read(unsigned seqid, unsigned addr, void *data, size_t size);
int qspi_write(unsigned seqid, unsigned addr, const void *data, size_t size);

#ifdef __cplusplus
}
#endif

#endif /* _QSPI_H_ */
