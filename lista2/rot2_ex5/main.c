#include <msp430.h> 
#include <stdint.h>
#define S1pressed (!(P2IN & BIT1))

void debounce (int max) {
    volatile uint16_t i;
    for (i=0; i<max ; i++);
    return ;
}

#pragma vector = PORT2_VECTOR
__interrupt void butaum() {
    P1OUT ^= BIT0;              // Troca o estado do led
    P2IFG &=~ BIT1;             // Limpa a flag de interrupcao
    debounce(30000);            // Debounce do flanco de descida
    while (S1pressed);
    debounce(30000);            // Debounce do flanco de subida
}

void config() {

    P2DIR &=~ BIT1;             // P2.1 é entrada (botao S1)
    P2REN |= BIT1;              // Ativa resistor
    P2OUT |= BIT1;              // Resistor é pull up

    P1DIR |= BIT0;              // P1.0 é saída
    P1OUT &=~ BIT0;             // Led começa apagado

    P2IE |= BIT1;               // Ativa interrupcao do botao S1
    P2IES &=~ BIT1;             // Interrupção acontece no flanco de descida
}

void main() {
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
	config();
	__enable_interrupt();
	while (1);
}
