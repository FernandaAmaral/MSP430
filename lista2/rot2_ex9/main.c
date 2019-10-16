#include <msp430.h> 
#define TA0_CCR0_INT 53 // Timer A0 CCR0 interruption priority

void config() {
    TA0CTL = (TACLR | TASSEL__ACLK | MC__UP);
    TA0CCR0 = 16385;
    TA0CCTL0 |= CCIE;


    P4DIR |= BIT7;                // P4.7 é saída
    P4OUT &=~ BIT7;               // Led comeca apagado
}

void main()
{
    WDTCTL = WDTPW | WDTHOLD;   	// stop watchdog timer
    config();
    __enable_interrupt();
    while(1);
}

#pragma vector=TA0_CCR0_INT
__interrupt void TA0_CCR0_ISR() {
    P4OUT ^= BIT7;                  // Toggle LED
    TA0CCTL0 &=~ CCIFG;             // Clear flag
}
