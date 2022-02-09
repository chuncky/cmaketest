
#include "common.h"
#include "EEHandler.h"



/*----------- Local type definitions -----------------------------------------*/

typedef struct { // this structure contains a 32 bit field for every R/W 
                      // register of CP15 that might be modified after init (in run time).
    UINT32 dCacheLockdown;  /* inst. cache lockdown - register 9 */
    UINT32 iCacheLockdown;  /* data  cache lockdown - register 9 */
    UINT32 traceProcessID;  /* trace process ID     - register 13*/ 
    UINT32 testState;       /* Test state           - register 15*/
    UINT32 cacheDebugIndex; /* Cache debug index    - register 15*/
}CommPM_CP15BackupS;


// This typedef describes the structure at which the CPU register's image is kept.
typedef struct  {/** Total size - 31*4 == 124 bytes == 0x7C bytes. **/
    UINT32 usrR1_R14[14];/**size=0x38**/// place to store relevant user\system registers (r1-r14)
    UINT32 cpsr;         /**size=0x4 **/// place to store the CPSR
    UINT32 fiqR8_R14[7]; /**size=0x1C**/// place to store all FIQ registers  (r8-r14)
    UINT32 abtSP_LR[2];  /**size=0x8 **/// place to store the abort stack pointer and link register
    UINT32 undSP_LR[2];  /**size=0x8 **/// place to store the undefined stack pointer and link register
    UINT32 irqSP_LR[2];  /**size=0x8 **/// place to store the interrupt stack pointer and link register
    UINT32 svcSP_LR[2];  /**size=0x8 **/// place to store the supervisor stack pointer and link register
}CommPM_CPUImageRegsS;

// This struct defines the data stored (DS) in the DTCM.
typedef struct {
    CommPM_CPUImageRegsS cpuRegs; // This field must be first (the assembly code relies on it).
    CommPM_CP15BackupS   cp15Backup;
}CommPM_DTCM_DSS;











volatile UINT32 d2_debug_status=0;
unsigned long d2_cpsr_reg;
CommPM_DTCM_DSS _commpmDTCMds; // Data store
/*----------- Globals --------------------------------------------------------*/
 EE_RegInfo_Data_t    __Saved_Registers; //this can be deleted and I can use the EEirambuffer directly


UINT32      FatalErrorHandler   ( void ) {return(0);}
UINT32      SWInterruptHandler  ( void ) {return(0);}
UINT32      UndefInstHandler    ( void ) {return(0);}
UINT32      ReservedIntHandler  ( void ) {return(0);}
UINT32      DataAbortHandler    ( void ) {return(0);}
UINT32      PrefetchAbortHandler( void ) {return(0);}

UINT32        BranchZeroIntHandler  ( void )
{
	return 0;
}

