// New HMI main that initializes drivers and runs the Application state machine
#include "./Application/state_machine.h"
#include "./HW/LCD.h"
#include "./HW/keypad.h"
#include "./MC/uart.h"
#include "./MC/buffer.h"
#include "./HW/potentiometer.h"
#include "./MC/SysTick_Driver.h"
#include "tm4c123gh6pm.h"

int main(void)
{
    LCD_init();        
    Pot_Init();
    keypad_init();
    UART0_Init();

    ResetBuffer();

    // Initialize application state machine
    INIT_MACHINE();

    // Enable global interrupts
    __asm("CPSIE I");

    // Main loop: run the HMI state machine which reacts to keypad and UART
    for (;;)
    {
        STATE_MACHINE();
    }
}