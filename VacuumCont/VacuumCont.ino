#include <LiquidCrystal.h>

/*
Controller for a vacuum pump

Requires a pot, and a pressure sensor as inputs

has 3 outputs
Relay, Solinoid, LCD

Reads the pressure in interrupt cycle
tracks the pressure with lastValue

Will auto set maxVal when pressure stabalizes within delta

will turn on/off pump when pressure outside of range

will keep pump off when pressure stabalizes below minval.
*/

// LCD pins
#define LCDReadSelect 12
#define LCDEnable 11
#define LCDDB7 2
#define LCDDB6 3
#define LCDDB5 4
#define LCDDB4 5

// INPUTS
#define POTPIN A0
#define PRESSUREPIN A1

// OUTPUTS
#define RELAYPIN A2
#define SOLINOIDPIN A3 //yes

// Constants
#define histSize 32 //no more than 255, due to byte for nextPos

const unsigned int timer1Start = 39065; // half sampling frequency, max of 65535, currently set for 5 seconds.

const double conversionFactor = 1.0; 
const double delta = 3.0; // how far apart can the hist values be for it to be within range?
const double diff = 0.5; // how far below the final value, if hist is set, for max value to be.

double vacuumhist[histSize];
double* lastValue;
double maxVal;
double minVal;
double histVal;

byte nextPos = 0; //for vacuumhist
byte i = 0; //temp value

bool contLoop = 0; // for interrupt
bool pumpOn;
bool err;

unsigned long mildelay;
#define sec 1000
  
LiquidCrystal lcd(LCDReadSelect, LCDEnable, LCDDB4, LCDDB5, LCDDB6, LCDDB7);

void delay1sec() {
  mildelay = millis() + sec;
  while(mildelay > millis());
}

void display() {
  if (!err) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(minVal);
    lcd.setCursor(8, 0);
    lcd.print(*lastValue);
    lcd.setCursor(0, 1);
    lcd.print(maxVal);
    lcd.setCursor(8, 1);
    lcd.print(nextPos);
  } else {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Error");
    lcd.setCursor(0, 1);
    lcd.print("Hist Val Low");
  }
}


void toggleOn() {
  digitalWrite(RELAYPIN, HIGH);
  delay1sec();
  digitalWrite(SOLINOIDPIN, HIGH);
}


void toggleOff() {
  digitalWrite(SOLINOIDPIN, LOW);
  delay1sec();
  digitalWrite(RELAYPIN, LOW);
}

void toggle() {
  if (!err) {
  	if (pumpOn) {
  		toggleOff();
      pumpOn = false;
  	} else {
  		toggleOn();
      pumpOn = true;
  	}
  } else {
    toggleOff();
  }
}

boolean checkToggle() {
  for(i = 1; i<histSize; i++) {
    if (abs(vacuumhist[i] - vacuumhist[i-1]) > delta) {
      break;
    }
  }
  
  if (i >= histSize) {
    histVal = *lastValue;
    if (pumpOn) {
      maxVal = histVal - diff;
    }
    if (histVal <= minVal && pumpOn) {
      err = true;
      return true;
    }
  }
      
	
 
	if (pumpOn) {
		if (maxVal > 0 && *lastValue > maxVal)
			return true;
	} else {
		if (*lastValue < minVal)
			return true;
	}
	return false;
}


void setup()
{
	pinMode(LED_BUILTIN, OUTPUT);
	pinMode(POTPIN, INPUT);
	pinMode(PRESSUREPIN, INPUT);
	pinMode(RELAYPIN, OUTPUT);
	pinMode(SOLINOIDPIN, OUTPUT);
 
	for(i = 0; i<histSize; i++) {
		vacuumhist[i] = -1;
	}
	lastValue = &vacuumhist[0];
  pumpOn = true;
  toggleOn();
 
	lcd.begin(16, 2);

  noInterrupts();
  TCCR1A = 0; //comp out & waveform gen, lower bits
  TCCR1B = 0; //input noise, prescaler, waveform gen upper
  TCCR1C = 0; // force output compare.
  TCCR1B |= (1<<WGM12); //ctc
  TCCR1B |= (1<<CS12) | (1<<CS10); //1024 prescaler.
  OCR1A = timer1Start; // 5 seconds comparison.
  TIMSK1 = 0; // reset all other things using this.
  TIMSK1 |= (1<<OCIE1A); //enable interrupt
  TCNT1 = 0; //reset timer.
  interrupts();
}


void loop()
{
  minVal = analogRead(POTPIN)*conversionFactor;
  if (checkToggle()) {
  	toggle();
  }
  display();
  delay1sec();
}

ISR(TIMER1_COMPA_vect)
{
  if (!contLoop) {
    vacuumhist[nextPos] = analogRead(PRESSUREPIN)*conversionFactor;
    lastValue = &vacuumhist[nextPos];
    nextPos = (nextPos+1)%histSize;
    digitalWrite(LED_BUILTIN, HIGH);
  } else {
    digitalWrite(LED_BUILTIN, LOW);
  }
  contLoop = !contLoop;
}

