// Contar tempo entre o acionamento dos botoes
// Detecção dos botoes é feita de modo sequencial (primeiro monitora so sw1, dps so sw2)

#include <msp430.h> 
#include <stdint.h>
#define S1pressed (!(P2IN & BIT1))
#define S2pressed (!(P1IN & BIT1))
uint32_t interval;
int state = 1;

void debounce () {
    volatile int i;
    for (i=0; i<500; i++);
    return ;
}

void acende_leds() {
    // ACLK com ID_4 demora 8seg para contar até 65536
    // 1 Seg -> 8192; 3 seg -> 24576


    if (interval < 8192) {
        P1OUT |= BIT0;
        P4OUT &=~ BIT7;
    }
    else if (interval < 24576) {
        P1OUT |= BIT0;
        P4OUT |= BIT7;
    }
    else {
        P1OUT &=~ BIT0;
        P4OUT |= BIT7;
    }
}

void config() {
    P2DIR &=~ BIT1;             // P2.1 é entrada
    P2REN |= BIT1;              // Ativa resistor
    P2OUT |= BIT1;              // Resistor é pull up

    P1DIR &= ~BIT1;              // P1.1 \e entrada
    P1REN |= BIT1;              // Ativa resistor
    P1OUT |= BIT1;              // Resistor é pull up

    P1DIR |= BIT0;              // P1.0 é saída
    P1OUT &=~ BIT0;             // Led começa apagado

    P4DIR |= BIT7;              // P4.7 é saída
    P4OUT &=~ BIT7;             // Led começa apagado

    TA0CTL = TASSEL__ACLK | TACLR | MC__CONTINOUS | ID__4;
    TA0CCTL0 = CAP | CM_2;      // Flanco de descida
}

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer
    config();
    int S1state = 0;            // S1 not pressed
    int S2state = 0;            // S2 not pressed

    while(1) {
        if (state==1) {
            if(S1pressed) {
                if (S1state == 1);
                else {
                    interval = TA0R;
                    state=2;
                    S1state = 1;
                    debounce();
                }}
            else {
                if (S1state == 0);
                else {
                    S1state = 0;
                    debounce();
                }
            }}
        else {

            if(S2pressed) {
                if (S2state == 1);
                else {
                    if (TA0CCTL0 & COV) // Deu overflow
                        interval = interval - TA0R;
                    else
                        interval = TA0R - interval;

                    acende_leds();
                    state=1;
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
}
