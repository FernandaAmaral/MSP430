#include <msp430.h> 
#include <stdint.h>

#define S1pressed (!(P2IN & BIT1))
#define S2pressed (!(P1IN & BIT1))

uint8_t counter = 0;

void debounce () {
    volatile uint16_t i;
    for (i=0; i<30000 ; i++);
}

#pragma vector = PORT2_VECTOR
__interrupt void S1_button() {
    if (counter==0);
    else {
        counter--;
        switch (counter) {
        case 0:             // led1 = 0, led2 = 0
            P1OUT &=~ BIT0;
            P4OUT &=~ BIT7;
            break;
        case 1:             // led1 = 0, led2 = 1
            P1OUT &=~ BIT0;
            P4OUT |= BIT7;
            break;
        case 2:
            P1OUT |= BIT0;  // led1 = 1, led2 = 0
            P4OUT &=~ BIT7;
            break;
        case 3:
            P1OUT |= BIT0;  // led1 = 1, led2 = 1
            P4OUT |= BIT7;
            break;
        }
    }

    P2IFG &=~ BIT1;             // Limpa a flag de interrupcao
    debounce();                 // Debounce do flanco de descida
    while (S1pressed);
    debounce();                 // Debounce do flanco de subida
}

#pragma vector = PORT1_VECTOR
__interrupt void S2_button() {
    if (counter==3);
    else {
        counter++;
        switch (counter) {
        case 0:             // led1 = 0, led2 = 0
            P1OUT &=~ BIT0;
            P4OUT &=~ BIT7;
            break;
        case 1:             // led1 = 0, led2 = 1
            P1OUT &=~ BIT0;
            P4OUT |= BIT7;
            break;
        case 2:
            P1OUT |= BIT0;  // led1 = 1, led2 = 0
            P4OUT &=~ BIT7;
            break;
        case 3:
            P1OUT |= BIT0;  // led1 = 1, led2 = 1
            P4OUT |= BIT7;
            break;
        }
    }

    P1IFG &=~ BIT1;             // Limpa a flag de interrupcao
    debounce();                 // Debounce do flanco de descida
    while (S2pressed);
    debounce();                 // Debounce do flanco de subida
}

void config() {

    P2DIR &=~ BIT1;             // P2.1 é entrada (botao S1)
    P2REN |= BIT1;              // Ativa resistor
    P2OUT |= BIT1;              // Resistor é pull up

    P1DIR &=~ BIT1;             // P1.1 é entrada (botao S1)
    P1REN |= BIT1;              // Ativa resistor
    P1OUT |= BIT1;              // Resistor é pull up

    P1DIR |= BIT0;              // P1.0 é saída
    P1OUT &=~ BIT0;             // Led começa apagado

    P4DIR |= BIT7;              // P4.7 é saída
    P4OUT &=~ BIT7;             // Led começa apagado

    P2IE |= BIT1;               // Ativa interrupcao do botao S1
    P2IES &=~ BIT1;             // Interrupção acontece no flanco de descida

    P1IE |= BIT1;               // Ativa interrupcao do botao S2
    P1IES &=~ BIT1;             // Interrupção acontece no flanco de descida
}

void main() {
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer
    config();
    __enable_interrupt();
    __no_operation();
}
