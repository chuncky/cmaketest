;==================================================================
; Copyright ARM Ltd 2006-2014. All rights reserved.
;
; Cortex-R5(F) Embedded example - Startup Code
;==================================================================

; MPU region defines

Region_32B     EQU 2_00100
Region_64B     EQU 2_00101
Region_128B    EQU 2_00110
Region_256B    EQU 2_00111
Region_512B    EQU 2_01000
Region_1K      EQU 2_01001
Region_2K      EQU 2_01010
Region_4K      EQU 2_01011
Region_8K      EQU 2_01100
Region_16K     EQU 2_01101
Region_32K     EQU 2_01110
Region_64K     EQU 2_01111
Region_128K    EQU 2_10000
Region_256K    EQU 2_10001
Region_512K    EQU 2_10010
Region_1M      EQU 2_10011
Region_2M      EQU 2_10100
Region_4M      EQU 2_10101
Region_8M      EQU 2_10110
Region_16M     EQU 2_10111
Region_32M     EQU 2_11000
Region_64M     EQU 2_11001
Region_128M    EQU 2_11010
Region_256M    EQU 2_11011
Region_512M    EQU 2_11100
Region_1G      EQU 2_11101
Region_2G      EQU 2_11110
Region_4G      EQU 2_11111

Region_Enable  EQU 2_1

Execute_Never  EQU 0x1000  ; Bit 12
Normal_nShared EQU 0x03 ; Outer and Inner write-back, no write-allocate
Device_nShared EQU 0x10
Normal_nShared_nCacheable EQU 0x08 ; normal memory, non-shared, non-cacheable.
Normal_nShared_Cacheable EQU 0x03 ; normal memory, non-shared, cacheable.
Normal_Shared_nCacheable EQU 0x0C ; normal memory, shared, non-cacheable.
Full_Access    EQU 2_011
Read_Only      EQU 2_110













Mode_USR        EQU     0x10
Mode_FIQ        EQU     0x11
Mode_IRQ        EQU     0x12
Mode_SVC        EQU     0x13
Mode_ABT        EQU     0x17
Mode_UND        EQU     0x1B
Mode_SYS        EQU     0x1F

I_Bit           EQU     0x80            ; when I bit is set, IRQ is disabled
F_Bit           EQU     0x40            ; when F bit is set, FIQ is disabled

UND_Stack_Size  EQU     0x00000000
SVC_Stack_Size  EQU     0x00001000
ABT_Stack_Size  EQU     0x00000000
FIQ_Stack_Size  EQU     0x00000000
IRQ_Stack_Size  EQU     0x00001000
USR_Stack_Size  EQU     0x00000000
Stack_Size      EQU     (UND_Stack_Size + SVC_Stack_Size + ABT_Stack_Size + \
                         FIQ_Stack_Size + IRQ_Stack_Size)




				
				PRESERVE8

                AREA    STACK, NOINIT, READWRITE, ALIGN=3

Stack_Mem       SPACE   Stack_Size
Stack_Top
                AREA  Init, CODE,READONLY
                ENTRY




                EXPORT  Reset_Handler
                
Reset_Handler
; normal vector address selected
                MRC   p15, 0, r1, c1, c0, 0
                BIC   r1, r1, #0x2000
                MCR   p15, 0, r1, c1, c0, 0









;==================================================================
; Disable MPU and caches
;==================================================================

; Disable MPU and cache in case it was left enabled from an earlier run
; This does not need to be done from a cold reset

	MRC 	p15, 0, r0, c1, c0, 0		; Read CP15 Control Register
	BIC 	r0, r0, #0x05				; Disable MPU (M bit) and data cache (C bit)
	BIC 	r0, r0, #0x1000 			; Disable instruction cache (I bit)
	DSB 								; Ensure all previous loads/stores have completed
	MCR 	p15, 0, r0, c1, c0, 0		; Write CP15 Control Register
	ISB 								; Ensure subsequent insts execute wrt new MPU settings

;==================================================================
; Disable Branch prediction
;==================================================================

; In the Cortex-R5, the Z-bit of the SCTLR does not control the program flow prediction.
; Some control bits in the ACTLR control the program flow and prefetch features instead.
; These are enabled by default, but are shown here for completeness.

	MRC 	p15, 0, r0, c1, c0, 1		; Read ACTLR
	ORR 	r0, r0, #(0x1 << 17)		; Enable RSDIS bit 17 to disable the return stack
	ORR 	r0, r0, #(0x1 << 16)		; Clear BP bit 15 and set BP bit 16:
	BIC 	r0, r0, #(0x1 << 15)		; Branch always not taken and history table updates disabled
	MCR 	p15, 0, r0, c1, c0, 1		; Write ACTLR


; Setup Stack for each mode

				LDR 	R0, =Stack_Top

;  Enter Undefined Instruction Mode and set its Stack Pointer
				MSR 	CPSR_c, #Mode_UND:OR:I_Bit:OR:F_Bit
				MOV 	SP, R0
				SUB 	R0, R0, #UND_Stack_Size

;  Enter Abort Mode and set its Stack Pointer
				MSR 	CPSR_c, #Mode_ABT:OR:I_Bit:OR:F_Bit
				MOV 	SP, R0
				SUB 	R0, R0, #ABT_Stack_Size

;  Enter FIQ Mode and set its Stack Pointer
				MSR 	CPSR_c, #Mode_FIQ:OR:I_Bit:OR:F_Bit
				MOV 	SP, R0
				SUB 	R0, R0, #FIQ_Stack_Size

;  Enter IRQ Mode and set its Stack Pointer
				MSR 	CPSR_c, #Mode_IRQ:OR:I_Bit:OR:F_Bit
				MOV 	SP, R0
				SUB 	R0, R0, #IRQ_Stack_Size

