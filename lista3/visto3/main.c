#include <msp430.h> 
#include <stdint.h>

#define TA0_CCR0_VECTOR 53
#define TB0_CCR0_VECTOR 59
uint8_t timer_count;
uint8_t aux;

//RS=0; RW=0 -> Escreve instrução
//RS=1; RW=0 -> Escrever no DR
//RS=0; RW=1 -> Ler o bit de ocupado
//RS=1; RW=1 -> Ler o DR

#define BIT_RS      (BIT0)
#define BIT_RW      (BIT1)
#define BIT_EN      (BIT2)
#define BIT_BL      (BIT3)
#define BIT_D4      (BIT4)
#define BIT_D5      (BIT5)
#define BIT_D6      (BIT6)
#define BIT_D7      (BIT7)

#define UP_ARROW     0x06
#define DOWN_ARROW   0x05
#define LEFT_ARROW   0x7F
#define RIGHT_ARROW  0x7E
#define BRANCO       0X10
#define PRETO        0xFF
#define SLAVE_ADDR   0x27

#define BIT_X       BIT4
#define BIT_Y       BIT6
#define X           0
#define Y           1

const uint8_t down_arrow[8] = {0x00, 0x04, 0x0E, 0X15, 0X04, 0X04, 0X04, 0X00};
const uint8_t up_arrow[8] = {0x00, 0X04, 0X04, 0X04, 0X15, 0x0E, 0x04, 0x00};

volatile uint8_t FrameBuffer_new[16][2];
volatile uint8_t FrameBuffer_old [16][2];

void config_I2C() {

    // Configuração do I2C
    UCB0CTL1 |= UCSWRST;
    UCB0BRW = 10;                               // SMCLK/10 = 100kbps
    UCB0I2CSA = 0x27;                           // Endereço
    UCB0CTL0 = UCMST | UCMODE_3 | UCSYNC;       // Ativa modo sincrono, no modo mestre com UCMODE_3 (I2C)

    // Configuração das portas do I2C
    P3REN |= BIT0;                              // Ativa pullup resistor -> SDA = P3.0
    P3OUT |= BIT0;
    P3SEL |= BIT0;

    P3REN |= BIT1;                              // Ativa pullup resistor -> SCL = P3.1
    P3OUT |= BIT1;
    P3SEL |= BIT1;

    UCB0CTL1 = UCSSEL__SMCLK;                   // Escolhe SMCLK e remove rst

    uint8_t i=16,j=2;
    while(i--)
        while(j--) {
            FrameBuffer_new[j-1][i-1]=BRANCO;
            FrameBuffer_old[j-1][i-1]=BRANCO;

        }
}

void config_ADC() {
    // Configuração das portas do ADC
    P6DIR &= BIT_X;                             // CANAL A4 -> Entrada
    P6OUT &= ~BIT_X;                            // Pull down resistor
    P6REN |= BIT_X;                             // R enable
    P6SEL |= BIT_X;                             // Liga P6.4 no ADC

    P6DIR &= BIT_Y;                             // CANAL A6 -> Entrada
    P6OUT &= ~BIT_Y;                            // Pull down resistor
    P6REN |= BIT_Y;                             // R enable
    P6SEL |= BIT_Y;                             // Liga P6.6 no ADC

    // Configuração ADC
    ADC12CTL0 &= ~ADC12ENC;
    ADC12CTL0 = ADC12ON             |           // Liga conversor
            ADC12SHT0_8         |           // 8 batidas de clock
            ADC12MSC            ;           // Ativa MSC -> 1 trigger converte tds os canais

    ADC12CTL1 = ADC12CSTARTADD_0    |           // Inicio de endereço = MEM0
            ADC12SHS_0          |           // Seleciona start conversion source -> ADC12SC bit
            ADC12SHP            |           // Usa timer interno
            ADC12SSEL_3         |           // Clock = SMCLK
            ADC12CONSEQ_3;                  // Sequence of channels, single conversion mode

    ADC12MCTL0 = ADC12INCH_4;                   // MEM0 -> A4
    ADC12MCTL1 = ADC12INCH_6 | ADC12EOS;        // MEM1 -> A6 (end of sequence)

    ADC12CTL2 = ADC12TCOFF          |           // Desliga sensor de temperatura
            ADC12RES_0;                     // Resolução 8 bits

    ADC12CTL0 |= ADC12ENC;                      // Enable conversion

    // Configuração do timer do ADC -> 1Hz N=32768-1
    TB0CTL = TASSEL__ACLK | TACLR | MC__UP;    // ACLK -> 32768 Hz
    TB0CCTL0 = CCIE;
    TB0CCR0 = 32767;
}

