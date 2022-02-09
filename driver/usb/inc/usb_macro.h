#ifndef _USB_MACRO_INCLUDED
#define _USB_MACRO_INCLUDED

/*===========================================================================

            U S B   M A C R O   H E A D E R   F I L E

DESCRIPTION
  Configuration for Wukong MIFI.

  Copyright (c) 2012 by MARVELL Incorporated. All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  when       who          what, where, why
--------   ---        ----------------------------------------------------------
  02/02/12   jinzhou      Initial release.

===========================================================================*/

/* USB Modem RX Endpoint. */
#define MODEM_RX_ENDPOINT                   0x01

/* USB Modem TX Endpoint. */
#define MODEM_TX_ENDPOINT                   0x02

/* USB diag RX Endpoint. */
#define DIAG_RX_ENDPOINT                    0x03

/* USB diag TX Endpoint. */
#define DIAG_TX_ENDPOINT                    0x04

/* USB net control Endpoint. */
#define USB_NET_CRTL_ENDPOINT               0x05

/* USB net RX Endpoint. */
#define USB_NET_RX_ENDPOINT                 0x06

/* USB net TX Endpoint. */
#define USB_NET_TX_ENDPOINT                 0x07

/* USB mass storage RX Endpoint */
#define USB_MASS_STORAGE_RX_ENDPOINT        0x08

/* USB mass storage TX Endpoint */
#define USB_MASS_STORAGE_TX_ENDPOINT        0x09

/* USB Modem control Endpoint. */
#define MODEM_CTRL_ENDPOINT                 0x0A

/* USB diag control Endpoint. */
#define DIAG_CTRL_ENDPOINT                  0x0B

/* USB sulog TX Endpoint. */
#define SULOG_TX_ENDPOINT                   0x0C

/* USB sulog RX Endpoint. */
#define SULOG_RX_ENDPOINT                   0x0D

/* USB UART TX Endpoint. */
#define USB_UART_TX_ENDPOINT                0x0E

/* USB UART RX Endpoint. */
#define USB_UART_RX_ENDPOINT                0x0F

/* USB UART TX Endpoint. */
#define MODEM_1_TX_ENDPOINT                0x0E

/* USB UART RX Endpoint. */
#define MODEM_1_RX_ENDPOINT                0x0F

/* merger from 1802s as follows  */
/* USB Modem 2rd control point*/
#define AT_CTRL_ENDPOINT                    0x0C

/* USB Modem 2rd RX point*/
#define AT_RX_ENDPOINT                      0x0D

/* USB Modem 2rd TX point*/
#define AT_TX_ENDPOINT                      0x0E

#endif /* _USB_MACRO_INCLUDED */