;  Enter Supervisor Mode and set its Stack Pointer
				MSR 	CPSR_c, #Mode_SVC:OR:I_Bit:OR:F_Bit
				MOV 	SP, R0

;  Open Interrupt 
				MRS 	r0, cpsr
				BIC 	r0, r0, #0xc0
				MSR 	cpsr_c, r0




;==================================================================
; Cache invalidation
;==================================================================

	DSB 				; Complete all outstanding explicit memory operations

	MOV 	r0, #0

	MCR 	p15, 0, r0, c7, c5, 0		; Invalidate entire instruction cache
	MCR 	p15, 0, r0, c15, c5, 0		; Invalidate entire data cache

;==================================================================
; Enable Branch prediction
;==================================================================

; In the Cortex-R5, the Z-bit of the SCTLR does not control the program flow prediction.
; Some control bits in the ACTLR control the program flow and prefetch features instead.
; These are enabled by default, but are shown here for completeness.

	MRC 	p15, 0, r0, c1, c0, 1		; Read ACTLR
	BIC 	r0, r0, #(0x1 << 17)		; Clear RSDIS bit 17 to enable return stack
	BIC 	r0, r0, #(0x1 << 16)		; Clear BP bit 15 and BP bit 16:
	BIC 	r0, r0, #(0x1 << 15)		; Normal operation, BP is taken from the global history table.
	MCR 	p15, 0, r0, c1, c0, 1		; Write ACTLR



;	B updater
	EXTERN updater
	LDR r0, =updater
	BX r0

LOOPFOREVER  B		LOOPFOREVER				; forever loop

;************************************************************
; DisableIrqInterrupts
;************************************************************

DisableIrqInterrupts
	STMFD		SP!, {R0-R4, LR}
   	MRS    	r0,CPSR        ; get the processor status
   	ORR    	r0,r0,#0x80    ; Disable IRQ interrupts
   	MSR    	cpsr_cf,r0     ; SVC 32 mode with interrupts disabled
	LDMFD   	SP!,    {R0-R4, PC}

;***********************************************************************************
;  r0 = Boot Image Load Address
;***********************************************************************************
   EXPORT TransferControl
TransferControl
	mov pc, r0

transfer_control
	DCD 0xF57FF04F	;DSB
	DCD 0xF57FF06F	;ISB
	DCD 0xF8900A00	;RFEIA r0

	EXPORT sctlr_get
sctlr_get
	mrc p15, 0, r0, c1, c0, 0
	bx		lr

	EXPORT sctlr_set
sctlr_set
	mcr p15, 0, r0, c1, c0, 0
	DCD 0xF57FF06F	;ISB
	bx		lr
	
	EXPORT mpu_get_region_num
mpu_get_region_num
	mrc p15, 0, r0, c6, c2, 0
	bx		lr
	
	EXPORT mpu_set_region_num
mpu_set_region_num
	mcr p15, 0, r0, c6, c2, 0
	DCD 0xF57FF06F	;ISB
	bx		lr

	EXPORT mpu_get_region_base_addr
mpu_get_region_base_addr
	mrc p15, 0, r0, c6, c1, 0
	bx		lr
	
	EXPORT mpu_set_region_base_addr
mpu_set_region_base_addr
	mcr p15, 0, r0, c6, c1, 0
	DCD 0xF57FF06F	;ISB
	bx		lr
	
	EXPORT mpu_get_region_size
mpu_get_region_size
	mrc p15, 0, r0, c6, c1, 2
	and		r0, r0, #0x3E
	mov		r0, r0, lsr #1
	bx		lr
	
	EXPORT mpu_set_region_size
mpu_set_region_size
	STMFD	SP!, {R0-R4, LR}
	mov 	r1, r0
	mrc 		p15, 0, r0, c6, c1, 2
	bic		r0, r0, #0x3E
	mov		r1, r1, LSL #1
	orr		r0, r0, r1
	mcr 		p15, 0, r0, c6, c1, 2
	DCD 	0xF57FF06F	;ISB
	LDMFD   	SP!,    {R0-R4, PC}
	
	EXPORT mpu_get_region_size_enable
mpu_get_region_size_enable
	mrc p15, 0, r0, c6, c1, 2
	bx		lr
	
	EXPORT mpu_get_region_access_ctrl
mpu_get_region_access_ctrl
	mrc p15, 0, r0, c6, c1, 4
	bx		lr
	
	EXPORT mpu_set_region_access_ctrl
mpu_set_region_access_ctrl
	mcr p15, 0, r0, c6, c1, 4
	DCD 0xF57FF06F	;ISB
	bx		lr
	
	EXPORT mpu_enable_region
mpu_enable_region
	STMFD	SP!, {R0-R4, LR}
	bl		mpu_set_region_num
	mrc 		p15, 0, r0, c6, c1, 2
	orr		r0, r0, #0x1 ;set bit 0 to enable
	mcr 		p15, 0, r0, c6, c1, 2
	DCD 	0xF57FF06F	;ISB
	LDMFD   	SP!,    {R0-R4, PC}
	EXPORT CPUCleanDCacheLine
CPUCleanDCacheLine
	DSB								;ensure ordering with previous memory access
	MCR		p15, 0, R0, c7, c10, 1
	DSB								;ensure ordering with previous memory access
	BX LR

	EXPORT CPUInvalidateDCacheLine
CPUInvalidateDCacheLine
	DSB								;ensure ordering with previous memory access
	MCR		p15, 0, R0, c7, c6, 1	;invalidate L1 Cache
	DSB								;ensure ordering with previous memory access
	BX LR
;-------------------------------------------------------	
	

   END

