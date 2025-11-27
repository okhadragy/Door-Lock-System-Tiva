#include "timer.h"

void Timer0A_Init(void)
{
    SYSCTL_RCGCTIMER_R |= 0x01;
    while ((SYSCTL_PRTIMER_R & 0x01) == 0)
    {
    }
    TIMER0_CTL_R = 0x00000000;
    TIMER0_CFG_R = 0x00000000;
    TIMER0_TAMR_R = 0x00000001; // one-shot mode
    TIMER0_ICR_R = 0x00000001;
    TIMER0_IMR_R = 0x00000001;
    NVIC_PRI4_R = (NVIC_PRI4_R & 0x00FFFFFF) | 0x80000000;
    NVIC_EN0_R = 1 << 19;
}

void Timer0A_DelayMs(unsigned int ms)
{
    TIMER0_CTL_R = 0x00000000;   // disable during setup
    TIMER0_TAILR_R = 16000 * ms; // set reload value
    TIMER0_ICR_R = 0x00000001;   // clear any old flag
    TIMER0_CTL_R = 0x00000001;   // start one-shot
}

void Timer0AIntHandler(void)
{
    TIMER0_ICR_R = TIMER_ICR_TATOCINT; // clear flag
    GPIO_PORTF_DATA_R ^= RED_LED;
}
