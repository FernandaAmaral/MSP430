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
; Salva em R10 (LSB) R11 (MSB) o maior numero da sequencia fibonacci que caiba em 32 bits

inicio:
			call		#FIB32
			jmp		$
			nop

FIB32:
			push		R4						; R4 -> An-2 (LSB)
			push		R5						; R5 -> An-2 (MSB)
			push		R6						; R6 -> An-1	 (LSB)
			push		R7						; R7 ->	An-1 (MSB)
			push		R8						; R8 -> An (LSB)
			push		R9						; R9 -> An (MSB)
			mov		#1,R4
			mov		#0,R5
			mov		#1,R6
			mov		#0,R7
			mov		#0,R8
			mov		#0,R9
			call		#loop
			pop		R9
			pop		R8
			pop		R7
			pop		R6
			pop		R5
			pop		R4
			ret


loop:
			mov		R4,R8
			mov		R5,R9
			add		R6,R8
			addc		R7,R9
			jc		achou
			mov		R6,R4
			mov		R7,R5
			mov		R9,R7
			mov		R8,R6
			jmp		loop


achou:
			mov		R6,	R10
			mov		R7, R11
			ret
                                            

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
            
