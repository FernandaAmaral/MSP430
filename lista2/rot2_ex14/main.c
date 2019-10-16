#include <msp430.h> 
#include <stdint.h>
#define TA0_CCR0_INT 53 // Timer A0 CCR0 interruption priority
#define TA1_CCR2_INT 48 // Timer A1 CCR2 interruption priority

// De acordo com o datasheet o pino P2.1 (botão S1) está ligado com o TA1.2
// E o pino P1.1 (botão S2) no TA0.0

volatile uint16_t counter = 1, edge1, edge2, S1pressed = 0;
const uint16_t freq = 32768;

void config() {
    TA0CTL = (TACLR | TASSEL__ACLK | MC__CONTINOUS);
    TA1CTL = (TACLR | TASSEL__ACLK | MC__CONTINOUS);
    TA0CCTL0 = (CM_2 | CAP | CCIE);     // P o botao S2
    TA1CCTL2 = (CM_2 | CAP | CCIE);     // P o botao S1
    TA1CCTL0 |= CCIE;

    P4DIR |= BIT7;                      // P4.7 é saída
    P4OUT &=~ BIT7;                     // Led comeca apagado
    P1DIR |= BIT0;                      // P1.0 é saída
    P1OUT &=~ BIT0;                     // Led comeca apagado

    P1DIR &= ~BIT1;                     // P1.1 é entrada
    P1REN |= BIT1;                      // Ativa resistor
    P1OUT |= BIT1;                      // Pull up
    P1SEL |= BIT1;                      // Ativa P1.1 p o modo de captura de TA0.0

    P2DIR &= ~BIT1;                     // P2.1 é entrada
    P2REN |= BIT1;                      // Ativa resistor
    P2OUT |= BIT1;                      // Pull up
    P2SEL |= BIT1;                      // Ativa P1.1 p o modo de captura de TA1.2
}

void main()
{
    WDTCTL = WDTPW | WDTHOLD;	        // stop watchdog timer
    config();
    __enable_interrupt();
    _no_operation();
}

#pragma vector = TIMER0_A0_VECTOR       // TA0CCR0
__interrupt void TA0_CCR0_ISR() {
    if (!S1pressed)                     // Botao S2 foi pressionado
        return;
    else;
        // Medir tempo
}

#pragma vector = TIMER1_A2_VECTOR       // Botão S1 foi pressionado?
__interrupt void TA1_CCR2_ISR() {
    switch(TA1IV) {
        case 2: break;                  // CCR1
        case 4:                         // CCR2
            edge1 = TA1CCR2;
            S1pressed=1;
            break;
        case 6: break;
        case 8: break;
        case 10: break;
        case 12: break;
        case 14: break;
    }
}

#pragma vector = TIMER1_A0_VECTOR       // Reiniciou a contagem do TA1CCR0
__interrupt void TA1_CCR0_ISR() {
    if (!S1pressed)
        return;
    else
        counter++;
}
