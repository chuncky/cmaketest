;------------------------------------------------------------
; (C) Copyright [2006-2008] Marvell International Ltd.
; All Rights Reserved
;------------------------------------------------------------

;
; INTEL CONFIDENTIAL
; Copyright 2004 Intel Corporation All Rights Reserved.
;
; The source code contained or described herein and all documents
; related to the source code (Material) are owned by Intel Corporation
; or its suppliers or licensors.  Title to the Material remains with
; Intel Corporation or its suppliers and licensors. The Material contains
; trade secrets and proprietary and confidential information of Intel
; or its suppliers and licensors. The Material is protected by worldwide
; copyright and trade secret laws and treaty provisions. No part of the
; Material may be used, copied, reproduced, modified, published, uploaded,
; posted, transmitted, distributed, or disclosed in any way without Intel's
; prior express written permission.
;
; No license under any patent, copyright, trade secret or other intellectual
; property right is granted to or conferred upon you by disclosure or
; delivery of the Materials, either expressly, by implication, inducement,
; estoppel or otherwise. Any license under such intellectual property rights
; must be express and approved by Intel in writing.
;
;
; Module Name:  vectors.s
;
; Abstract:
;          The main ENTRY is called in the init.s module and provides startup point.
;          the size of the init.s must be less than 0x1C0.
;          At the offset 0x1C0 the LoadTable is placed.
;          The reset of init code is implemented in the module init_continue.s
;          Since the dlm_jmptable is too big it is also placed in the init_continue.s
;*********************************************************************************
;

	PRESERVE8
  AREA Vect, CODE, READONLY

    ;---------------------
    EXPORT  IRQ_Addr
    EXPORT  FIQ_Addr
    ;---------------------

;/* Define the interrupt vector table, INT_Vectors.  This table is
;   assumed to be loaded or copied to address 0.  If coexistence with a
;   target-resident-monitor program is required, it is important to only
;   copy the IRQ and possibly the FIQ interrupt vectors in this table into
;   the actual table.  The idea is to not mess with the monitor's vectors.  */
;VOID    *INT_Vectors[NU_MAX_VECTORS];
;       Entry is also HERE !!!

 EXPORT  INT_Vectors
INT_Vectors
	LDR pc,(INT_Table + 0)
	LDR	pc,(INT_Table + 4)
	LDR	pc,(INT_Table + 8)
	LDR	pc,(INT_Table + 12)
	LDR	pc,(INT_Table + 16)
	LDR	pc,(INT_Table + 20)
	LDR	pc,(INT_Table + 24)
	LDR	pc,(INT_Table + 28)

   EXPORT  INT_Table
INT_Table
Initialize_Addr         DCD     INT_BranchZeroIntHandler
Undef_Inst_Addr         DCD     INT_Undef_Inst
Software_Addr           DCD     INT_Software
Abort_Prefetch_Addr     DCD     INT_Abort_Prefetch
Abort_Data_Addr         DCD     INT_Abort_Data
Reserved_Addr           DCD     INT_Reserved
IRQ_Addr                DCD     INT_IRQ
FIQ_Addr                DCD     INT_FIQ

  AREA D2_Vect, CODE, READONLY
  EXPORT  D2_INT_Vectors
D2_INT_Vectors
	LDR pc,(D2_INT_Table + 0)
	LDR	pc,(D2_INT_Table + 4)
	LDR	pc,(D2_INT_Table + 8)
	LDR	pc,(D2_INT_Table + 12)
	LDR	pc,(D2_INT_Table + 16)
	LDR	pc,(D2_INT_Table + 20)
	LDR	pc,(D2_INT_Table + 24)
	LDR	pc,(D2_INT_Table + 28)

   EXPORT  D2_INT_Table
   EXPORT  D2_Initialize_Addr
   EXPORT  D2_Undef_Inst_Addr
   EXPORT  D2_Software_Addr
   EXPORT  D2_Abort_Prefetch_Addr
   EXPORT  D2_Abort_Data_Addr
   EXPORT  D2_Reserved_Addr
   EXPORT  D2_IRQ_Addr
   EXPORT  D2_FIQ_Addr


   
