#include "potentiometer.h"

void Pot_Init(void) {
    SYSCTL_RCGCADC_R |= 1;
    SYSCTL_RCGCGPIO_R |= 2;
    while((SYSCTL_PRGPIO_R & 2) == 0);

    GPIO_PORTB_AFSEL_R |= 0x10;
    GPIO_PORTB_DEN_R &= ~0x10;
    GPIO_PORTB_AMSEL_R |= 0x10;

    ADC0_ACTSS_R &= ~8;
    ADC0_EMUX_R &= ~0xF000;
    ADC0_SSMUX3_R = 10;
    ADC0_SSCTL3_R = 6;
    ADC0_ACTSS_R |= 8;
}

void Pot_Read_Timeout(unsigned int *timeout) {
    ADC0_PSSI_R = 8;
    while((ADC0_RIS_R & 8) == 0);
    ADC0_ISC_R = 8;
    *timeout = 5 + ((ADC0_SSFIFO3_R * 25) / 4095);
}