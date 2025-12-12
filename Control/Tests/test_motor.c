#include "test_functions.h"
#include "../Drivers/motor.h"

void test_motor_init(void)
{
    // check PWM output is enabled
    ASSERT_TRUE(PWM1_ENABLE_R & (1 << 3));

    // check LOAD value (frequency)
    ASSERT_EQUAL(PWM1_1_LOAD_R, 4999);

    // check initial duty cycle
    ASSERT_EQUAL(PWM1_1_CMPB_R, 5000 - 500);
}

void test_open(void) {
    open_Door();
    ASSERT_EQUAL(PWM1_1_CMPB_R, 5000 - 500);
}

void test_close(void) {
    close_Door();
    ASSERT_EQUAL(PWM1_1_CMPB_R, 5000 - 250);
}

void Run_All_Motor_Tests(void)
{
    test_motor_init();
    test_open();
    test_close();
}