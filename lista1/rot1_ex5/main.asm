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
; Soma cada elemento de (vetores) R5 + R6 e salva em R7 (vetor)

inicio:
			mov		#Vetor1,R5
			mov		#Vetor2,R6
			mov		#Soma,R7					; R7 -> Endereço do vetor resultado
			mov		#7,R4
			call		#SUM16
			mov		#Soma,R7
			mov		#7,R4
			call		#teste
			jmp		$
			nop

SUM16:
			push		R4
			mov		@R5+, R4 				; contador
			mov		@R6+, 0(R7)
			add		#2, R7

loop:
			mov		@R5+,0(R7)
			add		@R6+,0(R7)
			add		#2,R7
			dec		R4
			jnz		loop
			pop		R4
			ret

teste:
			mov		@R7+,R8
			dec		R4
			jnz 		teste
			ret

			.data
; Declarar os vetores com 7 elementos
Vetor1: 		.word 		7, 2021, 2223, 2425, 2627, 2829, 3031, 3233
Vetor2: 		.word 		7, 6867, 6665, 6463, 6261, 6059, 5857, 5655
Soma:
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
            
