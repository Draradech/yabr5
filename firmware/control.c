#include "stdinc.h"

int16_t lastControl;
int16_t timeout;
int8_t turndir;

void control(void)
{
   int32_t angleDiff;
   int16_t byAngle, byRate;
   
   if(controlTimeout)
   {
      controlTimeout--;
      rawSensorData.position = LIMIT(controlValue * 20, lastControl - 10, lastControl + 10);
      rawSensorData.position = LIMIT(rawSensorData.position, -5000, 5000);
      lastControl = rawSensorData.position;
      timeout = 5000;
   }
   else if(autonomous)
   {
      int8_t drive = LIMIT((ultra[0] / 301) -2, -2, 5);
      int8_t turn = ultra[1] > ultra[2] ? 1 : -1;
      
      if(attitude.speed < -100)
      {
         timeout = 5000;
         turndir = (timer & 0x01) ? -2 : 2;
      }
      
      if(actuator.tempDisabled) timeout = 5000;
      
      if(timeout) timeout--;
      else if(drive <= 0) turn = turndir;
      
      rawSensorData.position += drive;
      rawSensorData.diffSide += turn;
   }

   if (actuator.tempDisabled)
   {
      rawSensorData.position = 0;
      rawSensorData.diffSide = 0;
   }

   if (override < 0)
   {
      if(attitude.voltage > 14500) override = 700;
      else override = 0;
   } 
   if (override) override--;
   if (override) rawSensorData.position = 0;
   
   if (  override
      && attitude.angleFused <  450000
      && attitude.angleFused > -450000
      && attitude.speed < 100
      && attitude.speed > -100
      && SIGN(attitude.angleFused) == SIGN(attitude.angleRateY)
      )
   {
      override = 0;
   }

   controlState.angleBySpeed = (int32_t)(attitude.speedLeft + attitude.speedRight) * controlPara.angleBySpeed;
   controlState.angleByPosition = attitude.position * controlPara.angleByPosition;
   controlState.angleTarget = controlState.angleBySpeed + controlState.angleByPosition;

   angleDiff = (attitude.angleFused - controlState.angleTarget );
   byAngle = angleDiff / controlPara.powerByAngleDiv / 16;
   if(override) byAngle = angleDiff / controlPara.powerByAngleDiv / 24;
   byRate = attitude.angleRateY / controlPara.powerByAngleRateDiv / 256;

   actuator.powerLeft = -byAngle + byRate - attitude.diffSide;
   actuator.powerRight = -byAngle + byRate + attitude.diffSide;
   
}
