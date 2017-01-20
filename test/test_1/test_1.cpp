#include <iostream>
#include <unity.h>
#include <Transition.h>
#include <AxxTest.h>

#ifdef UNIT_TEST

bool callback_called;
Transition* callback_transition;
int callback_value;
unsigned int callback_count = 0;

void onTransitionValueChange(Transition& transition, long newValue){
    callback_called = true;
    callback_transition = &transition;
    callback_value = newValue;
    callback_count++;
}

Transition transition(10, onTransitionValueChange);
Transition transition2(10, onTransitionValueChange);

void reset_callback(){

    // Reset callback function state
    callback_called = false;
    callback_transition = new Transition(10000, onTransitionValueChange);
    callback_value = 0;
}

void setUp(void) {

    // Initialise Transition
    transition.init(50);

    // Reset callback variables
    reset_callback();

    // Reset callback_count
    callback_count = 0;

    AxxTest::initialiseArduinoMock();
}

void test_init(void){

    TEST_ASSERT_EQUAL(50, transition.getValue());
    TEST_ASSERT_EQUAL(false, callback_called);
    TEST_ASSERT_EQUAL(0, callback_value);
}

void test_setAndGetValue(void){

    transition.setValue(100);
    TEST_ASSERT_EQUAL(100, transition.getValue());
}

void test_setValueAndCheckCallbackIfDifferentValue(void){

    transition.setValue(100);
    TEST_ASSERT_EQUAL(true, callback_called);
    TEST_ASSERT_EQUAL(100, callback_value);
}

void test_setValueAndNoCallbackIfSameValue(void){

    transition.setValue(50);
    TEST_ASSERT_EQUAL(false, callback_called);
    TEST_ASSERT_EQUAL(0, callback_value);
}

void test_setAndGetTarget(void){

    transition.init(50);
    transition.setTarget(94);
    TEST_ASSERT_EQUAL(94, transition.getTarget());
}

void test_stepTowardsValue(){

    // Ensure that setting the target takes place at T=47ms
    When(Method(ArduinoHardwareMock, millis)).AlwaysReturn(47);

    transition.setTarget(100);

    // Now pass in some later times
    When(Method(ArduinoHardwareMock, millis)).Return(58, 97, 139, 167, 246, 247, 248, 300, 4304, 5200).AlwaysReturn(5500);

    transition.loop();
    TEST_ASSERT_EQUAL(false, callback_called);

    transition.loop();
    TEST_ASSERT_EQUAL(false, callback_called);

    transition.loop();
    TEST_ASSERT_EQUAL(false, callback_called);

    transition.loop();
    TEST_ASSERT_EQUAL(true, callback_called);
    TEST_ASSERT_EQUAL(51, callback_value);
    reset_callback();

    transition.loop();
    TEST_ASSERT_EQUAL(false, callback_called);

    transition.loop();
    TEST_ASSERT_EQUAL(true, callback_called);
    TEST_ASSERT_EQUAL(52, callback_value);
    reset_callback();

    transition.loop();
    TEST_ASSERT_EQUAL(false, callback_called);

    transition.loop();
    TEST_ASSERT_EQUAL(false, callback_called);

    transition.loop();
    TEST_ASSERT_EQUAL(true, callback_called);
    TEST_ASSERT_EQUAL(92, callback_value);
    reset_callback();

    transition.loop();
    TEST_ASSERT_EQUAL(true, callback_called);
    TEST_ASSERT_EQUAL(101, callback_value);         // TODO: This is wrong! Should be 100
    reset_callback();

    transition.loop();
}

