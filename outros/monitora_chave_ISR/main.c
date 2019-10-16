// Monitora ambas as chaves usando interrupções e altera o estado dos leds a depender do que foi pressionado
#include <msp430.h> 

void config() {

    P2DIR &=~ BIT1;             // P2.1 é entrada
    P2REN |= BIT1;              // Ativa resistor
    P2OUT |= BIT1;              // Resistor é pull up
    P2IE |= BIT1;               // Ativa interrupção de P2.1
    P2IES |= BIT1;              // Flanco de descida

    P1DIR &=~ BIT1;             // P1.1 é entrada
    P1REN |= BIT1;              // Ativa resistor
    P1OUT |= BIT1;              // Resistor é pull up
    P1IE |= BIT1;               // Ativa interrupção de P1.1
    P1IES |= BIT1;              // Flanco de descida

    P1DIR |= BIT0;              // P1.0 é saída
    P1OUT &=~ BIT0;             // Led começa apagado

    P4DIR |= BIT7;              // P4.7 é saída
    P4OUT &=~ BIT7;             // Led começa apagado
    __enable_interrupt();
}

void debounce () {
    volatile int i;
    for (i=0; i<500 ; i++);
    return ;
}

void main()
{
    WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
    config();
    while(1);
}

#pragma vector = PORT1_VECTOR
__interrupt void botao1 () {
    debounce();
    switch (P1IV) {
    case 4:                     // P1.1
        if (P1IES & BIT1) {
            P1IES &= ~BIT1;
            P4OUT ^= BIT7;
        }
        else
            P1IES |= BIT1;
        break;
    }
}

#pragma vector = PORT2_VECTOR
__interrupt void botao2 () {
    debounce();
    switch (P2IV) {
    case 4:                     // P2.1
        if (P2IES & BIT1) {
            P2IES &= ~BIT1;
            P1OUT ^= BIT0;
        }
        else
            P2IES |= BIT1;
        break;
    }
}
