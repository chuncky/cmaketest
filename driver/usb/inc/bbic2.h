/* ************************************************************************** *
 *                                                                            *
 *    Copyright (c) 2006, Marvell company                                   *
 *    All rights reserved.                                                    *
 *                                                                            *
 *    File name   : bbic2.h                                                  *
 *    Description : BBIC2 register definitions.                        *
 *                                                                            *
 *    History        :                                                        *
 *    1. Date        : Feb 19, 2005                                           *
 *       Author      :                                               *
 *       Description :                                                        *
 *                                                                            *
 * ************************************************************************** */
#ifndef BBIC2_H
#define BBIC2_H
//------------define interrupt vectors ------------------
#define IRQ_NUM_WATCHDOG      0
#define IRQ_NUM_RESERVED1      1
#define IRQ_NUM_UART0              2
#define IRQ_NUM_UART1              3
#define IRQ_NUM_EARIN               4  //earphone in
#define IRQ_NUM_EAROUT            5  //earphone out
#define IRQ_NUM_DMA                  6
#define IRQ_NUM_GPIO0               7
#define IRQ_NUM_GPIO1               8
#define IRQ_NUM_GPIO2               9
#define IRQ_NUM_GPIO3              10
#define IRQ_NUM_SCI                   11
#define IRQ_NUM_I2C                   12
#define IRQ_NUM_SSI                   13
#define IRQ_NUM_TIMER0             14
#define IRQ_NUM_TIMER1             15
#define IRQ_NUM_TIMER2             16
#define IRQ_NUM_TIMER3             17
#define IRQ_NUM_RTC                   18
#define IRQ_NUM_SPI                   19
#define IRQ_NUM_GPIO4               20
#define IRQ_NUM_GPIO5               21
#define IRQ_NUM_PMINT0		     22
#define IRQ_NUM_PMINT1		     23
#define IRQ_NUM_CLK_GEN           24
#define IRQ_NUM_ZTIMER0           25
#define IRQ_NUM_RESERVED2       26
#define IRQ_NUM_MAILBOX           27
#define IRQ_NUM_TDMA1              28
#define IRQ_NUM_TDMA2              29
#define IRQ_NUM_SDIO                 30
#define IRQ_NUM_USB                   31

#define IRQ_NUM_KEYPAD             IRQ_NUM_GPIO1

//DSP Interrupt clear
#define DSP_INTERRUPT_CLEAR 0x10
#define CLK_GEN_INTERRUPT_CLEAR 0x80
                                                                                   
///////////////////////////////////////////////////////////////////////////////
//                                                                           //
//                           ARM subSystem                                   //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// ARM Base Address                                                          //
///////////////////////////////////////////////////////////////////////////////

#define APBCLKGEN_BASE       0xF8000000   //APB Device Clock & Reset Generator Base Address
#define SYS_CONTROL_BASE   0xF8000040   //System Control Registers Base Address
#define POW_UTILITY_BASE   0xF8000080   //Power Utility Base Address
//#define USBPHY_BASE            0xF80000C0  //USB PHY
#define USBPHY_BASE 		0xD4207000 //@Hongji ,according to tavorl usb note book
#define SRAM_TEST_BASE      0xF80000E0   //SRAM setting and test
#define PM_BASE                     0xF8000100   //Power Management 
#define PWL_BASE                   0xF8001000   //Pulse Width Light 
#define GPIO0_BASE                0xF8010000   //GPIO0 Base Address
#define GPIO1_BASE                0xF8011000   //GPIO1 Base Address
#define GPIO2_BASE                0xF8012000   //GPIO2 Base Address
#define GPIO3_BASE                0xF8013000   //GPIO3 Base Address
#define GPIO4_BASE                0xF8014000   //GPIO4 Base Address
#define GPIO5_BASE                0xF8015000   //GPIO5 Base Address
#define UART0_BASE               0xF8020000   //UART0 Base Address
#define UART1_BASE               0xF8024000   //UART1 Base Address
#define SCI_BASE                    0xF8040000   //SIM/PIM Card Interface
#define LCD_BASE                    0xF8060000   //Serial LCD Interface
#define KEYPAD_BASE              0xF8070000   //KeyPad Base Address
#define RTC_BASE                    0xF8080000   //Real Time Clock Base
#define TIMER0_BASE              0xF8081000   //TIMER0 Base Address
#define TIMER1_BASE              0xF8082000   //TIMER1 Base Address
#define TIMER2_BASE              0xF8083000   //TIMER2 Base Address
#define TIMER3_BASE              0xF8084000   //TIMER3 Base Address
#define WDOG_BASE                0xF8090000   //WatchDog Base Address
//#define USB_BASE	       	0xFFFF7000 //USB device base address
//@Hongji
#define USB_BASE	       	0xD4208000 //USB OTG base address
#define SDIO_BASE                  0xFFFF9000   //SD card interface
#define SSMC_BASE                  0xFFFFB000   //Synchronous Static Memory Controller Base Address
#define DMAC_BASE                 0xFFFFD000   //Direct Memory Access Controller Base Address
#define NAND_BASE                 0xFFFFD600  //NAND Flash controller base address
#define VIC_BASE                    0xFFFFF000   //Vectored Interrupt Controller Base Address

///////////////////////////////////////////////////////////////////////////////
//                ARM APB Clock Generation Registers                         //
//                  Base Address: 0xF8000000                                 //
///////////////////////////////////////////////////////////////////////////////
#define CYG_DEVICE_APB_CLK_1MSR       \
    ((volatile U32 *)(APBCLKGEN_BASE + 0x000))//1MS Clock register
#define CYG_DEVICE_APB_UART1_CLKR     \
    ((volatile U32 *)(APBCLKGEN_BASE + 0x004))//UART1 Clock Register, 
                                                     //reserved
#define CYG_DEVICE_APB_MCI_CLKR       \
    ((volatile U32 *)(APBCLKGEN_BASE + 0x008))//MCI Clock register
#define CYG_DEVICE_APB_SCI_CLKR       \
    ((volatile U32 *)(APBCLKGEN_BASE + 0x00c))//SCI Clock register
#define CYG_DEVICE_APB_RTC_CLKR       \
    ((volatile U32 *)(APBCLKGEN_BASE + 0x010))//RTC Clock register, 
                                                     //reserved
#define CYG_DEVICE_APB_WDOG_CLKR      \
    ((volatile U32 *)(APBCLKGEN_BASE + 0x014))//WatchDog Clock register
#define CYG_DEVICE_APB_TIMER0_CLKR    \
    ((volatile U32 *)(APBCLKGEN_BASE + 0x020))//TIMER0 Clock register
#define CYG_DEVICE_APB_TIMER1_CLKR    \
    ((volatile U32 *)(APBCLKGEN_BASE + 0x024))//TIMER1 Clock register
#define CYG_DEVICE_APB_TIMER2_CLKR    \
    ((volatile U32 *)(APBCLKGEN_BASE + 0x028))//TIMER2 Clock register
#define CYG_DEVICE_APB_TIMER3_CLKR    \
    ((volatile U32 *)(APBCLKGEN_BASE + 0x02c))//TIMER3 Clock register

///////////////////////////////////////////////////////////////////////////////
//                          ARM System Registers                             //
//                          Base Address: 0xF8000040                         //
///////////////////////////////////////////////////////////////////////////////
#define CYG_DEVICE_ARM_INT            \
    ((volatile U32 *)(SYS_CONTROL_BASE + 0x000))//Interrupt register
#define CYG_DEVICE_ARM_CLK            \
    ((volatile U32 *)(SYS_CONTROL_BASE + 0x004))//Clock register
#define CYG_DEVICE_ARM_RESET          \
    ((volatile U32 *)(SYS_CONTROL_BASE + 0x008))//Reset register
#define CYG_DEVICE_ARM_CONFR          \
    ((volatile U32 *)(SYS_CONTROL_BASE + 0x00c))//Configuration register
#define CYG_DEVICE_ARM_GPR            \
    ((volatile U32 *)(SYS_CONTROL_BASE + 0x010))//ARM general purpose register 0: sys remap
#define CYG_DEVICE_ARM_CIDL            \
    ((volatile U32 *)(SYS_CONTROL_BASE + 0x020))//Unique Chip ID[31:0]
#define CYG_DEVICE_ARM_CIDH            \
    ((volatile U32 *)(SYS_CONTROL_BASE + 0x024))//Unique Chip ID[63:32]
#define CYG_DEVICE_ARM_PLLCTL          \
    ((volatile U32 *)(SYS_CONTROL_BASE + 0x028))//PLL, UDAC, RTC control & status
                           
///////////////////////////////////////////////////////////////////////////////
//                          Power Utility Registers                          //
//                          Base Address: 0xF8000080                         //
///////////////////////////////////////////////////////////////////////////////
                           
#define CYG_DEVICE_POW_UTCLKR      \
    ((volatile U32 *)(POW_UTILITY_BASE + 0x000)) //Clock Divider
#define CYG_DEVICE_POW_VOLTR       \
    ((volatile U32 *)(POW_UTILITY_BASE + 0x004)) //Voltage value
#define CYG_DEVICE_POW_TEMPR       \
    ((volatile U32 *)(POW_UTILITY_BASE + 0x008)) //Temperature Value
#define CYG_DEVICE_POW_MISCR       \
    ((volatile U32 *)(POW_UTILITY_BASE + 0x00c)) //Miscellaneous Value
#define CYG_DEVICE_POW_UCTLR       \
    ((volatile U32 *)(POW_UTILITY_BASE + 0x010)) //Utility Control
#define CYG_DEVICE_POW_UMULTS1       \
    ((volatile U32 *)(POW_UTILITY_BASE + 0x014)) //Utility Control
#define CYG_DEVICE_POW_UMULTS2       \
    ((volatile U32 *)(POW_UTILITY_BASE + 0x018)) //Utility Control
#define CYG_DEVICE_POW_UMULTS3       \
    ((volatile U32 *)(POW_UTILITY_BASE + 0x01c)) //Utility Control
#define CYG_DEVICE_POW_UMULTS4       \
    ((volatile U32 *)(POW_UTILITY_BASE + 0x020)) //Utility Control
#define CYG_DEVICE_POW_UMULTS5       \
    ((volatile U32 *)(POW_UTILITY_BASE + 0x024)) //Utility Control
#define CYG_DEVICE_POW_UMULTS6       \
    ((volatile U32 *)(POW_UTILITY_BASE + 0x028)) //Utility Control
#define CYG_DEVICE_POW_UMULTS7       \
    ((volatile U32 *)(POW_UTILITY_BASE + 0x02c)) //Utility Control
    
#define CYG_DEVICE_POW_TCLK_MUX    \
    ((volatile U32 *)(POW_UTILITY_BASE + 0x044)) //Test Clock output select

///////////////////////////////////////////////////////////////////////////////
//                          USB PHY Registers                          //
//                          Base Address: 0xD4207000                         //
//				modified by @Hongji
///////////////////////////////////////////////////////////////////////////////
#define CYG_DEVICE_USBPHY_CTRL		((volatile U32 *)(USBPHY_BASE+0x04))
#define CYG_DEVICE_USBPHY_PLL      	((volatile U32 *)(USBPHY_BASE+0x08))
#define CYG_DEVICE_USBPHY_TX        	((volatile U32 *)(USBPHY_BASE+0x0C))
#define CYG_DEVICE_USBPHY_RX        	((volatile U32 *)(USBPHY_BASE+0x10))
#define CYG_DEVICE_USBPHY_REF   	((volatile U32 *)(USBPHY_BASE+0x14))
#define CYG_DEVICE_USBPHY_TST0	((volatile U32 *)(USBPHY_BASE+0x18))
#define CYG_DEVICE_USBPHY_TST1 	((volatile U32 *)(USBPHY_BASE+0x1C))
#define CYG_DEVICE_USBPHY_TST2 	((volatile U32 *)(USBPHY_BASE+0x20))
//#define CYG_DEVICE_USBPHY_ID           ((volatile U32 *)(USBPHY_BASE+0x1C))
//#define CYG_DEVICE_USBPHY_IDOUT    ((volatile U32 *)(USBPHY_BASE+0x30))
#define CYG_DEVICE_USBPHY_IDGRP           ((volatile U32 *)(USBPHY_BASE+0x24))
#define CYG_DEVICE_USBPHY_USBINT           ((volatile U32 *)(USBPHY_BASE+0x28)) 
#define CYG_DEVICE_USBPHY_DBGCTL           ((volatile U32 *)(USBPHY_BASE+0x2C))
#define CYG_DEVICE_USBPHY_CTL1           ((volatile U32 *)(USBPHY_BASE+0x30))
#define CYG_DEVICE_USBPHY_TST3	((volatile U32 *)(USBPHY_BASE+0x34))
#define CYG_DEVICE_USBPHY_TST4	((volatile U32 *)(USBPHY_BASE+0x38))
#define CYG_DEVICE_USBPHY_TST5	((volatile U32 *)(USBPHY_BASE+0x3C))





                           
///////////////////////////////////////////////////////////////////////////////
//                          SRAM setting and test registers                          //
//                          Base Address: 0xF80000E0                         //
///////////////////////////////////////////////////////////////////////////////
#define CYG_DEVICE_SRAM_MBISTSTART      \
    ((volatile U32 *)(SRAM_TEST_BASE + 0x000)) //MBIST start
#define CYG_DEVICE_SRAM_MBISTDONE      \
        ((volatile U32 *)(SRAM_TEST_BASE + 0x004)) //MBIST finish
#define CYG_DEVICE_SRAM_MBISTSTS         \
        ((volatile U32 *)(SRAM_TEST_BASE + 0x008)) //MBIST result for SRAM
#define CYG_DEVICE_SRAM_TIMING             \
        ((volatile U32 *)(SRAM_TEST_BASE + 0x00c)) //SRAM read and write timing
                           
///////////////////////////////////////////////////////////////////////////////
//                          Power Management (PM) Registers                  //
//                          Base Address: 0xF8000100                         //
///////////////////////////////////////////////////////////////////////////////

#define CYG_DEVICE_PM_COR      \
    ((volatile U32 *)(PM_BASE + 0x000))//PM configuration
#define CYG_DEVICE_PM_PCR      \
    ((volatile U32 *)(PM_BASE + 0x004))//PM power control
#define CYG_DEVICE_PM_VAR      \
    ((volatile U32 *)(PM_BASE + 0x008))//PM voltage adjust
#define CYG_DEVICE_PM_CCSR     \
    ((volatile U32 *)(PM_BASE + 0x00c))//PM Charger control and status        
