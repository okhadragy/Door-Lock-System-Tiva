#include "motor.h"

void initialize_motor(void)
{
    // clock for M1PWM3 and Port A
    SYSCTL_RCGCPWM_R |= (1 << 1);
    SYSCTL_RCGCGPIO_R |= (1 << 0);
    volatile int dummy = SYSCTL_RCGCGPIO_R;

    // clock divider
    SYSCTL_RCC_R |= SYSCTL_RCC_USEPWMDIV;
    SYSCTL_RCC_R &= ~SYSCTL_RCC_PWMDIV_M;
    SYSCTL_RCC_R |= SYSCTL_RCC_PWMDIV_64;

    // configure pin for PWM
    GPIO_PORTA_AFSEL_R |= (1 << 7);
    GPIO_PORTA_PCTL_R &= ~(0xF << 28); // Clear PCTL for PA7
    GPIO_PORTA_PCTL_R |= (0x5 << 28);
    GPIO_PORTA_DEN_R |= (1 << 7);
    // Disable generator
    PWM1_1_CTL_R = 0;
    // load: number of clock cycles after division needed to produce an output of 50hz
    PWM1_1_LOAD_R = 4999;
    // Duty cycle
    PWM1_1_CMPB_R = 5000 - 500;
    // Load and COmpare ouputs
    PWM1_1_GENB_R = 0x0000080C;

    // enable generator
    PWM1_1_CTL_R = 1;
    PWM1_ENABLE_R |= (1 << 3);
}

void open_Door(void){
    PWM1_1_CMPB_R = 5000 - 500;
}

void close_Door(void){
    PWM1_1_CMPB_R = 5000 - 250;
}