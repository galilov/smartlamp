//
// Created by Alexander Galilov <alexander.galilov@gmail.com>
// on 11/3/19.
//

#ifndef SMART_LAMP_LEDEFFECTS_H
#define SMART_LAMP_LEDEFFECTS_H

#include "mgos.h"
#include <stdint.h>
#include <stdio.h>
#include <string>
#include "driver/gpio.h"
#include "Font.h"

#define LED_PIN  GPIO_NUM_32
const int LED_PER_MATRIX_X = 16; //16x16
const int LED_PER_MATRIX_Y = 16; //16x16
const int MATRIX_COUNT = 2;
const int LED_COUNT = 512;
const int PERIOD = 40; // milliseconds for timer

struct LedMatrixCoefficient {
    union {
        struct {
            float r;
            float g;
            float b;
        } coefficient;
        struct {
            int r;
            int g;
            int b;
        } color;
    } data;
    bool shouldUseCoefficient;
    bool shouldUseColor;

    LedMatrixCoefficient() : shouldUseCoefficient(false), shouldUseColor(false) {
        //
    }
};

class LedEffectAnimation;

class LedMask {
    LedMatrixCoefficient _coefficients[LED_PER_MATRIX_Y][LED_PER_MATRIX_X * MATRIX_COUNT];
public:
    inline LedMatrixCoefficient &at(int row, int col) {
        return _coefficients[row][col];
    }

    bool showText(const char *text, const Font &font, const LedMatrixCoefficient &lmc, int x, int y);

    void fill(const LedMatrixCoefficient &lmc);
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class LedEffectHandler {
    LedMask _ledMask;
    std::string _text;
    int _textPosition;
    Font _font;
    LedMatrixCoefficient _lcm, _lmcBg, _lcmDefault;
public:
    virtual void doStep(LedEffectAnimation& owner) = 0;
    virtual void doStop(LedEffectAnimation& owner) = 0;
    LedEffectHandler();
    virtual ~LedEffectHandler() {};

    void setLedColor(int x, int y, int r, int g, int b);

    void showLeds();
    void hideLeds();
    int getWidth() { return LED_PER_MATRIX_X * MATRIX_COUNT; }

    int getHeight() { return LED_PER_MATRIX_Y; }

    LedMask &getMask() { return _ledMask; }
    void handleTimer(LedEffectAnimation& owner);
    void showText(const char* text);
private:
    int ledXY2Num(int x, int y);
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class LedColor : public LedEffectHandler {
public:
    LedColor();

    void doStep(LedEffectAnimation& owner);
    void doStop(LedEffectAnimation& owner);

    void turnOn() {
        _isTurningOn = true;
        _brightness = 0;
    }

    void setColorTemperature(int temperature) {
        _prepareColors(temperature);
    }

    void setPower(int percent) {
        if (percent < 0) _max = 0;
        else if (percent > 100) _max = 100;
        else _max = percent;
    }

private:
    void _prepareColors(int temperature);

private:
    const int _d = 2;
    volatile int _max = 50;

    int _brightness, _r, _g, _b;
    volatile bool _isTurningOn;

};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class LedFire2D : public LedEffectHandler {
private:
    uint8_t _heat[LED_PER_MATRIX_Y][LED_PER_MATRIX_Y * MATRIX_COUNT];
    int _offset;
public:
    LedFire2D();

    void doStep(LedEffectAnimation& owner);
    void doStop(LedEffectAnimation& owner);

private:
    void _setPixelHeatColor(int x, int y, int temperature);

    int random(int min, int max);
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class LedRainbow : public LedEffectHandler {
public:
    LedRainbow();

    void doStep(LedEffectAnimation& owner);
    void doStop(LedEffectAnimation& owner);

private:
    uint16_t j;

    uint8_t *wheel(uint8_t pos);
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class LedEffectAnimation {
public:
private:
    volatile mgos_timer_id _timerId;
    LedEffectHandler *_handler;
    LedMask _mask;
public:
    void start(LedEffectHandler *handler);

    void stop();

    LedEffectAnimation();

    ~LedEffectAnimation();

private:
    LedEffectAnimation(const LedEffectAnimation &);

    LedEffectAnimation &operator=(LedEffectAnimation &);

    static void _timer_cb(void *arg);
};
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif //SMART_LAMP_LEDEFFECTS_H
