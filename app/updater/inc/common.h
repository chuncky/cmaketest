#ifndef __COMMON_H__
#define __COMMON_H__
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stdint.h>
#include "reg.h"

//typedef char int8_t;
//typedef short int16_t;
//typedef int int32_t;
//typedef unsigned char uint8_t;
//typedef unsigned short uint16_t;
//typedef unsigned int uint32_t;
//typedef unsigned long long int uint64_t;

 #ifndef VOID
    #define VOID	void
 #endif
//typedef char int8_t;
//typedef short int16_t;
//typedef int int32_t;
//typedef unsigned char uint8_t ;
//typedef unsigned short uint16_t;
//typedef unsigned int uint32_t;
//typedef unsigned long long uint64_t;
typedef signed char     INT8;         /* Signed 8-bit quantity          */
typedef signed short    INT16;        /* Signed 16-bit quantity         */
typedef signed long     INT32;        /* Signed 32-bit quantity         */
typedef unsigned char   UINT8;        /* Unsigned 8-bit quantity        */
typedef unsigned short  UINT16;       /* Unsigned 16-bit quantity       */
typedef unsigned int   UINT32;       /* Unsigned 32-bit quantity       */
typedef  unsigned char BOOL;
#define TRUE 1
#define FALSE 0 

//#define REG32(x)	*((volatile unsigned long*)(x))

#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))

#define max(a,b) (((a) > (b))? (a) : (b))
#define min(a,b) (((a) < (b))? (a) : (b))

int uart_printf(const char *fmt, ...);

/*+++++++++++++UPDATER Log_Priority Start++++++++++++++++++++++++*/
#define UPDATER_LOGSEVERITY_D 1
#define UPDATER_LOGSEVERITY_I 2
#define UPDATER_LOGSEVERITY_W 3
#define UPDATER_LOGSEVERITY_E 4

#ifndef UPDATER_LOGSEVERITY
#define UPDATER_LOGSEVERITY UPDATER_LOGSEVERITY_I	/* use macro "UPDATER_LOGSEVERITY" to control uart log priority */
#endif

#ifndef UPDATER_LOGNDEBUG
	#ifdef UPDATER_NDEBUG						/* use macro "UPDATER_NDEBUG" to control debug log in specific *.c file */
	#define UPDATER_LOGNDEBUG 1		
	#else
	#define UPDATER_LOGNDEBUG 0
	#endif
#endif

#define UPDATER_UART_PRINTF uart_printf

#if UPDATER_LOGNDEBUG
#define CP_LOGD(x,...)
#else
#define CP_LOGD(x,...) do{if (UPDATER_LOGSEVERITY <= UPDATER_LOGSEVERITY_D) UPDATER_UART_PRINTF(x,##__VA_ARGS__);}while(0)
#endif

#define CP_LOGI(x,...) do{if (UPDATER_LOGSEVERITY <= UPDATER_LOGSEVERITY_I) UPDATER_UART_PRINTF(x,##__VA_ARGS__);}while(0)
#define CP_LOGW(x,...) do{if (UPDATER_LOGSEVERITY <= UPDATER_LOGSEVERITY_W) UPDATER_UART_PRINTF(x,##__VA_ARGS__);}while(0)
#define CP_LOGE(x,...) do{if (UPDATER_LOGSEVERITY <= UPDATER_LOGSEVERITY_E) UPDATER_UART_PRINTF(x,##__VA_ARGS__);}while(0)

/*+++++++++++++UPDATER Log_Priority End++++++++++++++++++++++++*/


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

#define BU_U32 			unsigned int 
#define BU_U16 			unsigned short 
#define BU_U8 			unsigned char

#define BU_REG_READ(x) (*(volatile BU_U32 *)(x))
#define BU_REG_WRITE(x,y) ((*(volatile BU_U32 *)(x)) = y )

#define BU_REG_RDSET(x,y)  (BU_REG_WRITE(x,((BU_REG_READ(x))|y)))
#define BU_REG_RDCLEAR(x,y)  (BU_REG_WRITE(x,((BU_REG_READ(x))&(~y))))
#define ALIGN(val,exp)                  (((val) + ((exp)-1)) & ~((exp)-1))
#define BIT(nr)			(1UL << (nr))


