#include "test_functions.h"
#include "../Drivers/buzzer.h"

void test_Buzzer_Init(void)
{
    // after init, buzzer must be OFF
    ASSERT_FALSE(GPIO_PORTE_DATA_R & BUZZER_PIN);
}

void test_Buzzer_Output(void)
{
    Buzzer_Output(1);
    ASSERT_TRUE(GPIO_PORTE_DATA_R & BUZZER_PIN);

    Buzzer_Output(0);
    ASSERT_FALSE(GPIO_PORTE_DATA_R & BUZZER_PIN);
}

void test_Buzzer_ON(void)
{
    // turn ON for 10ms
    Buzzer_ON(10);

    // After Buzzer_ON(), buzzer MUST be OFF
    ASSERT_FALSE(GPIO_PORTE_DATA_R & BUZZER_PIN);
}

void Run_All_Buzzer_Tests(void)
{
    test_Buzzer_Init();
    test_Buzzer_Output();
    test_Buzzer_ON();
}
