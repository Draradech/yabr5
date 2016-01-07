#include "stdinc.h"

// timing is 1250ns (400, 400, 450)
// 1 clock cycle is 50 ns -> 8, 8, 9 cc


noinline void bit0()
{
   PORTA |= (1 << PA2);
   _NOP(); _NOP(); _NOP(); _NOP(); _NOP(); _NOP();
   PORTA &= ~(1 << PA2);
   _NOP(); _NOP(); _NOP(); _NOP(); _NOP(); _NOP(); _NOP(); _NOP();
}

noinline void bit1()
{
   PORTA |= (1 << PA2);
   _NOP(); _NOP(); _NOP(); _NOP(); _NOP(); _NOP();
   _NOP(); _NOP(); _NOP(); _NOP(); _NOP(); _NOP(); _NOP(); _NOP();
   PORTA &= ~(1 << PA2);
}

static void byte(uint8_t byte)
{
   for(int8_t i = 7; i >= 0; i--)
      if(byte & (1 << i))
         bit1();
      else
         bit0();
}

void leds(void)
{
   if(timer == 20 || timer == 70 || timer == 110)
   {
           if(attitude.voltage > 14500) ledsColor(  0, 255,   0);
      else if(attitude.voltage > 13500) ledsColor(255, 255,   0);
      else if(attitude.voltage > 13000) ledsColor(255,   0,   0);
      else ledsColor(timer == 110 ? 255 : 0,   0,   0);
   }
}

void ledsColor(uint8_t r, uint8_t g, uint8_t b)
{
   ATOMIC_BLOCK(ATOMIC_FORCEON)
   {
      byte(g); // g
      byte(r); // r
      byte(b); // b
   }
}
