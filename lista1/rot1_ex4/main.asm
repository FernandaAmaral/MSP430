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
; Subrotina extremos, recebe o endereço em R5 e retorna
; R6 -> Menor elemento
; R7 -> Maior elemento

inicio:
			mov		#vetor,R5
			call		#extremos
			jmp 		$
			nop

extremos:
			mov		#0,R7
			mov		#0xFFFF,R6
			push		R4
			mov		@R5+,R4
			call 	#loop_maior
			call		#loop_menor
			pop		R4
			ret

loop_menor:
			cmp	 	@R5,R6
			jge 		menor
			inc 		R5
			dec 		R4
			jnz		loop_menor
			mov		#vetor,R5 				; Inicializa R5 com o endereço do vetor
            mov		@R5+,R4
			ret

menor:
			mov		@R5+,R6
			dec 		R4
			jnz 		loop_menor
			mov		#vetor,R5 				; Inicializa R5 com o endereço do vetor
            mov		@R5+,R4					; Contador
            ret

loop_maior:
			cmp	 	@R5,R7
			jl 		maior
			inc 		R5
			dec 		R4
			jnz		loop_maior
			mov		#vetor,R5 				; Inicializa R5 com o endereço do vetor
            mov		@R5+,R4
			ret

maior:
			mov		@R5+,R7
			dec 		R4
			jnz 		loop_maior
			mov		#vetor,R5 				; Inicializa R5 com o endereço do vetor
            mov		@R5+,R4					; Contador
            ret

.data
vetor: 		.word 	8, 121, 234, 567, -1990, 117, 867, 45, -1980
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
            
