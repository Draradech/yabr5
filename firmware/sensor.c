#include "stdinc.h"

static volatile int16_t wssRight, wssLeft;

#define X 0

static int8_t steps[4][4] = {{0, -1, 1, X}, {1, 0, X, -1}, {-1, X, 0, 1}, {X, 1, -1, 0}};

static uint8_t spiSendRecv(uint8_t byte)
{
   SPDR = byte;
   while(!(SPSR & (1 << SPIF)));
   return SPDR;
}

static void writeReg(uint8_t pin, uint8_t addr, uint8_t val)
{
   PORTB &= ~(1 << pin);
   spiSendRecv(addr);
   spiSendRecv(val);
   PORTB |= (1 << pin);
}

static uint8_t readReg(uint8_t pin, uint8_t addr)
{
   uint8_t val;
   PORTB &= ~(1 << pin);
   spiSendRecv(addr | 0x80);
   val = spiSendRecv(0);
   PORTB |= (1 << pin);
   return val;
}

void initSensors(void)
{
   /* gyro */
   writeReg(PB2, 0x20, 0xff);
   writeReg(PB2, 0x21, 0x44);
   writeReg(PB2, 0x22, 0x00);
   writeReg(PB2, 0x23, 0x90);
   writeReg(PB2, 0x24, 0x00);

   /* acc */
   writeReg(PB3, 0x20, 0x97);
   writeReg(PB3, 0x21, 0x00);
   writeReg(PB3, 0x22, 0x00);
   writeReg(PB3, 0x23, 0x88);
   writeReg(PB3, 0x24, 0x00);
   writeReg(PB3, 0x25, 0x00);
}

static void readGyro(void)
{
   rawSensorData.temperature = readReg(PB2, 0x26);

   rawSensorData.angleRateX  = ((int16_t)((uint16_t)readReg(PB2, 0x29) << 8));
   rawSensorData.angleRateX += readReg(PB2, 0x28);

   rawSensorData.angleRateY  = ((int16_t)((uint16_t)readReg(PB2, 0x2B) << 8));
   rawSensorData.angleRateY += readReg(PB2, 0x2A);

   rawSensorData.angleRateZ  = ((int16_t)((uint16_t)readReg(PB2, 0x2D) << 8));
   rawSensorData.angleRateZ += readReg(PB2, 0x2C);
}

static void readAcc(void)
{
   rawSensorData.accX  = ((int16_t)((uint16_t)readReg(PB3, 0x29) << 8));
   rawSensorData.accX += readReg(PB3, 0x28);

   rawSensorData.accY  = ((int16_t)((uint16_t)readReg(PB3, 0x2B) << 8));
   rawSensorData.accY += readReg(PB3, 0x2A);

   rawSensorData.accZ  = ((int16_t)((uint16_t)readReg(PB3, 0x2D) << 8));
   rawSensorData.accZ += readReg(PB3, 0x2C);
}

static void readBattery(void)
{
   rawSensorData.voltage = ADC;
}

static void readWss(void)
{
   ATOMIC_BLOCK(ATOMIC_FORCEON)
   {
      rawSensorData.wssRight = -wssRight;
      wssRight = 0;
      rawSensorData.wssLeft = wssLeft;
      wssLeft = 0;
   }

   rawSensorData.position += rawSensorData.wssRight + rawSensorData.wssLeft;
   rawSensorData.diffSide += rawSensorData.wssRight - rawSensorData.wssLeft;
}

void readSensors(void)
{
   readBattery();
   readAcc();
   readGyro();
   readWss();
}

volatile static uint8_t pbll;
ISR(PCINT1_vect)
{
   uint8_t pb = PINB & 0x03;
   wssRight += steps[pbll][pb];
   pbll = pb;
}

volatile static uint8_t pcll;
ISR(PCINT2_vect)
{
   uint8_t pc = PINC & 0x03;
   wssLeft += steps[pcll][pc];
   pcll = pc;
}
