//
// Created by Richard on 09/01/2017.
//

#include "Transition.h"
//#include <iostream>

Transition::Transition(long stepsPerSecond, TransitionValueChangedCallback callback) {
    setDefaultStepsPerSecond(stepsPerSecond);
    this->callback = callback;
}

void Transition::init(long initialValue) {
    this->currentValue = initialValue;
    this->targetValue = initialValue;
}

void Transition::setTarget(long newTarget) {

    setTargetAndSteps(newTarget, defaultStepsPerSecond);
}

void Transition::setTarget(long newTarget, unsigned long durationMilliseconds) {

    long valueDelta = abs(newTarget - getValue());
    long stepsPerSecond = (valueDelta * 1000) / durationMilliseconds;

    setTargetAndSteps(newTarget, stepsPerSecond);
}

void Transition::setTargetAndSteps(long newTarget, long stepsPerSecond){

    this->targetValue = newTarget;
    this->transitionStartValue = getValue();
    this->currentStepsPerSecond = stepsPerSecond;

    unsigned long now = millis();

    this->transitionStartTime = now;
    this->lastChangeTimeStamp = now;
    this->nextChangeTimeStamp = now + this->millisUntilNextChange;

    updateStepDelay();
}

void Transition::setDefaultStepsPerSecond(long stepsPerSecond) {

    this->defaultStepsPerSecond = stepsPerSecond;
    this->currentStepsPerSecond = stepsPerSecond;

    updateStepDelay();
}

void Transition::setValue(long newValue) {

    // Only change the value and call the callback if the value has changed
    if(this->currentValue != newValue){
        this->currentValue = newValue;
        if(this->callback){
            this->callback(*this, newValue);
        }
    }
}

long Transition::getValue() {
    return this->currentValue;
}

long Transition::getTarget() {
    return this->targetValue;
}

void Transition::loop(){

    // Only bother calculating the next value if we haven't already reach the target...
    if(getValue() != getTarget()){
        // .. and enough time has passed
        unsigned long now = millis();
        unsigned long timeDelta = now - lastChangeTimeStamp;

//        std::cout << "[LOOP] now=" << now << ": lastChangeTimeStamp=" << lastChangeTimeStamp << "; timeDelta=" << timeDelta << ": millisUntilNextChange=" << millisUntilNextChange << std::endl;

        if(timeDelta >= millisUntilNextChange) {
//            std::cout << "-----DO CALCULATION" << std::endl;
            unsigned long transitionDelta = now - transitionStartTime;     // TODO: Overflow risk?
            long steps = currentStepsPerSecond * transitionDelta / 1000;
            if (steps > 0) {
                long newValue;
                if(getDirection() == Transition::DirectionAscending){
                    newValue = transitionStartValue + steps;
                } else {
                    newValue = transitionStartValue - steps;
                }
                setValue(newValue);
                lastChangeTimeStamp = transitionStartTime + (steps * millisUntilNextChange);
                nextChangeTimeStamp += millisUntilNextChange;
            }
        }
    }
}

// TODO: loop not called for a long time - clamp value to getTarget()

bool Transition::is(const Transition &compareObject){

    // Compare pointers
    return (this == &compareObject);
}

void Transition::updateStepDelay() {
    this->millisUntilNextChange = 1000 / this->currentStepsPerSecond;
}

int8_t Transition::getDirection() {

    if(getValue() > getTarget()){
        return DirectionDescending;
    } else if (getValue() < getTarget()){
        return DirectionAscending;
    } else {
        return DirectionStationary;
    }
}