#define CYG_DEVICE_PM_IEVR     \
    ((volatile U32 *)(PM_BASE + 0x010))//PM interrupt event
#define CYG_DEVICE_PM_IMR      \
    ((volatile U32 *)(PM_BASE + 0x014))//PM interrupt mask
#define CYG_DEVICE_PM_ICR      \
    ((volatile U32 *)(PM_BASE + 0x018))//PM interrupt clear
#define CYG_DEVICE_PM_RINT     \
    ((volatile U32 *)(PM_BASE + 0x01c))//PM raw interrupt event
#define CYG_DEVICE_PM_BSR      \
    ((volatile U32 *)(PM_BASE + 0x020))//PM boot up status
                           
///////////////////////////////////////////////////////////////////////////////
//                          Pulse Width Light (PWL) Registers                //
//                          Base: 0xF8001000                                 //
///////////////////////////////////////////////////////////////////////////////

#define CYG_DEVICE_PWL_LOAD1       \
    ((volatile U32 *)(PWL_BASE + 0x000))//PWL counter1 load register
#define CYG_DEVICE_PWL_CTL1        \
    ((volatile U32 *)(PWL_BASE + 0x004))//PWL counter1 output register
#define CYG_DEVICE_PWL_CT1         \
    ((volatile U32 *)(PWL_BASE + 0x008))//PWL counter1 control register
#define CYG_DEVICE_PWL_LOAD2       \
    ((volatile U32 *)(PWL_BASE + 0x010))//PWL counter2 load register
#define CYG_DEVICE_PWL_CTL2        \
    ((volatile U32 *)(PWL_BASE + 0x014))//PWL counter2 output register
#define CYG_DEVICE_PWL_CT2         \
    ((volatile U32 *)(PWL_BASE + 0x018))//PWL counter2 control register
                           
///////////////////////////////////////////////////////////////////////////////
//                          GPIO0 Registers                                  //
//                          Base Address: 0xF8010000                         //
///////////////////////////////////////////////////////////////////////////////
#define CYG_DEVICE_GPIO0_DATABASE    \
    ((volatile U32 *)(GPIO0_BASE + 0x000))//Start Address of GPIO0 
                                                 //Data Register
#define CYG_DEVICE_GPIO0_DIR         \
    ((volatile U32 *)(GPIO0_BASE + 0x400))//GPIO Data Direction Register
#define CYG_DEVICE_GPIO0_IS          \
    ((volatile U32 *)(GPIO0_BASE + 0x404))//GPIO Interrupt Sense Register
#define CYG_DEVICE_GPIO0_IBE         \
    ((volatile U32 *)(GPIO0_BASE + 0x408))//GPIO Interrupt Both Edges 
                                                 //Register
#define CYG_DEVICE_GPIO0_IEV         \
    ((volatile U32 *)(GPIO0_BASE + 0x40c))//GPIO Interrupt Event Register
#define CYG_DEVICE_GPIO0_IE          \
    ((volatile U32 *)(GPIO0_BASE + 0x410))//GPIO Interrupt Mask
#define CYG_DEVICE_GPIO0_RIS         \
    ((volatile U32 *)(GPIO0_BASE + 0x414))//GPIO Raw Interrupt Status
#define CYG_DEVICE_GPIO0_MIS         \
    ((volatile U32 *)(GPIO0_BASE + 0x418))//GPIO Masked Interrupt Status
#define CYG_DEVICE_GPIO0_INT_CLEAR          \
    ((volatile U32 *)(GPIO0_BASE + 0x41c))//GPIO Interrupt Clear      
#define CYG_DEVICE_GPIO0_AFESEL      \
    ((volatile U32 *)(GPIO0_BASE + 0x420))//GPIO Mode Control Select
                           
#define CYG_DEVICE_GPIO0_PERIPH_ID0  \
    ((volatile U32 *)(GPIO0_BASE + 0xFE0))//Peripheral Identification 
                                                 //Register 7:0
#define CYG_DEVICE_GPIO0_PERIPH_ID1  \
    ((volatile U32 *)(GPIO0_BASE + 0xFE4))//Peripheral Identification 
                                                 //Register 15:8
#define CYG_DEVICE_GPIO0_PERIPH_ID2  \
    ((volatile U32 *)(GPIO0_BASE + 0xFE8))//Peripheral Identification 
                                                 //Register 23:16
#define CYG_DEVICE_GPIO0_PERIPH_ID3  \
    ((volatile U32 *)(GPIO0_BASE + 0xFEc))//Peripheral Identification 
                                                 //Register 31:24
#define CYG_DEVICE_GPIO0_PCELL_ID0   \
    ((volatile U32 *)(GPIO0_BASE + 0xFF0))//Prime Cell Identification 
                                                 //Register 7:0
#define CYG_DEVICE_GPIO0_PCELL_ID1   \
    ((volatile U32 *)(GPIO0_BASE + 0xFF4))//Prime Cell Identification 
                                                 //Register 15:8
#define CYG_DEVICE_GPIO0_PCELL_ID2   \
    ((volatile U32 *)(GPIO0_BASE + 0xFF8))//Prime Cell Identification 
                                                 //Register 23:16
#define CYG_DEVICE_GPIO0_PCELL_ID3   \
    ((volatile U32 *)(GPIO0_BASE + 0xFFc))//Prime Cell Identification 
                                                 //Register 31:24
                           
                           
///////////////////////////////////////////////////////////////////////////////
//                          GPIO1 Registers                                  //
//                          Base Address: 0xF8011000                         //
///////////////////////////////////////////////////////////////////////////////

#define CYG_DEVICE_GPIO1_DATABASE    \
    ((volatile U32 *)(GPIO1_BASE + 0x000))//Start Address of GPIO0 Data 
                                                 //Register
#define CYG_DEVICE_GPIO1_DIR         \
    ((volatile U32 *)(GPIO1_BASE + 0x400))//GPIO Data Direction Register
#define CYG_DEVICE_GPIO1_IS          \
    ((volatile U32 *)(GPIO1_BASE + 0x404))//GPIO Interrupt Sense 
                                                 //Register
#define CYG_DEVICE_GPIO1_IBE         \
    ((volatile U32 *)(GPIO1_BASE + 0x408))//GPIO Interrupt Both Edges 
                                                 //Register
#define CYG_DEVICE_GPIO1_IEV         \
    ((volatile U32 *)(GPIO1_BASE + 0x40c))//GPIO Interrupt Event Register
#define CYG_DEVICE_GPIO1_IE          \
    ((volatile U32 *)(GPIO1_BASE + 0x410))//GPIO Interrupt Mask
#define CYG_DEVICE_GPIO1_RIS         \
    ((volatile U32 *)(GPIO1_BASE + 0x414))//GPIO Raw Interrupt Status
#define CYG_DEVICE_GPIO1_MIS         \
    ((volatile U32 *)(GPIO1_BASE + 0x418))//GPIO Masked Interrupt Status
#define CYG_DEVICE_GPIO1_INT_CLEAR          \
    ((volatile U32 *)(GPIO1_BASE + 0x41c))//GPIO Interrupt Clear      
#define CYG_DEVICE_GPIO1_AFESEL     \
    ((volatile U32 *)(GPIO1_BASE + 0x420))//GPIO Mode Ccontrol Select
                                            
#define CYG_DEVICE_GPIO1_PERIPH_ID0\
    ((volatile U32 *)(GPIO1_BASE + 0xFE0))//Peripheral Identification Register 7:0
#define CYG_DEVICE_GPIO1_PERIPH_ID1\
    ((volatile U32 *)(GPIO1_BASE + 0xFE4))//Peripheral Identification Register 15:8
#define CYG_DEVICE_GPIO1_PERIPH_ID2\
    ((volatile U32 *)(GPIO1_BASE + 0xFE8))//Peripheral Identification Register 23:16
#define CYG_DEVICE_GPIO1_PERIPH_ID3\
    ((volatile U32 *)(GPIO1_BASE + 0xFEc))//Peripheral Identification Register 31:24
#define CYG_DEVICE_GPIO1_PCELL_ID0 \
    ((volatile U32 *)(GPIO1_BASE + 0xFF0))//Prime Cell Identification Register 7:0
#define CYG_DEVICE_GPIO1_PCELL_ID1 \
    ((volatile U32 *)(GPIO1_BASE + 0xFF4))//Prime Cell Identification Register 15:8
#define CYG_DEVICE_GPIO1_PCELL_ID2 \
    ((volatile U32 *)(GPIO1_BASE + 0xFF8))//Prime Cell Identification Register 23:16
#define CYG_DEVICE_GPIO1_PCELL_ID3 \
    ((volatile U32 *)(GPIO1_BASE + 0xFFc))//Prime Cell Identification Register 31:24
                           
                           
///////////////////////////////////////////////////////////////////////////////
//                          GPIO2 Registers                                  //
//                          Base Address: 0xF8012000                         //
///////////////////////////////////////////////////////////////////////////////
#define CYG_DEVICE_GPIO2_DATABASE  \
    ((volatile U32 *)(GPIO2_BASE + 0x000))//Start Address of GPIO0 Data Register
#define CYG_DEVICE_GPIO2_DIR       \
    ((volatile U32 *)(GPIO2_BASE + 0x400))//GPIO Data Direction Register
#define CYG_DEVICE_GPIO2_IS        \
    ((volatile U32 *)(GPIO2_BASE + 0x404))//GPIO Interrupt Sense Register
#define CYG_DEVICE_GPIO2_IBE       \
    ((volatile U32 *)(GPIO2_BASE + 0x408))//GPIO Interrupt Both Edges Register
#define CYG_DEVICE_GPIO2_IEV       \
    ((volatile U32 *)(GPIO2_BASE + 0x40c))//GPIO Interrupt Event Register
#define CYG_DEVICE_GPIO2_IE        \
    ((volatile U32 *)(GPIO2_BASE + 0x410))//GPIO Interrupt Mask
#define CYG_DEVICE_GPIO2_RIS       \
    ((volatile U32 *)(GPIO2_BASE + 0x414))//GPIO Raw Interrupt Status
#define CYG_DEVICE_GPIO2_MIS       \
    ((volatile U32 *)(GPIO2_BASE + 0x418))//GPIO Masked Interrupt Status
#define CYG_DEVICE_GPIO2_INT_CLEAR        \
    ((volatile U32 *)(GPIO2_BASE + 0x41c))//GPIO Interrupt Clear      
#define CYG_DEVICE_GPIO2_AFESEL    \
    ((volatile U32 *)(GPIO2_BASE + 0x420))//GPIO Mode Ccontrol Select
                                            
#define CYG_DEVICE_GPIO2_PERIPH_ID0\
    ((volatile U32 *)(GPIO2_BASE + 0xFE0))//Peripheral Identification Register 7:0
#define CYG_DEVICE_GPIO2_PERIPH_ID1\
    ((volatile U32 *)(GPIO2_BASE + 0xFE4))//Peripheral Identification Register 15:8
#define CYG_DEVICE_GPIO2_PERIPH_ID2\
    ((volatile U32 *)(GPIO2_BASE + 0xFE8))//Peripheral Identification Register 23:16
#define CYG_DEVICE_GPIO2_PERIPH_ID3\
    ((volatile U32 *)(GPIO2_BASE + 0xFEc))//Peripheral Identification Register 31:24
#define CYG_DEVICE_GPIO2_PCELL_ID0 \
    ((volatile U32 *)(GPIO2_BASE + 0xFF0))//Prime Cell Identification Register 7:0
#define CYG_DEVICE_GPIO2_PCELL_ID1 \
    ((volatile U32 *)(GPIO2_BASE + 0xFF4))//Prime Cell Identification Register 15:8
#define CYG_DEVICE_GPIO2_PCELL_ID2 \
    ((volatile U32 *)(GPIO2_BASE + 0xFF8))//Prime Cell Identification Register 23:16
#define CYG_DEVICE_GPIO2_PCELL_ID3 \
    ((volatile U32 *)(GPIO2_BASE + 0xFFc))//Prime Cell Identification Register 31:24
                           
                           
///////////////////////////////////////////////////////////////////////////////
//                          GPIO3 Registers                                  //
//                          Base Address: 0xF8013000                         //
///////////////////////////////////////////////////////////////////////////////

#define CYG_DEVICE_GPIO3_DATABASE  \
    ((volatile U32 *)(GPIO3_BASE + 0x000))//Start Address of GPIO0 Data Register
#define CYG_DEVICE_GPIO3_DIR       \
    ((volatile U32 *)(GPIO3_BASE + 0x400))//GPIO Data Direction Register
#define CYG_DEVICE_GPIO3_IS        \
    ((volatile U32 *)(GPIO3_BASE + 0x404))//GPIO Interrupt Sense Register
#define CYG_DEVICE_GPIO3_IBE       \
    ((volatile U32 *)(GPIO3_BASE + 0x408))//GPIO Interrupt Both Edges Register
#define CYG_DEVICE_GPIO3_IEV       \
    ((volatile U32 *)(GPIO3_BASE + 0x40c))//GPIO Interrupt Event Register
#define CYG_DEVICE_GPIO3_IE        \
    ((volatile U32 *)(GPIO3_BASE + 0x410))//GPIO Interrupt Mask
#define CYG_DEVICE_GPIO3_RIS       \
    ((volatile U32 *)(GPIO3_BASE + 0x414))//GPIO Raw Interrupt Status
#define CYG_DEVICE_GPIO3_MIS       \
    ((volatile U32 *)(GPIO3_BASE + 0x418))//GPIO Masked Interrupt Status
#define CYG_DEVICE_GPIO3_INT_CLEAR        \
    ((volatile U32 *)(GPIO3_BASE + 0x41c))//GPIO Interrupt Clear      
#define CYG_DEVICE_GPIO3_AFESEL    \
    ((volatile U32 *)(GPIO3_BASE + 0x420))//GPIO Mode Ccontrol Select
                                            
#define CYG_DEVICE_GPIO3_PERIPH_ID0\
    ((volatile U32 *)(GPIO3_BASE + 0xFE0))//Peripheral Identification Register 7:0
#define CYG_DEVICE_GPIO3_PERIPH_ID1\
    ((volatile U32 *)(GPIO3_BASE + 0xFE4))//Peripheral Identification Register 15:8
#define CYG_DEVICE_GPIO3_PERIPH_ID2\
    ((volatile U32 *)(GPIO3_BASE + 0xFE8))//Peripheral Identification Register 23:16
