#include <msp430.h> 
#include <stdint.h>

#define BIT_X BIT4
#define BIT_Y BIT6
#define BIT_LED1 BIT3
#define BIT_LED2 BIT7
#define TA0_CCR0_VECTOR 53
#define TB0_CCRN_VECTOR 58

void config() {
    __enable_interrupt();

    // Configuração das portas
    P6DIR &= BIT_X;                       // CANAL A4 -> Entrada
    P6OUT &= ~BIT_X;                      // Pull down resistor
    P6REN |= BIT_X;                       // R enable
    P6SEL |= BIT_X;                       // Liga P6.4 no ADC

    P6DIR &= BIT_Y;                       // CANAL A6 -> Entrada
    P6OUT &= ~BIT_Y;                      // Pull down resistor
    P6REN |= BIT_Y;                       // R enable
    P6SEL |= BIT_Y;                       // Liga P6.6 no ADC

    PMAPKEYID = 0x02D52;                  // Get authorization to remap port function
    P4MAP3    = PM_TB0CCR1A;              // Map P4.3 to output of timer B, channel 1
    P4MAP7    = PM_TB0CCR2A;              // Map P4.7 to output of timer B, channel 2

    P4DIR |= BIT_LED1;                    // LED1 -> Saída
    P4SEL |= BIT_LED1;
    P4DIR |= BIT_LED2;                    // LED2 -> Saída
    P4SEL |= BIT_LED2;

    // Configuração timers dos leds -> B0.1 e B0.2
    // Leds piscam a 50Hz, com SMCLK N=20000
    TB0CTL = TBSSEL__SMCLK | TBCLR | MC__UP;
    TB0CCR0 = 20000;                       // Toggle on CCR0 -> freq de 50Hz
    TB0CCR1 = 10000;                       // Reset on CCR1 -> duty cycle de 50%
    TB0CCR2 = 10000;                       // Reset on CCR2 -> duty cycle de 50%
    TB0CCTL2 = OUTMOD_6;                   // Toggle set
    TB0CCTL1 = OUTMOD_6;

    // Configuração ADC
    ADC12CTL0 &= ~ADC12ENC;
    ADC12CTL0 = ADC12ON             |       // Liga conversor
                ADC12SHT0_8         |       // 8 batidas de clock
                ADC12MSC            ;       // Ativa MSC -> 1 trigger converte tds os canais

    ADC12CTL1 = ADC12CSTARTADD_0    |       // Inicio de endereço = MEM0
                ADC12SHS_0          |       // Seleciona start conversion source -> ADC12SC bit
                ADC12SHP            |       // Usa timer interno
                ADC12SSEL_3         |       // Clock = SMCLK
                ADC12CONSEQ_3;              // Sequence of channels, single conversion mode

    ADC12MCTL0 = ADC12INCH_4;               // MEM0 -> A4
    ADC12MCTL1 = ADC12INCH_6 | ADC12EOS;    // MEM1 -> A6 (end of sequence)

    ADC12CTL2 = ADC12TCOFF          |       // Desliga sensor de temperatura
                ADC12RES_0;                 // Resolução 8 bits

    ADC12CTL0 |= ADC12ENC;                  // Enable conversion

    // Configuração do timer do ADC -> 50Hz N=20000-1
    TA0CTL = TASSEL__SMCLK | TACLR | MC__UP;// SMCLK -> 1MHz
    TA0CCTL0 = CCIE;
    TA0CCR0 = 19999;
}

void trigger() {                            // Trigger = flanco de SC
    ADC12CTL0 &= ~ADC12SC;
    ADC12CTL0 |= ADC12SC;
    ADC12CTL0 &= ~ADC12SC;
}

void main()
{
    WDTCTL = WDTPW | WDTHOLD;	            // stop watchdog timer
    config();
    while(1);
}

#pragma vector = TA0_CCR0_VECTOR
__interrupt void TA0_CCR0_ISR() {
    TA0CCTL0 &= ~CCIFG;                     // Clear flag
    trigger();
    while(!(ADC12IFG & ADC12IFG0));         // Espera acabar a conversão p MEM0
    TB0CCR1 = ADC12MEM0*78;
    while(!(ADC12IFG & ADC12IFG1));         // Espera acabar a conversão p MEM1
    TB0CCR2 = ADC12MEM1*78;
}
