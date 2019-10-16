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
; Recebe em R5 o endereço de um vetor de bytes e retorna
; R6 : Qtde de multiplos de 2
; R7 : Qtde de multiplos de 4

inicio:
			mov		#vetor,R5 				; Inicializa R5 com o endereço do vetor
            call		#M2M4
            jmp		$
            nop

M2M4:
			mov		#0,R6
			mov		#0,R7
			push		R4
			push		R8
			mov.b	@R5+,R4					; Contador
			call		#compara
			pop 		R8
			pop		R4
			ret

compara:
			mov.b	@R5,R8
			rrc		R8
			jnc		multiplo_de_2
			inc		R5
			dec		R4
			jnz		compara
			ret

multiplo_de_2:
			add		#1, R6
			rrc		R8
			jnc		multiplo_de_4
			inc		R5
			dec		R4
			jnz		compara
			ret

multiplo_de_4:
			add 		#1,R7
			inc		R5
			dec		R4
			jnz		compara
			ret

.data
vetor:		.byte 8, "FERNANDA"
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
            