void trigger() {                            // Trigger = flanco de SC
    ADC12CTL0 &= ~ADC12SC;
    ADC12CTL0 |= ADC12SC;
    ADC12CTL0 &= ~ADC12SC;
}

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
        while(timer_count);                      // Espera acabar a contagem do timer
        delay_ms(time_ms-2000);
    }
}

void write_data(char data){

    UCB0I2CSA = SLAVE_ADDR;                     // Endereço do Escravo
    UCB0CTL1 |= UCTR   |  UCTXSTT;              // Mestre transmissor, gera START e envia PCF_ADDR

    while(!(UCB0IFG & UCTXIFG));                // terminar STT

    if((UCB0IFG & UCNACKIFG) == UCNACKIFG)      // Se deu erro...
        while(1) {                              // Travar
            P1OUT ^= BIT0;                      // Pisca led
            delay_ms(1000);
        }

    UCB0TXBUF = data;                           // Dado a ser escrito
    while(!(UCB0IFG & UCTXIFG));                // Esperar Transmitir

    UCB0CTL1 |= UCTXSTP;                        // STOP
    while((UCB0CTL1 & UCTXSTP) == UCTXSTP);     // Esperar STOP terminar

    delay_ms(2);
}

void write_instr(char instruction) {
    char instrH = instruction & 0xF0;
    char instrL = instruction & 0x0F;

    // Tem q enviar pulso de enable
    write_data(instrH | BIT_BL);
    write_data(instrH | BIT_EN | BIT_BL);

    instrL <<= 4;
    write_data(instrL | BIT_BL);
    write_data(instrL | BIT_EN | BIT_BL);
}

void LCD_init_4bits() {
    uint8_t i=4;

    // Como não sabemos se o lcd está esperando um nibble ou um byte,
    // Enviamos a informação 3x para garantir o recebimento
    // Instrução 3 -> Set DDRAM address

    while(i--) {
        write_data(0x30 | BIT_BL);                            // Instrução 0011 -> 0x3
        write_data(0x30 | BIT_EN | BIT_BL);                   // Flanco de subida do Enable
        write_data(0x30 | BIT_BL);                            // Flanco de descida do Enable
        delay_ms(5);                                         // Espera 5ms
    }

    // Modo 4 bits -> Instrução 0010 -> 0x2
    write_data(0x20 | BIT_BL);
    write_data(0x20 | BIT_EN | BIT_BL);
    delay_ms(5);

    // A partir de agora estamos no modo 4 bits
    // Os valores são enviados nibble a nibble

    write_instr(0x28); // Instrução 0010 NFxx -> 0x28 (N=1 -> 2 linhas, F=0 -> 5x8 font type)
    write_instr(0x08); // (desativa LCD -> 0000 1DCB, Display, Cursor, Blinking of cursor on/off -> td off)
    write_instr(0x01); // Instrução 0000 0001 -> 0x01 -> Limpa o mostrador
    write_instr(0x06); // Instrução 0000 01(I/D)SH -> 0000 0110 -> 0x06
    // I/D: Direção de movimento do cursor 1: direita, SH: blinking do display=0
    write_instr(0x0C); // Instrução 0000 11CB -> 0x0C
    // (ativa LCD -> 0000 1DCB, Display, Cursor, Blinking of cursor on/off )
}

void send_word(char data) {
    // Para escrever um dado RS=1, RW=0

    char dataH = data & 0xF0;
    char dataL = data & 0x0F;

    // Tem q enviar pulso de enable
    write_data(dataH | BIT_RS | BIT_BL);
    write_data(dataH | BIT_EN | BIT_RS | BIT_BL);

    dataL <<= 4;
    write_data(dataL | BIT_RS | BIT_BL);
    write_data(dataL | BIT_EN | BIT_RS | BIT_BL);
}

