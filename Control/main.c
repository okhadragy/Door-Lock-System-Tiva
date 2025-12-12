#include "./Drivers/eeprom.h"
#include "./Drivers/uart.h"
#include "./Drivers/timer.h"
#include "./Drivers/buzzer.h"
#include "./Drivers/motor.h"
#include "./Tests/test_functions.h"

// UNIT
void Run_All_Buzzer_Tests(void);
void Run_All_Motor_Tests(void);
void Run_All_Timer_Tests(void);
void Run_All_EEPROM_Tests(void);
void UART_Run_All_Tests(void);

// INTEGRATION
void test_init_to_main_menu(void);
void test_mainmenu_invalid(void);
void test_mainmenu_plus(void);
void test_mainmenu_minus(void);
void test_mainmenu_star(void);
void test_checkpassword_open_success(void);
void test_checkpassword_change_password_success(void);
void test_checkpassword_settimeout_success(void);
void test_checkpassword_fail_once(void);
void test_checkpassword_fail_three(void);
void test_change_password_flow(void);
void test_settimeout_valid_10(void);
void test_full_settimeout_then_open(void);

int main()
{
    EEPROM_Init();
    UART0_Init();
    Timer0A_Init();
    Buzzer_Init();
    initialize_motor();
    enable_gpio(GPIO_PORTF);
    __asm("CPSIE I");
    
    /*************************************************************
    * UNIT TESTS
    *************************************************************/
    
    /*

    Test_Init();             // initialize test system
    
    // RED LED -> error in test
    // GREEN LED -> all test cases passed
    
    Run_All_Timer_Tests(); // run all timer tests
    Run_All_Motor_Tests(); // run all motor tests
    Run_All_Buzzer_Tests();  // run all buzzer tests
    Run_All_EEPROM_Tests(); // run all eeprom tests
    
    pass(); // if we reach here all tests succeed (uart not considered)
    
    UART0_Init();
    UART_Run_All_Tests();
    
    // PuTTY Expected Output:
    
    // --- UART SELF TEST START ---
    // FR=.. DR=.. IBRD=.. FBRD=..
    // TX Test: Hello from Tiva!
    // RX Test: Type something...
    // Buffer Overflow Test
    // !
    // After overflow, bufferIndex = 0
    //
    // --- UART SELF TEST END ---
    
    */

    /*************************************************************
    * INTEGRATION TESTS
    *************************************************************/
    
    // test_init_to_main_menu(); // PUTTY OUTPUT: S, SUCCEED!
    // test_mainmenu_invalid(); // PUTTY OUTPUT: !, SUCCEED!
    // test_mainmenu_plus(); // PUTTY OUTPUT: S, SUCCEED!
    // test_mainmenu_minus(); // PUTTY OUTPUT: S, SUCCEED!
    // test_mainmenu_star(); // PUTTY OUTPUT: S, SUCCEED!
    // test_checkpassword_open_success(); // PUTTY OUTPUT: SS, MOTOR ACTIVATES FOR 5 SECONDS THEN STOPS, SUCCEED! 
    // test_checkpassword_change_password_success(); // PUTTY OUTPUT: S, SUCCEED!
    // test_checkpassword_settimeout_success(); // PUTTY OUTPUT: S, SUCCEED!
    // test_checkpassword_fail_once(); // PUTTY OUTPUT: F, SUCCEED!
    // test_checkpassword_fail_three(); // PUTTY OUTPUT: FFF, BUZZER, S, SUCCEED!
    // test_change_password_flow(); // PUTTY OUTPUT: SSS, SUCCEED!
    // test_settimeout_valid_10(); // PUTTY OUTPUT: S, SUCCEED!
    // test_full_settimeout_then_open(); // PUTTY OUTPUT: SSSSSS, OPEN, WAIT 10 SECONDS, CLOSE, SUCCEED!
    
    while(1);
}
