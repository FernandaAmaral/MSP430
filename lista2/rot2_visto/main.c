#include <msp430.h> 
#include <stdint.h>

#define S1pressed (!(P2IN & BIT1))
#define TRIGGER BIT1   // P6.1
#define ECHO BIT3  // P1.3
#define DISTANCE_THRESHOLD1 20 //cm
#define DISTANCE_THRESHOLD2 40 //cm
#define LED2 BIT7   // P4.7
#define LED1 BIT0   // P1.0

uint8_t flag_Echo_Terminou;
uint32_t distance_measured = 0;
uint16_t lastCount = 0;

#define TEMPO_DEBOUNCER 1000            //parametro usado no debounce
volatile uint8_t contador_debounce;    //variavel auxiliar para o debounce

void config_trena(){
    P6DIR |= TRIGGER;           // Pino do trigger (P6.1) é saída
    P6OUT &= ~TRIGGER;          // Trigger desativado

    P1DIR &= ~ECHO;             // Pino do echo (P1.3) é entrada
    P1SEL |= ECHO;              // Liga echo em TA0CCR2

    // Configura TA0 para capturar em CCR2
    // Detecção flanco de subida e descida
    TA0CCTL2 = CM_3 | CCIS_0 | SCS | CAP | CCIE;

    // Configura TA0 com ACLK / 4 = 8192 Hz
    TA0CTL = TASSEL__ACLK | ID__4 | MC__CONTINUOUS | TACLR;
    _enable_interrupt();
}

void debounce() {
    for (contador_debounce = 0; contador_debounce != TEMPO_DEBOUNCER;contador_debounce++) {}
}

void mede_distancia(){
    volatile uint8_t trigWait;
    flag_Echo_Terminou=2;
    P6OUT |= TRIGGER;               // Liga trigger

    // Espera cerca de 10 ciclos do clock (1Mhz), pois o HCSR04 precisa
    // enviar um pulso de pelo menos 10us para o echo funcionar
    for (trigWait = 0; trigWait < 12; trigWait++) {}

    P6OUT &= ~TRIGGER;              // Desliga trigger

    while(!flag_Echo_Terminou);     // Wait for echo end

    distance_measured = lastCount; // Inicia o calculo da distancia em cm
    distance_measured *= 34000;    // Multiplica deltaT pela velocidade * 100 (pq queremos em cm)
    distance_measured /= 16384;    // Pois ACLK foi setado com 8192 Hz
    // Tem q dividir por 2 também pq a distancia é percorrida 2x
}

void config_button_leds() {

    P2DIR &=~ BIT1;             // P2.1 é entrada
    P2REN |= BIT1;              // Ativa resistor
    P2OUT |= BIT1;              // Resistor é pull up

    P1DIR |= LED1;              // P1.0 é saída
    P1OUT &=~ LED1;             // Led começa apagado

    P4DIR |= LED2;              // P4.7 é saída
    P4OUT &=~ LED2;             // Led começa apagado
}

void acende_led_de_acordo_distancia(){
    if (distance_measured <= DISTANCE_THRESHOLD1) {//se a distancia detectada for menor que o threshould (limite), acende o led
        P1OUT |= LED1;          // Turn on LED1
        P4OUT &=~ LED2;         // Turn off LED2
    }

    else if (distance_measured >= DISTANCE_THRESHOLD1 && distance_measured < DISTANCE_THRESHOLD2) {//se a distancia for maior que o threshould (limite), apaga o led
        P4OUT |= LED2;          // Turn on LED2
        P1OUT &=~ LED1;         // Turn off LED1
    }

    else if(distance_measured >= DISTANCE_THRESHOLD2){
        P1OUT |= LED1;          // Turn on LED1
        P4OUT |= LED2;          // Turn on LED2
    }
}

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer
    config_button_leds();       // Configuracao dos pinos do botão e dos LEDs
    uint8_t S1state = 0;        // S1 not pressed
    config_trena();             // Configuração dos pinos ECHO, TRIGGER e do timer

    while(1) {
        if(S1pressed) {
            if (S1state == 1);
            else {
                S1state = 1;
                mede_distancia();
                acende_led_de_acordo_distancia();
                debounce();
            }}
        else {
            if (S1state == 0);
            else {
                S1state = 0;
                debounce();
            }
        }
    }
}

#pragma vector = TIMER0_A1_VECTOR
__interrupt void TIMER0_A1_ISR (void) {
    // Estado do pino echo mudou

    if(flag_Echo_Terminou==2){              // Primeira leitura - flanco de subida
        lastCount = TA0CCR2;                // Salva a leitura atual (TA0CCR2 e um acumulador)
        flag_Echo_Terminou=0;
    }
    else if(flag_Echo_Terminou==0){
        lastCount = TA0CCR2 - lastCount;    // salva em lastCount (posicao 0x200C) o intervalo medido
        flag_Echo_Terminou=1;
    }

    TA0IV = 0;
}
