# ArduinoProjects
Collection of projects put together for various microcontrollers. Mostly the Arduino Uno (atmega328) and the ATTiny85.

### OneWireDebug
* Debug library build for sending data via one pin to another micro controller, designed to be printed directly to serial.
* Does not use timers
* is not asyncronous. 
* Only uses header files for better compiler optimizations.

### ATTiny85_i2c_ADC
* Incomplete project to use ATTiny85 as a multi input ADC
* On hold till I figure out if I am using hardware to do i2c or software.
* One of the ADC pins uses the clock pin for SPI hardware.

### InternalTempFlasher
* Uses ATTIny85 Internal Temperature sensor to change the color of a dual-color LED
* Still WIP, as calibration doesn't work quite properly
  * calibration is done by pulling pin0 high and holding an ice cube to the ATTiny till the dual-color changes colors.
  
### VacuumCont
* controller for a vacuum pump, to disable it when it reaches a threshold, re-enable it when it reaches another
* uses LCD display in 4 bit mode
* needs most external hardware, comments at top of file list hardware.
* Built for Uno.
