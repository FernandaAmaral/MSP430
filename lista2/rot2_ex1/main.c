#include <msp430.h> 

// Led verde: P4.7
// Chave S1: P2.1

void config() {
    P2DIR &=~ BIT1;             // P2.1 é entrada
    P2REN |= BIT1;              // Ativa resistor
    P2OUT |= BIT1;              // Resistor é pull up

    P4DIR |= BIT7;              // P4.7 é saída
    P4OUT &=~ BIT7;
}

void debounce () {
    volatile int i=0;
    for (; i<4000 ; i++);
    return ;
}

void main(void)
{

    WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
    volatile int teste;
    config();
    while(1) {
        teste= !(P2IN & BIT1);
        if (teste)
            P4OUT |= BIT7;
        else
            P4OUT &=~ BIT7;
        //debounce();
    }


}
