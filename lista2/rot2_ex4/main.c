#include <msp430.h> 
#include <stdint.h>
#define S1pressed (!(P2IN & BIT1))
#define S2pressed (!(P1IN & BIT1))


void config() {

    P2DIR &=~ BIT1;             // P2.1 é entrada
    P2REN |= BIT1;              // Ativa resistor
    P2OUT |= BIT1;              // Resistor é pull up

    P1DIR &=~ BIT1;             // P1.1 é entrada
    P1REN |= BIT1;              // Ativa resistor
    P1OUT |= BIT1;              // Resistor é pull up

    P1DIR |= BIT0;              // P4.7 é saída
    P1OUT &=~ BIT0;             // Led começa apagado
}

void debounce () {
    volatile uint8_t i;
    for (i=0; i<500 ; i++);
    return ;
}

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
    config();
    uint8_t button_state = 0;            // S1 and S2 not pressed

    while(1) {
        if (S1pressed || S2pressed) {
            if (button_state == 1);
            else {
                P1OUT ^= BIT0;
                button_state = 1;
                debounce();
            }
        }
        else {
            if (button_state == 0);
            else {
                button_state = 0;
                debounce();
            }
        }
    }
}
