#include <msp430.h>
#include <stdint.h>
#define S1pressed (!(P2IN & BIT1))
#define UCA0RXIFG_VECTOR = 56;

void config() {

    // First Reset module
    UCA0CTL1 |= UCSWRST;

    UCA0CTL0  =                 // UCA Control 0 register
            //              UCPEN         | // Enable parity (clear to disable     )
            //              UCPAR         | // Even parity   (clear to odd         )
            //              UCMSB         | // MSB first     (clear for LSB first  )
            //              UC7BIT        | // 7 bits        (clear this for 8-bits)
            //              UCSPB         | // Two bits      (clear for one)
            //              UCMODE_0      | // Mode 0 - UART (11 for automatic baud)
            //              UCSYNC        | // Synchronous   (clear for async)
            0;

    UCA0CTL1  =                 // UCA Control 1 register
            UCSSEL__SMCLK | // SMCLK is set to 1 MHz
            //              UCRXEIE       | // Erroneous-character interrupt enable
            //              UCBRKIE       | // Break character interrupt enable
            //              UCDORM        | // Puts UCSI in sleep mode
            //              UCTXADDR      | // Transmit address (next frame is address)
            //              UCTXBRK       | // Transmit break
            UCSWRST       | // Module is in reset mode while config.
            0;

    // Clock prescaler          // For SMCLK = 1MHz, UCOS = 0
    // See table 34-4           // UCBRx=104 UCBRSx=1 UCBRFx=0
    UCA0BRW   = 27;

    UCA0MCTL  =                 // Modulation Control Register
            UCBRF_0       | // First  modulation stage select
            UCBRS_2       | // Second modulation stage select
            //              UCOS16        | // Oversampling mode (clear for disabled)
            0;

    // Pins P3.3 and P3.4 are dedicated pins of USCI-A0
    P3SEL = (BIT3 | BIT4);

    // Clear reset bit
    UCA0CTL1 &= ~UCSWRST;       // This will start serial interface (TXIFG = 1)

    UCA0IE    =                 // Module interruptions
            //UCTXIE        | // TX interrupt enable (buffer empty )
            //UCRXIE        | // RX interrupt enable (received word)
            0;
}

void debounce () {
    volatile int i;
    for (i=0; i<500 ; i++);
    return ;
}

void main()
{
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer
    config();

    char nome_tx[14] = "Fernanda Melo";
    char nome_rx[14];
    uint8_t i=0;

    while (nome_tx[i] != '\0') {
        UCA0TXBUF = nome_tx[i];
        while (!(UCA0IFG & UCTXIFG));
        nome_rx[i++] = UCA0RXBUF;
    }

}
