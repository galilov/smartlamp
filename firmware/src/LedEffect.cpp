//
// Created by Alexander Galilov <alexander.galilov@gmail.com>
// on 11/3/19.
//
#include "mgos_system.h"
#include "LedEffect.h"
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <algorithm>
#include "WS2812B.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct ColorRGB {
    int r, g, b;

    ColorRGB(int r, int g, int b) : r(r), g(g), b(b) {}

    ColorRGB() : r(0), g(0), b(0) {}

    ColorRGB(const ColorRGB &src) : r(src.r), g(src.g), b(src.b) {}
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static const int gamma8[] = {
//        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
//        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1,
//        1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2,
//        2, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 5, 5, 5,
//        5, 6, 6, 6, 6, 7, 7, 7, 7, 8, 8, 8, 9, 9, 9, 10,
//        10, 10, 11, 11, 11, 12, 12, 13, 13, 13, 14, 14, 15, 15, 16, 16,
//        17, 17, 18, 18, 19, 19, 20, 20, 21, 21, 22, 22, 23, 24, 24, 25,
//        25, 26, 27, 27, 28, 29, 29, 30, 31, 32, 32, 33, 34, 35, 35, 36,
//        37, 38, 39, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 50,
//        51, 52, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 66, 67, 68,
//        69, 70, 72, 73, 74, 75, 77, 78, 79, 81, 82, 83, 85, 86, 87, 89,
//        90, 92, 93, 95, 96, 98, 99, 101, 102, 104, 105, 107, 109, 110, 112, 114,
//        115, 117, 119, 120, 122, 124, 126, 127, 129, 131, 133, 135, 137, 138, 140, 142,
//        144, 146, 148, 150, 152, 154, 156, 158, 160, 162, 164, 167, 169, 171, 173, 175,
//        177, 180, 182, 184, 186, 189, 191, 193, 196, 198, 200, 203, 205, 208, 210, 213,
//        215, 218, 220, 223, 225, 228, 231, 233, 236, 239, 241, 244, 247, 249, 252, 255

        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2,
        2, 2, 2, 3, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5,
        6, 6, 6, 7, 7, 7, 8, 8, 8, 9, 9, 9, 10, 10, 11, 11,
        11, 12, 12, 13, 13, 13, 14, 14, 15, 15, 16, 16, 17, 17, 18, 18,
        19, 19, 20, 21, 21, 22, 22, 23, 23, 24, 25, 25, 26, 27, 27, 28,
        29, 29, 30, 31, 31, 32, 33, 34, 34, 35, 36, 37, 37, 38, 39, 40,
        40, 41, 42, 43, 44, 45, 46, 46, 47, 48, 49, 50, 51, 52, 53, 54,
        55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70,
        71, 72, 73, 74, 76, 77, 78, 79, 80, 81, 83, 84, 85, 86, 88, 89,
        90, 91, 93, 94, 95, 96, 98, 99, 100, 102, 103, 104, 106, 107, 109, 110,
        111, 113, 114, 116, 117, 119, 120, 121, 123, 124, 126, 128, 129, 131, 132, 134,
        135, 137, 138, 140, 142, 143, 145, 146, 148, 150, 151, 153, 155, 157, 158, 160,
        162, 163, 165, 167, 169, 170, 172, 174, 176, 178, 179, 181, 183, 185, 187, 189,
        191, 193, 194, 196, 198, 200, 202, 204, 206, 208, 210, 212, 214, 216, 218, 220,
        222, 224, 227, 229, 231, 233, 235, 237, 239, 241, 244, 246, 248, 250, 252, 255
};


int clamp(int x, int min, int max) {
    if (x < min) { return min; }
    if (x > max) { return max; }
    return x;
}

