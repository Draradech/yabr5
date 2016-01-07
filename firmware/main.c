#include "stdinc.h"

static volatile bool loopActive;
static volatile uint8_t ticksSinceLoopStart;

void noreturn main(void)
{
   init();
   initSensors();

   for(;;)
   {
      while(!loopActive)
      {
         ; // wait for next loop
      }

      readSensors();
      attitudeCalculation();
      control();
      actuate();
      triggerSonar();
      input();
      output();
      leds();

      ATOMIC_BLOCK(ATOMIC_FORCEON)
      {
         lastLoopTicks = ticksSinceLoopStart;
         loopActive = false;
      }
   }
}

ISR(TIMER0_COMPA_vect)
{
   // Mainloop Control
   ticksSinceLoopStart++;
   if(loopActive == false && ticksSinceLoopStart >= TICKS_PER_LOOP)
   {
      ticksSinceLoopStart = 0;
      loopActive = true;
   }
}
