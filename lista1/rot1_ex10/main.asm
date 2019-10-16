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
inicio:
			mov		#vetor,R4
			mov		#14001,R6
			call		#W16_ASC
			jmp		$
			nop
W16_ASC:
			push		R5
			mov.b	R6,R5					; Nibble 0
			bic		#240,R5
			call		#NIB_ASC

			mov.b	R6,R5					; Nibble 1
			rra		R5
			rra		R5
			rra		R5
			rra		R5
			call		#NIB_ASC

			swpb		R6

			mov.b	R6,R5					; Nibble 2
			bic		#240,R5
			call		#NIB_ASC

			mov.b	R6,R5					; Nibble 3
			rra		R5
			rra		R5
			rra		R5
			rra		R5
			call		#NIB_ASC

			swpb		R6
			pop		R5
			ret


NIB_ASC:
			cmp		#10,R5
			jlo		de_0_a_9
			add		#55,R5
			mov		R5,0(R4)
			add		#2,R4
			ret

de_0_a_9:
			add		#48,R5
			mov		R5,0(R4)
			add		#2,R4
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
            
