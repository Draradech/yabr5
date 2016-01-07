#include "stdinc.h"

void attitudeCalculation(void)
{
   int16_t tmp16;
   int32_t tmp32;

   tmp16 = (int32_t) rawSensorData.voltage * 5394 >> 8;      // * 21576 (21,576V) / 1024
   attitude.voltage = PT1(tmp16, attitude.voltage, 16);

   tmp16 = 4843 - rawSensorData.temperature * 84;            // 4843 (48,43 °C) - x * 84 (0,84 °C)
   attitude.temperature = PT1(tmp16, attitude.temperature, 2);

   tmp32 = (int32_t) rawSensorData.accX * 1533 >> 8;         // * 196200 (19.62m/ss) / 32768
   attitude.accX = PT1(tmp32, attitude.accX, 32);

   tmp32 = (int32_t) rawSensorData.accY * 1533 >> 8;         // * 196200 (19.62m/ss) / 32768
   attitude.accY = PT1(tmp32, attitude.accY, 32);

   tmp32 = (int32_t) rawSensorData.accZ * 1533 >> 8;         // * 196200 (19.62m/ss) / 32768
   attitude.accZ = PT1(tmp32, attitude.accZ, 32);

   tmp32 = (int32_t) rawSensorData.angleRateX * 175;    // * 0.00875 -> 0.0001
   attitude.angleRateX = PT1(tmp32, attitude.angleRateX, 4);

   tmp32 = (int32_t) rawSensorData.angleRateY * 175;    // * 2500000 (250°/s) / 32768
   attitude.angleRateY = PT1(tmp32, attitude.angleRateY, 4);

   tmp32 = (int32_t) rawSensorData.angleRateZ * 175;    // * 2500000 (250°/s) / 32768
   attitude.angleRateZ = PT1(tmp32, attitude.angleRateZ, 4);

   tmp16 = (int16_t) rawSensorData.wssRight * 89;            // * 1000 (1 m/s) / (5600 / 500)
   attitude.speedRight = PT1(tmp16, attitude.speedRight, 16);

   tmp16 = (int16_t) rawSensorData.wssLeft * 89;             // * 1000 (1 m/s) / (5600 / 500)
   attitude.speedLeft = PT1(tmp16, attitude.speedLeft, 16);
   
   attitude.speed = (attitude.speedRight + attitude.speedLeft) / 2;
   
   tmp32 = (int32_t) rawSensorData.position * 91 >> 10; // 1000 (1m) / 11250
   attitude.position = tmp32;

   tmp32 = (int32_t) rawSensorData.diffSide * 91 >> 10;
   attitude.diffSide = tmp32;

   attitude.angleAcc = (int32_t)-100 * atan2_cordic(attitude.accZ / 8, attitude.accX / 8);

   tmp32 = -attitude.angleRateY / LOOPS_PER_SECOND - sensorOffsets.angleRate;
   attitude.angleGyro += tmp32;
   attitude.angleFused += tmp32;

   tmp32 = attitude.angleAcc;
   attitude.angleFused = PT1(tmp32, attitude.angleFused, 1024);

   if(attitude.angleGyro < attitude.angleFused)
   {
      sensorOffsets.subAngleRate--;
   }
   else if(attitude.angleGyro > attitude.angleFused)
   {
      sensorOffsets.subAngleRate++;
   }

   if(sensorOffsets.subAngleRate > 1000)
   {
      sensorOffsets.angleRate++;
      sensorOffsets.subAngleRate = 0;
      attitude.angleGyro = attitude.angleFused;
   }

   if(sensorOffsets.subAngleRate < -1000)
   {
      sensorOffsets.angleRate--;
      sensorOffsets.subAngleRate = 0;
      attitude.angleGyro = attitude.angleFused;
   }
}
