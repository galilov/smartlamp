//
// Created by Alexander Galilov <alexander.galilov@gmail.com>
// on 11/21/19.
//

#ifndef FIRMWARE_EXPORTS_H
#define FIRMWARE_EXPORTS_H

#ifdef __cplusplus
extern "C" {
#endif

void showLedColor();

void turnLedColorOn();

void setLedColorTemperature(int temperature);

void setLedColorPower(int percent);

void hideLeds();

void showRainbow();

void showClock();

void showAdv();

#ifdef __cplusplus
}
#endif

#endif //FIRMWARE_EXPORTS_H