#define CYG_DEVICE_GPIO3_PERIPH_ID3\
    ((volatile U32 *)(GPIO3_BASE + 0xFEc))//Peripheral Identification Register 31:24
#define CYG_DEVICE_GPIO3_PCELL_ID0 \
    ((volatile U32 *)(GPIO3_BASE + 0xFF0))//Prime Cell Identification Register 7:0
#define CYG_DEVICE_GPIO3_PCELL_ID1 \
    ((volatile U32 *)(GPIO3_BASE + 0xFF4))//Prime Cell Identification Register 15:8
#define CYG_DEVICE_GPIO3_PCELL_ID2 \
    ((volatile U32 *)(GPIO3_BASE + 0xFF8))//Prime Cell Identification Register 23:16
#define CYG_DEVICE_GPIO3_PCELL_ID3 \
    ((volatile U32 *)(GPIO3_BASE + 0xFFc))//Prime Cell Identification Register 31:24
                           
                           
///////////////////////////////////////////////////////////////////////////////
//                          GPIO4 Registers                                  //
//                          Base Address: 0xF8014000                         //
///////////////////////////////////////////////////////////////////////////////

#define CYG_DEVICE_GPIO4_DATABASE  \
    ((volatile U32 *)(GPIO4_BASE + 0x000))//Start Address of GPIO0 Data Register
#define CYG_DEVICE_GPIO4_DIR       \
    ((volatile U32 *)(GPIO4_BASE + 0x400))//GPIO Data Direction Register
#define CYG_DEVICE_GPIO4_IS        \
    ((volatile U32 *)(GPIO4_BASE + 0x404))//GPIO Interrupt Sense Register
#define CYG_DEVICE_GPIO4_IBE       \
    ((volatile U32 *)(GPIO4_BASE + 0x408))//GPIO Interrupt Both Edges Register
#define CYG_DEVICE_GPIO4_IEV       \
    ((volatile U32 *)(GPIO4_BASE + 0x40c))//GPIO Interrupt Event Register
#define CYG_DEVICE_GPIO4_IE        \
    ((volatile U32 *)(GPIO4_BASE + 0x410))//GPIO Interrupt Mask
#define CYG_DEVICE_GPIO4_RIS       \
    ((volatile U32 *)(GPIO4_BASE + 0x414))//GPIO Raw Interrupt Status
#define CYG_DEVICE_GPIO4_MIS       \
    ((volatile U32 *)(GPIO4_BASE + 0x418))//GPIO Masked Interrupt Status
#define CYG_DEVICE_GPIO4_INT_CLEAR        \
    ((volatile U32 *)(GPIO4_BASE + 0x41c))//GPIO Interrupt Clear      
#define CYG_DEVICE_GPIO4_AFESEL    \
    ((volatile U32 *)(GPIO4_BASE + 0x420))//GPIO Mode Ccontrol Select
                                            
#define CYG_DEVICE_GPIO4_PERIPH_ID0\
    ((volatile U32 *)(GPIO4_BASE + 0xFE0))//Peripheral Identification Register 7:0
#define CYG_DEVICE_GPIO4_PERIPH_ID1\
    ((volatile U32 *)(GPIO4_BASE + 0xFE4))//Peripheral Identification Register 15:8
#define CYG_DEVICE_GPIO4_PERIPH_ID2\
    ((volatile U32 *)(GPIO4_BASE + 0xFE8))//Peripheral Identification Register 23:16
#define CYG_DEVICE_GPIO4_PERIPH_ID3\
    ((volatile U32 *)(GPIO4_BASE + 0xFEc))//Peripheral Identification Register 31:24
#define CYG_DEVICE_GPIO4_PCELL_ID0 \
    ((volatile U32 *)(GPIO4_BASE + 0xFF0))//Prime Cell Identification Register 7:0
#define CYG_DEVICE_GPIO4_PCELL_ID1 \
    ((volatile U32 *)(GPIO4_BASE + 0xFF4))//Prime Cell Identification Register 15:8
#define CYG_DEVICE_GPIO4_PCELL_ID2 \
    ((volatile U32 *)(GPIO4_BASE + 0xFF8))//Prime Cell Identification Register 23:16
#define CYG_DEVICE_GPIO4_PCELL_ID3 \
    ((volatile U32 *)(GPIO4_BASE + 0xFFc))//Prime Cell Identification Register 31:24
                           
                           
///////////////////////////////////////////////////////////////////////////////
//                          GPIO5 Registers                                  //
//                          Base Address: 0xF8015000                         //
///////////////////////////////////////////////////////////////////////////////

#define CYG_DEVICE_GPIO5_DATABASE  \
    ((volatile U32 *)(GPIO5_BASE + 0x000))//Start Address of GPIO0 Data Register
#define CYG_DEVICE_GPIO5_DIR       \
    ((volatile U32 *)(GPIO5_BASE + 0x400))//GPIO Data Direction Register
#define CYG_DEVICE_GPIO5_IS        \
    ((volatile U32 *)(GPIO5_BASE + 0x404))//GPIO Interrupt Sense Register
#define CYG_DEVICE_GPIO5_IBE       \
    ((volatile U32 *)(GPIO5_BASE + 0x408))//GPIO Interrupt Both Edges Register
#define CYG_DEVICE_GPIO5_IEV       \
    ((volatile U32 *)(GPIO5_BASE + 0x40c))//GPIO Interrupt Event Register
#define CYG_DEVICE_GPIO5_IE        \
    ((volatile U32 *)(GPIO5_BASE + 0x410))//GPIO Interrupt Mask
#define CYG_DEVICE_GPIO5_RIS       \
    ((volatile U32 *)(GPIO5_BASE + 0x414))//GPIO Raw Interrupt Status
#define CYG_DEVICE_GPIO5_MIS       \
    ((volatile U32 *)(GPIO5_BASE + 0x418))//GPIO Masked Interrupt Status
#define CYG_DEVICE_GPIO5_INT_CLEAR        \
    ((volatile U32 *)(GPIO5_BASE + 0x41c))//GPIO Interrupt Clear      
#define CYG_DEVICE_GPIO5_AFESEL    \
    ((volatile U32 *)(GPIO5_BASE + 0x420))//GPIO Mode Ccontrol Select
                                            
#define CYG_DEVICE_GPIO5_PERIPH_ID0\
    ((volatile U32 *)(GPIO5_BASE + 0xFE0))//Peripheral Identification Register 7:0
#define CYG_DEVICE_GPIO5_PERIPH_ID1\
    ((volatile U32 *)(GPIO5_BASE + 0xFE4))//Peripheral Identification Register 15:8
#define CYG_DEVICE_GPIO5_PERIPH_ID2\
    ((volatile U32 *)(GPIO5_BASE + 0xFE8))//Peripheral Identification Register 23:16
#define CYG_DEVICE_GPIO5_PERIPH_ID3\
    ((volatile U32 *)(GPIO5_BASE + 0xFEc))//Peripheral Identification Register 31:24
#define CYG_DEVICE_GPIO5_PCELL_ID0 \
    ((volatile U32 *)(GPIO5_BASE + 0xFF0))//Prime Cell Identification Register 7:0
#define CYG_DEVICE_GPIO5_PCELL_ID1 \
    ((volatile U32 *)(GPIO5_BASE + 0xFF4))//Prime Cell Identification Register 15:8
#define CYG_DEVICE_GPIO5_PCELL_ID2 \
    ((volatile U32 *)(GPIO5_BASE + 0xFF8))//Prime Cell Identification Register 23:16
#define CYG_DEVICE_GPIO5_PCELL_ID3 \
    ((volatile U32 *)(GPIO5_BASE + 0xFFc))//Prime Cell Identification Register 31:24
                           
                           
///////////////////////////////////////////////////////////////////////////////
//                          UART0 Registers                                  //
//                          Base Address: 0xF8020000                         //
///////////////////////////////////////////////////////////////////////////////
#define CYG_DEVICE_UART0_DR        \
    ((volatile U32 *)(UART0_BASE + 0x00))//Data Register
#define CYG_DEVICE_UART0_RSR       \
    ((volatile U32 *)(UART0_BASE + 0x04))//Receive Status Register(read),
#define CYG_DEVICE_UART0_ECR       \
    ((volatile U32 *)(UART0_BASE + 0x04))//Eror Clear Register(write)                                                                                                //Eror Clear Register(write)
#define CYG_DEVICE_UART0_FR        \
    ((volatile U32 *)(UART0_BASE + 0x18))//Flag Register
#define CYG_DEVICE_UART0_ILPR      \
    ((volatile U32 *)(UART0_BASE + 0x20))//IrDA Low-power Counter Register
#define CYG_DEVICE_UART0_IBRD      \
    ((volatile U32 *)(UART0_BASE + 0x24))//Interger Baud Rate Divisor Register
#define CYG_DEVICE_UART0_FBRD      \
    ((volatile U32 *)(UART0_BASE + 0x28))//Fractional Baud Rate Divisor Register
#define CYG_DEVICE_UART0_LCRH      \
    ((volatile U32 *)(UART0_BASE + 0x2c))//Line Control Register, high byte
#define CYG_DEVICE_UART0_CR       \
    ((volatile U32 *)(UART0_BASE + 0x30))//Control Register
                           
#define CYG_DEVICE_UART0_IFLS      \
    ((volatile U32 *)(UART0_BASE + 0x34))//Interrupt FIFO Level Select Register
#define CYG_DEVICE_UART0_IMSC      \
    ((volatile U32 *)(UART0_BASE + 0x38))//Interrupt Mask Set/Clear
#define CYG_DEVICE_UART0_RIS       \
    ((volatile U32 *)(UART0_BASE + 0x3c))//Raw Interrupt Status
#define CYG_DEVICE_UART0_MIS       \
    ((volatile U32 *)(UART0_BASE + 0x40))//Masked Interrupt Status
#define CYG_DEVICE_UART0_ICR       \
    ((volatile U32 *)(UART0_BASE + 0x44))//Interrupt Clear 
#define CYG_DEVICE_UART0_DMACR     \
    ((volatile U32 *)(UART0_BASE + 0x48))//DMA Control Register
                           
#define CYG_DEVICE_UART0_PERIPH_ID0\
    ((volatile U32 *)(UART0_BASE + 0xFE0))//Peripheral Identification Register 7:0
#define CYG_DEVICE_UART0_PERIPH_ID1\
    ((volatile U32 *)(UART0_BASE + 0xFE4))//Peripheral Identification Register 15:8
#define CYG_DEVICE_UART0_PERIPH_ID2\
    ((volatile U32 *)(UART0_BASE + 0xFE8))//Peripheral Identification Register 23:16
#define CYG_DEVICE_UART0_PERIPH_ID3\
    ((volatile U32 *)(UART0_BASE + 0xFEc))//Peripheral Identification Register 31:24
#define CYG_DEVICE_UART0_PCELL_ID0 \
    ((volatile U32 *)(UART0_BASE + 0xFF0))//Prime Cell Identification Register 7:0
#define CYG_DEVICE_UART0_PCELL_ID1 \
    ((volatile U32 *)(UART0_BASE + 0xFF4))//Prime Cell Identification Register 15:8
#define CYG_DEVICE_UART0_PCELL_ID2 \
    ((volatile U32 *)(UART0_BASE + 0xFF8))//Prime Cell Identification Register 23:16
#define CYG_DEVICE_UART0_PCELL_ID3 \
    ((volatile U32 *)(UART0_BASE + 0xFFc))//Prime Cell Identification Register 31:24
                           
                           
///////////////////////////////////////////////////////////////////////////////
//                          UART1 Registers                                  //
//                          Base Address: 0xF8024000                         //
///////////////////////////////////////////////////////////////////////////////
#define CYG_DEVICE_UART1_DR        \
    ((volatile U32 *)(UART1_BASE + 0x00))//Data Register
#define CYG_DEVICE_UART1_RSR       \
    ((volatile U32 *)(UART1_BASE + 0x04))//Receive Status Register(read),
                                                //Eror Clear Register(write)
#define CYG_DEVICE_UART1_FR        \
    ((volatile U32 *)(UART1_BASE + 0x18))//Flag Register
#define CYG_DEVICE_UART1_ILPR      \
    ((volatile U32 *)(UART1_BASE + 0x20))//IrDA Low-power Counter Register
#define CYG_DEVICE_UART1_IBRD      \
    ((volatile U32 *)(UART1_BASE + 0x24))//Interger Baud Rate Divisor Register
#define CYG_DEVICE_UART1_FBRD      \
    ((volatile U32 *)(UART1_BASE + 0x28))//Fractional Baud Rate Divisor Register
#define CYG_DEVICE_UART1_LCRH      \
    ((volatile U32 *)(UART1_BASE + 0x2c))//Line Control Register, high byte
#define CYG_DEVICE_UART1_CR       \
    ((volatile U32 *)(UART1_BASE + 0x30))//Control Register
                           
#define CYG_DEVICE_UART1_IFLS      \
    ((volatile U32 *)(UART1_BASE + 0x34))//Interrupt FIFO Level Select Register
#define CYG_DEVICE_UART1_IMSC      \
    ((volatile U32 *)(UART1_BASE + 0x38))//Interrupt Mask Set/Clear
#define CYG_DEVICE_UART1_RIS       \
    ((volatile U32 *)(UART1_BASE + 0x3c))//Raw Interrupt Status
#define CYG_DEVICE_UART1_MIS       \
    ((volatile U32 *)(UART1_BASE + 0x40))//Masked Interrupt Status
#define CYG_DEVICE_UART1_ICR       \
    ((volatile U32 *)(UART1_BASE + 0x44))//Interrupt Clear 
#define CYG_DEVICE_UART1_DMACR     \
    ((volatile U32 *)(UART1_BASE + 0x48))//DMA Control Register
                           
#define CYG_DEVICE_UART1_PERIPH_ID0\
    ((volatile U32 *)(UART1_BASE + 0xFE0))//Peripheral Identification Register 7:0
#define CYG_DEVICE_UART1_PERIPH_ID1\
    ((volatile U32 *)(UART1_BASE + 0xFE4))//Peripheral Identification Register 15:8
#define CYG_DEVICE_UART1_PERIPH_ID2\
    ((volatile U32 *)(UART1_BASE + 0xFE8))//Peripheral Identification Register 23:16
#define CYG_DEVICE_UART1_PERIPH_ID3\
    ((volatile U32 *)(UART1_BASE + 0xFEc))//Peripheral Identification Register 31:24
#define CYG_DEVICE_UART1_PCELL_ID0 \
    ((volatile U32 *)(UART1_BASE + 0xFF0))//Prime Cell Identification Register 7:0
