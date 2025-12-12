#include "test_functions.h"
#define LED_RED   (1U << 1)   // PF1
#define LED_GREEN (1U << 3)   // PF3

void Test_Init(void)
{
    SYSCTL_RCGCGPIO_R |= 0x20;
    GPIO_PORTF_DIR_R |= LED_RED | LED_GREEN;
    GPIO_PORTF_DEN_R |= LED_RED | LED_GREEN;
}

void pass(void)
{
    GPIO_PORTF_DATA_R = LED_GREEN;
}

static void fail(void)
{
    GPIO_PORTF_DATA_R = LED_RED;
    while(1); // stop everything
}

void ASSERT_TRUE(int cond)
{
    if (!cond) fail();
}

void ASSERT_FALSE(int cond)
{
    if (cond) fail();
}

void ASSERT_EQUAL(int a, int b)
{
    if (a != b) fail();
}
