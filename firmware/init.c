#include "stdinc.h"

void init(void)
{
   // PA0 - Battery

   // PA1

   // PA2 - WS2812 Led (Battery indicator)
   DDRA |= (1 << PA2);

   // PA3

   // PA4 - MaxSonar Trigger
   DDRA |= (1 << PA4);

   // PA5

   // PA6

   // PA7

   // PB0 - M1 Enc A

   // PB1 - M1 Enc B

   // PB2 - Gyro SS
   DDRB  |= (1 << PB2);
   PORTB |= (1 << PB2);        // ! Gyro Select

   // PB3 - Acc SS
   DDRB  |= (1 << PB3);
   PORTB |= (1 << PB3);        // ! Acc Select

   // PB4 - SS
   DDRB |= (1 << PB4);         // has to be output or high for SPI master mode

   // PB5 - MOSI
   DDRB |= (1 << PB5);

   // PB6 - MISO

   // PB7 - SCK
   DDRB |= (1 << PB7);

   // PC0 - M2 Enc A

   // PC1 - M2 Enc B

   // PC2 - M2 B
   DDRC |= (1 << PC2);

   // PC3 - M2 A
   DDRC |= (1 << PC3);

   // PC4 - M1 B
   DDRC |= (1 << PC4);

   // PC5 - M1 A
   DDRC |= (1 << PC5);

   // PC6

   // PC7

   // PD0 - Bluetooth RX

   // PD1 - Bluetooth TX

   // PD2 - MaxSonar RX

   // PD3

   // PD4 - Speaker (enabled by soundplayer when needed)

   // PD5

   // PD6 - M1 PWM
   DDRD |= (1 << PD6);

   // PD7 - M2 PWM
   DDRD |= (1 << PD7);

   // ADC
   ADMUX  =  (1 << REFS0);     // AVCC with external cap
   ADCSRA =  (1 << ADEN)       // ADC Enabled
          |  (1 << ADATE)      // ADC Auto Trigger
          |  (1 << ADPS0)
          |  (1 << ADPS1)
          |  (1 << ADPS2);     // prescalar 128
   ADCSRA |= (1 << ADSC);      // ADC start conversion

   // SPI
   SPCR = (1 << SPE)           // SPI enable
        | (1 << MSTR)          // SPI Master mode
        | (1 << CPOL)
        | (1 << CPHA);

   SPSR = (1 << SPI2X);        // 10 MHz SPI clock

   // PCINT
   PCICR  = (1 << PCIE1)
          | (1 << PCIE2);      // pin change interrupts for ports B and C
   PCMSK1 = (1 << PCINT8)      // pin change interrupt for PB0
          | (1 << PCINT9);     // pin change interrupt for PB1
   PCMSK2 = (1 << PCINT16)     // pin change interrupt for PC0
          | (1 << PCINT17);    // pin change interrupt for PC1

   // UART0 (already initialized by bootloader)
   UCSR0B |= (1 << RXCIE0);    // RX Complete Interrupt
   
   // UART1
   UCSR1C = (1 << UCSZ11)
          | (1 << UCSZ10);     // 8bit
   UBRR1  = 129;               // 9600 baud
   UCSR1B = (1 << RXEN1)
          | (1 << RXCIE1);

   // Timer 0 - Mainloop
   TCCR0A = (1 << WGM01);      // CTC mode,
   TCCR0B = (1 << CS01);
   OCR0A  = 50 - 1;            // Prescalar 8, TOP 50 -> 50000 Hz

   TIMSK0 = (1 << OCIE0A);     // Output compare interrupt enable

   // Timer 1 - frequency generation on OC1A (Speaker)
   TCCR1A = (1 << COM1A0);     // toggle on compare match
   TCCR1B = (1 << WGM12)       // CTC with OCR1A
          | (1 << CS11);       // prescalar 8

   // Timer 2 - 2x PWM
   TCCR2A = (1 << COM2A1)
          | (1 << COM2B1)
          | (1 << WGM20);      // non-inverting phase-correct PWM mode
   TCCR2B = (1 << CS20);       // Prescalar 1 -> ~80khz PWM

   // Ram Vars
   attitude.voltage =  14800;          // initialize battery voltage to lipo nominal
   actuator.tempDisabled = true;
   actuator.initCountdown = 50;
   
   sensorOffsets.angleRate = 0;    // initialize gyro offset (self learning)
   
   controlPara.angleBySpeed = -200;
   controlPara.angleByPosition = -300;
   controlPara.powerByAngleDiv = 180;
   controlPara.powerByAngleRateDiv = 120;
   
   autonomous = 1;
   
   // Enable Interrupts
   sei();
}