// ---------------------------------------------------------------
// Memory Map
// ---------------------------------------------------------------
#define QSPI0_REG_BASE       0xd420B000 //QSPI1_IPS_BASE_ADDR
#define QSPI0_ARDB_BASE      0xa0000000 //AHB RX Data Buffer base addr(QSPI_ARDB0 to QSPI_ARDB31)
#define QSPI0_AMBA_BASE      0x80000000 //AHB base addr
#define QSPI0_FLASH_A1_BASE  0x80000000
#define QSPI0_FLASH_A1_TOP   0x88000000
#define QSPI0_FLASH_A2_BASE  0x88000000
#define QSPI0_FLASH_A2_TOP   0x90000000
#define QSPI0_FLASH_B1_BASE  0x90000000
#define QSPI0_FLASH_B1_TOP   0x98000000
#define QSPI0_FLASH_B2_BASE  0x98000000
#define QSPI0_FLASH_B2_TOP   0xa0000000

// ---------------------------------------------------------------
// Register definitions 
// ---------------------------------------------------------------
#define QSPI_MCR_OFFSET                      0x000
#define QSPI_IPCR_OFFSET                     0x008
#define QSPI_FLSHCR_OFFSET                   0x00C
#define QSPI_BUF0CR_OFFSET                   0x010
#define QSPI_BUF1CR_OFFSET                   0x014
#define QSPI_BUF2CR_OFFSET                   0x018
#define QSPI_BUF3CR_OFFSET                   0x01C
#define QSPI_BFGENCR_OFFSET                  0x020 
#define QSPI_SOCCR_OFFSET                    0x024 
#define QSPI_BUF0IND_OFFSET                  0x030 
#define QSPI_BUF1IND_OFFSET                  0x034 
#define QSPI_BUF2IND_OFFSET                  0x038 
#define QSPI_SFAR_OFFSET                     0x100 
#define QSPI_SFACR_OFFSET                    0x104
#define QSPI_SMPR_OFFSET                     0x108 
#define QSPI_RBSR_OFFSET                     0x10C 
#define QSPI_RBCT_OFFSET                     0x110 
#define QSPI_TBSR_OFFSET                     0x150 
#define QSPI_TBDR_OFFSET                     0x154 
#define QSPI_TBCT_OFFSET                     0x158 
#define QSPI_SR_OFFSET                       0x15C 
#define QSPI_FR_OFFSET                       0x160 
#define QSPI_RSER_OFFSET                     0x164 
#define QSPI_SPNDST_OFFSET                   0x168 
#define QSPI_SPTRCLR_OFFSET                  0x16C 
#define QSPI_SFA1AD_OFFSET                   0x180 
#define QSPI_SFA2AD_OFFSET                   0x184 
#define QSPI_SFB1AD_OFFSET                   0x188 
#define QSPI_SFB2AD_OFFSET                   0x18C 
#define QSPI_DLPV_OFFSET                     0x190 
#define QSPI_RBDR0_OFFSET                    0x200 
#define QSPI_LUTKEY_OFFSET                   0x300 
#define QSPI_LCKCR_OFFSET                    0x304 
#define QSPI_LUT0_OFFSET                     0x310
#define QSPI_LUT1_OFFSET                     0x314
#define QSPI_LUT2_OFFSET                     0x318
#define QSPI_LUT3_OFFSET                     0x31C

