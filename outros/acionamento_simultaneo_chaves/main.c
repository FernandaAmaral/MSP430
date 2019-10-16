// Confere se as duas chaves foram pressionadas "ao mesmo tempo"
// Caso a primeira seja pressionada um pouco antes, acende o led verde e vice versa

#include <msp430.h> 
#define S1pressed (!(P2IN & BIT1))
#define S2pressed (!(P1IN & BIT1))

void config() {

    P2DIR &=~ BIT1;             // P2.1 é entrada
    P2REN |= BIT1;              // Ativa resistor
    P2OUT |= BIT1;              // Resistor é pull up

    P1DIR &=~ BIT1;             // P1.1 é entrada
    P1REN |= BIT1;              // Ativa resistor
    P1OUT |= BIT1;              // Resistor é pull up

    P1DIR |= BIT0;              // P1.0 é saída
    P1OUT &=~ BIT0;             // Led começa apagado

    P4DIR |= BIT7;              // P4.7 é saída
    P4OUT &=~ BIT7;             // Led começa apagado
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
    int S2state = 0;            // S2 not pressed

    while(1) {
        if(S1pressed) {
            if (S1state == 1);
            else {
                if (S2pressed)
                    P1OUT ^= BIT0;      // ALTERAR AQ
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

        if(S2pressed) {
            if (S2state == 1);
            else {
                if (S1pressed)
                    P4OUT ^= BIT7;      // ALTERAR AQ
                S2state = 1;
                debounce();
            }}
        else {
            if (S2state == 0);
            else {
                S2state = 0;
                debounce();
            }
        }
    }
}