void test_stepTowardsValueDescending(){

    // Ensure that setting the target takes place at T=47ms
    When(Method(ArduinoHardwareMock, millis)).AlwaysReturn(47);

    transition.setTarget(0);

    // Now pass in some later times
    When(Method(ArduinoHardwareMock, millis)).Return(58, 97, 139, 167, 246, 247, 248, 300, 4304, 5200).AlwaysReturn(5500);

    transition.loop();
    TEST_ASSERT_EQUAL(false, callback_called);

    transition.loop();
    TEST_ASSERT_EQUAL(false, callback_called);

    transition.loop();
    TEST_ASSERT_EQUAL(false, callback_called);

    transition.loop();
    TEST_ASSERT_EQUAL(true, callback_called);
    TEST_ASSERT_EQUAL(49, callback_value);
    reset_callback();

    transition.loop();
    TEST_ASSERT_EQUAL(false, callback_called);

    transition.loop();
    TEST_ASSERT_EQUAL(true, callback_called);
    TEST_ASSERT_EQUAL(48, callback_value);
    reset_callback();

    transition.loop();
    TEST_ASSERT_EQUAL(false, callback_called);

    transition.loop();
    TEST_ASSERT_EQUAL(false, callback_called);

    transition.loop();
    TEST_ASSERT_EQUAL(true, callback_called);
    TEST_ASSERT_EQUAL(8, callback_value);
    reset_callback();

    transition.loop();
    TEST_ASSERT_EQUAL(true, callback_called);
    TEST_ASSERT_EQUAL(-1, callback_value);      //TODO: This should equal 0
    reset_callback();

    transition.loop();
}

void test_stepTowardsValueTriggerEveryStep(){

    // Ensure that setting the target takes place at T=0ms
    When(Method(ArduinoHardwareMock, millis)).AlwaysReturn(0);

    transition.setTarget(100);

    for(int i = 0; i < 5100; i++){
        When(Method(ArduinoHardwareMock, millis)).AlwaysReturn(i);
        transition.loop();
    }

    TEST_ASSERT_EQUAL(50, callback_count);
    TEST_ASSERT_EQUAL(100, callback_value);
}

void test_stepTowardsValueLongDelayBetweenCalls(){

    // Reset callback_count
    callback_count = 0;

    // Ensure that setting the target takes place at T=0ms
    When(Method(ArduinoHardwareMock, millis)).AlwaysReturn(0);

    transition.setTarget(100);

    for(int i = 0; i < 5500; i+=500){
        When(Method(ArduinoHardwareMock, millis)).AlwaysReturn(i);
        transition.loop();
    }

    TEST_ASSERT_EQUAL(10, callback_count);
    TEST_ASSERT_EQUAL(100, callback_value);
}

void test_stepTowardsValueCustomDuration(){

    When(Method(ArduinoHardwareMock, millis)).AlwaysReturn(0);

    // Transition from 50 to 70 over 4 seconds
    transition.setTarget(70, 4000);

    for(int i = 0; i < 20; i++){
        When(Method(ArduinoHardwareMock, millis)).AlwaysReturn(i * 250);
        transition.loop();
    }

    TEST_ASSERT_EQUAL(16, callback_count);
    TEST_ASSERT_EQUAL(70, callback_value);
}

void test_getDirection(){

    // Check that it starts stationary
    TEST_ASSERT_EQUAL(Transition::DirectionStationary, transition.getDirection());

    transition.setTarget(100);
    // Check that it is now ascending
    TEST_ASSERT_EQUAL(Transition::DirectionAscending, transition.getDirection());

    transition.setTarget(40);
    // Check that it is now descending
    TEST_ASSERT_EQUAL(Transition::DirectionDescending, transition.getDirection());
}

void test_equality(){

    When(Method(ArduinoHardwareMock, millis)).AlwaysReturn(100);

    transition.setTarget(100);
    transition2.setTarget(20);

    When(Method(ArduinoHardwareMock, millis)).AlwaysReturn(200);

    transition.loop();

    TEST_ASSERT_TRUE(transition.is(*callback_transition));
    TEST_ASSERT_FALSE(transition2.is(*callback_transition));

    transition2.loop();

    TEST_ASSERT_TRUE(transition2.is(*callback_transition));
    TEST_ASSERT_FALSE(transition.is(*callback_transition));
}


int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_init);
    RUN_TEST(test_setAndGetValue);
    RUN_TEST(test_setValueAndCheckCallbackIfDifferentValue);
    RUN_TEST(test_setValueAndNoCallbackIfSameValue);
    RUN_TEST(test_setAndGetTarget);
    RUN_TEST(test_stepTowardsValue);
    RUN_TEST(test_stepTowardsValueDescending);
    RUN_TEST(test_stepTowardsValueTriggerEveryStep);
    RUN_TEST(test_stepTowardsValueLongDelayBetweenCalls);
    RUN_TEST(test_stepTowardsValueCustomDuration);
    RUN_TEST(test_getDirection);
    RUN_TEST(test_equality);
    UNITY_END();

    return 0;
}

#endif