//
// Created by Richard on 09/01/2017.
//
#include <Arduino.h>

#ifndef TRANSITION_TRANSITION_H_H
#define TRANSITION_TRANSITION_H_H

// Forward reference
class Transition;

// Transition callback
typedef void (*TransitionValueChangedCallback)(Transition&, long);

class Transition{

private:
    unsigned long lastChangeTimeStamp;
    unsigned long nextChangeTimeStamp;
    unsigned long millisUntilNextChange;

    long currentValue;
    long targetValue;
    long transitionStartValue;
    unsigned long transitionStartTime;

    unsigned long defaultStepsPerSecond;
    TransitionValueChangedCallback callback;

    unsigned long currentStepsPerSecond;

    void updateStepDelay();

public:
    Transition(long stepsPerSecond, TransitionValueChangedCallback callback);
    void init(long initialValue);
    void setTarget(long newTarget);
    void setTarget(long newTarget, unsigned long durationMilliseconds);
    void setTargetAndSteps(long newTarget, long stepsPerSecond);
    void setDefaultStepsPerSecond(long stepsPerSecond);
    void setValue(long newValue);
    long getValue();
    long getTarget();
    int8_t getDirection();
    void loop();

    bool is(const Transition& compareObject);

    static const int8_t DirectionAscending = 1;
    static const int8_t DirectionDescending = -1;
    static const int8_t DirectionStationary = 0;

};

#endif //TRANSITION_TRANSITION_H_H
