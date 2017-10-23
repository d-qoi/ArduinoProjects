/*
 * This is an analog to digital converter that will just record values as fast as possible on all available pins of the ATTiny
 * 
 * It will be pollable via i2c
 * 
 * The ATTiny will act as a slave
 * 
 * This one uses the SPI buffer and clock, limiting the ADC pins to 3
 * 
 * I2C commands
 * 
 * address + specific read (0, 1, 2) => will return high then low.
 * 
 * address + 3 => Will return all bytes, 0(h, l), 1(h, l), 2(h, l)
 * 
 * 
 * This is assuming that the clock frequency is set to 8MHz, if lower or higher, will need to change ADC prescaler.
 */

// ADC, in free running mode, will let us change the selected pin after the conversion is completed.
// Selected pin will be delayed by 1 result, want to read it and start the next conversion asap
// extended the loop by 1 to make the math take less time.
const byte analogInPin[] = {0x80, 0x82, 0x83}; //pb2, ADC1, is also clock, 0x8x because enable bit is in mux reg.
byte pos = 1; //start at 1 because it will read 0 first.

//i2c address and instruction storage
const byte address = 0x60;
byte inst = 0;

// storing things as bytes, think it may be faster?
//                      0h    0l    2h    2l    3h    3l
byte analogResults[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};


void setup() {
  // ADC setup
  ADMUX = (1<<ADLAR); //and left align, set mux to point at pin 0;
  ADCSRA = (1<<ADEN) | (1<<ADSC) | (1<<ADATE);// start the conversions, enable auto trigger, free running mode is already set
  // no interrupts, changing pins is handled in main loop.
  ADCSRA |= (1<<ADPS2) | (1<<ADPS1); //set prescaler to 64x, will bring clock frequency down from 8MHz to 125KHz
  DIDR0 = (1<<ADC0D) | (1<<ADC2D) | (1<<ADC3D); // disabling digital reading for pins 3, 4, 5
  ADMUX |= analogInPin[pos]; // set the next analog in pin.

  //I2C setup
}

void loop() {
  // put your main code here, to run repeatedly:
  // ADC will give result of previous loop, due to the lag between setting mux while running, and using mux.
  while (!bitRead(ADCSRA, ADIF)); //loop till interrupt flag is enabled, signaling end of conversion.
  bitClear(ADCSRA, ADIF); // clear the bit.
  pos -= 1; // need to write to previous place holder.
  analogResults[pos<<1] = ADCH; // while adc2 is running, log adc0
  analogResults[pos<<1 + 1] = ADCL;
  // update pos to new position. (one after current itteration)
  pos = (pos+2)%3;
  ADMUX = analogInPin[pos]; // set new mux after logging adc0, set to adc3, as adc2 is running;
}

ISR(USI_START_vect) {
  
}


