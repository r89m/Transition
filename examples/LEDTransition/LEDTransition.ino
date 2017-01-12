//
// Created by Richard on 12/01/2017.
//

// To build example: pio ci --lib "." src examples\LEDTransition\LEDTransition.ino --board=uno

#include <Arduino.h>
#include <Transition.h>

const int ledPin = 13;

const int ledValueMin = 0;
const int ledValueMax = 255;

void onLedChange(Transition& transition, long newValue);

Transition ledTransition(100, onLedChange);

void setup(){

    pinMode(ledPin, OUTPUT);

    ledTransition.init(0);
}

void loop(){

    if(ledTransition.getValue() == ledValueMin){
        ledTransition.setTarget(ledValueMax);
    } else if (ledTransition.getValue() == ledValueMax){
        ledTransition.setTarget(ledValueMin);
    }

    ledTransition.loop();
}

void onLedChange(Transition& transition, long newValue){

    digitalWrite(ledPin, newValue);
}