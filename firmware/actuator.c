#include "stdinc.h"

void actuate()
{
   int16_t powerLeft, powerRight, powerLimit;
   powerLimit = 255;

   powerLeft  = (int32_t)actuator.powerLeft * 16000 / attitude.voltage;
   powerRight = (int32_t)actuator.powerRight * 16000 / attitude.voltage;

   if (attitude.voltage < 12000) actuator.deactivated = true;
   if (actuator.initCountdown) actuator.initCountdown--;

   if (  attitude.angleFused >  450000
      || attitude.angleFused < -450000
      )
   {
      if(!actuator.tempDisabled)
      {
         actuator.tempDisabled = true;
      }
   }
   else if (actuator.tempDisabled && (powerLeft + powerRight) < 20 && (powerLeft + powerRight) > -20 && !actuator.initCountdown)
   {
      actuator.tempDisabled = false;
   }
   
   if(override)
   {
      actuator.tempDisabled = false;
   }

   if(actuator.tempDisabled)
   {
      powerLeft = powerRight = 0;
   }

   if (powerLeft < 0 && !actuator.deactivated)
   {
      PORTC |=  (1 << PC3);
      PORTC &= ~(1 << PC2);
      OCR2B = MIN(-powerLeft, powerLimit);
   }
   else if (powerLeft > 0 && !actuator.deactivated)
   {
      PORTC &= ~(1 << PC3);
      PORTC |=  (1 << PC2);
      OCR2B = MIN(powerLeft, powerLimit);
   }
   else
   {
      PORTC &= ~(1 << PC2);
      PORTC &= ~(1 << PC3);
      OCR2B = 0;
   }

   if (powerRight < 0 && !actuator.deactivated)
   {
      PORTC |=  (1 << PC4);
      PORTC &= ~(1 << PC5);
      OCR2A = MIN(-powerRight, powerLimit);
   }
   else if (powerRight > 0 && !actuator.deactivated)
   {
      PORTC &= ~(1 << PC4);
      PORTC |=  (1 << PC5);
      OCR2A = MIN(powerRight, powerLimit);
   }
   else
   {
      PORTC &= ~(1 << PC4);
      PORTC &= ~(1 << PC5);
      OCR2A = 0;
   }
}
