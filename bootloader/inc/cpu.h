#ifndef _CPU_H_
#define _CPU_H_

#include <stdint.h>
#include <stdio.h>
#include "reg.h"

/* MFP */
#define CRANE_MFPR_BASE    0xD401E000
/* IRQ CONTROLLER */
#define M_ICU_BASE         0xD4282000

/* UART */
#define CRANE_UART0_BASE    0xD4017000

#define UART0_IRQ    27
struct crane_uart_registers {
    union {
        uint32_t rbr; //0x0
        uint32_t thr;
        uint32_t dll;
    } uartx0;
    union {
        uint32_t dlh; //0x4
        uint32_t ier;
    } uartx4;
    union  {
        uint32_t iir; //0x8
        uint32_t fcr;
    } uartx8;
    uint32_t lcr;   //0xc
    uint32_t mcr;   //0x10
    uint32_t lsr;
    uint32_t msr;
    uint32_t scr;
    uint32_t isr; //0x20
    uint32_t ffor;
    uint32_t abr;
    uint32_t acr;
};
#define IIR_NIP         (1 << 0)            /* no interrupt pending */
#define IIR_ABL         (1 << 4)            /* Auto-band Lock */
#define LSR_DR          (1 << 0)            /* Data Ready */
#define LSR_TDRQ        (1 << 5)            /* Transmit Data Request */
#define FCR_RESETTF     (1 << 2)            /* reset transmit fifo */
#define FCR_RESETRF     (1 << 1)            /* reset receive fifo*/
#define LCR_WLS(x)      (((x) & 0x3) << 0)  /* word length select*/
#define LCR_DLAB        (1 << 7)            /* Divisor Latch Access Bit */
#define MCR_OUT2        (1 << 3)            /* enable uart interrupt */
#define FCR_TRFIFOE     (1 << 0)            /* transmit and receive fifo enable */
#define FCR_TIL         (1 << 3)            /* interrupt/DMA request when FIFO empty */
#define FCR_ITL(x)      (((x) & 0x3) << 6)  /* interrupt trigger threshold 8 bytes */
#define IER_RAVIE       (1 << 0)            /* receiver data available */
#define IER_TIE         (1 << 1)            /* transmit data request interrupt enable */
#define IER_RLSE        (1 << 2)            /* receiver line status */
#define IER_RTOIE       (1 << 4)            /* receiver timeout */
#define IER_UUE         (1 << 6)            /* uart unit enable */
#define IER_HSE         (1 << 8)            /* high speed uart enable */
#define LSR_TEMT        (1 << 6)            /* transmitter empty */
#define LSR_BI          (1 << 4)            /* break interrupt */
#define LSR_FE          (1 << 3)            /* framing error */
#define LSR_PE          (1 << 2)            /* parity error */
#define LSR_OE          (1 << 1)            /* overrun error */
#define ABR_ABT         (1 << 3)            /* table used to calc baud rates */
#define ABR_ABUP        (1 << 2)            /* uart programs divisor latch reg */
#define ABR_ABLIE       (1 << 1)            /* enable auto-baud-lock interrupt*/
#define ABR_ABE         (1 << 0)            /* enable auto-baud */

/* Timer */
#define CRANE_APB_TIMER0_BASE   0xD4014000

#define APB_TIMER0_MAX_DEV_NUM  3   /* counter */
#define APB_TIMER0_MAX_CHAN_NUM 3   /* match */
#define APB_TIMER0_DEV0_IRQ     13
#define APB_TIMER0_DEV1_IRQ     14
#define APB_TIMER0_DEV2_IRQ     14

struct crane_timer_registers {
    uint32_t cer;           /* 0x0 */
    uint32_t cmr;           /* 0x4 */
    uint32_t crsr;          /* 0x8 */
    uint32_t ccr;           /* 0xc */
    uint32_t t0_match[3];   /* 0x10 */
    uint8_t pad0[0x20 - 0x18 - 4];
    uint32_t t1_match[3];   /* 0x20 */
    uint8_t pad1[0x30 - 0x28 - 4];
    uint32_t t2_match[3];   /* 0x30 */
    uint8_t pad2[0x40 - 0x38 - 4];
    uint32_t plvr[3];       /* 0x40 */
    uint8_t pad3[0x50 - 0x48 - 4];
    uint32_t plcr[3];       /* 0x50 */
    uint8_t pad4[0x60 - 0x58 - 4];
    uint32_t ier[3];        /* 0x60 */
    uint8_t pad5[0x70 - 0x68 - 4];
    uint32_t icr[3];        /* 0x70 */
    uint8_t pad6[0x80 - 0x78 - 4];
    uint32_t sr[3];         /* 0x80 */
    uint8_t pad7[0x90 - 0x88 - 4];
    uint32_t cr[3];         /* 0x90 */
    uint8_t pad8[0xb0 - 0x98 - 4];
    uint32_t wfar;          /* 0xb0 */
    uint32_t wsar;          /* 0xb4 */
    uint32_t wmer;          /* 0xb8 */
    uint32_t wmr;           /* 0xbc */
    uint32_t wsr;           /* 0xc0 */
    uint32_t wicr;          /* 0xc4 */
    uint32_t wcr;           /* 0xc8 */
    uint32_t wvr;           /* 0xcc */
};

#define HW_PLATFORM_TYPE_SILICON 0
#define HW_PLATFORM_TYPE_FPGA    1
#define HW_PLATFORM_TYPE_ZEBU_Z1 2
#define HW_PLATFORM_TYPE_VDK     3

#define APBC_BASE     0xD4015000

#define PMU_MAIN_BASE 0xD4050000

/* PMU AP */
#define PMU_BASE 0xD4282800

#define PMU_USB_CLK_RES_CTRL         0x5C
#define USB_AXI_CLK_EN               (1 << 3)
#define USB_DONGLE_CLK_GATE_MASK     (1 << 1)
#define USB_AXI_RESET                (1 << 0)

/* USB dev */
#define CRANE_USB_BASE 0xD4208000

struct crane_udc {
    uint32_t pad0[80];
    uint32_t usbcmd;        /* 0x140 */
    uint32_t usbsts;        /* 0x144 */
    uint32_t usbintr;       /* 0x148 */
    uint32_t frindex;
    uint32_t pad1[1];
    uint32_t devaddr;       /* 0x154 */
    uint32_t epinitaddr;    /* 0x158 */
    uint32_t pad2[10];
    uint32_t portsc;        /* 0x184 */
    uint32_t pad3[8];
    uint32_t usbmode;       /* 0x1a8 */
    uint32_t epstat;        /* 0x1ac */
    uint32_t epprime;       /* 0x1b0 */
    uint32_t epflush;       /* 0x1b4 */
    uint32_t pad4;
    uint32_t epcomp;        /* 0x1bc */
    uint32_t epctrl[16];    /* 0x1c0 */
};

#define EPCTRL_TX_ENABLE             (1 << 23)
#define EPCTRL_RX_ENABLE             (1 << 7)
#define EPCTRL_TX_EP_TYPE_SHIFT      18
#define EPCTRL_RX_EP_TYPE_SHIFT      2

/* USB phy */
#define UTMI_BASE 0xD4207000

#define UTMI_USB2_REG01              0x4
#define UTMI_USB2_REG0D              0x34
#define PLL_IS_READY                 (1 << 0)

/* QSPI */
#define QSPI_IPB_BASE   0xD420B000

/* DMA */
#define DMA_BASE 0xD4000000

#endif /* _CPU_H_ */