D2_INT_Table
D2_Initialize_Addr         DCD     CommPMD2ExitHandlerAsmPhase2
D2_Undef_Inst_Addr         DCD     INT_Undef_Inst
D2_Software_Addr           DCD     INT_Software
D2_Abort_Prefetch_Addr     DCD     INT_Abort_Prefetch
D2_Abort_Data_Addr         DCD     INT_Abort_Data
D2_Reserved_Addr           DCD     INT_Reserved
D2_IRQ_Addr                DCD     INT_IRQ
D2_FIQ_Addr                DCD     INT_FIQ

T_BIT               EQU     &20             ; Thumb mode indication in CPSR and SPSR

;;==============================================
;;==============================================


	GBLL EE_HANDLER_ENABLE       ;always enable EEHandler call-outs in the low-level exception handlers


; ************************
; Exception Handlers
; ************************
;
;
;/* Define extern function references.  */
;
	IMPORT  INT_Initialize

   IF :DEF: EE_HANDLER_ENABLE

Saved_Registers
  IMPORT  __Saved_Registers
  DCD     __Saved_Registers

; THE FOLLOWING MACRO SAVES THE REGISTER CONTEXT AND PASSES CONTROL TO THE COMMON EXCEPTION HANDLER
; NOTE: r0-r12 are saved into Saved_Registers and can be restored from there
;       exception mode SP and CPSR are maintained; exception mode LR is saved on stack and restored from there
;       since the (SP,LR,CPSR) values in Saved_Registers are modified by the upper handler to reflect the excepted thread context
	MACRO
  EE_HANDLE   $handler
    IMPORT       $handler
    STMFD   sp!,{r0}                         ; Save r0
    LDR     r0,Saved_Registers               ; Pick up Saved_Registers variable's address
    add     r0,r0,#4                         ; reserve space for r0
    STMIA   r0!,{r1-r12,sp,lr}               ; save all registers r1-r12+lr+sp
    LDMFD   sp!,{r2}
    STMFD   sp!,{lr}
    STR     r2,[r0, #-60]                    ; Store original r0 into Saved_Registers.r0
    MRC     p15, 0, r2, c5, c0               ; extract FSR into r2
    STR     r2,[r0, #0x8]                    ; Store r2 into Saved_Registers.FSR
    MRC     p15, 0, r2, c6, c0               ; extract FAR into r2
    STR     r2,[r0, #0xC]                    ; Store r2 into Saved_Registers.FAR
    MRS     r2,CPSR                          ; extract CPSR into r2
    STR     r2,[r0, #0x4]                    ; Store r2 into Saved_registers.CPSR
    BL      $handler                         ; Jump to upper level handler; may return if disabled
    TST     r0,#1                            ; Return value indicates if to restore (1) or stall (0)
    LDR     r0,Saved_Registers               ; Pick up Saved_Registers variable's address
    LDMIA   r0,{r0-r12}                      ; restore registers
    LDMFD   sp!,{lr}                         ; restore original LR
    ; If return - use Saved_Registers.pc adjusted by the upper. level handler (either retry, skip, or whatever else).
    LDRNE   lr,Saved_Registers
    LDRNE   lr,[lr,#0x3c]
  MEND

  MACRO
  STALL_HERE  $label
$label
  BEQ     $label                    ; Stall if upper level handler returns 0
  MEND

  ELSE

	MACRO
	EE_HANDLE   $handler
  MEND

  MACRO
  STALL_HERE  $label
$label
   B       $label                    ; Stuck always
  MEND

   ENDIF

INT_BranchZeroIntHandler
	EE_HANDLE   BranchZeroIntHandler
  STALL_HERE  INT_BranchZeroIntHandler_loop
  MOVS    pc, lr

INT_Reserved
   EE_HANDLE   ReservedIntHandler
   STALL_HERE  INT_Reserved_loop
   MOVS    pc, lr

INT_Undef_Inst
   EE_HANDLE  UndefInstHandler
   STALL_HERE INT_Undef_Inst_loop                ; Undefined Instruction
   MOVS    pc, lr

INT_Software
   EE_HANDLE  SWInterruptHandler
   STALL_HERE INT_Software_loop
   MOVS    pc, lr

INT_Abort_Data
 ;  B .

   EE_HANDLE       DataAbortHandler                ; Abort Data
   STALL_HERE      INT_Abort_Data_loop
; ATTENTION: MUST EXECUTE THE BELOW CODE TO RETURN PRECISELY TO THE EXCEPTED INSTRUCTION
INT_Abort_Data_Restore
	 STMFD    sp!,{r0}
	 MRS      r0,SPSR
	 TST      r0,#T_BIT
	 LDMFD    sp!,{r0}
   SUBEQS   pc, lr, #8                             ; RETURN TO ARM MODE
	 SUBNES   pc, lr, #6                             ; RETURN TO THUMB MODE
;}


INT_Abort_Prefetch
	 STMFD   sp!,{r0,r1}
	 MRC     p15, 0, r0, c5, c0                  ; Read FSR
	 LDR     r1,fsrmask
	 AND     r0,r0,r1
   SUB     r0,r0,#0x400
   SUBS    r0,r0,#8
	 BNE     INT_Abort_Prefetch_err             ; Otherwise handle ICache parity error:
	 MCR     p15, 0, R14, c7, c5, 1              ; Invalidate the line
	 LDR     r0,count
	 LDR     r1,[r0,#0]
	 ADD     r1,r1,#1
	 STR     r1,[r0,#0]
	 LDMFD   sp!,{r0,r1}
   SUBS    pc, lr, #4                          ; RETRY

INT_Abort_Prefetch_err
	 LDMFD   sp!,{r0,r1}

   EE_HANDLE       PrefetchAbortHandler            ; Abort Prefetch
   STALL_HERE      INT_Abort_Prefetch_loop
   SUBS    pc, lr, #4

fsrmask DCD 0x40F
count   DCD ICParityCount

	 EXPORT ICParityCount
ICParityCount
	DCD     0


INT_Fatal_Error
   EE_HANDLE  FatalErrorHandler
   STALL_HERE INT_Fatal_Error_loop

;/////////////////////////////////////////////////////////////////////////
;
;   Symbol definitions for Coprocessor 15 and Coprocessor 15 Registers
;
;   Note: Multiple symbols for the same register may occur.  The names are
;         defined for usage in the MRC/MCR commands that access various
;         Coprocessor 15 functions.
;

CP15    CP  15 ; Alias for Coprocessor 15 (rather than just "p15")

;
;  CP15, Reg 0
;

CP15IDReg                   CN  0

Op2ProcID                   EQU 0
Op2CacheType                EQU 1
;/////////////////////////////////////////////////////////////////////////
;
;   General Macro Definitions for CP 15 Register Access
;
;   - The M terminator is used for all macros.
;   - When using the macros, the "$xxxReg" macro parameter must be in
;       {R0..R10} and should normally be R0 or R1.
;

;=========================================================================
; CPWAIT
;
;   Standard macro to let CP15 commands complete before moving to
;   next operation.  Taken from Intel XScale Core user's manual.
;
;   Input:         None
;   Return:        None
;   Side Effects:  R0 undefined
;
;
;;  The following macro should be used when software needs to be
;;  assured that a CP15 update has taken effect.
;;  It may only be used while in a privileged mode, because it
;;  accesses CP15.

    MACRO
        CPWAIT
        MRC CP15, 0, R0, c2, c0, 0 ; arbitrary read of CP15
        MOV R0, R0 ; wait for it
        ADR PC,{PC}+4 ; branch to next instruction; legacy form was SUB PC,PC,#4 - modified to avoid ARM RVCT warning A1789W 
        ; At this point, any previous CP15 writes are
        ; guaranteed to have taken effect.
    MEND


;/********************************************/
;/*    Final Action for EEhandler    */
;/********************************************/
;
  EXPORT Final_Action_Reset
Final_Action_Reset
;{
  LDR     r2,=0x04f6                       ; load reset value into the FSR
  MCR     p15, 0, r2, c5, c0               ;strore r2 -> FSR
  CPWAIT
  B     INT_Initialize
;}

  EXPORT Final_Action_Stall
Final_Action_Stall
;{
  B     Final_Action_Stall
;}

	GBLL COMPILE_OFF
COMPILE_OFF SETL {TRUE}

 IF COMPILE_OFF = {FALSE}
INT_HOOK_BRANCH_ZERO

; reset handler should first check whether this is a debug exception
; or a real RESET event.
; NOTE: r13 is only safe register to use.
; - For RESET, don't really care about which register is used
; - For debug exception, r13=DBG_r13, prevents application registers
; - from being corrupted, before debug handler can save.

	MACRO
	CHECK_DEBUG_MODE $DBG_REG,$DBG_HANDLER_ADDR
	mrs $DBG_REG, cpsr
	and $DBG_REG, $DBG_REG, #0x1f
	cmp $DBG_REG, #0x15 ; are we in DBG mode?
	beq $DBG_HANDLER_ADDR ; if so, go to the dbg handler stub
	mov $DBG_REG, #0x8000001c ; otherwise, enable debug, set MOE bits
	mcr p14, 0, $DBG_REG, c10, c0, 0 ; and continue with the reset handler
	MEND
; clobbered by the below register: relevant for BRANCH_ZERO case (SW debug) and JTAG hot-plug.
	 CHECK_DEBUG_MODE  r11,DBG_HANDLER_STUB
   EE_HANDLE   BranchZeroIntHandler
   STALL_HERE  INT_HOOK_BRANCH_ZERO_loop

; Hot-Debug Handler
; align code to a cache line boundary.
;DBG_ALIGNMENT DCD 0,0,0,0,0,0,0,0

	ALIGN		32
DBG_HANDLER_STUB
	;B ASSERT_DEBUGGER_CONNECTED
; First save the state of the IC enable/disable bit in DBG_LR[0].

	mrc p15, 0, r13, c1, c0, 0
	and r13, r13, #0x1000
	orr r14, r14, r13, lsr #12
; Next, enable the IC.
	mrc p15, 0, r13, c1, c0, 0
	orr r13, r13, #0x1000
	mcr p15, 0, r13, c1, c0, 0
; do a sync operation to ensure all outstanding instr fetches have
; completed before continuing. The invalidate cache line function
; serves as a synchronization operation, that's why it is used
; here. The target line is some scratch address in memory.
	adr r13, line2
	mcr p15, 0, r13, c7, c5, 1

; invalidate BTB. make sure downloaded vector table does not hit one of
; the application's branches cached in the BTB, branch to the wrong place
	mcr p15, 0, r13, c7, c5, 6

; Now, send 憆eady for download' message to debugger, indicating debugger
; can begin the download. 憆eady for download' = 0x00B00000.
TXloop mrc p14, 0, r15, c14, c0, 0 ; first make sure TX reg. is available
	bvs TXloop
	mov r13, #0x00B00000
	mcr p14, 0, r13, c8, c0, 0 ; now write to TX
; Wait for debugger to indicate that the download is complete.

RXloop mrc p14, 0, r15, c14, c0, 0 ; spin in loop waiting for data from the
			 bpl RXloop ; debugger in RX.
; before reading the RX regsiter to get the address to branch to, restore
; the state of the IC (saved in DBG_r14[0]) to the value it have at the
; start of the debug handler stub. Also, note it must be restored before
; reading the RX register because of limited scratch registers (r13)
	mrc p15, 0, r13, c1, c0, 0
; First, check DBG_LR[0] to see if the IC was enabled or disabled
	tst r14, #0x1
; Then, if it was previously disabled, then disable it now, otherwise,
; there's no need to change the state, because its already enabled.
	biceq r13, r13, #0x1000
	mcr p15, 0, r13, c1, c0, 0
; Now r13 can be used to read RX and get the target address to branch to.
	mrc p14, 0, r13, c9, c0, 0																					  ; Read RX and
	mov pc, r13 																													; branch to downloaded address.
; scratch memory space used by the invalidate IC line function above.

; boundary, so nothing else is affected

; make sure it starts at a cache line
	ALIGN		32
line2 DCD 0,0,0,0,0,0,0,0
 ENDIF

	EXPORT get_performance_count
	EXPORT get_overflow_flag_status


get_overflow_flag_status

	MRC p15, 0, r0, c9, c12, 3 ;read overflow Flag Status
	MCR p15, 0, r0, c9, c12, 3 ;clear overflow Flag Status
	
	BX	  lr
	
get_performance_count
	STMFD SP!,{r2,r3, LR}

	MRC p15, 0, r0, c9, c13, 0 ;read cycle count register

	LDMFD SP!,{r2,r3, LR}	
	BX	  lr	

;----------Internal definitions------------------;
MODE_MASK	EQU	 0x1F		 ; Processor Mode Mask
SUP_MODE	EQU	 0x13		 ; Supervisor Mode (SVC)
IRQ_MODE	EQU	 0x12		 ; Interrupt Mode (IRQ)
FIQ_MODE	EQU	 0x11		 ; Fast Interrupt Mode (FIQ)
UND_MODE	EQU	 0x1B		 ; Undefined Mode (UND)
ABT_MODE	EQU	 0x17		 ; Abort Mode (ABT)

;; True/False definitions.
TRUE  EQU 0x1
FALSE EQU 0x0

  IMPORT _commpmDTCMds
  IMPORT d2_debug_status
  IMPORT d2_cpsr_reg

_pCommpmRegs
    DCD _commpmDTCMds ;; cpuRegs is the first field. TODO: define an "assembly" struct that will free us from the stupid remark in the commpm_data_store.c
_pD2_cpsr_reg
    DCD d2_cpsr_reg

_pD2_status
	  DCD d2_debug_status

  ;void CommPMD2ExitHandlerAsmPhase2(void);
  ;EXPORT CommPMD2ExitHandlerAsmPhase2   ;; D2 exit handler Phase2.

;; CommPMD2ExitHandlerAsmPhase2 - this is the real D2 exit handler (which address is set in the CPMU)
CommPMD2ExitHandlerAsmPhase2
    
  ;;LDR   R0, _pD2_cpsr_reg
  ;;MRS   R1, CPSR
  ;;STR   R1, [R0]

  LDR   R0, _pD2_status
  LDR   R1, =0xD0D0D0D0
  STR   R1, [R0]
  DSB
   
  LDR R0, _pCommpmRegs       ; load the backup struct's address
                               
  ;; No use in restoring the CPSR and usr registers now.
  ADD R0, R0, #0x3C          ; increment R0 by 15*UINT32 (the size of all registers)                               


  ;; Change mode to FIQ
  MRS     R1,CPSR            ; Pickup current CPSR
	BIC     R1,R1,#MODE_MASK   ; Clear the mode bits
	ORR     R1,R1,#FIQ_MODE    ; Set the new mode bits to the CPSR image
	MSR     CPSR_c,R1       ; Setup the new CPSR

  ;; Restore fiq CPU registers.
  LDMIA R0!, {R8-R14}       ; Restore the registers and increment the pointer.

    
  ;; Change mode to ABT
  MRS     R1,CPSR            ; Pickup current CPSR
	BIC     R1,R1,#MODE_MASK   ; Clear the mode bits
	ORR     R1,R1,#ABT_MODE    ; Set the new mode bits to the CPSR image
	MSR     CPSR_c,R1       ; Setup the new CPSR

  ;; Restore abt CPU registers.
  LDMIA R0!, {R13,R14}       ; Restore the registers and increment the pointer.


  ;; Change mode to UND
  MRS     R1,CPSR            ; Pickup current CPSR
	BIC     R1,R1,#MODE_MASK   ; Clear the mode bits
	ORR     R1,R1,#UND_MODE    ; Set the new mode bits to the CPSR image
	MSR     CPSR_c,R1       ; Setup the new CPSR

  ;; Restore und CPU registers. 
  LDMIA R0!, {R13,R14}       ; Restore the registers and increment the pointer.


  ;; Change mode to IRQ
  MRS     R1,CPSR            ; Pickup current CPSR
	BIC     R1,R1,#MODE_MASK   ; Clear the mode bits
	ORR     R1,R1,#IRQ_MODE    ; Set the new mode bits to the CPSR image
	MSR     CPSR_c,R1       ; Setup the new CPSR

  ;; Restore irq CPU registers.
  LDMIA R0!, {R13,R14}       ; Restore the registers and increment the pointer.
    

  ;; Change mode to SVC
  MRS     R1,CPSR            ; Pickup current CPSR
	BIC     R1,R1,#MODE_MASK   ; Clear the mode bits
	ORR     R1,R1,#SUP_MODE    ; Set the new mode bits to the CPSR image
	MSR     CPSR_c,R1       ; Setup the new CPSR

  ;; Restore svc CPU registers.
  LDMIA R0!, {R13,R14}       ; Restore the registers and increment the pointer.


  ;; Restore usr CPU registers.
  LDR R0, _pCommpmRegs       ; Reload the struct's address.
	; THE ABOVE RESULT ON RVCT 3.1 IN THIS: Error: A1329W: Unpredictable instruction (forced user mode transfer with write-back to base)
	; NO REAL NEED TO SAVE USR MODE REGS, BUT PRESERVE THIS FOR NOW
  ;LDMIA R0!, {R1-R14}^       ; Restore the registers and increment the pointer.
	LDMIA R0, {R1-R14}^       ; Restore the registers and increment the pointer.
	ADD R0,R0,#0x38		; increment the pointer (4*14 registers)
                               ; (R0 - will hold the "is real D2" answer.)

  ;; Restore the CPSR
  LDMIA R0, {R0}             ; Restore to the register.
	MSR   CPSR_cxsf,R0         ; Recover the CPSR itself.
                               
    
  ;; Set R0 to a TRUE value, indicating that real D2 took place.    
  MOV R0, #TRUE   
  ;; Branch back to the CommPMLPT.
  BX LR



	EXPORT cp_ReadCCNT
	EXPORT cp14ReadCCNT
cp_ReadCCNT
cp14ReadCCNT
	LDR  R0, =0xD021002C
  LDR  R0, [R0]
  BX   LR
  
  IMPORT  INTCInterruptHandlerIRQ
;  IMPORT  _tx_thread_context_save
;  IMPORT  _tx_thread_context_restore
	EXPORT  _irq_processing_return
	EXPORT	_tx_irq_processing_return 
  EXPORT asm_isb
  EXPORT asm_dsb
asm_isb
  ISB
  BX LR

asm_dsb
  DSB
  BX LR

INT_IRQ
;{
    ; This Code is used to correctly handle interrupts and
    ; is necessary due to the nature of the ARM7 architecture

;  B      _tx_thread_context_save         ; Call ThreadX context save routine
  STMDB  SP!, {R0-R3}                     ; Save some working registers
  ADD    R2,R2,#1                         ; Increment the interrupt counter
  STR    R2, [R3]                         ;  Store it back in the variable 

  MRS    R0, SPSR                         ; Pickup saved SPSR
  SUB    LR, LR,#4                        ; Adjust point of interrupt
  STMDB  SP!, {R0,R10,R12,LR}             ; Store other reisters

_irq_processing_return
_tx_irq_processing_return  
 BL      INTCInterruptHandlerIRQ         ; Call high level IRQ interrupt handler

  LDMIA SP!,{R0,R10,R12,LR} ; Recover SPSR, POI, and scratch regs
  MSR SPSR_cxfs, R0         ; Put SPSR back
  LDMIA SP!, {R0-R3}        ; Recover R0-R3
  MOVS PC, LR               ; Return to point of interrupt, MOVS will restore spsr to cpsr

;**********************************************************************************************


;******************************************************************************************
 ; Execute macro to unmask IRQ interrupts masked during nesting process

 ; UNNEST_INTERRUPT IRQ_MODE

;**********************************************************************************************

;  B       _tx_thread_context_restore   ; Call ThreadX context restore routine
;}


;/*************************************************************************/
;/* FUNCTION     INT_FIQ                                                  */
;/* DESCRIPTION                                                           */
;/*      This function services the INT_FIQ exception.                    */
;/* AUTHOR     Accelerated Technology, Inc.                               */
;/* CALLED BY     None       FIQ Exception                                */
;/* CALLS                                                                 */
;/* INPUTS                                                                */
;/* OUTPUTS      None                                                     */
;/*************************************************************************/

;	IMPORT  INTCInterruptHandlerFIQ
INT_FIQ

 END
























  END

