#include <msp430.h>

int miliseconds;
int distance;
long sensor;

void main(void)
{
  BCSCTL1 = CALBC1_1MHZ;
  DCOCTL = CALDCO_1MHZ;                    
  WDTCTL = WDTPW + WDTHOLD;                 
  
  CCTL0 = CCIE;                             
  CCR0 = 1000;				   
  TACTL = TASSEL_2 + MC_1;                  

  P1IFG  = 0x00;			    
  P1DIR |= 0x01;                            
  P1OUT &= ~0x01;                           
  
  _BIS_SR(GIE);                 	    
 
 while(1){
	P1IE &= ~0x01;			
	P1DIR |= 0x02;			
	P1OUT |= 0x02;			
	__delay_cycles(10);            
	P1OUT &= ~0x02;                 
  	P1DIR &= ~0x04;			
        P1IFG = 0x00;                   
	P1IE |= 0x04;			
	P1IES &= ~0x04;			
        __delay_cycles(30000);          
        distance = sensor/58;           
        if(distance < 20 && distance != 0) P1OUT |= 0x01;  
        else P1OUT &= ~0x01;            

        

	
 }
}

#pragma vector=PORT1_VECTOR
__interrupt void Port_1(void)
{
	if(P1IFG&0x04)  
        {
          if(!(P1IES&0x04)) 
          {			
            TACTL|=TACLR;   
            miliseconds = 0;
            P1IES |= 0x04;  
          }
          else
          {
            sensor = (long)miliseconds*1000 + (long)TAR;	

          }
	P1IFG &= ~0x04;				
	}
}

#pragma vector=TIMER0_A0_VECTOR
__interrupt void Timer_A (void)
{
  miliseconds++;
}