#define CYG_DEVICE_UART1_PCELL_ID1 \
    ((volatile U32 *)(UART1_BASE + 0xFF4))//Prime Cell Identification Register 15:8
#define CYG_DEVICE_UART1_PCELL_ID2 \
    ((volatile U32 *)(UART1_BASE + 0xFF8))//Prime Cell Identification Register 23:16
#define CYG_DEVICE_UART1_PCELL_ID3 \
    ((volatile U32 *)(UART1_BASE + 0xFFc))//Prime Cell Identification Register 31:24
                           
///////////////////////////////////////////////////////////////////////////////
//                          SCI Registers                                    //
//                          Base Address: 0xF8040000                         //
///////////////////////////////////////////////////////////////////////////////
#define CYG_DEVICE_SCI_DATA      \
    ((volatile U32 *)(SCI_BASE + 0x000))//Data Register
#define CYG_DEVICE_SCI_CR0       \
    ((volatile U32 *)(SCI_BASE + 0x004))//Control Register 0
#define CYG_DEVICE_SCI_CR1       \
    ((volatile U32 *)(SCI_BASE + 0x008))//Control Register 1
#define CYG_DEVICE_SCI_CR2       \
    ((volatile U32 *)(SCI_BASE + 0x00c))//Control Register 2
#define CYG_DEVICE_SCI_CLKICC    \
    ((volatile U32 *)(SCI_BASE + 0x010))//External Smart Card Frequency Divisor
#define CYG_DEVICE_SCI_VALUE     \
    ((volatile U32 *)(SCI_BASE + 0x014))//Number of SCIBAUD cycles/etu
#define CYG_DEVICE_SCI_BAUD      \
    ((volatile U32 *)(SCI_BASE + 0x018))//Baud Rate Clock Divisor
#define CYG_DEVICE_SCI_TIDE      \
    ((volatile U32 *)(SCI_BASE + 0x01c))//TX & RX FIFO Water Marks
#define CYG_DEVICE_SCI_DMACR     \
    ((volatile U32 *)(SCI_BASE + 0x020))//DMA Control Register
#define CYG_DEVICE_SCI_STABLE    \
    ((volatile U32 *)(SCI_BASE + 0x024))//Card Stable after insertion debounce duration
                           
#define CYG_DEVICE_SCI_ATIME     \
    ((volatile U32 *)(SCI_BASE + 0x028))//Card Activation event time
#define CYG_DEVICE_SCI_DTIME     \
    ((volatile U32 *)(SCI_BASE + 0x02c))//Card deactivation event time
#define CYG_DEVICE_SCI_ATR_STIME \
    ((volatile U32 *)(SCI_BASE + 0x030))//ATR start time register
#define CYG_DEVICE_SCI_ATR_DTIME \
    ((volatile U32 *)(SCI_BASE + 0x034))//ATR duration time register
#define CYG_DEVICE_SCI_STOP_TIME \
    ((volatile U32 *)(SCI_BASE + 0x038))//Duration before card clock to be stopped/
#define CYG_DEVICE_SCI_START_TIME\
    ((volatile U32 *)(SCI_BASE + 0x03c))//Duration before transcation can start after clk restarted
#define CYG_DEVICE_SCI_RETRY     \
    ((volatile U32 *)(SCI_BASE + 0x040))//Retry Limit Register
                          
#define CYG_DEVICE_SCI_CHTIMELS  \
    ((volatile U32 *)(SCI_BASE + 0x044))//Char to char timeout(least significant 16 bit)
#define CYG_DEVICE_SCI_CHTIMEMS  \
    ((volatile U32 *)(SCI_BASE + 0x048))//Char to char timeout(most significant 16 bit)
#define CYG_DEVICE_SCI_BLKTIMELS \
    ((volatile U32 *)(SCI_BASE + 0x04c))//Receive timeout between blocks(least significant 16 bit)
#define CYG_DEVICE_SCI_BLKTIMEMS \
    ((volatile U32 *)(SCI_BASE + 0x050))//Receive timeout between blocks(most significant 16 bit)
                          
#define CYG_DEVICE_SCI_CHGUARD   \
    ((volatile U32 *)(SCI_BASE + 0x054))//Char to char extra guard time
#define CYG_DEVICE_SCI_BLKGUARD  \
    ((volatile U32 *)(SCI_BASE + 0x058))//Block guard time
#define CYG_DEVICE_SCI_RXTIME    \
    ((volatile U32 *)(SCI_BASE + 0x05c))//Receive read time-out
#define CYG_DEVICE_SCI_FIFOSTATUS\
    ((volatile U32 *)(SCI_BASE + 0x060))//Transmit and receive FIFO status
#define CYG_DEVICE_SCI_TXCOUNT   \
    ((volatile U32 *)(SCI_BASE + 0x064))//Tx FIFO fill level.
#define CYG_DEVICE_SCI_RXCOUNT   \
    ((volatile U32 *)(SCI_BASE + 0x068))//Rx FIFO fill level.
#define CYG_DEVICE_SCI_IMSC      \
    ((volatile U32 *)(SCI_BASE + 0x06c))//Interrupt Mask Set Clear register
#define CYG_DEVICE_SCI_RIS       \
    ((volatile U32 *)(SCI_BASE + 0x070))//Raw Interrupt Status register
#define CYG_DEVICE_SCI_MIS       \
    ((volatile U32 *)(SCI_BASE + 0x074))//Masked Interrupt Status register
#define CYG_DEVICE_SCI_ICR       \
    ((volatile U32 *)(SCI_BASE + 0x078))//Interrupt Clear Register
                           
#define CYG_DEVICE_SCI_SYNCACT   \
    ((volatile U32 *)(SCI_BASE + 0x07c))//Sync. mode Activation Register
#define CYG_DEVICE_SCI_SYNCTX    \
    ((volatile U32 *)(SCI_BASE + 0x080))//Sync. mode transmit clock & data stream register
#define CYG_DEVICE_SCI_SYNCRX    \
    ((volatile U32 *)(SCI_BASE + 0x084))//Sync. mode receive clock & data stream register
                           
#define CYG_DEVICE_SCI_PERIPH_ID0\
    ((volatile U32 *)(SCI_BASE + 0xFE0))//Peripheral Identification Register 7:0
#define CYG_DEVICE_SCI_PERIPH_ID1\
    ((volatile U32 *)(SCI_BASE + 0xFE4))//Peripheral Identification Register 15:8
#define CYG_DEVICE_SCI_PERIPH_ID2\
    ((volatile U32 *)(SCI_BASE + 0xFE8))//Peripheral Identification Register 23:16
#define CYG_DEVICE_SCI_PERIPH_ID3\
    ((volatile U32 *)(SCI_BASE + 0xFEc))//Peripheral Identification Register 31:24
#define CYG_DEVICE_SCI_PCELL_ID0 \
    ((volatile U32 *)(SCI_BASE + 0xFF0))//Prime Cell Identification Register 7:0
#define CYG_DEVICE_SCI_PCELL_ID1 \
    ((volatile U32 *)(SCI_BASE + 0xFF4))//Prime Cell Identification Register 15:8
#define CYG_DEVICE_SCI_PCELL_ID2 \
    ((volatile U32 *)(SCI_BASE + 0xFF8))//Prime Cell Identification Register 23:16
#define CYG_DEVICE_SCI_PCELL_ID3 \
    ((volatile U32 *)(SCI_BASE + 0xFFc))//Prime Cell Identification Register 31:24
                           
                           
                           
///////////////////////////////////////////////////////////////////////////////
//                          Serial LCD interface Registers                   //
//                          Base Address: 0xF8060000                         //
///////////////////////////////////////////////////////////////////////////////

#define CYG_DEVICE_LCD_CLKMD \
         ((volatile U32 *) (LCD_BASE + 0x00))
#define CYG_DEVICE_LCD_TXCTL \
         ((volatile U32 *) (LCD_BASE + 0x04))
#define CYG_DEVICE_LCD_RXCTL \
         ((volatile U32 *) (LCD_BASE + 0x08))
#define CYG_DEVICE_LCD_INTCLR \
         ((volatile U32 *) (LCD_BASE + 0x0c))
#define CYG_DEVICE_LCD_INTSTU \
         ((volatile U32 *) (LCD_BASE + 0x10))
#define CYG_DEVICE_LCD_INTMSK \
         ((volatile U32 *) (LCD_BASE + 0x14))
#define CYG_DEVICE_LCD_TXDATA \
         ((volatile U32 *) (LCD_BASE + 0x18))
#define CYG_DEVICE_LCD_RXDATA \
         ((volatile U32 *) (LCD_BASE + 0x1c))
#define CYG_DEVICE_LCD_WMK \
         ((volatile U32 *) (LCD_BASE + 0x20))
#define CYG_DEVICE_LCD_TXTOTAL \
         ((volatile U32 *) (LCD_BASE + 0x24))

///////////////////////////////////////////////////////////////////////////////
//                          KeyPad Registers                                 //
//                          Base Address: 0xF8070000                         //
///////////////////////////////////////////////////////////////////////////////

#define CYG_DEVICE_KEYPAD_KEY_CLOCK   \
    ((volatile U32 *)(KEYPAD_BASE + 0x000))//Clock register
#define CYG_DEVICE_KEYPAD_KEY_REPEAT \
    ((volatile U32 *)(KEYPAD_BASE + 0x004))//Key Repeat trigger time
#define CYG_DEVICE_KEYPAD_KEY_ROW   \
    ((volatile U32 *)(KEYPAD_BASE + 0x008))//Key Row number
#define CYG_DEVICE_KEYPAD_KEY_COLUMN   \
    ((volatile U32 *)(KEYPAD_BASE + 0x00c))//Key column number
#define CYG_DEVICE_KEYPAD_KEY_INT_MASK  \
    ((volatile U32 *)(KEYPAD_BASE + 0x010))//Key interrupt mask
#define CYG_DEVICE_KEYPAD_KEY_CLEAR \
    ((volatile U32 *)(KEYPAD_BASE + 0x014))//Key Interrupt clear
#define CYG_DEVICE_KEYPAD_KEY_INT_STATUS \
    ((volatile U32 *)(KEYPAD_BASE + 0x018))//Key Interrupt Status
#define CYG_DEVICE_KEYPAD_KEY_CODE  \
    ((volatile U32 *)(KEYPAD_BASE + 0x01c))//Key Code
/*#define CYG_DEVICE_KEYPAD_KEY_POWER   \
    ((volatile U32 *)(KEYPAD_BASE + 0x020))//Key Power*/
#define CYG_DEVICE_KEYPAD_KEY_BIT_MATRIX_A   \
    ((volatile U32 *)(KEYPAD_BASE + 0x020))//Key Bit Matrix A
#define CYG_DEVICE_KEYPAD_KEY_BIT_MATRIX_B   \
    ((volatile U32 *)(KEYPAD_BASE + 0x024))//Key Bit Matrix B
#define CYG_DEVICE_KEYPAD_KEY_BIT_MATRIX_C   \
    ((volatile U32 *)(KEYPAD_BASE + 0x028))//Key Bit Matrix C
                           
                           
///////////////////////////////////////////////////////////////////////////////
//                          RTC Registers                                    //
//                          Base Address: 0xF8080000                         //
///////////////////////////////////////////////////////////////////////////////

#define CYG_DEVICE_RTC_DATA      \
    ((volatile U32 *)(RTC_BASE + 0x000))//Data register
#define CYG_DEVICE_RTC_MATCH     \
    ((volatile U32 *)(RTC_BASE + 0x004))//Match register
#define CYG_DEVICE_RTC_LOAD      \
    ((volatile U32 *)(RTC_BASE + 0x008))//Load register
#define CYG_DEVICE_RTC_CONTROL   \
    ((volatile U32 *)(RTC_BASE + 0x00c))//Control register
#define CYG_DEVICE_RTC_INT_MASK  \
    ((volatile U32 *)(RTC_BASE + 0x010))//Interrupt mask set and clear register
#define CYG_DEVICE_RTC_INT_RAW_STATUS    \
    ((volatile U32 *)(RTC_BASE + 0x014))//Raw interrupt status register
#define CYG_DEVICE_RTC_INT_MASKED_STATUS \
    ((volatile U32 *)(RTC_BASE + 0x018))//Masked interrupt status register
#define CYG_DEVICE_RTC_INT_CLEAR       \
    ((volatile U32 *)(RTC_BASE + 0x01c))//Interrupt clear register
#define CYG_DEVICE_RTC_WAKEUP_LOAD    \
    ((volatile U32 *)(RTC_BASE + 0x020))//Wake-up load register
#define CYG_DEVICE_RTC_ACR    \
    ((volatile U32 *)(RTC_BASE + 0x024))//Alarm control register
#define CYG_DEVICE_RTC_CTRWE    \
    ((volatile U32 *)(RTC_BASE + 0x080))//Control read&write enable register
#define CYG_DEVICE_RTC_WCTR    \
    ((volatile U32 *)(RTC_BASE + 0x090))//Free run RTC write data register
#define CYG_DEVICE_RTC_RCTHR    \
    ((volatile U32 *)(RTC_BASE + 0x094))//Free run RTC read data high part[41:10] register
#define CYG_DEVICE_RTC_RCTLR    \
    ((volatile U32 *)(RTC_BASE + 0x098))//Free run RTC read data low part[9:0] register
                           
#define CYG_DEVICE_RTC_PERIPH_ID0\
    ((volatile U32 *)(RTC_BASE + 0xFE0))//Peripheral Identification Register 7:0
#define CYG_DEVICE_RTC_PERIPH_ID1\
    ((volatile U32 *)(RTC_BASE + 0xFE4))//Peripheral Identification Register 15:8
#define CYG_DEVICE_RTC_PERIPH_ID2\
    ((volatile U32 *)(RTC_BASE + 0xFE8))//Peripheral Identification Register 23:16
#define CYG_DEVICE_RTC_PERIPH_ID3\
    ((volatile U32 *)(RTC_BASE + 0xFEc))//Peripheral Identification Register 31:24
#define CYG_DEVICE_RTC_PCELL_ID0 \
    ((volatile U32 *)(RTC_BASE + 0xFF0))//Prime Cell Identification Register 7:0
#define CYG_DEVICE_RTC_PCELL_ID1 \
    ((volatile U32 *)(RTC_BASE + 0xFF4))//Prime Cell Identification Register 15:8
