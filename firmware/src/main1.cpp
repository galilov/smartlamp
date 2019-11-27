//
// Created by Alexander Galilov <alexander.galilov@gmail.com>
// on 11/3/19.
//

#include "LedEffect.h"
#include "exports.h"


LedColor ledColor;
LedEffectAnimation effect;
//LedFire2D fire2d;
LedRainbow rainbow;

void showRainbow() {
    //rainbow.showText("GDG DevFest Astrakhan 2019. DIY AGalilov.");
    //hideLeds();
    rainbow.showText("");
    effect.start(&rainbow);
}

void turnLedColorOn() {
    ledColor.turnOn();
}

void setLedColorTemperature(int temperature) {
    ledColor.setColorTemperature(temperature);
}

void setLedColorPower(int percent) {
    ledColor.setPower(percent);
}

void showLedColor() {
    //ledColor.showText("GDG DevFest Astrakhan 2019. DIY AGalilov.");
    //hideLeds();
    effect.start(&ledColor);
}

void hideLeds() {
    effect.stop();
}

void showClock() {
    rainbow.showText("@clock@");
    effect.start(&rainbow);
}

void showAdv() {
    //hideLeds();
    rainbow.showText("GDG DevFest Astrakhan 2019. DIY AGalilov.");
    effect.start(&rainbow);
}