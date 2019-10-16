/* Configure o módulo do conversor A/D para amostrar a tensão de entrada
 * que representa apenas um eixo da posição do joystick */

#include <msp430.h> 
#include <stdint.h>

uint16_t xAxis[512];
uint16_t aux=512;
#define BIT_ADC BIT4
#define TA0_CCR0_VECTOR 53
uint8_t timer_count;

void delay_us (uint32_t time_us) {
    timer_count=1;
    if (time_us < 65535) {
        TA0CCR0 = time_us+1;
        TA0CTL = TASSEL__SMCLK | TACLR | MC__UP; // SMCLK -> 1MHz
        TA0CCTL0 = CCIE;
        while(timer_count);                      // Espera acabar a contagem do timer
    }
    else {
        TA0CTL = TASSEL__SMCLK | TACLR | MC__UP; // SMCLK -> 1MHz
        TA0CCTL0 = CCIE;
        TA0CCR0 = 65535;
        while(timer_count);                      // Espera acabar a contagem do timer
        delay_us(time_us-65535);
    }
}

void delay_ms (uint32_t time_ms) {
    timer_count=1;
    // 65535 = 2 seg ->> 1ms ~ 33
    if (time_ms*33 < 65535) {
        TA0CCR0 = time_ms*33+1;
        TA0CTL = TASSEL__ACLK | TACLR | MC__UP;  // ACLK -> 32768Hz
        TA0CCTL0 = CCIE;
        while(timer_count);                      // Espera acabar a contagem do timer
    }
    else {
        TA0CTL = TASSEL__ACLK | TACLR | MC__UP;
        TA0CCTL0 = CCIE;
        TA0CCR0 = 65535;
        while(timer_count);                  // Espera acabar a contagem do timer
        delay_ms(time_ms-2000);
    }
}

void trigger() {                            // Trigger = flanco de SC
    ADC12CTL0 &= ~ADC12SC;
    ADC12CTL0 |= ADC12SC;
    ADC12CTL0 &= ~ADC12SC;
}

void config() {
    __enable_interrupt();

    // Configuração da porta
    P6DIR &= BIT_ADC;                       // CANAL A4 -> Entrada
    P6OUT &= ~BIT_ADC;                      // Pull down resistor
    P6REN |= BIT_ADC;                       // R enable
    P6SEL |= BIT_ADC;                       // Liga P6.4 no ADC

    // Configuração ADC
    ADC12CTL0 &= ~ADC12ENC;                 // Desativa interrupção
    ADC12CTL0 = ADC12ON | ADC12SHT0_8;      // Liga conversor, 8 batidas de clock

    ADC12CTL1 = ADC12CSTARTADD_0    |       // Inicio de endereço = MEM0
            ADC12SHS_0          |           // Seleciona start conversion source -> ADC12SC bit
            ADC12SHP            |           // Usa timer interno
            ADC12SSEL_3         |           // Clock = SMCLK
            ADC12CONSEQ_0;                  // Single channel, single conversion mode

    ADC12MCTL0 = ADC12INCH_4 | ADC12EOS;    // MEM0 -> A4 (end of sequence)

    ADC12CTL2 = ADC12TCOFF          |       // Desligar sensor de temperatura
            ADC12RES_2;                     // Resolução 12 bits

    ADC12CTL0 |= ADC12ENC;                  // Enable conversion
}

void main() {
    WDTCTL = WDTPW | WDTHOLD;	            // stop watchdog timer
    config();
    while(aux--) {
        delay_us(10400);
        trigger();
        while(!(ADC12IFG & ADC12IFG0));     // Espera acabar a conversão
        xAxis[aux]=ADC12MEM0;
    }
    while(1);
}

#pragma vector = TA0_CCR0_VECTOR
__interrupt void TA0_CCR0_ISR() {
    TA0CTL = MC__STOP;                          // Stop counter
    TA0CCTL0 &= ~CCIFG;                         // Clear flag
    timer_count=0;
}
