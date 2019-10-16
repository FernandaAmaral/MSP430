/* Neste exercício, iremos programar um jogo de adivinhação.
O jogador deve adivinhar um número de 1 a 6.
A cada rodada, o jogo gera um novo número e o jogador deve
tentar adivinhar apertando N vezes o botão S1.
Tendo feito a escolha, o jogador aperta o botão S2 e verifica
se acertou ou não. Se o LED vermelho acender, significa que ele
 errou. Se o LED verde acender, significa que o jogador acertou
 e nesse momento o jogo gera um novo número. */

#include <msp430.h> 
#include <stdint.h>

#define S1pressed (!(P2IN & BIT1))
#define S2pressed (!(P1IN & BIT1))

uint8_t goal = 1;
uint8_t trial = 0;

unsigned char rand() {
    static unsigned char num = 5;
    num = (num * 17) % 7;
    return num;
}

void debounce (uint16_t max) {
    volatile uint16_t i;
    for (i=0; i<max ; i++);
}

#pragma vector = PORT2_VECTOR
__interrupt void S1_button() {
    P1OUT &=~ BIT0;             // Desliga ambos os leds
    P4OUT &=~ BIT7;

    if (trial==6);
    else
        trial++;

    P2IFG &=~ BIT1;             // Limpa a flag de interrupcao
    debounce(30000);                 // Debounce do flanco de descida
    while (S1pressed);
    debounce(30000);                 // Debounce do flanco de subida
}

#pragma vector = PORT1_VECTOR
__interrupt void S2_button() {

    if (trial == goal)          // Acende o led verde para acerto
        P4OUT |= BIT7;
    else                        // Ou o vermelho para erro
        P1OUT |= BIT0;

    goal=rand();                // Procura proximo valor aleatorio
    trial=0;                    // Zera contagem das tentativas

    P1IFG &=~ BIT1;             // Limpa a flag de interrupcao
    debounce(60000);           // Debounce do flanco de descida
    while (S2pressed);
    debounce(60000);           // Debounce do flanco de subida
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
    goal=rand();
    __enable_interrupt();
    __no_operation();
}