void clear_LCD() {
    // Comando para limpar o LCD -> 0000 0001
    write_instr(0x01);
}

void jump_line() {
    // D6-D0 recebem um numero q indica posição do cursor
    // Primeira linha vai de 0x00 a 0x27 e segunda linha vai de 0x40 a 0x67
    // Para pular a linha para a segunda queremos escrever 0x40 = [1]100 0000 (b7 é smp 1) -> C 0
    write_instr(0xC0);
}

void return_home() {
    write_instr(0x02);
}

void define_arrow() {
    uint8_t i;

    // 0x68 CGRAM address (0[RW] 1[RS] 101[caracter 5] 000[primeira linha])
    write_instr(0x68);
    for (i=0;i<8;i++)
        send_word(down_arrow[i]);

    // 0x70 CGRAM address (0[RW] 1[RS] 110[caracter 6] 000[primeira linha])
    write_instr(0x70);
    for (i=0;i<8;i++)
        send_word(up_arrow[i]);

    clear_LCD();
}

void draw_arrow() {
    FrameBuffer_new[7][0] = LEFT_ARROW;
    FrameBuffer_new[8][0] = RIGHT_ARROW;
    FrameBuffer_new[7][1] = UP_ARROW;
    FrameBuffer_new[8][1] = DOWN_ARROW;

    /*
    // Left arrow -> cursor at [1]000 1000 = 88
    write_instr(0x87);
    send_word(LEFT_ARROW);
    // Right arrow -> cursor at [1]000 1001 = 89
    write_instr(0x88);
    send_word(RIGHT_ARROW);
    // Up arrow -> cursor at [1]100 1000 = C8
    write_instr(0xC7);
    send_word(UP_ARROW);
    // Down arrow -> cursor at [1]100 1001 = C9
    write_instr(0xC8);
    send_word(DOWN_ARROW);

    write_instr(0x02);*/
}

void updateFrameBuffer_new(uint8_t i, uint8_t axis){
    if (i<7) {
        do
            FrameBuffer_new[i][axis]=PRETO;
        while((i++)!=7);
    }
    else {
        do
            FrameBuffer_new[i+2][axis]=PRETO;
        while((i++)!=13);
    }
}

void define_range(uint8_t range, uint8_t axis) {
    uint8_t i = 0x0F;
    if (range==128)
        return;
    clear_LCD();
    while(i--) {
        if(range>>4 > i-1) {
            updateFrameBuffer_new(i-1,axis);
            aux=i-1;
        }
    }
}

void update_LCD() {
    uint8_t i=16, j=2;
    draw_arrow();
    while(i--)
        while(j--)
            if (FrameBuffer_old[i-1][j-1] != FrameBuffer_new[i-1][j-1])
                send_word(FrameBuffer_new[i-1][j-1]);
}

void update_FrameBuffer_old() {
    uint8_t i=16, j=2;
        while(i--)
            while(j--)
                FrameBuffer_old[i-1][j-1] = FrameBuffer_new[i-1][j-1];
}

void main() {
    WDTCTL = WDTPW | WDTHOLD;                  // stop watchdog timer
    __enable_interrupt();
    config_I2C();
    LCD_init_4bits();

    config_ADC();

    //update_LCD();
    while(1);


    while(1) {
        if (FrameBuffer_old != FrameBuffer_new) {
            update_LCD();
            update_FrameBuffer_old();
        }
    }
}

#pragma vector = TA0_CCR0_VECTOR
__interrupt void TA0_CCR0_ISR() {
    TA0CTL = MC__STOP;                          // Stop counter
    TA0CCTL0 &= ~CCIFG;                         // Clear flag
    timer_count=0;
}

#pragma vector = TB0_CCR0_VECTOR
__interrupt void TB0_CCR0_ISR() {
    TB0CCTL0 &= ~CCIFG;                         // Clear flag
    trigger();
    while(!(ADC12IFG & ADC12IFG0));             // Espera acabar a conversão p MEM0
    define_range(ADC12MEM0, X);
    while(!(ADC12IFG & ADC12IFG1));             // Espera acabar a conversão p MEM1
    define_range(ADC12MEM1, Y);
}
