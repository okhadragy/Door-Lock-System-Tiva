#include "./Drivers/eeprom.h"
#include "./Drivers/uart.h"
#include "./Drivers/timer.h"
#include "./Drivers/buzzer.h"
#include "./Logic/statemachine.h"

int main()
{
    EEPROM_Init();
    UART0_Init();
    Timer0A_Init();
    Buzzer_Init();
    enable_gpio(GPIO_PORTF);
    __asm("CPSIE I");

    for (;;)
    {
        StateMachine();
    }
}
