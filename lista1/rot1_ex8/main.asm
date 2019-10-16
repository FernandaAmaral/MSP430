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
; Maior numero da seq fibonacci q cabe em 16 bits -> R10

inicio:
			call		#FIB16
			jmp		$
			nop

FIB16:
			push		R4						; R4 -> An-2
			push		R6						; R6 -> An-1
			push		R7						; R7 ->	An
			mov		#1,R4
			mov		#1,R6
			mov		#0,R7
			call		#loop
			pop		R7
			pop		R6
			pop		R4
			ret


loop:
			mov		R4,R7
			add		R6,R7
			jc		achou
			mov		R6,R4
			mov		R7,R6
			jmp		loop


achou:
			mov		R6,	R10
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
            
