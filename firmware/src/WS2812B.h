//
// Created by Alexander Galilov <alexander.galilov@gmail.com>
// on 11/10/19.
//

#ifndef SMART_LAMP_WS2812B_H
#define SMART_LAMP_WS2812B_H

#include <stdint.h>
#include <vector>
#include "freertos/FreeRTOS.h"
#include "driver/rmt.h"
#include "driver/gpio.h"

class WS2812B {
public:
    union RGB {
        struct __attribute__ ((packed)) {
            uint8_t r;
            uint8_t g;
            uint8_t b;
        };
        uint32_t rgb;
    };
private:
    typedef std::vector<rmt_item32_t> RMTItemVector;
    WS2812B() {
        _isInitialized = false;
    }
    ~WS2812B();
    WS2812B(const WS2812B &);

    WS2812B &operator=(WS2812B &);

    void _rmt_tx_init();
    RMTItemVector _items;
    gpio_num_t _pin;
    bool _isInitialized;
public:
    static WS2812B &getInstance() {
        // instance will be destroyed automatically on the program exit :)
        // yes, this programm will never exit :)
        static WS2812B instance;
        return instance;
    }

    void setLedCount(int nLeds, gpio_num_t pin);
    void setLedColor(int index, int r, int g, int b);
    void show();
};


#endif //SMART_LAMP_WS2812B_H
