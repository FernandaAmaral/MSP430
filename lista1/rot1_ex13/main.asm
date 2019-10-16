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
DELAY1      .equ        50
DELAY2      .equ        50

inicio:
			BIC.b		#BIT0, &P1OUT		; Led P1.0 começa apagado
			BIS.b 		#BIT0, &P1DIR		; Porta P1.0 é saída

			BIC.b		#BIT7, &P4OUT		; Led P4.7 começa apagado
			BIS.b 		#BIT7, &P4DIR		; Porta P4.7 é saída

			BIS.b		#BIT1, &P2REN		; Resistor enable
			BIC.b 		#BIT1, &P2DIR		; Porta P2.1 é entrada
			BIS.b		#BIT1, &P2OUT		; Pull up resistor

			BIS.b		#BIT1, &P1REN		; Resistor Enable
			BIC.b 		#BIT1, &P1DIR		; Porta P1.1 é entrada
			BIS.b		#BIT1, &P1OUT		; Pull up resistor

loop:
			BIC.b		#BIT0, &P1OUT		; Turn led 1 off
			BIC.b		#BIT7, &P4OUT		; Turn led 2 off
			BIT.b		#BIT1, &P1IN			; Bit test
			JZ			S1_pressed			; 0 if pressed

			BIC.b		#BIT0, &P1OUT		; Turn led 1 off
			BIC.b		#BIT7, &P4OUT		; Turn led 2 off
			BIT.b		#BIT1, &P2IN			; Bit test
			JZ			S2_pressed			; 0 if pressed

S1_pressed:
			XOR.b		#BIT7, &P4OUT		; Toggle LED 1 state
S1_test:		BIT.b		#BIT1, &P1IN			; Bit test
			JZ			S1_test				; Confere se n tem rebote
			call			ROT_ATZ				; Delay
			jmp			loop
S2_pressed:
			XOR.b		#BIT0, &P1OUT		; Toggle LED 2 state
S2_test:		BIT.b		#BIT1, &P2IN			; Bit test
			JZ			S2_test				; Confere se n tem rebote
			call			ROT_ATZ				; Delay
			jmp			loop

ROT_ATZ:    MOV         #DELAY1,R6
RT1:        MOV         #DELAY2,R5
RT2: 		DEC 			R5
            JNZ         RT2
            DEC         R6
            JNZ         RT1
            RET
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
            