#define CYG_DEVICE_RTC_PCELL_ID2 \
    ((volatile U32 *)(RTC_BASE + 0xFF8))//Prime Cell Identification Register 23:16
#define CYG_DEVICE_RTC_PCELL_ID3 \
    ((volatile U32 *)(RTC_BASE + 0xFFc))//Prime Cell Identification Register 31:24
                           
///////////////////////////////////////////////////////////////////////////////
//                          TIMER0 Registers                                 //
//                          Base Address: 0xF8081000                         //
///////////////////////////////////////////////////////////////////////////////

#define CYG_DEVICE_TIMER0_1_LOAD    \
    ((volatile U32 *)(TIMER0_BASE + 0x000))//Timer Load register
#define CYG_DEVICE_TIMER0_1_VALUE   \
    ((volatile U32 *)(TIMER0_BASE + 0x004))//Timer Value
#define CYG_DEVICE_TIMER0_1_CONTROL \
    ((volatile U32 *)(TIMER0_BASE + 0x008))//Timer Control Register
#define CYG_DEVICE_TIMER0_1_CLEAR   \
    ((volatile U32 *)(TIMER0_BASE + 0x00c))//Timer Clear Register
#define CYG_DEVICE_TIMER0_1_INTRAW  \
    ((volatile U32 *)(TIMER0_BASE + 0x010))//Raw Interrupt Status Register
#define CYG_DEVICE_TIMER0_1_INT     \
    ((volatile U32 *)(TIMER0_BASE + 0x014))//Interrupt Status Register
#define CYG_DEVICE_TIMER0_1_LOADBG  \
    ((volatile U32 *)(TIMER0_BASE + 0x018))//Background load Register
                           
#define CYG_DEVICE_TIMER0_2_LOAD    \
    ((volatile U32 *)(TIMER0_BASE + 0x020))//Timer Load register
#define CYG_DEVICE_TIMER0_2_VALUE   \
    ((volatile U32 *)(TIMER0_BASE + 0x024))//Timer Value
#define CYG_DEVICE_TIMER0_2_CONTROL \
    ((volatile U32 *)(TIMER0_BASE + 0x028))//Timer Control Register
#define CYG_DEVICE_TIMER0_2_CLEAR   \
    ((volatile U32 *)(TIMER0_BASE + 0x02c))//Timer Clear Register
#define CYG_DEVICE_TIMER0_2_INTRAW  \
    ((volatile U32 *)(TIMER0_BASE + 0x030))//Raw Interrupt Status Register
#define CYG_DEVICE_TIMER0_2_INT     \
    ((volatile U32 *)(TIMER0_BASE + 0x034))//Interrupt Status Register
#define CYG_DEVICE_TIMER0_2_LOADBG  \
    ((volatile U32 *)(TIMER0_BASE + 0x038))//Background load Register
                                              
#define CYG_DEVICE_TIMER0_ITCR      \
    ((volatile U32 *)(TIMER0_BASE + 0x080))//Integration Test Control Register
#define CYG_DEVICE_TIMER0_ITOP      \
    ((volatile U32 *)(TIMER0_BASE + 0x084))//Intergration Test Output set Register 
                           
#define CYG_DEVICE_TIMER0_PERIPH_ID0\
    ((volatile U32 *)(TIMER0_BASE + 0xA0))//Peripheral Identification Register 7:0
#define CYG_DEVICE_TIMER0_PERIPH_ID1\
    ((volatile U32 *)(TIMER0_BASE + 0xA4))//Peripheral Identification Register 15:8
#define CYG_DEVICE_TIMER0_PERIPH_ID2\
    ((volatile U32 *)(TIMER0_BASE + 0xA8))//Peripheral Identification Register 23:16
#define CYG_DEVICE_TIMER0_PERIPH_ID3\
    ((volatile U32 *)(TIMER0_BASE + 0xAc))//Peripheral Identification Register 31:24
#define CYG_DEVICE_TIMER0_PCELL_ID0 \
    ((volatile U32 *)(TIMER0_BASE + 0xB0))//Prime Cell Identification Register 7:0
#define CYG_DEVICE_TIMER0_PCELL_ID1 \
    ((volatile U32 *)(TIMER0_BASE + 0xB4))//Prime Cell Identification Register 15:8
#define CYG_DEVICE_TIMER0_PCELL_ID2 \
    ((volatile U32 *)(TIMER0_BASE + 0xB8))//Prime Cell Identification Register 23:16
#define CYG_DEVICE_TIMER0_PCELL_ID3 \
    ((volatile U32 *)(TIMER0_BASE + 0xBc))//Prime Cell Identification Register 31:24
                           
                           
///////////////////////////////////////////////////////////////////////////////
//                          TIMER1 Registers                                 //
//                          Base Address: 0xF8082000                         //
///////////////////////////////////////////////////////////////////////////////

#define CYG_DEVICE_TIMER1_1_LOAD    \
    ((volatile U32 *)(TIMER1_BASE + 0x000))//Timer Load register
#define CYG_DEVICE_TIMER1_1_VALUE   \
    ((volatile U32 *)(TIMER1_BASE + 0x004))//Timer Value
#define CYG_DEVICE_TIMER1_1_CONTROL \
    ((volatile U32 *)(TIMER1_BASE + 0x008))//Timer Control Register
#define CYG_DEVICE_TIMER1_1_CLEAR   \
    ((volatile U32 *)(TIMER1_BASE + 0x00c))//Timer Clear Register
#define CYG_DEVICE_TIMER1_1_INTRAW  \
    ((volatile U32 *)(TIMER1_BASE + 0x010))//Raw Interrupt Status Register
#define CYG_DEVICE_TIMER1_1_INT     \
    ((volatile U32 *)(TIMER1_BASE + 0x014))//Interrupt Status Register
#define CYG_DEVICE_TIMER1_1_LOADBG  \
    ((volatile U32 *)(TIMER1_BASE + 0x018))//Background load Register
                           
#define CYG_DEVICE_TIMER1_2_LOAD    \
    ((volatile U32 *)(TIMER1_BASE + 0x020))//Timer Load register
#define CYG_DEVICE_TIMER1_2_VALUE   \
    ((volatile U32 *)(TIMER1_BASE + 0x024))//Timer Value
#define CYG_DEVICE_TIMER1_2_CONTROL \
    ((volatile U32 *)(TIMER1_BASE + 0x028))//Timer Control Register
#define CYG_DEVICE_TIMER1_2_CLEAR   \
    ((volatile U32 *)(TIMER1_BASE + 0x02c))//Timer Clear Register
#define CYG_DEVICE_TIMER1_2_INTRAW  \
    ((volatile U32 *)(TIMER1_BASE + 0x030))//Raw Interrupt Status Register
#define CYG_DEVICE_TIMER1_2_INT     \
    ((volatile U32 *)(TIMER1_BASE + 0x034))//Interrupt Status Register
#define CYG_DEVICE_TIMER1_2_LOADBG  \
    ((volatile U32 *)(TIMER1_BASE + 0x038))//Background load Register
                           
#define CYG_DEVICE_TIMER1_ITCR      \
    ((volatile U32 *)(TIMER1_BASE + 0x080))//Integration Test Control Register
#define CYG_DEVICE_TIMER1_ITOP      \
    ((volatile U32 *)(TIMER1_BASE + 0x084))//Intergration Test Output set Register 
                           
#define CYG_DEVICE_TIMER1_PERIPH_ID0\
    ((volatile U32 *)(TIMER1_BASE + 0x0A0))//Peripheral Identification Register 7:0
#define CYG_DEVICE_TIMER1_PERIPH_ID1\
    ((volatile U32 *)(TIMER1_BASE + 0x0A4))//Peripheral Identification Register 15:8
#define CYG_DEVICE_TIMER1_PERIPH_ID2\
    ((volatile U32 *)(TIMER1_BASE + 0x0A8))//Peripheral Identification Register 23:16
#define CYG_DEVICE_TIMER1_PERIPH_ID3\
    ((volatile U32 *)(TIMER1_BASE + 0x0Ac))//Peripheral Identification Register 31:24
#define CYG_DEVICE_TIMER1_PCELL_ID0 \
    ((volatile U32 *)(TIMER1_BASE + 0x0B0))//Prime Cell Identification Register 7:0
#define CYG_DEVICE_TIMER1_PCELL_ID1 \
    ((volatile U32 *)(TIMER1_BASE + 0x0B4))//Prime Cell Identification Register 15:8
#define CYG_DEVICE_TIMER1_PCELL_ID2 \
    ((volatile U32 *)(TIMER1_BASE + 0x0B8))//Prime Cell Identification Register 23:16
#define CYG_DEVICE_TIMER1_PCELL_ID3 \
    ((volatile U32 *)(TIMER1_BASE + 0x0Bc))//Prime Cell Identification Register 31:24
                           
                           
///////////////////////////////////////////////////////////////////////////////
//                          TIMER2 Registers                                 //
//                          Base Address: 0xF8083000                         //
///////////////////////////////////////////////////////////////////////////////

#define CYG_DEVICE_TIMER2_1_LOAD    \
    ((volatile U32 *)(TIMER2_BASE + 0x000))//Timer Load register
#define CYG_DEVICE_TIMER2_1_VALUE   \
    ((volatile U32 *)(TIMER2_BASE + 0x004))//Timer Value
#define CYG_DEVICE_TIMER2_1_CONTROL \
    ((volatile U32 *)(TIMER2_BASE + 0x008))//Timer Control Register
#define CYG_DEVICE_TIMER2_1_CLEAR   \
    ((volatile U32 *)(TIMER2_BASE + 0x00c))//Timer Clear Register
#define CYG_DEVICE_TIMER2_1_INTRAW  \
    ((volatile U32 *)(TIMER2_BASE + 0x010))//Raw Interrupt Status Register
#define CYG_DEVICE_TIMER2_1_INT     \
    ((volatile U32 *)(TIMER2_BASE + 0x014))//Interrupt Status Register
#define CYG_DEVICE_TIMER2_1_LOADBG  \
    ((volatile U32 *)(TIMER2_BASE + 0x018))//Background load Register
                           
#define CYG_DEVICE_TIMER2_2_LOAD    \
    ((volatile U32 *)(TIMER2_BASE + 0x020))//Timer Load register
#define CYG_DEVICE_TIMER2_2_VALUE   \
    ((volatile U32 *)(TIMER2_BASE + 0x024))//Timer Value
#define CYG_DEVICE_TIMER2_2_CONTROL \
    ((volatile U32 *)(TIMER2_BASE + 0x028))//Timer Control Register
#define CYG_DEVICE_TIMER2_2_CLEAR   \
    ((volatile U32 *)(TIMER2_BASE + 0x02c))//Timer Clear Register
#define CYG_DEVICE_TIMER2_2_INTRAW  \
    ((volatile U32 *)(TIMER2_BASE + 0x030))//Raw Interrupt Status Register
#define CYG_DEVICE_TIMER2_2_INT     \
    ((volatile U32 *)(TIMER2_BASE + 0x034))//Interrupt Status Register
#define CYG_DEVICE_TIMER2_2_LOADBG  \
    ((volatile U32 *)(TIMER2_BASE + 0x038))//Background load Register
                           
#define CYG_DEVICE_TIMER2_ITCR      \
    ((volatile U32 *)(TIMER2_BASE + 0x080))//Integration Test Control Register
#define CYG_DEVICE_TIMER2_ITOP      \
    ((volatile U32 *)(TIMER2_BASE + 0x084))//Intergration Test Output set Register 
                           
#define CYG_DEVICE_TIMER2_PERIPH_ID0\
    ((volatile U32 *)(TIMER2_BASE + 0x0A0))//Peripheral Identification Register 7:0
#define CYG_DEVICE_TIMER2_PERIPH_ID1\
    ((volatile U32 *)(TIMER2_BASE + 0x0A4))//Peripheral Identification Register 15:8
#define CYG_DEVICE_TIMER2_PERIPH_ID2\
    ((volatile U32 *)(TIMER2_BASE + 0x0A8))//Peripheral Identification Register 23:16
#define CYG_DEVICE_TIMER2_PERIPH_ID3\
    ((volatile U32 *)(TIMER2_BASE + 0x0Ac))//Peripheral Identification Register 31:24
#define CYG_DEVICE_TIMER2_PCELL_ID0 \
    ((volatile U32 *)(TIMER2_BASE + 0x0B0))//Prime Cell Identification Register 7:0
#define CYG_DEVICE_TIMER2_PCELL_ID1 \
    ((volatile U32 *)(TIMER2_BASE + 0x0B4))//Prime Cell Identification Register 15:8
#define CYG_DEVICE_TIMER2_PCELL_ID2 \
    ((volatile U32 *)(TIMER2_BASE + 0x0B8))//Prime Cell Identification Register 23:16
#define CYG_DEVICE_TIMER2_PCELL_ID3 \
    ((volatile U32 *)(TIMER2_BASE + 0x0Bc))//Prime Cell Identification Register 31:24
                           
                           
///////////////////////////////////////////////////////////////////////////////
//                          TIMER3 Registers                                 //
//                          Base Address: 0xF8084000                         //
///////////////////////////////////////////////////////////////////////////////

#define CYG_DEVICE_TIMER3_1_LOAD    \
    ((volatile U32 *)(TIMER3_BASE + 0x000))//Timer Load register
#define CYG_DEVICE_TIMER3_1_VALUE   \
    ((volatile U32 *)(TIMER3_BASE + 0x004))//Timer Value
#define CYG_DEVICE_TIMER3_1_CONTROL \
    ((volatile U32 *)(TIMER3_BASE + 0x008))//Timer Control Register
#define CYG_DEVICE_TIMER3_1_CLEAR   \
    ((volatile U32 *)(TIMER3_BASE + 0x00c))//Timer Clear Register
#define CYG_DEVICE_TIMER3_1_INTRAW  \
    ((volatile U32 *)(TIMER3_BASE + 0x010))//Raw Interrupt Status Register
#define CYG_DEVICE_TIMER3_1_INT     \
    ((volatile U32 *)(TIMER3_BASE + 0x014))//Interrupt Status Register
#define CYG_DEVICE_TIMER3_1_LOADBG  \
    ((volatile U32 *)(TIMER3_BASE + 0x018))//Background load Register
                           
#define CYG_DEVICE_TIMER3_2_LOAD    \
    ((volatile U32 *)(TIMER3_BASE + 0x020))//Timer Load register
#define CYG_DEVICE_TIMER3_2_VALUE   \
    ((volatile U32 *)(TIMER3_BASE + 0x024))//Timer Value
