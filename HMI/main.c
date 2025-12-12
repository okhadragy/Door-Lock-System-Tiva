#include "./HW/LCD.h"
#include "./HW/keypad.h"
#include "./MC/uart.h"
#include "./MC/buffer.h"
#include "./HW/potentiometer.h"
#include "./MC/SysTick_Driver.h"
#include "tm4c123gh6pm.h"
#include "./Tests/test_functions.h"

void Test_Buffer_Logic(void);
void Run_All_SysTick_Tests(void);
void Run_All_Timer_Tests(void);

int main()
{
    Test_Init();             // initialize test system
    // RED LED -> error in test
    // GREEN LED -> all test cases passed
    
    Test_Buffer_Logic();  // run buffer test (rese buffer)
    Run_All_SysTick_Tests(); // run all motor tests
    Run_All_Timer_Tests(); // run all timer tests
    
    pass(); // if we reach here all tests succeed
    
    while(1);
}
