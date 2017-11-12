#include "platform.h"

class OneWireDebugReceiver;

class OneWireDebugReceiver
{
public:
    OneWireDebugReceiver(uint8_t pin)
    {
        base = PIN_TO_BASEREG(pin);
        mask = PIN_TO_BITMASK(pin);
        pinMode(pin, INPUT);
        DIRECT_WRITE_LOW(base, mask);
    }

    ~OneWireDebugReceiver()
    {

    }

    void detect_timing()
    {
        while(!DIRECT_READ(base, mask)); // should be assembly, can't because trying to keep generic.
        do {
            clk_delay++;
        } while(DIRECT_READ(base, mask));
        clk_delay += clk_delay/2; // will be around middle of 1/0 value
    }

    uint8_t receive_8()
    {
        uint8_t received;
        for (uint8_t i = 0; i<8; i++) {
            clk_cnt = clk_delay;
            while(!DIRECT_READ(base, mask)); // wait till returns high
            do {
                if (!DIRECT_READ(base, mask)) { // line goes low for zero
                    received <<= 1;
                    break;
                }
                clk_cnt -= 1;
            } while (clk_cnt != 0);
            if (clk_cnt == 0) {
                received += 1;
                received <<= 1;
            }

        }
        return received;
    }
    uint16_t receive_16()
    {
        uint16_t received;
        for (uint8_t i = 0; i<16; i++) {
            clk_cnt = clk_delay;
            while(!DIRECT_READ(base, mask)); // wait till returns high
            do {
                if (!DIRECT_READ(base, mask)) { // line goes low for zero
                    received <<= 1;
                    break;
                }
                clk_cnt -= 1;
            } while (clk_cnt != 0);
            if (clk_cnt == 0) {
                received += 1;
                received <<= 1;
            }
        }
        return received;
    }
    uint32_t receive_32()
    {
        uint32_t received;
        for (uint8_t i = 0; i<32; i++) {
            clk_cnt = clk_delay;
            while(!DIRECT_READ(base, mask)); // wait till returns high
            do {
                if (!DIRECT_READ(base, mask)) { // line goes low for zero
                    received <<= 1;
                    break;
                }
                clk_cnt -= 1;
            } while (clk_cnt != 0);
            if (clk_cnt == 0) {
                received += 1;
                received <<= 1;
            }
        }
        return received;
    }

private:
    uint16_t clk_delay = 0;
    uint16_t clk_cnt = clk_delay;
    volatile uint8_t fake = false;
    volatile uint8_t *base;
    volatile uint8_t mask;

};
