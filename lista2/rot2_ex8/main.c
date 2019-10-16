#include <msp430.h> 

void config() {
    TA0CTL |= (TASSEL__ACLK | TACLR | MC__UP);
    TA0CCR0 = 32768;
    TA0CCR1 = 16385;

    P4DIR |= BIT7;
    P4OUT &=~ BIT7;
}
void main()
{
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
	config();
	while(1) {
	    if (TA0CCTL1 & CCIFG) {  // Chegou em CCR1
	        P4OUT |= BIT7;
	        TA0CCTL1 &= ~CCIFG; }
	    else if (TA0CCTL0 & CCIFG) {
	        P4OUT &= ~BIT7;
	        TA0CCTL0 &= ~CCIFG;
	    }
	}
}