// ---------------------------------------------------------------
// Register definitions 
// ---------------------------------------------------------------
#define QSPI0_MCR            (QSPI0_REG_BASE+QSPI_MCR_OFFSET)
#define QSPI0_IPCR           (QSPI0_REG_BASE+QSPI_IPCR_OFFSET)
#define QSPI0_FLSHCR         (QSPI0_REG_BASE+QSPI_FLSHCR_OFFSET)
#define QSPI0_BUF0CR         (QSPI0_REG_BASE+QSPI_BUF0CR_OFFSET)
#define QSPI0_BUF1CR         (QSPI0_REG_BASE+QSPI_BUF1CR_OFFSET)
#define QSPI0_BUF2CR         (QSPI0_REG_BASE+QSPI_BUF2CR_OFFSET)
#define QSPI0_BUF3CR         (QSPI0_REG_BASE+QSPI_BUF3CR_OFFSET)
#define QSPI0_BFGENCR        (QSPI0_REG_BASE+QSPI_BFGENCR_OFFSET)
#define QSPI0_SOCCR          (QSPI0_REG_BASE+QSPI_SOCCR_OFFSET)
#define QSPI0_BUF0IND        (QSPI0_REG_BASE+QSPI_BUF0IND_OFFSET)
#define QSPI0_BUF1IND        (QSPI0_REG_BASE+QSPI_BUF1IND_OFFSET)
#define QSPI0_BUF2IND        (QSPI0_REG_BASE+QSPI_BUF2IND_OFFSET)
#define QSPI0_SFAR           (QSPI0_REG_BASE+QSPI_SFAR_OFFSET)
#define QSPI0_SFACR          (QSPI0_REG_BASE+QSPI_SFACR_OFFSET)
#define QSPI0_SMPR           (QSPI0_REG_BASE+QSPI_SMPR_OFFSET)
#define QSPI0_RBSR           (QSPI0_REG_BASE+QSPI_RBSR_OFFSET)
#define QSPI0_RBCT           (QSPI0_REG_BASE+QSPI_RBCT_OFFSET)
#define QSPI0_TBSR           (QSPI0_REG_BASE+QSPI_TBSR_OFFSET)
#define QSPI0_TBDR           (QSPI0_REG_BASE+QSPI_TBDR_OFFSET)
#define QSPI0_TBCT           (QSPI0_REG_BASE+QSPI_TBCT_OFFSET)
#define QSPI0_SR             (QSPI0_REG_BASE+QSPI_SR_OFFSET)
#define QSPI0_FR             (QSPI0_REG_BASE+QSPI_FR_OFFSET)
#define QSPI0_RSER           (QSPI0_REG_BASE+QSPI_RSER_OFFSET)
#define QSPI0_SPNDST         (QSPI0_REG_BASE+QSPI_SPNDST_OFFSET)
#define QSPI0_SPTRCLR        (QSPI0_REG_BASE+QSPI_SPTRCLR_OFFSET)
#define QSPI0_SFA1AD         (QSPI0_REG_BASE+QSPI_SFA1AD_OFFSET)
#define QSPI0_SFA2AD         (QSPI0_REG_BASE+QSPI_SFA2AD_OFFSET)
#define QSPI0_SFB1AD         (QSPI0_REG_BASE+QSPI_SFB1AD_OFFSET)
#define QSPI0_SFB2AD         (QSPI0_REG_BASE+QSPI_SFB2AD_OFFSET)
#define QSPI0_DLPV           (QSPI0_REG_BASE+QSPI_DLPV_OFFSET)
#define QSPI0_RBDR0          (QSPI0_REG_BASE+QSPI_RBDR0_OFFSET)
#define QSPI0_LUTKEY         (QSPI0_REG_BASE+QSPI_LUTKEY_OFFSET)
#define QSPI0_LCKCR          (QSPI0_REG_BASE+QSPI_LCKCR_OFFSET)
#define QSPI0_LUT0           (QSPI0_REG_BASE+QSPI_LUT0_OFFSET)
#define QSPI0_LUT1           (QSPI0_REG_BASE+QSPI_LUT1_OFFSET)
#define QSPI0_LUT2           (QSPI0_REG_BASE+QSPI_LUT2_OFFSET)
#define QSPI0_LUT3           (QSPI0_REG_BASE+QSPI_LUT3_OFFSET)

#define QSPI0_TBSR_TRBFL_SHIFT	8
#define QSPI0_TBSR_TRBFL_MASK	(0xff << QSPI0_TBSR_TRBFL_SHIFT)

#define QSPI0_MCR_XIP_EN	BIT_23
#define QSPI0_MCR_SW_PROG_ERASE	BIT_22

#define QSPI0_SR_AHB_ACC	BIT_2
#define QSPI0_SR_IP_ACC		BIT_1
#define QSPI0_SR_BUSY		BIT_0

#define QSPI0_FR_DLPFF		BIT_31
#define QSPI0_FR_TBFF		BIT_27
#define QSPI0_FR_TBUF		BIT_26
#define QSPI0_FR_ILLINE		BIT_23
#define QSPI0_FR_RBOF		BIT_17
#define QSPI0_FR_RBDF		BIT_16
#define QSPI0_FR_ABSEF		BIT_15
#define QSPI0_FR_AITEF		BIT_14
#define QSPI0_FR_AIBSEF		BIT_13
#define QSPI0_FR_ABOF		BIT_12
#define QSPI0_FR_IUEF		BIT_11
#define QSPI0_FR_IPAEF		BIT_7
#define QSPI0_FR_IPIEF		BIT_6
#define QSPI0_FR_IPGEF		BIT_4
#define QSPI0_FR_XIP_SUSPEND	BIT_3
#define QSPI0_FR_XIP_RESUME	BIT_2
#define QSPI0_FR_XIP_ON		BIT_1
#define QSPI0_FR_TFF		BIT_0

