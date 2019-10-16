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
; Salvar os 20 primeiros numeros de fibonacci

inicio:
			mov		#vetor,	R5
			call		#FIB
			;mov		#20,R4
			;mov		#vetor,R5
			;call		#teste
			jmp		$
			nop

FIB:
			push		R4						; R4 -> An-2
			push		R6						; R6 -> An-1
			push		R7						; R7 ->	An
			push		R8						; R8 -> Contador
			mov		#1,R4
			mov		R4,0(R5)
			mov		#1,R6
			mov		R6,2(R5)
			mov		#0,R7
			mov		#18,R8
			add		#4,R5
			call		#loop
			pop		R8
			pop		R7
			pop		R6
			pop		R4
			ret


loop:
			mov		R4,R7
			add		R6,R7
			mov		R6,R4
			mov		R7,R6
			mov		R7,0(R5)
			add		#2,R5
			dec		R8
			jnz		loop
			ret

teste:
			mov		@R5+,R8
			dec		R4
			jnz		teste
			ret


			.data
vetor:
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
            
