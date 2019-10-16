// Ex. Configurar PWM com freq 4Hz para piscar o led vermelho
// Chave 1 aumenta o duty cycle em 10% e chave 2 diminui

#include <msp430.h> 
#define S1pressed (!(P2IN & BIT1))
#define S2pressed (!(P1IN & BIT1))
#define TA0_CCR0_VECTOR 53
#define TA0_CCRN_VECTOR 52

void debounce () {
    volatile int i;
    for (i=0; i<500 ; i++);
    return ;
}

void config() {
    P2DIR &=~ BIT1;             // P2.1 é entrada
    P2REN |= BIT1;              // Ativa resistor
    P2OUT |= BIT1;              // Resistor é pull up

    P1DIR &=~ BIT1;             // P1.1 é entrada
    P1REN |= BIT1;              // Ativa resistor
    P1OUT |= BIT1;              // Resistor é pull up

    P1DIR |= BIT0;              // P1.0 é saída
    P1OUT &=~ BIT0;             // Led começa apagado

    TA0CTL = TASSEL__ACLK | TACLR | MC__UP;
    TA0CCTL0 = CCIE;
    TA0CCTL4 = CCIE;
    TA0CCR0 = 8191;             // Toggle on CCR0 -> freq de 4Hz
    TA0CCR4 = 818;              // Reset on CCR4 -> duty cycle de 10%
}

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer
    config();
    __enable_interrupt();
    int S1state = 0;            // S1 not pressed
    int S2state = 0;            // S2 not pressed

    while(1) {
        if(S1pressed) {
            if (S1state == 1);
            else {
                if (TA0CCR4 < 8000)
                    TA0CCR4 += 818; // Aumenta duty cycle em 10%
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
                if (TA0CCR4 > 0)
                    TA0CCR4 -= 818; // Diminui duty cycle em 10%
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

#pragma vector = TA0_CCR0_VECTOR
__interrupt void TA0_CCR0_ISR() {
    P1OUT ^= BIT0;              // Toggle on CCR0
    TA0CCTL0 &= ~CCIFG;         // Clear flag
}

#pragma vector = TA0_CCRN_VECTOR
__interrupt void TA0_CCRN_ISR() {
    switch(TA0IV) {
    case 8:                     // Reset on CCR4
        P1OUT &= ~BIT0;
        break;
    }
}
