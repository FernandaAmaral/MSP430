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
; R6: Maior elemento do vetor e R7: Frequencia de ocorrencia

inicio:
			mov		#vetor,R5 				; Inicializa R5 com o endereço do vetor
         	call		#MAIOR16					; Chamar subrotina
            jmp		$
            nop

MAIOR16:
			push		R4
			mov.b	@R5,R4					; Contador
			inc 		R5
            mov		#0, R7
            mov 		#0, R6
            call 	#loop
            ret

loop:
			cmp	 	@R5,R6
			jlo 		maior
			inc 		R5
			dec 		R4
			jnz		loop
			mov		#vetor,R5 				; Inicializa R5 com o endereço do vetor
            mov		@R5+,R4
			jmp		freq

maior:
			mov		@R5+,R6
			dec 		R4
			jnz 		loop
			mov		#vetor,R5 				; Inicializa R5 com o endereço do vetor
            mov.b	@R5,R4					; Contador
            inc 		R5

freq:
			cmp		@R5+,R6
			jz		inc_freq
			dec 		R4
			jnz 		freq
			ret

inc_freq:
			inc 		R7
			dec		R4
			jnz		freq
			ret

.data
vetor:		.byte 4,0, "FERNANDA"


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
            
