#include <msp430.h> 
#define TA0_CCR0_INT 53 // Timer A0 CCR0 interruption priority
#define TA0_CCRN_INT 52 // Timer A0 CCRN interruption priority

void config() {
    TA0CTL = (TACLR | TASSEL__ACLK | MC__UP);
    TA0CCR1 = 16385;
    TA0CCR2 = 8193;
    TA0CCR0 = 16385;
    TA0CCTL1 |= CCIE;
    TA0CCTL2 |= CCIE;

    P4DIR |= BIT7;                // P4.7 é saída
    P4OUT &=~ BIT7;               // Led comeca apagado
    P1DIR |= BIT0;                // P4.7 é saída
    P1OUT &=~ BIT0;               // Led comeca apagado
}

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
    config();
    __enable_interrupt();
    while(1);
}

#pragma vector=TA0_CCRN_INT
__interrupt void TA0_CCRN_ISR() {
    switch (TA0IV) {
    case 2:
        P1OUT ^= BIT0;
        P4OUT |= BIT7;
        break;

    case 4:
        P4OUT &=~ BIT7;
        break;

    case 6: break;
    case 8: break;
    case 10: break;
    case 12: break;
    case 14: break;
    }
}