#define QSPI0_RSER_DLPFIE	BIT_31
#define QSPI0_RSER_TBFIE	BIT_27
#define QSPI0_RSER_TBUIE	BIT_26
#define QSPI0_RSER_TBFDE	BIT_25
#define QSPI0_RSER_ILLINIE	BIT_23
#define QSPI0_RSER_RBDDE	BIT_21
#define QSPI0_RSER_RBOIE	BIT_17
#define QSPI0_RSER_RBDIE	BIT_16
#define QSPI0_RSER_ABSEIE	BIT_15
#define QSPI0_RSER_AITIE	BIT_14
#define QSPI0_RSER_AIBSIE	BIT_13
#define QSPI0_RSER_ABOIE	BIT_12
#define QSPI0_RSER_IUEIE	BIT_11
#define QSPI0_RSER_IPIEIE	BIT_6
#define QSPI0_RSER_IPGEIE	BIT_4
#define QSPI0_RSER_XIP_ON	BIT_1
#define QSPI0_RSER_TFIE		BIT_0

// ---------------------------------------------------------------
// Enumeration & Structure
// ---------------------------------------------------------------
enum QSPI_INST_E {
	QSPI_INSTR_STOP = 0x0,
	QSPI_INSTR_CMD = 0x1,
	QSPI_INSTR_ADDR = 0x2,
	QSPI_INSTR_DUMMY = 0x3,
	QSPI_INSTR_MODE = 0x4,
	QSPI_INSTR_MODE2 = 0x5,
	QSPI_INSTR_MODE4 = 0x6,
	QSPI_INSTR_READ = 0x7,
	QSPI_INSTR_WRITE = 0x8,
	QSPI_INSTR_JMP_ON_CS = 0x9,
	QSPI_INSTR_ADDR_DDR = 0xA,
	QSPI_INSTR_MODE_DDR = 0xB,
	QSPI_INSTR_MODE2_DDR = 0xC,
	QSPI_INSTR_MODE4_DDR = 0xD,
	QSPI_INSTR_READ_DDR = 0xE,
	QSPI_INSTR_WRITE_DDR = 0xF,
	QSPI_INSTR_DATA_LEARN = 0x10
};
#if 0
enum QSPI_PAD_E {
	QSPI_PAD_1X = 0x0,
	QSPI_PAD_2X = 0x1,
	QSPI_PAD_4X = 0x2,
	QSPI_PAD_RSVD = 0x3
};
#endif

#define	EIO		5	/* I/O error */
#define	ENOMEM		12	/* Out of memory */
#define	ENODEV		19	/* No such device */
#define	EINVAL		22	/* Invalid argument */
#define	EBADMSG		74	/* Not a data message */
#define	ETIMEDOUT	110	/* Connection timed out */
#define	EUCLEAN		117	/* Chip needs cleaning */
#define ENOTSUPP	524	/* Operation is not supported */
#define   EAGAIN      11  /* Try again */

extern unsigned int disableInterrupts(void);
extern void restoreInterrupts(unsigned int cpsr);

#define FLASH_BLOCK_SIZE (0x00010000) //64*1024 - 1
#define FLASH_BLOCK_MASK (FLASH_BLOCK_SIZE-0x1) //64*1024 - 1





//OS api and define 
typedef void*   OSTaskRef;
typedef void*   OSSemaRef;
typedef void*   OSMutexRef;
typedef		void*				OS_HISR;
typedef UINT8   OSA_STATUS;
typedef unsigned int            UNSIGNED_INT;
#define OS_STATUS OSA_STATUS
#define DUMP_VOL "D:/"
#define OSA_SUSPEND             0xFFFFFFFF
#define OS_SUSPEND             OSA_SUSPEND
typedef void*   OSFlagRef;
typedef void*   OSTaskRef;
typedef void*   OSSemaRef;


