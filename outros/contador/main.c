/* Rotina que altera o estado do led vermelho sempre que o
 * Usuário apertar o botão S1
 */

#include <msp430.h> 
#include <stdint.h>
#define TA0_CCR0_VECTOR 53
uint8_t timer_count;
#define S1pressed (!(P2IN & BIT1))

void config() {

    // Configuração do timer
    TA0CTL = TASSEL__SMCLK | TACLR | MC__UP; // SMCLK -> 1MHz

    P2DIR &=~ BIT1;             // P2.1 é entrada
    P2REN |= BIT1;              // Ativa resistor
    P2OUT |= BIT1;              // Resistor é pull up

    P1DIR |= BIT0;              // P4.7 é saída
    P1OUT &=~ BIT0;             // Led começa apagado
}

void delay (uint32_t time_us) {
    timer_count=1;
    if (time_us < 65535) {
        TA0CCR0 = time_us+1;
        TA0CTL = TASSEL__SMCLK | TACLR | MC__UP; // SMCLK -> 1MHz
        TA0CCTL0 = CCIE;
        while(timer_count);     // Espera acabar a contagem do timer
    }
    else {
        TA0CTL = TASSEL__SMCLK | TACLR | MC__UP; // SMCLK -> 1MHz
        TA0CCTL0 = CCIE;
        TA0CCR0 = 65535;
        while(timer_count);     // Espera acabar a contagem do timer
        delay(time_us-65535);
    }

}

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer
    config();
    int S1state = 0;            // S1 not pressed
    __enable_interrupt();

    while(1) {
        if(S1pressed) {
            if (S1state == 1);
            else {
                P1OUT ^= BIT0;      // ALTERAR AQ
                delay(1000000);
                P1OUT ^= BIT0;      // ALTERAR AQ
                S1state = 1;
                delay(100);
            }}
        else {
            if (S1state == 0);
            else {
                S1state = 0;
                delay(100);
            }
        }
    }
}

#pragma vector = TA0_CCR0_VECTOR
__interrupt void TA0_CCR0_ISR() {
    TA0CCTL0 &= ~CCIE;
    TA0CCTL0 &= ~CCIFG;         // Clear flag
    timer_count=0;
}
