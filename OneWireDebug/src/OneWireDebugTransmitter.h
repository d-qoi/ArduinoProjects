#define ONEWIRE_DEBUG_H
#include "platform.h"

// How the receive works
/*

Detect timing will record the length of the start condition.

When it is time to read the value, a start condition will be sent

IF the line goes low after the start condition, before twice the start condition, it is a zero
IF the line remains high after the start cond, it is a 1

Everything in header file for optimization.

*/

class OneWireDebugTransmitter;

class OneWireDebugTransmitter
{
public:
    OneWireDebugTransmitter(uint8_t pin, uint8_t d)
    {
        base = PIN_TO_BASEREG(pin);
        mask = PIN_TO_BITMASK(pin);
        pinMode(pin, OUTPUT);
        DIRECT_WRITE_LOW(base, mask);
        delay = d;

    }

    OneWireDebugTransmitter(uint8_t pin)
    {
        base = PIN_TO_BASEREG(pin);
        mask = PIN_TO_BITMASK(pin);
        pinMode(pin, OUTPUT);
        DIRECT_WRITE_LOW(base, mask);
        delay = 8;
    }

    ~OneWireDebugTransmitter()
    {

    }

    void send_timing() {
        DIRECT_WRITE_LOW(base, mask);
        uint8_t del = delay;
        DIRECT_WRITE_HIGH(base, mask);
        while(del-- != 0);
        DIRECT_WRITE_LOW(base, mask);
    }

    void send_8(uint8_t toSend)
    {
        uint8_t del;
        for(uint8_t i = 0; i < 8; i++) {
            del = delay;
            DIRECT_WRITE_HIGH(base, mask);
            while(del-- != 0);
            if ((toSend & 0x1) == 0)
                DIRECT_WRITE_LOW(base, mask);
            del = delay;
            while(del-- != 0);
            DIRECT_WRITE_LOW(base, mask);
            del = delay;
            while(del-- != 0);
            toSend >>= 1;
        }

    }

    void send_16(uint16_t toSend)
    {
        uint8_t del;
        for(uint8_t i = 0; i < 16; i++) {
            del = delay;
            DIRECT_WRITE_HIGH(base, mask);
            while(del-- != 0);
            if ((toSend & 0x1) == 0)
                DIRECT_WRITE_LOW(base, mask);
            del = delay;
            while(del-- != 0);
            DIRECT_WRITE_LOW(base, mask);
            del = delay;
            while(del-- != 0);
            toSend >>= 1;
        }
    }

    void send_32(uint32_t toSend)
    {
        uint8_t del;
        for(uint8_t i = 0; i < 32; i++) {
            del = delay;
            DIRECT_WRITE_HIGH(base, mask);
            while(del-- != 0);
            if ((toSend & 0x1) == 0)
                DIRECT_WRITE_LOW(base, mask);
            del = delay;
            while(del-- != 0);
            DIRECT_WRITE_LOW(base, mask);
            del = delay;
            while(del-- != 0);
            toSend >>= 1;
        }
    }
private:
    volatile uint8_t* base;
    volatile uint8_t mask;
    uint8_t delay;
};