#define CYG_DEVICE_TIMER3_2_CONTROL \
    ((volatile U32 *)(TIMER3_BASE + 0x028))//Timer Control Register
#define CYG_DEVICE_TIMER3_2_CLEAR   \
    ((volatile U32 *)(TIMER3_BASE + 0x02c))//Timer Clear Register
#define CYG_DEVICE_TIMER3_2_INTRAW  \
    ((volatile U32 *)(TIMER3_BASE + 0x030))//Raw Interrupt Status Register
#define CYG_DEVICE_TIMER3_2_INT     \
    ((volatile U32 *)(TIMER3_BASE + 0x034))//Interrupt Status Register
#define CYG_DEVICE_TIMER3_2_LOADBG  \
    ((volatile U32 *)(TIMER3_BASE + 0x038))//Background load Register
                           
#define CYG_DEVICE_TIMER3_ITCR      \
    ((volatile U32 *)(TIMER3_BASE + 0x080))//Integration Test Control Register
#define CYG_DEVICE_TIMER3_ITOP      \
    ((volatile U32 *)(TIMER3_BASE + 0x084))//Intergration Test Output set Register 
                           
#define CYG_DEVICE_TIMER3_PERIPH_ID0\
    ((volatile U32 *)(TIMER3_BASE + 0x0A0))//Peripheral Identification Register 7:0
#define CYG_DEVICE_TIMER3_PERIPH_ID1\
    ((volatile U32 *)(TIMER3_BASE + 0x0A4))//Peripheral Identification Register 15:8
#define CYG_DEVICE_TIMER3_PERIPH_ID2\
    ((volatile U32 *)(TIMER3_BASE + 0x0A8))//Peripheral Identification Register 23:16
#define CYG_DEVICE_TIMER3_PERIPH_ID3\
    ((volatile U32 *)(TIMER3_BASE + 0x0Ac))//Peripheral Identification Register 31:24
#define CYG_DEVICE_TIMER3_PCELL_ID0 \
    ((volatile U32 *)(TIMER3_BASE + 0x0B0))//Prime Cell Identification Register 7:0
#define CYG_DEVICE_TIMER3_PCELL_ID1 \
    ((volatile U32 *)(TIMER3_BASE + 0x0B4))//Prime Cell Identification Register 15:8
#define CYG_DEVICE_TIMER3_PCELL_ID2 \
    ((volatile U32 *)(TIMER3_BASE + 0x0B8))//Prime Cell Identification Register 23:16
#define CYG_DEVICE_TIMER3_PCELL_ID3 \
    ((volatile U32 *)(TIMER3_BASE + 0x0Bc))//Prime Cell Identification Register 31:24
                           
///////////////////////////////////////////////////////////////////////////////
//                          WDOG Registers                                   //
//                          Base Address: 0xF8090000                         //
///////////////////////////////////////////////////////////////////////////////

#define CYG_DEVICE_WDOG_LOAD   \
    ((volatile U32 *)(WDOG_BASE + 0x000))//Watch Dog Timer Load register
#define CYG_DEVICE_WDOG_VALUE  \
    ((volatile U32 *)(WDOG_BASE + 0x004))//Watch Dog Timer Value
#define CYG_DEVICE_WDOG_CONTROL\
    ((volatile U32 *)(WDOG_BASE + 0x008))//Control Register
#define CYG_DEVICE_WDOG_CLEAR  \
    ((volatile U32 *)(WDOG_BASE + 0x00c))//Interrupt Clear register
#define CYG_DEVICE_WDOG_INTRAW \
    ((volatile U32 *)(WDOG_BASE + 0x010))//Raw Interrupt Status register
#define CYG_DEVICE_WDOG_INT    \
    ((volatile U32 *)(WDOG_BASE + 0x014))//Interrupt Status register
#define CYG_DEVICE_WDOG_LOCK   \
    ((volatile U32 *)(WDOG_BASE + 0x300))//Lock Register
                           
#define CYG_DEVICE_WDOG_TCR    \
    ((volatile U32 *)(WDOG_BASE + 0xF00))//Integration Test Control register
#define CYG_DEVICE_WDOG_ITOP   \
    ((volatile U32 *)(WDOG_BASE + 0xF04))//Integration Test Output set register
                           
#define CYG_DEVICE_WDOG_PERIPH_ID0\
    ((volatile U32 *)(WDOG_BASE + 0xFE0))//Peripheral Identification Register 7:0
#define CYG_DEVICE_WDOG_PERIPH_ID1\
    ((volatile U32 *)(WDOG_BASE + 0xFE4))//Peripheral Identification Register 15:8
#define CYG_DEVICE_WDOG_PERIPH_ID2\
    ((volatile U32 *)(WDOG_BASE + 0xFE8))//Peripheral Identification Register 23:16
#define CYG_DEVICE_WDOG_PERIPH_ID3\
    ((volatile U32 *)(WDOG_BASE + 0xFEc))//Peripheral Identification Register 31:24
#define CYG_DEVICE_WDOG_PCELL_ID0 \
    ((volatile U32 *)(WDOG_BASE + 0xFF0))//Prime Cell Identification Register 7:0
#define CYG_DEVICE_WDOG_PCELL_ID1 \
    ((volatile U32 *)(WDOG_BASE + 0xFF4))//Prime Cell Identification Register 15:8
#define CYG_DEVICE_WDOG_PCELL_ID2 \
    ((volatile U32 *)(WDOG_BASE + 0xFF8))//Prime Cell Identification Register 23:16
#define CYG_DEVICE_WDOG_PCELL_ID3 \
    ((volatile U32 *)(WDOG_BASE + 0xFFc))//Prime Cell Identification Register 31:24

///////////////////////////////////////////////////////////////////////////////
//                          USB Registers                                   //
//                          Base Address: 0xFFFF7000                         //
///////////////////////////////////////////////////////////////////////////////
#define CYG_DEVICE_USB_ID		   ((USB_BASE+0x0000))
#define CYG_DEVICE_USB_HWGEN 	   ((USB_BASE+0x0004))
#define CYG_DEVICE_USB_HWDEV     ((USB_BASE+0x000C))
#define CYG_DEVICE_USB_TXBUF   	   ((USB_BASE+0x0010))
#define CYG_DEVICE_USB_RXBUF       ((USB_BASE+0x0014))

//#define CYG_DEVICE_USB_GPTIMER0LD ((USB_BASE+0x0080))
//#define CYG_DEVICE_USB_GPTIMER0CTL ((USB_BASE+0x0084))
//#define CYG_DEVICE_USB_GPTIMER1LD  ((USB_BASE+0x0088))
//#define CYG_DEVICE_USB_GPTIMER1CTL ((USB_BASE+0x008C))
//@Hongji
//TavorL has removed these registers

#define CYG_DEVICE_USB_CAPLEN     	((USB_BASE+0x0100))
#define CYG_DEVICE_USB_DCIVER         ((USB_BASE+0x0120))
#define CYG_DEVICE_USB_DCCPARAMS  ((USB_BASE+0x0124))

#define CYG_DEVICE_USB_CMD            	((USB_BASE+0x0140))
#define CYG_DEVICE_USB_STS   		((USB_BASE+0x0144))
#define CYG_DEVICE_USB_INTR           	((USB_BASE+0x0148))
#define CYG_DEVICE_USB_FRINDX     	((USB_BASE+0x014C))
#define CYG_DEVICE_USB_DEVADDR  	((USB_BASE+0x0154))
#define CYG_DEVICE_USB_ENDPLADDR 	((USB_BASE+0x0158))
#define CYG_DEVICE_USB_BURSTSIZE   ((USB_BASE+0x0160))

//#define CYG_DEVICE_USB_ULPIVIEW     ((USB_BASE+0x0170))
//@Hongji
//TavorL has removed these registers



#define CYG_DEVICE_USB_ENDPNAK      ((USB_BASE+0x0178))
#define CYG_DEVICE_USB_ENDPNAKEN  ((USB_BASE+0x017C))

#define CYG_DEVICE_USB_PORTSC1   	((USB_BASE+0x0184))
#define CYG_DEVICE_OTG_SC 			((USB_BASE+0x01A4))
//@Hongji add
#define CYG_DEVICE_USB_MODE  		((USB_BASE+0x01A8))
#define CYG_DEVICE_USB_ENDPSETSTS ((USB_BASE+0x01AC))
#define CYG_DEVICE_USB_ENDPPRIME   ((USB_BASE+0x01B0))
#define CYG_DEVICE_USB_ENDPFLUSH   ((USB_BASE+0x01B4))
#define CYG_DEVICE_USB_ENDPSTS       ((USB_BASE+0x01B8))
#define CYG_DEVICE_USB_ENDPCOMP   	 ((USB_BASE+0x01BC))
#define CYG_DEVICE_USB_ENDPCTL0      ((USB_BASE+0x01C0))
#define CYG_DEVICE_USB_ENDPCTL1      ((USB_BASE+0x01C4))
#define CYG_DEVICE_USB_ENDP2CTL2    ((USB_BASE+0x01C8))
#define CYG_DEVICE_USB_ENDP3CTL3    ((USB_BASE+0x01CC))
                           
///////////////////////////////////////////////////////////////////////////////
//                          SDIO Registers                                   //
//                          Base Address: 0xFFFF9000                         //
///////////////////////////////////////////////////////////////////////////////
#define CYG_DEVICE_SDIO_SDADDR      \
    ((volatile U32 *)(SDIO_BASE + 0x000))//system Address Register
#define CYG_DEVICE_SDIO_SDSIZE       \
    ((volatile U32 *)(SDIO_BASE + 0x004))//Block Size and count Register
#define CYG_DEVICE_SDIO_SDARG       \
    ((volatile U32 *)(SDIO_BASE + 0x008))//SD Command Argument Register
#define CYG_DEVICE_SDIO_SDXFRMOD       \
    ((volatile U16 *)(SDIO_BASE + 0x00c))//Transfer Mode Register
#define CYG_DEVICE_SDIO_SDXFRCMD       \
    ((volatile U16 *)(SDIO_BASE + 0x00e))//Command Register
#define CYG_DEVICE_SDIO_SDCMDRSP0       \
    ((volatile U32 *)(SDIO_BASE + 0x010))// Response Register 0
#define CYG_DEVICE_SDIO_SDCMDRSP1       \
    ((volatile U32 *)(SDIO_BASE + 0x014))// Response Register 1
#define CYG_DEVICE_SDIO_SDCMDRSP2       \
    ((volatile U32 *)(SDIO_BASE + 0x018))// Response Register 2
#define CYG_DEVICE_SDIO_SDCMDRSP3       \
    ((volatile U32 *)(SDIO_BASE + 0x01c))// Response Register 3
#define CYG_DEVICE_SDIO_SDBUFPORT    \
    ((volatile U32 *)(SDIO_BASE + 0x020))// Buffer Data Port Register
#define CYG_DEVICE_SDIO_SDSTATE     \
    ((volatile U32 *)(SDIO_BASE + 0x024))// Present State Register
#define CYG_DEVICE_SDIO_SDCTRL0      \
    ((volatile U32 *)(SDIO_BASE + 0x028))// Host control Register 0
#define CYG_DEVICE_SDIO_SDCTRL1      \
    ((volatile U32 *)(SDIO_BASE + 0x02c))// Host control Register 1
#define CYG_DEVICE_SDIO_SDINT     \
    ((volatile U16 *)(SDIO_BASE + 0x030))//Normal Interrupt Status Register
#define CYG_DEVICE_SDIO_SDERRINT    \
    ((volatile U16 *)(SDIO_BASE + 0x032))//Error Interrupt Status Register
#define CYG_DEVICE_SDIO_SDINTEN     \
    ((volatile U16 *)(SDIO_BASE + 0x034))//Normal Interrupt Status Enable Register
#define CYG_DEVICE_SDIO_SDERRINTEN     \
    ((volatile U16 *)(SDIO_BASE + 0x036))//Error Interrupt Status Enable Register
#define CYG_DEVICE_SDIO_SDSIGINTEN     \
    ((volatile U16 *)(SDIO_BASE + 0x038))//Normal Interrupt Signal Enable Register
#define CYG_DEVICE_SDIO_SDERRSIGINTEN     \
    ((volatile U16 *)(SDIO_BASE + 0x03a))//Error Interrupt Signal Enable Register
#define CYG_DEVICE_SDIO_SDAUTOCMDERR     \
    ((volatile U16 *)(SDIO_BASE + 0x03c))//Auto CMD12 Error Status  Register
#define CYG_DEVICE_SDIO_SDCAPAB     \
    ((volatile U32 *)(SDIO_BASE + 0x040))// Capabilities Register
#define CYG_DEVICE_SDIO_SDMAXCURR     \
    ((volatile U32 *)(SDIO_BASE + 0x048))// Maximum Current Capabilities Register
#define CYG_DEVICE_SDIO_SDSLOT     \
    ((volatile U16 *)(SDIO_BASE + 0x0fc))// Slot Interrupt Status Register
                           
///////////////////////////////////////////////////////////////////////////////
//                          SSMC Registers                                   //
//                          Base Address: 0xFFFFB000                         //
///////////////////////////////////////////////////////////////////////////////

#define CYG_DEVICE_SSMC_SMB_IDCYR0     \
    ((volatile U32 *)(SSMC_BASE + 0x000))//Bank0 Idle Cycle Control reg.
#define CYG_DEVICE_SSMC_SMB_WSTRDR0    \
    ((volatile U32 *)(SSMC_BASE + 0x004))//Bank0 read wait state Control reg.
#define CYG_DEVICE_SSMC_SMB_WSTRR0     \
    ((volatile U32 *)(SSMC_BASE + 0x008))//Bank0 write wait state control reg.
#define CYG_DEVICE_SSMC_SMB_WSTOENR0   \
    ((volatile U32 *)(SSMC_BASE + 0x00c))//Bank0 Output Enable Assertion delay Control reg.
#define CYG_DEVICE_SSMC_SMB_WSTWENR0   \
    ((volatile U32 *)(SSMC_BASE + 0x010))//Bank0 write Enable Assertion delay Control reg.
#define CYG_DEVICE_SSMC_SMB_CR0        \
    ((volatile U32 *)(SSMC_BASE + 0x014))//Bank0 Control reg.
#define CYG_DEVICE_SSMC_SMB_SR0        \
    ((volatile U32 *)(SSMC_BASE + 0x018))//Bank0 status reg.
#define CYG_DEVICE_SSMC_SMB_WSTBRDR0   \
    ((volatile U32 *)(SSMC_BASE + 0x01c))//Bank0 burst read wait delay Control reg.
                           
