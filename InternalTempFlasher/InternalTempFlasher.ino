/*
 * 
 * ATTiny Internal Temp Sensor to PWM dual-color LED.
 * 
 * Pulling pin 0 high will cause it to calibrate
 * 
 * pins 1 and 2 will control the LED
 * 
 * Analog Temp general
 * 
 * C      -40     025    085
 * F      -40     077    185
 * LSB    230     300    370
 * 
 * Calibration happens at defined value
 */

 #define SETGREEN bitSet(PORTB, PORTB1); bitClear(PORTB, PORTB2);
 #define SETRED bitSet(PORTB, PORTB2); bitClear(PORTB, PORTB1);

 const unsigned int calibVal = 274; // 32 F
 const unsigned int startAlertVal = 300;//313; // 95 F
 const unsigned int endAlertVal = 325; // 115 F 
 const byte IEFlags = (1<<OCIE1A) | (1<<TOIE1);
 
 int offset = 0;
 unsigned int tempVal;
 unsigned int oldTempVal = 0;
 
 byte unsigned btn = 0;
 byte unsigned nextCompVal = 0x00;

 //byte i = 0;
 
ISR(TIMER1_COMPA_vect) { // Green
  SETGREEN
}

ISR(TIMER1_OVF_vect) { // Red
  SETRED
}

void calibrate() {
  unsigned int twoOldTempVal = 0;
  while (bitRead(ADCSRA, ADSC)); //wait till conversion is completed;
  tempVal = ADC;
  SETRED
  while(abs(tempVal - calibVal) > 10) { // Hold ice on it till it is good.
    bitSet(ADCSRA, ADSC);
    PORTB ^= (1<<PORTB2) | (1<<PORTB1);
    while(bitRead(ADCSRA, ADSC));
    tempVal = ADC;
  }
  
  SETGREEN
  offset = calibVal - tempVal;//- (ADCH<<8 + ADCL);
  while (bitRead(EECR, EEPE));
  EECR = (0<<EEPM1) | (0<<EEPM0);
  EEAR = 0; // we will only use 0 for this.
  EEDR = offset;
  EECR |= (1<<EEMPE);
  EECR |= (1<<EEPE);
  
}

void restore() {
 while (bitRead(EECR, EEPE));
 EEAR = 0;
 EECR |= (1<<EERE);
 offset = EEDR; 
}

void setup() {
  // ADC setup
  ADMUX = (0<<REFS2) | (1<<REFS1) | (0<<REFS0) | 0xF; // Sets mux to 0b1111 which is temp in. and refrence to be 1.1v internal
  ADCSRA = (1<<ADEN) | (1<<ADSC) | (1<<ADPS2) | (0<<ADPS1) | (1<<ADPS0); // Probably don't want it to be this slow, but whatever. Currently set to 128, set ADPS0 to 0 to be 64

  // PORTB
  DDRB = (1<<DDB1) | (1<<DDB2) | (0<<DDB0); // 1 and 2 are output, 0 is input
  PORTB = (1<<PORTB0); // sets pullup for pin 0, toggling ports 1 and 2 will switch source and sink

  // Timer for things.
  TCCR1 = 0x0A; //Trying the A prescaler for now.IDK
  OCR1A = 0x00; //Compare at 0 for now.
  DDRB |= (1<<DDB4); // debugging
  if (bitRead(PINB, PINB0)) {
    PORTB |= (1<<PORTB4);
    calibrate();
    bitClear(PORTB, PORTB4);
   } else {
    restore();
   }
  
}

void loop() {
  while (bitRead(ADCSRA, ADSC));
  oldTempVal = tempVal;
  tempVal = ADC;
  tempVal += offset;    
  if (tempVal < startAlertVal) {
    TIMSK = 0;
    SETGREEN
  } else if (tempVal >= startAlertVal && tempVal <= endAlertVal && tempVal != oldTempVal) {
    TIMSK = IEFlags;
    OCR1A = (unsigned byte)((tempVal - startAlertVal)*(0xFF/((unsigned byte)(startAlertVal - endAlertVal))));
  } else {
    TIMSK = 0;
    SETRED
  }
  ADCSRA |= (1<<ADSC);
}




