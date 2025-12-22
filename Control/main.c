#include "./MC/eeprom.h"
#include "./MC/uart.h"
#include "./MC/timer.h"
#include "./HW/buzzer.h"
#include "./HW/motor.h"
#include "./Application/statemachine.h"

#define STACK_TOP    0x20008000
#define STACK_SIZE   (4*1024)          // 4 KB
#define STACK_BASE   (STACK_TOP - STACK_SIZE)

void StackPaint(void)
{
    uint32_t *p = (uint32_t *)STACK_BASE;
    uint32_t *end = (uint32_t *)STACK_TOP;

    while (p < end)
    {
        *p++ = 0xAAAAAAAA;
    }
}



int main()
{
    StackPaint();  
    enable_gpio(GPIO_PORTF);
    EEPROM_Init();
    UART0_Init();
    Timer0A_Init();
    Buzzer_Init();
    initialize_motor();
    __asm("CPSIE I");
    
    SYSCTL_RCGCGPIO_R |= (1 << 5);
    GPIO_PORTF_DEN_R |= (1 << 1);
    GPIO_PORTF_DIR_R |= (1 << 1);
    GPIO_PORTF_DATA_R &= ~(1 << 1);
    GPIO_PORTF_DATA_R &= ~(1 << 2);
    GPIO_PORTF_DATA_R &= ~(1 << 3);
    
    for (;;)
    {
      //UART0_Send_String("LO");
      StateMachine();
    }
}