#define CYG_DEVICE_SSMC_SMB_IDCYR1     \
    ((volatile U32 *)(SSMC_BASE + 0x020))//Bank1 Idle Cycle Control reg.
#define CYG_DEVICE_SSMC_SMB_WSTRDR1    \
    ((volatile U32 *)(SSMC_BASE + 0x024))//Bank1 read wait state Control reg.
#define CYG_DEVICE_SSMC_SMB_WSTRR1     \
    ((volatile U32 *)(SSMC_BASE + 0x028))//Bank1 write wait state control reg.
#define CYG_DEVICE_SSMC_SMB_WSTOENR1   \
    ((volatile U32 *)(SSMC_BASE + 0x02c))//Bank1 Output Enable Assertion delay Control reg.
#define CYG_DEVICE_SSMC_SMB_WSTWENR1   \
    ((volatile U32 *)(SSMC_BASE + 0x030))//Bank1 write Enable Assertion delay Control reg.
#define CYG_DEVICE_SSMC_SMB_CR1        \
    ((volatile U32 *)(SSMC_BASE + 0x034))//Bank1 Control reg.
#define CYG_DEVICE_SSMC_SMB_SR1        \
    ((volatile U32 *)(SSMC_BASE + 0x038))//Bank1 status reg.
#define CYG_DEVICE_SSMC_SMB_WSTBRDR1   \
    ((volatile U32 *)(SSMC_BASE + 0x03c))//Bank1 burst read wait delay Control reg.
                           
#define CYG_DEVICE_SSMC_SMB_IDCYR2     \
    ((volatile U32 *)(SSMC_BASE + 0x040))//Bank2 Idle Cycle Control reg.
#define CYG_DEVICE_SSMC_SMB_WSTRDR2    \
    ((volatile U32 *)(SSMC_BASE + 0x044))//Bank2 read wait state Control reg.
#define CYG_DEVICE_SSMC_SMB_WSTRR2     \
    ((volatile U32 *)(SSMC_BASE + 0x048))//Bank2 write wait state control reg.
#define CYG_DEVICE_SSMC_SMB_WSTOENR2   \
    ((volatile U32 *)(SSMC_BASE + 0x04c))//Bank2 Output Enable Assertion delay Control reg.
#define CYG_DEVICE_SSMC_SMB_WSTWENR2   \
    ((volatile U32 *)(SSMC_BASE + 0x050))//Bank2 write Enable Assertion delay Control reg.
#define CYG_DEVICE_SSMC_SMB_CR2        \
    ((volatile U32 *)(SSMC_BASE + 0x054))//Bank2 Control reg.
#define CYG_DEVICE_SSMC_SMB_SR2        \
    ((volatile U32 *)(SSMC_BASE + 0x058))//Bank2 status reg.
#define CYG_DEVICE_SSMC_SMB_WSTBRDR2   \
    ((volatile U32 *)(SSMC_BASE + 0x05c))//Bank2 burst read wait delay Control reg.
                           
#define CYG_DEVICE_SSMC_SMB_IDCYR3     \
    ((volatile U32 *)(SSMC_BASE + 0x060))//Bank3 Idle Cycle Control reg.
#define CYG_DEVICE_SSMC_SMB_WSTRDR3    \
    ((volatile U32 *)(SSMC_BASE + 0x064))//Bank3 read wait state Control reg.
#define CYG_DEVICE_SSMC_SMB_WSTRR3     \
    ((volatile U32 *)(SSMC_BASE + 0x068))//Bank3 write wait state control reg.
#define CYG_DEVICE_SSMC_SMB_WSTOENR3   \
    ((volatile U32 *)(SSMC_BASE + 0x06c))//Bank3 Output Enable Assertion delay Control reg.
#define CYG_DEVICE_SSMC_SMB_WSTWENR3   \
    ((volatile U32 *)(SSMC_BASE + 0x070))//Bank3 write Enable Assertion delay Control reg.
#define CYG_DEVICE_SSMC_SMB_CR3        \
    ((volatile U32 *)(SSMC_BASE + 0x074))//Bank3 Control reg.
#define CYG_DEVICE_SSMC_SMB_SR3        \
    ((volatile U32 *)(SSMC_BASE + 0x078))//Bank3 status reg.
#define CYG_DEVICE_SSMC_SMB_WSTBRDR3   \
    ((volatile U32 *)(SSMC_BASE + 0x07c))//Bank3 burst read wait delay Control reg.
                           
#define CYG_DEVICE_SSMC_SMB_IDCYR4     \
    ((volatile U32 *)(SSMC_BASE + 0x080))//Bank4 Idle Cycle Control reg.
#define CYG_DEVICE_SSMC_SMB_WSTRDR4    \
    ((volatile U32 *)(SSMC_BASE + 0x084))//Bank4 read wait state Control reg.
#define CYG_DEVICE_SSMC_SMB_WSTRR4     \
    ((volatile U32 *)(SSMC_BASE + 0x088))//Bank4 write wait state control reg.
#define CYG_DEVICE_SSMC_SMB_WSTOENR4   \
    ((volatile U32 *)(SSMC_BASE + 0x08c))//Bank4 Output Enable Assertion delay Control reg.
#define CYG_DEVICE_SSMC_SMB_WSTWENR4   \
    ((volatile U32 *)(SSMC_BASE + 0x090))//Bank4 write Enable Assertion delay Control reg.
#define CYG_DEVICE_SSMC_SMB_CR4        \
    ((volatile U32 *)(SSMC_BASE + 0x094))//Bank4 Control reg.
#define CYG_DEVICE_SSMC_SMB_SR4        \
    ((volatile U32 *)(SSMC_BASE + 0x098))//Bank4 status reg.
#define CYG_DEVICE_SSMC_SMB_WSTBRDR4\
    ((volatile U32 *)(SSMC_BASE + 0x09c))//Bank4 burst read wait delay Control reg.
                           
#define CYG_DEVICE_SSMC_SMB_IDCYR5  \
    ((volatile U32 *)(SSMC_BASE + 0x0a0))//Bank5 Idle Cycle Control reg.
#define CYG_DEVICE_SSMC_SMB_WSTRDR5 \
    ((volatile U32 *)(SSMC_BASE + 0x0a4))//Bank5 read wait state Control reg.
#define CYG_DEVICE_SSMC_SMB_WSTRR5  \
    ((volatile U32 *)(SSMC_BASE + 0x0a8))//Bank5 write wait state control reg.
#define CYG_DEVICE_SSMC_SMB_WSTOENR5\
    ((volatile U32 *)(SSMC_BASE + 0x0ac))//Bank5 Output Enable Assertion delay Control reg.
#define CYG_DEVICE_SSMC_SMB_WSTWENR5\
    ((volatile U32 *)(SSMC_BASE + 0x0b0))//Bank5 write Enable Assertion delay Control reg.
#define CYG_DEVICE_SSMC_SMB_CR5     \
    ((volatile U32 *)(SSMC_BASE + 0x0b4))//Bank5 Control reg.
#define CYG_DEVICE_SSMC_SMB_SR5     \
    ((volatile U32 *)(SSMC_BASE + 0x0b8))//Bank5 status reg.
#define CYG_DEVICE_SSMC_SMB_WSTBRDR5\
    ((volatile U32 *)(SSMC_BASE + 0x0bc))//Bank5 burst read wait delay Control reg.
                           
#define CYG_DEVICE_SSMC_SMB_IDCYR6  \
    ((volatile U32 *)(SSMC_BASE + 0x0c0))//Bank6 Idle Cycle Control reg.
#define CYG_DEVICE_SSMC_SMB_WSTRDR6 \
    ((volatile U32 *)(SSMC_BASE + 0x0c4))//Bank6 read wait state Control reg.
#define CYG_DEVICE_SSMC_SMB_WSTRR6  \
    ((volatile U32 *)(SSMC_BASE + 0x0c8))//Bank6 write wait state control reg.
#define CYG_DEVICE_SSMC_SMB_WSTOENR6\
    ((volatile U32 *)(SSMC_BASE + 0x0cc))//Bank6 Output Enable Assertion delay Control reg.
#define CYG_DEVICE_SSMC_SMB_WSTWENR6\
    ((volatile U32 *)(SSMC_BASE + 0x0d0))//Bank6 write Enable Assertion delay Control reg.
#define CYG_DEVICE_SSMC_SMB_CR6     \
    ((volatile U32 *)(SSMC_BASE + 0x0d4))//Bank6 Control reg.
#define CYG_DEVICE_SSMC_SMB_SR6     \
    ((volatile U32 *)(SSMC_BASE + 0x0d8))//Bank6 status reg.
#define CYG_DEVICE_SSMC_SMB_WSTBRDR6\
    ((volatile U32 *)(SSMC_BASE + 0x0dc))//Bank6 burst read wait delay Control reg.
                           
#define CYG_DEVICE_SSMC_SMB_IDCYR7  \
    ((volatile U32 *)(SSMC_BASE + 0x0e0))//Bank7 Idle Cycle Control reg.
#define CYG_DEVICE_SSMC_SMB_WSTRDR7 \
    ((volatile U32 *)(SSMC_BASE + 0x0e4))//Bank7 read wait state Control reg.
#define CYG_DEVICE_SSMC_SMB_WSTRR7  \
    ((volatile U32 *)(SSMC_BASE + 0x0e8))//Bank7 write wait state control reg.
#define CYG_DEVICE_SSMC_SMB_WSTOENR7\
    ((volatile U32 *)(SSMC_BASE + 0x0ec))//Bank7 Output Enable Assertion delay Control reg.
#define CYG_DEVICE_SSMC_SMB_WSTWENR7\
    ((volatile U32 *)(SSMC_BASE + 0x0f0))//Bank7 write Enable Assertion delay Control reg.
#define CYG_DEVICE_SSMC_SMB_CR7     \
    ((volatile U32 *)(SSMC_BASE + 0x0f4))//Bank7 Control reg.
#define CYG_DEVICE_SSMC_SMB_SR7     \
    ((volatile U32 *)(SSMC_BASE + 0x0f8))//Bank7 status reg.
#define CYG_DEVICE_SSMC_SMB_WSTBRDR7\
    ((volatile U32 *)(SSMC_BASE + 0x0fc))//Bank7 burst read wait delay Control reg.
                           
#define CYG_DEVICE_SSMC_SR          \
    ((volatile U32 *)(SSMC_BASE + 0x200))//SSMC status reg.
#define CYG_DEVICE_SSMC_CR          \
    ((volatile U32 *)(SSMC_BASE + 0x204))//SSMC control reg.
#define CYG_DEVICE_SSMC_ITCR        \
    ((volatile U32 *)(SSMC_BASE + 0x208))//SSMC test control reg.
#define CYG_DEVICE_SSMC_ITIP        \
    ((volatile U32 *)(SSMC_BASE + 0x20c))//SSMC test input reg.
#define CYG_DEVICE_SSMC_ITOP        \
    ((volatile U32 *)(SSMC_BASE + 0x210))//SSMC test output reg.
                           
#define CYG_DEVICE_SSMC_PERIPH_ID0  \
    ((volatile U32 *)(SSMC_BASE + 0xFE0))//Peripheral Identification Register 7:0
#define CYG_DEVICE_SSMC_PERIPH_ID1  \
    ((volatile U32 *)(SSMC_BASE + 0xFE4))//Peripheral Identification Register 15:8
#define CYG_DEVICE_SSMC_PERIPH_ID2  \
    ((volatile U32 *)(SSMC_BASE + 0xFE8))//Peripheral Identification Register 23:16
#define CYG_DEVICE_SSMC_PERIPH_ID3  \
    ((volatile U32 *)(SSMC_BASE + 0xFEc))//Peripheral Identification Register 31:24
#define CYG_DEVICE_SSMC_PCELL_ID0   \
    ((volatile U32 *)(SSMC_BASE + 0xFF0))//Prime Cell Identification Register 7:0
#define CYG_DEVICE_SSMC_PCELL_ID1   \
    ((volatile U32 *)(SSMC_BASE + 0xFF4))//Prime Cell Identification Register 15:8
#define CYG_DEVICE_SSMC_PCELL_ID2   \
    ((volatile U32 *)(SSMC_BASE + 0xFF8))//Prime Cell Identification Register 23:16
#define CYG_DEVICE_SSMC_PCELL_ID3   \
    ((volatile U32 *)(SSMC_BASE + 0xFFc))//Prime Cell Identification Register 31:24
                           
///////////////////////////////////////////////////////////////////////////////
//                          DMAC Registers                                   //
//                          Base Address: 0xFFFFD000                         //
///////////////////////////////////////////////////////////////////////////////

#define CYG_DEVICE_DMAC_INT_STATUS         \
    ((volatile U32 *)(DMAC_BASE + 0x000))//DMAC interrupt 
                                                //Status Register
#define CYG_DEVICE_DMAC_INT_TC_STATUS       \
    ((volatile U32 *)(DMAC_BASE + 0x004))//DMAC interrupt TC 
                                                //Status Register
#define CYG_DEVICE_DMAC_INT_TC_CLEAR        \
    ((volatile U32 *)(DMAC_BASE + 0x008))//DMAC interrupt TC 
                                                //clear Register
#define CYG_DEVICE_DMAC_INT_ERR_STATUS      \
    ((volatile U32 *)(DMAC_BASE + 0x00c))//DMAC interrupt error 
                                                //status Register
#define CYG_DEVICE_DMAC_INT_ERR_CLEAR       \
    ((volatile U32 *)(DMAC_BASE + 0x010))//DMAC interrupt error 
                                                //clear Register
#define CYG_DEVICE_DMAC_RAW_INTTC_STATUS   \
    ((volatile U32 *)(DMAC_BASE + 0x014))//DMAC raw interrupt TC 
                                                //status Register
#define CYG_DEVICE_DMAC_RAW_INTERR_STATUS  \
    ((volatile U32 *)(DMAC_BASE + 0x018))//DMAC raw interrupt error 
                                                //status Register
#define CYG_DEVICE_DMAC_ENBLD_CHNS       \
    ((volatile U32 *)(DMAC_BASE + 0x01c))//DMAC enabled channels 
                                                //Register
                           
#define CYG_DEVICE_DMAC_SOFT_BREQ        \
    ((volatile U32 *)(DMAC_BASE + 0x020))//DMAC software burst 
                                                //request Register
#define CYG_DEVICE_DMAC_SOFT_SREQ        \
    ((volatile U32 *)(DMAC_BASE + 0x024))//DMAC software single 
                                                //request Register
