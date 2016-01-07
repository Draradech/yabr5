#ifndef _RAM_H_
#define _RAM_H_

#include "stdinc.h"

#ifdef _DEFINE_RAM_VARS_
#define global
#else
#define global extern
#endif

typedef struct SRawSensorData
{
   int16_t   voltage;
   int16_t   temperature;
   int16_t   accX;
   int16_t   accY;
   int16_t   accZ;
   int16_t   angleRateX;
   int16_t   angleRateY;
   int16_t   angleRateZ;
   int16_t   wssLeft;
   int16_t   wssRight;
   int32_t   position;
   int16_t   diffSide;
} SRawSensorData;

typedef struct
{
   int16_t   voltage;        // LSB: 0.001 V
   int16_t   temperature;    // LSB: 0.001 °C
   int32_t   accX;           // LSB: 0.0001 m/ss
   int32_t   accY;           // LSB: 0.0001 m/ss
   int32_t   accZ;           // LSB: 0.0001 m/ss
   int32_t   angleRateX;     // LSB: 0.0001 °/s
   int32_t   angleRateY;     // LSB: 0.0001 °/s
   int32_t   angleRateZ;     // LSB: 0.0001 °/s
   int32_t   angleAcc;       // LSB: 0.0001 °
   int32_t   angleGyro;      // LSB: 0.0001 °
   int32_t   angleFused;     // LSB: 0.0001 °
   int16_t   speedRight;     // LSB: 0.001 m/s
   int16_t   speedLeft;      // LSB: 0.001 m/s
   int16_t   speed;          // LSB: 0.001 m/s
   int32_t   position;       // LSB: 0.001 m
   int16_t   diffSide;       // LSB: 0.001 m
} SAttitude;

typedef struct
{
    int16_t angleRate;
    int16_t subAngleRate;

    int16_t angle;

    int16_t longAcc;
    int16_t latAcc;
    int16_t vertAcc;
} SSensorOffsets;

typedef struct
{
    int16_t powerLeft;
    int16_t powerRight;

    int8_t  deactivated;
    int8_t  tempDisabled;
    uint8_t initCountdown;
} SActuator;

typedef struct
{
    int16_t angleBySpeed;
    int16_t angleByPosition;
    int16_t powerByAngleDiv;
    int16_t powerByAngleRateDiv;
    int8_t targetPosition;
    int8_t targetRL;
} SControlPara;

typedef struct
{
    int32_t angleBySpeed;
    int32_t angleByPosition;
    int32_t angleTarget;
} SControlState;

global SRawSensorData rawSensorData;
global SAttitude attitude;
global SSensorOffsets sensorOffsets;
global SControlPara controlPara;
global SControlState controlState;
global SControlPara controlPara;
global SActuator actuator;

global uint8_t lastLoopTicks;
global bool measurementOut;
global uint8_t controlTimeout;
global int16_t controlValue;
global int16_t ultra[3];
global uint8_t timer; //synchronize ultrasonic readout and led control
global uint8_t timerout; //synchronize ultrasonic readout and led control
global int16_t override;
global uint8_t autonomous;

#endif // _RAM_H_
