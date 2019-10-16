#include <msp430.h>
#include <pmm.h>
#include <clock.h>

/*****************************************************
 *  #define PCF_ADDR 0x3f    //Endereço PCF8574AT         *
 */ #define PCF_ADDR 0x27    //Endereço PCF8574T          *
/*****************************************************/

#define LED_VERMELHO    (BIT0)
#define LED_VERM_OUT    (P1OUT)
#define LED_VERDE       (BIT7)
#define LED_VERD_OUT    (P4OUT)

// I2C pins
#define SDA_SEL     P3SEL
#define SDA_BIT     BIT0
#define SDA_REN     P3REN
#define SDA_OUT     P3OUT

#define SCL_SEL     P3SEL
#define SCL_BIT     BIT1
#define SCL_REN     P3REN
#define SCL_OUT     P3OUT

#define BR100K      10      //100 kbps

// Bits para controle do LCD
#define BIT_RS   (BIT0)
#define BIT_RW   (BIT1)
#define BIT_EN   (BIT2)
#define BIT_BL   (BIT3)
#define BIT_D4   (BIT4)
#define BIT_D5   (BIT5)
#define BIT_D6   (BIT6)
#define BIT_D7   (BIT7)

#define CLR_TIME            (2)
#define ENTRY_MODE_TIME     (0.039)
#define RW_TIME             (0.043)

void zerarLCD(void);
void initLCD_4bits();

void send_rw_rs(char data);
void send_rw_RS(char data);

void configI2C(void);
void PCF_write(char dado);

int fullline = 0, linecont = 0;


void main(void) {
    WDTCTL = WDTPW | WDTHOLD;   // Stop watchdog timer
    // Configure clock
    // This should make MCLK @16MHz, SMCLK @1MHz and ACLK @32768Hz
    clockInit();
    configI2C();
    initLCD_4bits();
    zerarLCD();

    char strr[1] = {'B'};
    send_word(strr, 1, 0xC0);

    //zerarLCD();
    while(1);
}

void configI2C(void){
    SDA_SEL |=  SDA_BIT;
    SDA_REN |=  SDA_BIT;
    SDA_OUT |=  SDA_BIT;

    SCL_SEL |=  SCL_BIT;
    SCL_REN |=  SCL_BIT;
    SCL_OUT |=  SCL_BIT;

    UCB0CTL1 |= UCSWRST;

    UCB0CTL0 = UCMST |      // Mestre
               UCMODE_3 |   // Modo I2C
               UCSYNC;      // Sync

    UCB0CTL1 |= UCSSEL_2;   // SMCLK
    UCB0BRW = BR100K;       // @ 100 kbps

    UCB0CTL1 &= ~UCSWRST;   // Zera RST
}

// Escrever dado na porta
void PCF_write(char dado){
    UCB0I2CSA = PCF_ADDR;                       // Endereço do Escravo

    UCB0CTL1 |= UCTR    |                       // Mestre transmissor
                UCTXSTT;                        // Gerar START e envia PCF_ADDR

    while((UCB0IFG & UCTXIFG) == 0);            // terminar STT

    if((UCB0IFG & UCNACKIFG) == UCNACKIFG){     // Se deu erro...
        LED_VERM_OUT |= LED_VERMELHO;           // Acender LED Vermelho
        while(1);                               // Travar
    }

    UCB0TXBUF = dado;                           // Dado a ser escrito

    while((UCB0IFG & UCTXIFG) == 0);            // Esperar Transmitir

    UCB0CTL1 |= UCTXSTP;                        // STOP

    while((UCB0CTL1 & UCTXSTP) == UCTXSTP);     // Esperar STOP terminar
    delay_ms(2);
}

volatile char porta = 0;

// Zerar toda a porta
void zerarLCD(void){
    porta = 0x01;
    send_rw_rs(porta);
    blacklightOn();
}

void send_rw_rs(char data){
    char dataH = data & 0xF0;
    char dataL = data & 0x0F;

    porta = dataH;
    PCF_write(porta);
    delay_ms(5);

    porta = dataH | BIT_EN;
    PCF_write(porta);
    delay_ms(5);

    porta = dataH;
    PCF_write(porta);
    delay_ms(5);


    porta = dataL;
    porta <<= 4;
    PCF_write(porta);
    delay_ms(5);

    porta &= 0xF0;
    porta |= BIT_EN;
    PCF_write(porta);
    delay_ms(5);

    porta -= BIT_EN;
    PCF_write(porta);
    delay_ms(5);
}

void send_rw_RS(char data){
    char dataH = data & 0xF0;
    char dataL = data & 0x0F;

    porta = dataH | BIT_RS | BIT_BL;
    PCF_write(porta);
    delay_ms(5);

    porta = (dataH | BIT_RS) | BIT_EN | BIT_BL;
    PCF_write(porta);
    delay_ms(5);

    porta = dataH | BIT_RS | BIT_BL;
    PCF_write(porta);
    delay_ms(5);


    porta = dataL;
    porta <<= 4;
    porta |= BIT_RS | BIT_BL;
    PCF_write(porta);
    delay_ms(5);

    porta |= BIT_RS | BIT_EN | BIT_BL;
    PCF_write(porta);
    delay_ms(5);

    porta -= BIT_EN;
    PCF_write(porta);
    delay_ms(5);
}

void send_word(char * str, int tam, int cursor){
    send_rw_rs(cursor | BIT_D7);

    while(tam--){
        porta = *str;
        send_rw_RS(porta);
        cursor++;
        str++;
    }
}

// Inicialização 4 bits
void initLCD_4bits(){
    zerarLCD();

    //definir condições (8bits)
    delay_ms(15);
    PCF_write(0x38);
    delay_ms(15);
    PCF_write(0x3C);
    delay_ms(15);

    PCF_write(0x38);
    delay_ms(5);
    PCF_write(0x3C);
    delay_ms(5);

    PCF_write(0x38);
    delay_ms(5);
    PCF_write(0x3C);
    delay_ms(5);

    PCF_write(0x28);
    delay_ms(5);
    PCF_write(0x2C);
    delay_ms(5);

    //definir condições (4 bits), N e F
    PCF_write(0x20);
    PCF_write(0x2C);

    PCF_write(0x80);
    PCF_write(0x8C);


    PCF_write(0x00);
    PCF_write(0x0C);


    PCF_write(0x80);
    PCF_write(0x8C);

    PCF_write(0x00);
    PCF_write(0x0C);

    PCF_write(0x10);
    PCF_write(0x1C);

    PCF_write(0x00);
    PCF_write(0x0C);

    PCF_write(0x60);
    PCF_write(0x6C);

    PCF_write(0x00);
    PCF_write(0x0C);

    PCF_write(0xF0);
    PCF_write(0xFC);
}