#define CYG_DEVICE_DMAC_SOFT_LBREQ       \
    ((volatile U32 *)(DMAC_BASE + 0x028))//DMAC software last burst 
                                                //request Register
#define CYG_DEVICE_DMAC_SOFT_LSREQ       \
    ((volatile U32 *)(DMAC_BASE + 0x02c))//DMAC software last single 
                                                //request Register
                           
#define CYG_DEVICE_DMAC_CONFIG           \
    ((volatile U32 *)(DMAC_BASE + 0x030))//DMAC configuration Register
#define CYG_DEVICE_DMAC_SYNC             \
    ((volatile U32 *)(DMAC_BASE + 0x034))//DMAC synchronization Register
                           
#define CYG_DEVICE_DMAC_CHN0_SRC_ADDR      \
    ((volatile U32 *)(DMAC_BASE + 0x100))//DMAC channel 0 
                                                //source Register
#define CYG_DEVICE_DMAC_CHN0_DEST_ADDR     \
    ((volatile U32 *)(DMAC_BASE + 0x104))//DMAC channel 0 
                                                //destination Register
#define CYG_DEVICE_DMAC_CHN0_LLI           \
    ((volatile U32 *)(DMAC_BASE + 0x108))//DMAC channel 0 linked list 
                                                //address Register
#define CYG_DEVICE_DMAC_CHN0_CTRL          \
    ((volatile U32 *)(DMAC_BASE + 0x10c))//DMAC channel 0 
                                                //control Register
#define CYG_DEVICE_DMAC_CHN0_CFG        \
    ((volatile U32 *)(DMAC_BASE + 0x110))//DMAC channel 0 
                                                //configuration Register
                           
#define CYG_DEVICE_DMAC_CHN1_SRC_ADDR      \
    ((volatile U32 *)(DMAC_BASE + 0x120))//DMAC channel 0 
                                                //source Register
#define CYG_DEVICE_DMAC_CHN1_DEST_ADDR     \
    ((volatile U32 *)(DMAC_BASE + 0x124))//DMAC channel 0 
                                                //destination Register
#define CYG_DEVICE_DMAC_CHN1_LLI           \
    ((volatile U32 *)(DMAC_BASE + 0x128))//DMAC channel 0 linked list 
                                                //address Register
#define CYG_DEVICE_DMAC_CHN1_CTRL          \
    ((volatile U32 *)(DMAC_BASE + 0x12c))//DMAC channel 0 
                                                //control Register
#define CYG_DEVICE_DMAC_CHN1_CFG        \
    ((volatile U32 *)(DMAC_BASE + 0x130))//DMAC channel 0 
                                                //configuration Register
                           
#define CYG_DEVICE_DMAC_PERIPH_ID0       \
    ((volatile U32 *)(DMAC_BASE + 0xFE0))//Peripheral Identification 
                                                //Register 7:0
#define CYG_DEVICE_DMAC_PERIPH_ID1       \
    ((volatile U32 *)(DMAC_BASE + 0xFE4))//Peripheral Identification 
                                                //Register 15:8
#define CYG_DEVICE_DMAC_PERIPH_ID2       \
    ((volatile U32 *)(DMAC_BASE + 0xFE8))//Peripheral Identification 
                                                //Register 23:16
#define CYG_DEVICE_DMAC_PERIPH_ID3       \
    ((volatile U32 *)(DMAC_BASE + 0xFEc))//Peripheral Identification 
                                                //Register 31:24
#define CYG_DEVICE_DMAC_PCELL_ID0        \
    ((volatile U32 *)(DMAC_BASE + 0xFF0))//Prime Cell Identification 
                                                //Register 7:0
#define CYG_DEVICE_DMAC_PCELL_ID1        \
    ((volatile U32 *)(DMAC_BASE + 0xFF4))//Prime Cell Identification 
                                                //Register 15:8
#define CYG_DEVICE_DMAC_PCELL_ID2        \
    ((volatile U32 *)(DMAC_BASE + 0xFF8))//Prime Cell Identification 
                                                //Register 23:16
#define CYG_DEVICE_DMAC_PCELL_ID3        \
    ((volatile U32 *)(DMAC_BASE + 0xFFc))//Prime Cell Identification 
                                                //Register 31:24

///////////////////////////////////////////////////////////////////////////////
//                          NAND Registers                                    //
//                          Base Address: 0xFFFFD600                         //
///////////////////////////////////////////////////////////////////////////////
#define CYG_DEVICE_NANDFLASH_CTL        \
    ((volatile U32 *)(NAND_BASE + 0x000)) //NAND flash control
#define CYG_DEVICE_NANDFLASH_STS        \
        ((volatile U32 *)(NAND_BASE + 0x004)) //NAND flash status
#define CYG_DEVICE_NANDFLASH_ECC        \
        ((volatile U32 *)(NAND_BASE + 0x008)) //NAND flash ecc
                           
///////////////////////////////////////////////////////////////////////////////
//                          VIC Registers                                    //
//                          Base Address: 0xFFFFF000                         //
///////////////////////////////////////////////////////////////////////////////
                           
#define CYG_DEVICE_VIC_IRQ_STATUS       \
    ((volatile U32 *)(VIC_BASE + 0x000))//IRQ Status Register
#define CYG_DEVICE_VIC_FIQ_STATUS       \
    ((volatile U32 *)(VIC_BASE + 0x004))//FIQ Status Register
#define CYG_DEVICE_VIC_RAW_INT          \
    ((volatile U32 *)(VIC_BASE + 0x008))//RAW Interrupt Status 
                                                  //Register
#define CYG_DEVICE_VIC_INT_SELECT       \
    ((volatile U32 *)(VIC_BASE + 0x00c))//Interrupt Select Register
#define CYG_DEVICE_VIC_INT_ENABLE       \
    ((volatile U32 *)(VIC_BASE + 0x010))//Interrupt Enable Register
#define CYG_DEVICE_VIC_INT_ENCLR        \
    ((volatile U32 *)(VIC_BASE + 0x014))//Interrupt enable clear 
                                               //register
#define CYG_DEVICE_VIC_SOFT_INT         \
    ((volatile U32 *)(VIC_BASE + 0x018))//Software Interrupt Register
#define CYG_DEVICE_VIC_SOFT_INTCLR      \
    ((volatile U32 *)(VIC_BASE + 0x01c))//Software Interrupt Clear 
                                               //Register
#define CYG_DEVICE_VIC_PROTECTION       \
    ((volatile U32 *)(VIC_BASE + 0x020))//VIC Protection Register
#define CYG_DEVICE_VIC_VECT_ADDR        \
    ((volatile U32 *)(VIC_BASE + 0x030))//Vector Address Register
#define CYG_DEVICE_VIC_DEFVECT_ADDR     \
    ((volatile U32 *)(VIC_BASE + 0x034))//Default Vector Address 
                                               //Register 
                              
#define CYG_DEVICE_VIC_VECT_ADDR0       \
    ((volatile U32 *)(VIC_BASE + 0x100))//Vector Address 0 Register
#define CYG_DEVICE_VIC_VECT_ADDR1       \
    ((volatile U32 *)(VIC_BASE + 0x104))//Vector Address 1 Register
#define CYG_DEVICE_VIC_VECT_ADDR2       \
    ((volatile U32 *)(VIC_BASE + 0x108))//Vector Address 2 Register
#define CYG_DEVICE_VIC_VECT_ADDR3       \
    ((volatile U32 *)(VIC_BASE + 0x10c))//Vector Address 3 Register
#define CYG_DEVICE_VIC_VECT_ADDR4       \
    ((volatile U32 *)(VIC_BASE + 0x110))//Vector Address 4 Register
#define CYG_DEVICE_VIC_VECT_ADDR5       \
    ((volatile U32 *)(VIC_BASE + 0x114))//Vector Address 5 Register
#define CYG_DEVICE_VIC_VECT_ADDR6       \
    ((volatile U32 *)(VIC_BASE + 0x118))//Vector Address 6 Register
#define CYG_DEVICE_VIC_VECT_ADDR7       \
    ((volatile U32 *)(VIC_BASE + 0x11c))//Vector Address 7 Register
#define CYG_DEVICE_VIC_VECT_ADDR8       \
    ((volatile U32 *)(VIC_BASE + 0x120))//Vector Address 8 Register
#define CYG_DEVICE_VIC_VECT_ADDR9       \
    ((volatile U32 *)(VIC_BASE + 0x124))//Vector Address 9 Register
#define CYG_DEVICE_VIC_VECT_ADDR10      \
    ((volatile U32 *)(VIC_BASE + 0x128))//Vector Address 10 Register
#define CYG_DEVICE_VIC_VECT_ADDR11      \
    ((volatile U32 *)(VIC_BASE + 0x12c))//Vector Address 11 Register
#define CYG_DEVICE_VIC_VECT_ADDR12      \
    ((volatile U32 *)(VIC_BASE + 0x130))//Vector Address 12 Register
#define CYG_DEVICE_VIC_VECT_ADDR13      \
    ((volatile U32 *)(VIC_BASE + 0x134))//Vector Address 13 Register
#define CYG_DEVICE_VIC_VECT_ADDR14      \
    ((volatile U32 *)(VIC_BASE + 0x138))//Vector Address 14 Register
#define CYG_DEVICE_VIC_VECT_ADDR15      \
    ((volatile U32 *)(VIC_BASE + 0x13c))//Vector Address 15 Register
                           
#define CYG_DEVICE_VIC_VECT_CNTL0       \
    ((volatile U32 *)(VIC_BASE + 0x200))//Vector Control 0 Register
#define CYG_DEVICE_VIC_VECT_CNTL1       \
    ((volatile U32 *)(VIC_BASE + 0x204))//Vector Control 1 Register
#define CYG_DEVICE_VIC_VECT_CNTL2       \
    ((volatile U32 *)(VIC_BASE + 0x208))//Vector Control 2 Register
#define CYG_DEVICE_VIC_VECT_CNTL3       \
    ((volatile U32 *)(VIC_BASE + 0x20c))//Vector Control 3 Register
#define CYG_DEVICE_VIC_VECT_CNTL4       \
    ((volatile U32 *)(VIC_BASE + 0x210))//Vector Control 4 Register
#define CYG_DEVICE_VIC_VECT_CNTL5       \
    ((volatile U32 *)(VIC_BASE + 0x214))//Vector Control 5 Register
#define CYG_DEVICE_VIC_VECT_CNTL6       \
    ((volatile U32 *)(VIC_BASE + 0x218))//Vector Control 6 Register
#define CYG_DEVICE_VIC_VECT_CNTL7       \
    ((volatile U32 *)(VIC_BASE + 0x21c))//Vector Control 7 Register
#define CYG_DEVICE_VIC_VECT_CNTL8       \
    ((volatile U32 *)(VIC_BASE + 0x220))//Vector Control 8 Register
#define CYG_DEVICE_VIC_VECT_CNTL9       \
    ((volatile U32 *)(VIC_BASE + 0x224))//Vector Control 9 Register
#define CYG_DEVICE_VIC_VECT_CNTL10      \
    ((volatile U32 *)(VIC_BASE + 0x228))//Vector Control 10 Register
#define CYG_DEVICE_VIC_VECT_CNTL11      \
    ((volatile U32 *)(VIC_BASE + 0x22c))//Vector Control 11 Register
#define CYG_DEVICE_VIC_VECT_CNTL12      \
    ((volatile U32 *)(VIC_BASE + 0x230))//Vector Control 12 Register
#define CYG_DEVICE_VIC_VECT_CNTL13      \
    ((volatile U32 *)(VIC_BASE + 0x234))//Vector Control 13 Register
#define CYG_DEVICE_VIC_VECT_CNTL14      \
    ((volatile U32 *)(VIC_BASE + 0x238))//Vector Control 14 Register
#define CYG_DEVICE_VIC_VECT_CNTL15      \
    ((volatile U32 *)(VIC_BASE + 0x23c))//Vector Control 15 Register
                           
#define CYG_DEVICE_VIC_TCR              \
    ((volatile U32 *)(VIC_BASE + 0x300))    //Test Control Register
#define CYG_DEVICE_VIC_TIP1             \
    ((volatile U32 *)(VIC_BASE + 0x304))//Test Input Register (nVICIRQIN/nVICFIQIN)
#define CYG_DEVICE_VIC_TIP2             \
    ((volatile U32 *)(VIC_BASE + 0x308))//Test Input Register (VICVECTADDRIN)
#define CYG_DEVICE_VIC_TOP1             \
    ((volatile U32 *)(VIC_BASE + 0x30c))//Test Output Register (nVICIRQ/nVICFIQ)
#define CYG_DEVICE_VIC_TOP2             \
    ((volatile U32 *)(VIC_BASE + 0x310))//Test Output Register (VICVECTADDROUT)                           
#define CYG_DEVICE_VIC_PERIPH_ID0       \
    ((volatile U32 *)(VIC_BASE + 0xFE0))//Peripheral Identification 
                                                  //Register 7:0
#define CYG_DEVICE_VIC_PERIPH_ID1       \
    ((volatile U32 *)(VIC_BASE + 0xFE4))//Peripheral Identification 
                                                  //Register 15:8
#define CYG_DEVICE_VIC_PERIPH_ID2       \
    ((volatile U32 *)(VIC_BASE + 0xFE8))//Peripheral Identification 
                                                  //Register 23:16
#define CYG_DEVICE_VIC_PERIPH_ID3       \
    ((volatile U32 *)(VIC_BASE + 0xFEc))//Peripheral Identification 
                                                  //Register 31:24
#define CYG_DEVICE_VIC_PCELL_ID0        \
    ((volatile U32 *)(VIC_BASE + 0xFF0))//Prime Cell Identification 
                                                  //Register 7:0
#define CYG_DEVICE_VIC_PCELL_ID1        \
    ((volatile U32 *)(VIC_BASE + 0xFF4))//Prime Cell Identification 
                                                  //Register 15:8
#define CYG_DEVICE_VIC_PCELL_ID2        \
    ((volatile U32 *)(VIC_BASE + 0xFF8))//Prime Cell Identification 
                                                  //Register 23:16
#define CYG_DEVICE_VIC_PCELL_ID3        \
    ((volatile U32 *)(VIC_BASE + 0xFFc))//Prime Cell Identification 
                                                  //Register 31:24
#endif /* BBIC2_H */
/////////////////       END       //////////////////////////////////////           
                                                                                   
                                                                                   
                                                                                 
