#ifndef REGISTERS_H
#define REGISTERS_H

#include "tm4c123gh6pm.h"

#define GPIO_PORTA   (1U << 0)
#define GPIO_PORTB   (1U << 1)
#define GPIO_PORTC   (1U << 2)
#define GPIO_PORTD   (1U << 3)
#define GPIO_PORTE   (1U << 4)
#define GPIO_PORTF   (1U << 5)

#define SW2        (1U << 0)
#define RED_LED    (1U << 1)
#define BLUE_LED   (1U << 2)
#define GREEN_LED  (1U << 3)
#define SW1        (1U << 4)
#define BUZZER_PIN (1U << 0)

static inline void enable_gpio(unsigned int port_mask) {
    SYSCTL_RCGCGPIO_R |= port_mask;
    volatile unsigned int delay = SYSCTL_RCGCGPIO_R;
}

#endif
 