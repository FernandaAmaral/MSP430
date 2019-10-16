#include <msp430.h> 

void config() {
    TA0CTL = (TACLR | TASSEL__ACLK | MC__UP);
    TA0CCR0 = 328;
    TA0CCTL0 |= CCIE;

    P1DIR |= BIT0;                // P4.7 é saída
    P1OUT &=~ BIT0;               // Led comeca apagado
}

int main(void)
{
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
	config();
	__enable_interrupt();
	_no_operation();
}

#pragma vector=TIMER0_A0_VECTOR
__interrupt void TA0_CCR0_ISR() {
    P1OUT ^= BIT0;
}
