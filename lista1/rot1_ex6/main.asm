;-------------------------------------------------------------------------------
; MSP430 Assembler Code Template for use with TI Code Composer Studio
;
;
;-------------------------------------------------------------------------------
            .cdecls C,LIST,"msp430.h"       ; Include device header file
            
;-------------------------------------------------------------------------------
            .def    RESET                   ; Export program entry-point to
                                            ; make it known to linker.
;-------------------------------------------------------------------------------
            .text                           ; Assemble into program memory.
            .retain                         ; Override ELF conditional linking
                                            ; and retain current section.
            .retainrefs                     ; And retain any sections that have
                                            ; references to current section.

;-------------------------------------------------------------------------------
RESET       mov.w   #__STACK_END,SP         ; Initialize stackpointer
StopWDT     mov.w   #WDTPW|WDTHOLD,&WDTCTL  ; Stop watchdog timer


;-------------------------------------------------------------------------------
; Main loop here
;-------------------------------------------------------------------------------
; vetor1 -> R5
; vetor2 -> R6
; soma -> R8 (MSW), R9 (LSW)

inicio:
			mov		#Vetor1,R5
			mov		#Vetor2,R6
			call		#SUM_TOT
			jmp		$
			nop

SUM_TOT:
			push		R4
			push		R7
			mov		#0,R8
			mov		#0,R9
			mov		@R5+,R4
			add		#2,R6
			call		#loop
			pop		R7
			pop 		R4
			ret

loop:
			mov		@R5+, R7
			add		@R6+, R7
			addc		#0,R8
			add		R7,R9
			addc		#0,R8
			dec		R4
			jnz		loop
			ret
                                            
.data
; Declarar os vetores com 7 elementos
Vetor1: .word 7, 2021, 2223, 2425, 2627, 2829, 3031, 3233
Vetor2: .word 7, 6867, 6665, 6463, 6261, 6059, 5857, 5655
;-------------------------------------------------------------------------------
; Stack Pointer definition
;-------------------------------------------------------------------------------
            .global __STACK_END
            .sect   .stack
            
;-------------------------------------------------------------------------------
; Interrupt Vectors
;-------------------------------------------------------------------------------
            .sect   ".reset"                ; MSP430 RESET Vector
            .short  RESET
            
