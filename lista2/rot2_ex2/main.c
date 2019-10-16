#include <msp430.h> 

#define S1pressed (!(P2IN & BIT1))
#define S1notpressed ((P2IN & BIT1))
// Led vermelho: P1.0
// Chave S1: P2.1

void config() {

    P2DIR &=~ BIT1;             // P2.1 é entrada
    P2REN |= BIT1;              // Ativa resistor
    P2OUT |= BIT1;              // Resistor é pull up

    P1DIR |= BIT0;              // P4.7 é saída
    P1OUT &=~ BIT0;             // Led começa apagado
}

void debounce () {
    volatile int i;
    for (i=0; i<500 ; i++);
    return ;
}

void main(void)
{
    config();

    while(1) {
        while(S1notpressed);
        P1OUT ^= BIT0;
        debounce();
        while(S1pressed);
        debounce();
    }
}
