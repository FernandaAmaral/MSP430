/* Rotina que altera o estado do led vermelho sempre que o
 * Usuário apertar o botão S1
 */

#include <msp430.h> 
#define S1pressed (!(P2IN & BIT1))

/**
 * main.c
 */

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

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer
    config();
    int S1state = 0;            // S1 not pressed

    while(1) {
        if(S1pressed) {
            if (S1state == 1);
            else {
                P1OUT ^= BIT0;
                S1state = 1;
                debounce();
            }}
        else {
            if (S1state == 0);
            else {
                S1state = 0;
                debounce();
            }
        }
    }
}
