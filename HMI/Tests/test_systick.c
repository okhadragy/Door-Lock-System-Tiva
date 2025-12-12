#include "test_functions.h"
#include "../MC/SysTick_Driver.h"

void test_SysTick_Init(void)
{
    SysTick_Init();

    // Verify Control Register: Bit0 (ENABLE) + Bit2 (CLK_SRC)
    ASSERT_TRUE((NVIC_ST_CTRL_R & 0x05) == 0x05);
}

void test_SysTick_DelayMs(void)
{
    SysTick_Init();

    // Functional Delay Test (10ms)
    SysTick_DelayMs(10);

    // Reload calculation for 10ms at 16MHz
    ASSERT_TRUE(NVIC_ST_RELOAD_R == 15999);
}

void test_SysTick_DelayUs(void)
{
    SysTick_Init();

    // Functional Delay Test (100us)
    SysTick_DelayUs(100);

    // Reload calculation for 100us at 16MHz
    ASSERT_TRUE(NVIC_ST_RELOAD_R == 15);
}

void Run_All_SysTick_Tests(void)
{
    test_SysTick_Init();
    test_SysTick_DelayMs();
    test_SysTick_DelayUs();
}
