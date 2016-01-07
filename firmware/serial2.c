#include "stdinc.h"

uint8_t i;
uint8_t data[4];
uint8_t sensor;
uint8_t ready;

ISR(USART1_RX_vect)
{
   uint8_t byte;
   byte = UDR1;
   if(byte == 'R')
   {
      i = 0;
   }
   else if(byte == 13)
   {
      ready = true;
   }
   else
   {
      if(i < 4)
      {
         data[i++] = byte;
      }
   }
}

void triggerSonar(void)
{
   timer++;
   if(timer == 20) sensor = 0;
   if(timer == 128) PORTA |= (1 << PA4);
   if(timer == 129) PORTA &= ~(1 << PA4);
   if(timer == 130) timer = 0;
   
   if(ready)
   {
      if(sensor < 3) ultra[sensor++] = (data[0] - '0') * 1000 + (data[1] - '0') * 100 + (data[2] - '0') * 10 + (data[3] - '0');
      ready = 0;
      timerout= timer;
   }
}
