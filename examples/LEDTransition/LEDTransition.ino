//
// Created by Richard on 12/01/2017.
//

// To build example: pio ci --lib="." src examples\LEDTransition\LEDTransition.ino --board=uno

#include <Arduino.h>
#include <Transition.h>

const int ledPin = 13;
const int ledDirectionAscendingPin = 6;
const int ledDirectionDescendingPin = 7;

const int ledValueMin = 0;
const int ledValueMax = 255;

void onLedChange(Transition& transition, long newValue);

Transition ledTransition(100, onLedChange);

void setup(){

    pinMode(ledPin, OUTPUT);
    pinMode(ledDirectionAscendingPin, OUTPUT);
    pinMode(ledDirectionDescendingPin, OUTPUT);

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

    // Turn off both direction LEDs
    digitalWrite(ledDirectionAscendingPin, LOW);
    digitalWrite(ledDirectionDescendingPin, LOW);

    if(transition.getDirection() == Transition::DirectionAscending){
        digitalWrite(ledDirectionAscendingPin, HIGH);
    } else {
        digitalWrite(ledDirectionDescendingPin, HIGH);
    }

    digitalWrite(ledPin, newValue);
}