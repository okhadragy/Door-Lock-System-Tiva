// New HMI main that initializes drivers and runs the Application state machine
#include "state_machine.h"
#include "LCD.h"
#include "keypad.h"
#include "UART_DRIVER.h"
#include "buffer.h"
#include "poteniometer.h"
#include "SysTick_Driver.h"
#include "tm4c123gh6pm.h"

int main(void)
{
    LCD_init();        
    Pot_Init();
    keypad_init();
    UART0_INIT();

    ResetBuffer();

    // Initialize application state machine
    INIT_MACHINE();

    // Enable global interrupts
    __asm("CPSIE I");

    // Main loop: run the HMI state machine which reacts to keypad and UART
    while (1)
    {
        STATE_MACHINE();
    }

    return 0;
}