ColorRGB colorTemperatureToRGB(int kelvin) {
    int temp = kelvin / 100;
    int red, green, blue;

    if (temp <= 66) {
        red = 255;
        green = temp;
        green = 99.4708025861 * log(green) - 161.1195681661;

        if (temp <= 19) {
            blue = 0;
        } else {
            blue = temp - 10;
            blue = 138.5177312231 * log(blue) - 305.0447927307;
        }
    } else {
        red = temp - 60;
        red = 329.698727446 * pow(red, -0.1332047592);

        green = temp - 60;
        green = 288.1221695283 * pow(green, -0.0755148492);

        blue = 255;
    }

    return ColorRGB(
            gamma8[clamp(red, 0, 255)],
            gamma8[clamp(green, 0, 255)],
            gamma8[clamp(blue, 0, 255)]);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool LedMask::showText(const char *text, const Font &font, const LedMatrixCoefficient &lmc, int x, int y) {
    auto glyphs = font.getGlyphs();
    if (glyphs.empty()) return false;
    int outputXPosition = x;
    int outputYPosition = y;
    bool res = false;
    for (char c = *text; c != '\0'; c = *++text) {
        auto it = glyphs.find(c);
        if (it == glyphs.end()) it = glyphs.find(' ');
        auto glyph = it->second;
        if (outputXPosition + glyph.getWidth() - 1 < 0 || outputYPosition + glyph.getHeight() - 1 < 0) {
            outputXPosition += glyph.getWidth();
            continue;
        }
        if (outputXPosition >= LED_PER_MATRIX_X * MATRIX_COUNT || outputYPosition >= LED_PER_MATRIX_Y)
            break;
        int startCol = outputXPosition >= 0 ? 0 : -outputXPosition;
        auto endCol = outputXPosition + glyph.getWidth() <= (LED_PER_MATRIX_X * MATRIX_COUNT)
                      ? glyph.getWidth() : (LED_PER_MATRIX_X * MATRIX_COUNT) - outputXPosition;
        auto startRow = outputYPosition >= 0 ? 0 : -outputYPosition;
        auto endRow = outputYPosition + glyph.getHeight() <= (LED_PER_MATRIX_Y)
                      ? glyph.getHeight() : (LED_PER_MATRIX_Y) - outputYPosition;
        for (auto row = startRow; row < endRow; row++) {
            for (auto col = startCol; col < endCol; col++) {
                if (glyph.at(row, col)) {
                    _coefficients[outputYPosition + row][outputXPosition + col] = lmc;
                }
            }
        }
        res = true;
        outputXPosition += glyph.getWidth();
    }
    return res;
}

void LedMask::fill(const LedMatrixCoefficient &lmc) {
    for (auto row = 0; row < (LED_PER_MATRIX_Y); row++) {
        for (auto col = 0; col < (LED_PER_MATRIX_X * MATRIX_COUNT); col++) {
            _coefficients[row][col] = lmc;
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
LedEffectHandler::LedEffectHandler() {
    _lcm.shouldUseCoefficient = true;
    _lcm.data.coefficient.r = 1.0f;
    _lcm.data.coefficient.g = 1.0f;
    _lcm.data.coefficient.b = 1.0f;
    _lmcBg.shouldUseCoefficient = true;
    _lmcBg.data.coefficient.r = 0.0f;
    _lmcBg.data.coefficient.g = 0.0f;
    _lmcBg.data.coefficient.b = 0.0f;
}

void LedEffectHandler::setLedColor(int x, int y, int r, int g, int b) {
    auto coefficient = _ledMask.at(y, x);
    if (coefficient.shouldUseColor) {
        r = coefficient.data.color.r;
        g = coefficient.data.color.g;
        b = coefficient.data.color.b;
    } else if (coefficient.shouldUseCoefficient) {
        r *= coefficient.data.coefficient.r;
        g *= coefficient.data.coefficient.g;
        b *= coefficient.data.coefficient.b;
    }
    WS2812B::getInstance().setLedColor(ledXY2Num(x, y), r, g, b);
}

void LedEffectHandler::showLeds() {
    WS2812B::getInstance().show();
}

void LedEffectHandler::hideLeds() {
    for (auto row = 0; row < getHeight(); row++) {
        for (auto col = 0; col < getWidth(); col++) {
            WS2812B::getInstance().setLedColor(ledXY2Num(col, row), 0, 0, 0);
        }
    }
    WS2812B::getInstance().show();
}

int LedEffectHandler::ledXY2Num(int x, int y) {
    static const int LED_PER_MATRIX = LED_PER_MATRIX_X * LED_PER_MATRIX_Y;
    int offset = x < LED_PER_MATRIX_X ? LED_PER_MATRIX : 0;
    x %= LED_PER_MATRIX_X;
    offset += y * LED_PER_MATRIX_X + (y & 1 ? x : (LED_PER_MATRIX_X - x - 1));
    return offset;
}

void LedEffectHandler::handleTimer(LedEffectAnimation &owner) {
    static bool isMaskBg = false;
    static uint32_t nFrame = 0;
    static char text[32];
    if (!_text.empty()) {
        getMask().fill(_lmcBg);
        isMaskBg = true;
        const char *textToShow;
        if (_text == "@clock@") {
            if (nFrame % 30 == 0) {
                time_t now = time(NULL) + 4 * 3600;
                struct tm *t = localtime(&now);
                strftime(text, sizeof(text) - 1, "%a, %H:%M:%S", t);
            }
            textToShow = text;
        } else {
            textToShow = _text.c_str();
        }
        if (!getMask().showText(textToShow, _font, _lcm, _textPosition--, 0)) {
            _textPosition = LED_PER_MATRIX_X * MATRIX_COUNT - 1;
        }
    } else {
        if (isMaskBg) {
            getMask().fill(_lcmDefault);
            isMaskBg = false;
        }
    }
    doStep(owner);
    nFrame++;
}

void LedEffectHandler::showText(const char *text) {
    if (!_font.load("VGA-8x16.font.txt")) {
        printf("FONT IS NOT FOUND\n");
        return;
    }
    _text = text;
    _textPosition = LED_PER_MATRIX_X * MATRIX_COUNT - 1;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

LedRainbow::LedRainbow() : LedEffectHandler() {
    j = 0;
}

void LedRainbow::doStep(LedEffectAnimation &owner) {
    const int ledCount = 32;
    uint8_t *c;
    uint16_t i;
    for (i = 0; i < ledCount; i++) {
        c = wheel(((i * 256 / ledCount) + j) & 255);
        for (int y = 0; y < 16; y++) {
            setLedColor((i + y) % ledCount, y, *c, *(c + 1), *(c + 2));
        }
    }
    showLeds();
    if (j < 256 * 5) j++; // 5 cycles of all colors on wheel
    else j = 0;
}

void LedRainbow::doStop(LedEffectAnimation &owner) {
    hideLeds();
}

uint8_t *LedRainbow::wheel(uint8_t pos) {
    static uint8_t c[3];

    if (pos < 85) {
        c[0] = pos * 3;
        c[1] = 255 - pos * 3;
        c[2] = 0;
    } else if (pos < 170) {
        pos -= 85;
        c[0] = 255 - pos * 3;
        c[1] = 0;
        c[2] = pos * 3;
    } else {
        pos -= 170;
        c[0] = 0;
        c[1] = pos * 3;
        c[2] = 255 - pos * 3;
    }

    return c;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
LedEffectAnimation::LedEffectAnimation() {
    _timerId = 0;
    WS2812B::getInstance().setLedCount(LED_COUNT, LED_PIN);
}

LedEffectAnimation::~LedEffectAnimation() {
    stop();
}

void LedEffectAnimation::start(LedEffectHandler *handler) {
    if (_timerId == 0) {
        _handler = handler;
        _timerId = mgos_set_timer(PERIOD, MGOS_TIMER_REPEAT, _timer_cb, this);
    } else {
        mgos_clear_timer(_timerId);
        mgos_msleep(60);
        _handler = handler;
        _timerId = mgos_set_timer(PERIOD, MGOS_TIMER_REPEAT, _timer_cb, this);
    }
}

void LedEffectAnimation::stop() {
    if (_timerId != 0) {
        mgos_clear_timer(_timerId);
        _timerId = 0;
        mgos_msleep(60);
        _handler->doStop(*this);
        _handler = nullptr;
        mgos_msleep(60);
    }
}

void LedEffectAnimation::_timer_cb(void *arg) {
    LedEffectAnimation *self = (LedEffectAnimation *) arg;
    if (self->_handler != 0) {
        self->_handler->handleTimer(*self);
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
LedFire2D::LedFire2D() : LedEffectHandler() {
    _offset = 0;
    memset(&_heat, 0, sizeof(_heat));
}

void LedFire2D::doStep(LedEffectAnimation &owner) {
    static const int w = LED_PER_MATRIX_X * MATRIX_COUNT;
    static const int h = LED_PER_MATRIX_Y;
    //randomize the bottom row of the fire buffer
    for (int x = 0; x < w; x++) _heat[h - 1][x] = abs(32768 + rand()) % 256;
    //do the fire calculations for every pixel, from top to bottom
    for (int y = 0; y < h - 1; y++) {
        for (int x = 0; x < w; x++) {
            _heat[y][x] =
                    ((_heat[(y + 1) % h][(x - 1 + w) % w]
                      + _heat[(y + 1) % h][(x) % w]
                      + _heat[(y + 1) % h][(x + 1) % w]
                      + _heat[(y + 2) % h][(x) % w])
                     * 32) / 131;
        }
    }

    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            _setPixelHeatColor(x, y, _heat[y][x]);
        }
    }
    showLeds();
}

void LedFire2D::doStop(LedEffectAnimation &owner) {
    hideLeds();
}

int LedFire2D::random(int min, int max) {
    return min + (int) ((double) rand() * (max - min) / (RAND_MAX + 1.0));
}

void LedFire2D::_setPixelHeatColor(int x, int y, int temperature) {
    uint8_t t192 = round((temperature / 255.0) * 191);

    // calculate ramp up from
    uint8_t heatramp = t192 & 0x3F; // 0..63
    heatramp <<= 2; // scale up to 0..252

    // figure out which third of the spectrum we're in:
    if (t192 > 0x80) {                     // hottest
        setLedColor(x, y, 255, 255, heatramp);
    } else if (t192 > 0x40) {             // middle
        setLedColor(x, y, 255, heatramp, 0);
    } else {                               // coolest
        setLedColor(x, y, heatramp, 0, 0);
    }
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

LedColor::LedColor() : LedEffectHandler() {
    _isTurningOn = false;
    _brightness = 0;
    _prepareColors(4000);
}

void LedColor::_prepareColors(int temperature) {
    ColorRGB color = colorTemperatureToRGB(temperature);
    _r = color.r;
    _g = color.g;
    _b = color.b;
}

void LedColor::doStep(LedEffectAnimation &owner) {
    if (_isTurningOn) {
        if (_brightness >= _max) {
            _brightness = _max;
            return;
        }
    } else {
        if (_brightness <= 0) {
            _brightness = 0;
            return;
        }
    }
    int r = (int) (_brightness * _r / 100.0);
    int g = (int) (_brightness * _g / 100.0);
    int b = (int) (_brightness * _b / 100.0);
    for (auto row = 0; row < getHeight(); row++) {
        for (auto col = 0; col < getWidth(); col++) {
            setLedColor(col, row, r, g, b);
        }
    }
    _brightness += ((_isTurningOn ? 1 : -1) * _d);

    showLeds();
}

void LedColor::doStop(LedEffectAnimation &owner) {
    _isTurningOn = false;
    _brightness = 0;
    hideLeds();
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
