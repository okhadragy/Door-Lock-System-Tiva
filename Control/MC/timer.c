#include "timer.h"

void Timer0A_Init(void)
{
    SYSCTL_RCGCTIMER_R |= 0x01; // clock
    while ((SYSCTL_PRTIMER_R & 0x01) == 0)
    {
    }
    TIMER0_CTL_R = 0; // disable
    TIMER0_CFG_R = 0; // 32 bit
    TIMER0_TAMR_R = 0x01;  // one-shot mode
}

void Timer0A_DelayMs(unsigned int ms)
{
    TIMER0_CTL_R = 0;   // disable
    TIMER0_TAILR_R = 16000 * ms; // set reload value
    TIMER0_ICR_R = 0x00000001;   // clear any old flag
    TIMER0_CTL_R = 0x00000001;   // start one-shot
    while ((TIMER0_RIS_R & 0x01) == 0){}
}