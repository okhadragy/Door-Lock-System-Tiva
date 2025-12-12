#include "test_functions.h"
#include "../Drivers/timer.h"

void test_Timer0A_Init(void)
{
    Timer0A_Init();

    ASSERT_TRUE(SYSCTL_RCGCTIMER_R & 0x01);       // clock enabled
    ASSERT_TRUE(SYSCTL_PRTIMER_R & 0x01);         // peripheral ready
    ASSERT_EQUAL(TIMER0_CTL_R, 0x00);                // timer disabled
    ASSERT_EQUAL(TIMER0_CFG_R, 0x00);                // 32-bit config
    ASSERT_EQUAL(TIMER0_TAMR_R, 0x01);               // one-shot mode
}


void test_Timer0A_DelayMs(void)
{
    Timer0A_Init();                // ensure timer configured as one-shot
    Timer0A_DelayMs(5);            // blocks until timeout (5 ms)

    ASSERT_EQUAL(TIMER0_TAILR_R, 16000 * 5);  // reload was programmed
    ASSERT_TRUE( (TIMER0_RIS_R & 0x01) != 0 );  // timeout flag is set after return
    ASSERT_FALSE( (TIMER0_CTL_R & 0x01) != 0 ); // one-shot should have stopped the timer
}


void Run_All_Timer_Tests(void)
{
    test_Timer0A_Init();
    test_Timer0A_DelayMs();
}
