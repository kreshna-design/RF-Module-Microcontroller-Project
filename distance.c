#include <msp430.h>

#define   TXD       BIT2                                     // P1.1 = TXT
#define   RXD       BIT1                                     // P1.2 = RXD
#define   TRIG      BIT4                                     // P1.4 = TRIG
#define   ECHO      BIT5                                     // P1.5 = ECHO

int distance;                                                // Distance

void main(void)
{
  BCSCTL1 = CALBC1_1MHZ;                                     // Set range
  DCOCTL = CALDCO_1MHZ;                                      // Submainclock 1MHz
  WDTCTL = WDTPW + WDTHOLD;                                  // Stop WDT               
  BCSCTL2  &= ~(DIVS_3);                                     

  P1DIR    |=  ~ECHO;                                        // Echo input as interrupt
  P1DIR    |= TXD + TRIG;                                    // Echo input as interrupt

  P1SEL     = BIT1 + BIT2;                                   // RXD and TXT
  P1SEL2    = BIT1 + BIT2;                                   // RXD and TXT

  UCA0CTL1 |= UCSSEL_2;                                      // Use SMCLK
  UCA0BR0   = 104;                                           // Set baud rate to 9600 with 1MHz clock
  UCA0BR1   = 0;                                             // Set baud rate to 9600 with 1MHz clock
  UCA0MCTL  = UCBRS0;                                        // Modulation UCBRSx = 1
  UCA0CTL1 &= ~UCSWRST;                                      // Initialize USCI state machine

  P1OUT    |= TXD;                                           // Enable interupt on ECHO pin
  P1IE     |= ECHO;                                          // Enable interupt on ECHO pin
 
  TACTL = TACLR;                                             // Reset clock
  TACTL = TASSEL_2 + MC_2;                                   // SMCLK, upmode at 1 MHz
  
  __enable_interrupt();
 
 
 while(1){
	P1OUT |=  TRIG;                                      // Start trigger signal          
        __delay_cycles(12);                                  // Need a >10 us pulse and one clock cycle is 1 us
        P1OUT &= ~TRIG;                                      // End trigger signal
        P1IES &= ~ECHO;                                      // Interrupt on low to high
        __bis_SR_register(LPM0_bits + GIE);
        TAR = 10;                                            // For 10us width of the pulse
        P1IES |=  ECHO;                                      // Interrupt on high to low
        __bis_SR_register(LPM0_bits + GIE);
        distance = TAR / 58;                                 // Converting ECHO lenght into cm

        while (!(IFG2 & UCA0TXIFG));                         // Wait for TX buffer to be ready for new data
            UCA0TXBUF = distance;                            // Wait for TX buffer to be ready for new data
            __delay_cycles(30000);                           // delay for 30ms 

 }
}


#if defined(__TI_COMPILER_VERSION__)

#pragma vector=PORT1_VECTOR

__interrupt void port1_isr(void)

#else

  void __attribute__ ((interrupt(PORT1_VECTOR))) port1_isr (void)

#endif

{

  P1IFG = 0;

  __bic_SR_register_on_exit(LPM0_bits);                      // Take us out of low power mode
}