enum
{
  OS_SUCCESS = 0,		 /* 0x0 -no errors										  */
  OS_FAIL,				 /* 0x1 -operation failed code							  */
  OS_TIMEOUT,			 /* 0x2 -Timed out waiting for a resource				  */
  OS_NO_RESOURCES,		 /* 0x3 -Internal OS resources expired					  */
  OS_INVALID_POINTER,	 /* 0x4 -0 or out of range pointer value				  */
  OS_INVALID_REF,		 /* 0x5 -invalid reference								  */
  OS_INVALID_DELETE,	 /* 0x6 -deleting an unterminated task					  */
  OS_INVALID_PTR,		 /* 0x7 -invalid memory pointer 						  */
  OS_INVALID_MEMORY,	 /* 0x8 -invalid memory pointer 						  */
  OS_INVALID_SIZE,		 /* 0x9 -out of range size argument 					  */
  OS_INVALID_MODE,		 /* 0xA, 10 -invalid mode								  */
  OS_INVALID_PRIORITY,	 /* 0xB, 11 -out of range task priority 				  */
  OS_UNAVAILABLE,		 /* 0xC, 12 -Service requested was unavailable or in use  */
  OS_POOL_EMPTY,		 /* 0xD, 13 -no resources in resource pool				  */
  OS_QUEUE_FULL,		 /* 0xE, 14 -attempt to send to full messaging queue	  */
  OS_QUEUE_EMPTY,		 /* 0xF, 15 -no messages on the queue					  */
  OS_NO_MEMORY, 		 /* 0x10, 16 -no memory left							  */
  OS_DELETED,			 /* 0x11, 17 -service was deleted						  */
  OS_SEM_DELETED,		 /* 0x12, 18 -semaphore was deleted 					  */
  OS_MUTEX_DELETED, 	 /* 0x13, 19 -mutex was deleted 						  */
  OS_MSGQ_DELETED,		 /* 0x14, 20 -msg Q was deleted 						  */
  OS_MBOX_DELETED,		 /* 0x15, 21 -mailbox Q was deleted 					  */
  OS_FLAG_DELETED,		 /* 0x16, 22 -flag was deleted							  */
  OS_INVALID_VECTOR,	 /* 0x17, 23 -interrupt vector is invalid				  */
  OS_NO_TASKS,			 /* 0x18, 24 -exceeded max # of tasks in the system 	  */
  OS_NO_FLAGS,			 /* 0x19, 25 -exceeded max # of flags in the system 	  */
  OS_NO_SEMAPHORES, 	 /* 0x1A, 26 -exceeded max # of semaphores in the system  */
  OS_NO_MUTEXES,		 /* 0x1B, 27 -exceeded max # of mutexes in the system	  */
  OS_NO_QUEUES, 		 /* 0x1C, 28 -exceeded max # of msg queues in the system  */
  OS_NO_MBOXES, 		 /* 0x1D, 29 -exceeded max # of mbox queues in the system */
  OS_NO_TIMERS, 		 /* 0x1E, 30 -exceeded max # of timers in the system	  */
  OS_NO_MEM_POOLS,		 /* 0x1F, 31 -exceeded max # of mem pools in the system   */
  OS_NO_INTERRUPTS, 	 /* 0x20, 32 -exceeded max # of isr's in the system 	  */
  OS_FLAG_NOT_PRESENT,	 /* 0x21, 33 -requested flag combination not present	  */
  OS_UNSUPPORTED,		 /* 0x22, 34 -service is not supported by the OS		  */
  OS_NO_MEM_CELLS,		 /* 0x23, 35 -no global memory cells					  */
  OS_DUPLICATE_NAME,	 /* 0x24, 36 -duplicate global memory cell name 		  */
  OS_INVALID_PARM		 /* 0x25, 37 -invalid parameter 						  */
};
#define OSA_MIN_STACK_SIZE      256
#define OSA_ENABLE_INTERRUPTS   1
#define OSA_DISABLE_INTERRUPTS  2
#define OSA_PIPE_MEM_OVERHEAD   4
#define OSA_NO_SUSPEND          0
#define OSA_FLAG_AND            5
#define OSA_FLAG_AND_CLEAR      6
#define OSA_FLAG_OR             7
#define OSA_FLAG_OR_CLEAR       8
#define OSA_FIXED               9
#define OSA_VARIABLE            10
#define OSA_FIFO                11
#define OSA_PRIORITY            12
typedef void*   OSATaskRef;
typedef void*   OSAHISRRef;
typedef void*   OSASemaRef;
typedef void*   OSAMutexRef;
typedef void*   OSAMsgQRef;
typedef void*   OSAMailboxQRef;
typedef void*   OSAPoolRef;
typedef void*   OSATimerRef;
typedef void*   OSAFlagRef;
typedef void*	OSAPartitionPoolRef;

OSA_STATUS OSAFlagWait( OSFlagRef Ref, UINT32 mask, UINT32 operation, UINT32 *flags, UINT32 timeout );
void CacheInvalidateMemory(void *pMem, UINT32 size);
void CacheCleanMemory(void *pMem, UINT32 size);
#define ASSERT(x) {if((x)==0){uart_printf("%s:%d\r\n", __FILE__, __LINE__);while(1);}}
void clear_fota_magic(void);

#endif // COMMON_H
