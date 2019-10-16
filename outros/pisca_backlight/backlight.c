#include <msp430.h> 
#include <stdint.h>
#define TA0_CCR0_VECTOR 53
uint8_t timer_count;

void config() {
    __enable_interrupt();

    // Configuração do timer
    TA0CTL = TASSEL__SMCLK | TACLR | MC__UP; // SMCLK -> 1MHz

    // Configuração do I2C
    UCB0CTL1 |= UCSWRST;
    UCB0BRW = 10;                // SMCLK/10 = 100kbps
    UCB0I2CSA = 0x27;            // Endereço
    // Ativa modo sincrono, no modo mestre com UCMODE_3 (I2C)
    UCB0CTL0 = UCMST | UCMODE_3 | UCSYNC;

    // Configuração das portas
    P3REN |= BIT0;              // Ativa pullup resistor -> SDA = P3.0
    P3OUT |= BIT0;
    P3SEL |= BIT0;
    P3REN |= BIT1;              // Ativa pullup resistor -> SCL = P3.1
    P3OUT |= BIT1;
    P3SEL |= BIT1;

    UCB0CTL1 = UCSSEL__SMCLK;   // Escolhe SMCLK e remove rst

    // Configura LED
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

/*void start() {
    UCB0I2CSA = 0x27;                           // Endereço do Escravo

    UCB0CTL1 |= UCTR | UCTXSTT;                 // Mestre transmissor

    while(!(UCB0IFG & UCTXIFG));                // terminar STT

    if((UCB0IFG & UCNACKIFG) == UCNACKIFG){     // Se deu erro...
        P1OUT ^= BIT0;                          // Pisca led
        while(1);                               // Travar
    }
}

void stop() {
    UCB0CTL1 |= UCTXSTP;
    while(!(UCB0CTL1 & UCTXSTP));
    UCB0IFG &= ~UCTXIFG;         // Limpa a flag do TX
    while(1) {
        P1OUT ^= BIT0;           // Pisca led
        delay(1000000);
    }
}

void write(uint8_t data) {

    UCB0TXBUF = data;                           // Dado a ser escrito
    while((UCB0IFG & UCTXIFG) == 0);            // Esperar Transmitir// Quando a comunicação se encerra, o sistema puxa a flag do TX p 0
    if (UCB0IFG & UCNACKIFG)    // Se não recebe acknowledge -> Stop
        stop();
}*/

// Escrever dado na porta
void PCF_write(char dado){
    UCB0I2CSA = 0x27;                           // Endereço do Escravo

    UCB0CTL1 |= UCTR    |                       // Mestre transmissor
            UCTXSTT;                            // Gerar START e envia PCF_ADDR

    while((UCB0IFG & UCTXIFG) == 0);            // terminar STT

    if((UCB0IFG & UCNACKIFG) == UCNACKIFG){     // Se deu erro...
        P1OUT ^= BIT0;           // Pisca led
        while(1);                               // Travar
    }

    UCB0TXBUF = dado;                           // Dado a ser escrito
    while((UCB0IFG & UCTXIFG) == 0);            // Esperar Transmitir

    UCB0CTL1 |= UCTXSTP;                        // STOP
    while((UCB0CTL1 & UCTXSTP) == UCTXSTP);     // Esperar STOP terminar

    delay(2000);
}

void main()
{
    WDTCTL = WDTPW | WDTHOLD;	    // stop watchdog timer
    config();
        while(1) {
        PCF_write(0x08);
        delay(200000);
        PCF_write(0x00);
        delay(200000);
    }
}

#pragma vector = TA0_CCR0_VECTOR
__interrupt void TA0_CCR0_ISR() {
    TA0CTL = MC__STOP;          // Stop counter
    TA0CCTL0 &= ~CCIFG;         // Clear flag
    timer_count=0;
}
