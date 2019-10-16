#include <msp430.h>
#include <stdint.h>

int timerCount;
int bt1pressed = 0;
#define TA0_CCR0_INT 53

def_io()
{
    // configuration sw1 as input
    P2IE |= BIT1;                           // Liga interrução P2.1
    P2IES |= BIT1;                          // Flanco de descida
    P2DIR &= ~BIT1;
    P2REN |=  BIT1;
    P2OUT |=  BIT1;

    // configuration sw2 as input
    P1SEL |= BIT1;                           // Liga no timer TA0.0
    P1DIR &= ~BIT1;
    P1REN |=  BIT1;
    P1OUT |=  BIT1;

    // config red led
    P1DIR |= (BIT0);
    P1OUT &= ~(BIT0); //off

    // config green led
    P4DIR |= (BIT7);
    P4OUT &= ~(BIT7);
}

def_timer()
{
    // sw1 (P2.1) está ligado ao timer A1.2 --> ccr2 do timer A1
    // sw2 (P1.1) está ligado ao timer A0.0 --> ccr0 do timer A0

    // Configuração do timer: ACLK @32768 Hz com ID_3 fica com 8192Hz
    // MC_CONTINUOUS conta até FFFF em 8 segundos (tempo máximo de contagem entre as capturas)
    TA0CTL = TASSEL__ACLK |ID_3 | MC__STOP | TACLR;
    TA0CCTL0 = CM_1 | CAP | CCIE | CCIS_0 | SCS;
}

debounce()
{
    __delay_cycles(7000);
}

watchdog_timer()
{
    WDTCTL = WDTPW|WDTHOLD;
}

int main()
{
    watchdog_timer();
    def_io();
    def_timer();
    __enable_interrupt();

    while(1);
}

#pragma vector = PORT2_VECTOR
__interrupt void P2_ISR(){
    switch(P2IV){
    case 0x4:
        timerCount = TA0R;
        TA0CTL |= MC__CONTINOUS;
        P2IE &= ~BIT1;
        P2IFG &= ~BIT1;
        bt1pressed = 1;
        break;
    default:
        break;
    }
}

#pragma vector=TA0_CCR0_INT
__interrupt void TA0_CCR0_ISR(){
    if(!bt1pressed)
        return;

    int timeDif = 0;

    if(timerCount == 0){
        timeDif = TA0CCR0;
    }else{
        timeDif = TA0CCR0 - timerCount;
    }

    if(timeDif < 0){
        timeDif += 32768;
    }

    if(timeDif < 4097){
        P1OUT |= (BIT0);
        P4OUT &= ~(BIT7);
    }else if(timeDif < 12289){
        P1OUT |= (BIT0);
        P4OUT |= (BIT7);
    }else{
        P1OUT &= ~(BIT0);
        P4OUT |= (BIT7);
    }

    P2IE |= BIT1;
    P2IFG &= ~BIT1;

    bt1pressed = 0;
}
