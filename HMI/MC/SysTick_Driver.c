#include "../tm4c123gh6pm.h"
#include "SysTick_Driver.h"

void SysTick_Init(void)
{
    NVIC_ST_CTRL_R = 0;          // Disable SysTick
    NVIC_ST_RELOAD_R = 0x00FFFFFF; // Max reload (24-bit)
    NVIC_ST_CURRENT_R = 0;       // Clear current value
    NVIC_ST_CTRL_R = 5;          // Enable SysTick with system clock
}

void SysTick_DelayMs(unsigned int ms)
{
    unsigned int i;
    for(i = 0; i < ms; i++)
    {
        NVIC_ST_RELOAD_R = 16000 - 1;  // 1ms at 16MHz
        NVIC_ST_CURRENT_R = 0;         // Clear
        while((NVIC_ST_CTRL_R & (1 << 16)) == 0);
    }
}

void SysTick_DelayUs(unsigned int us) {
    unsigned int i;
    for(i = 0; i < us; i++) {
        NVIC_ST_RELOAD_R = 16 - 1;     // 1µs at 16MHz
        NVIC_ST_CURRENT_R = 0;         // Clear current
          while((NVIC_ST_CTRL_R & (1 << 16)) == 0); // Wait for Count flag
    }
}