#include <iostream>
#include <unity.h>
#include <Transition.h>
#include <AxxTest.h>

#ifdef UNIT_TEST

bool callback_called;
int callback_value;

void onTransitionValueChange(Transition& transition, long newValue){
    callback_called = true;
    callback_value = newValue;
}

Transition transition(10, onTransitionValueChange);

void reset_callback(){

    // Reset callback function state
    callback_called = false;
    callback_value = 0;
}

void setUp(void) {

    // Initialise Transition
    transition.init(50);

    reset_callback();

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


int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_init);
    RUN_TEST(test_setAndGetValue);
    RUN_TEST(test_setValueAndCheckCallbackIfDifferentValue);
    RUN_TEST(test_setValueAndNoCallbackIfSameValue);
    RUN_TEST(test_setAndGetTarget);
    RUN_TEST(test_stepTowardsValue);
    UNITY_END();

    return 0;
}

#endif