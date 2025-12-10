#ifndef CONFIG_H
#define CONFIG_H

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

// Configurable parameters
#define PASSWORD_LENGTH 5
#define TIMEOUT_LENGTH 4
#define MAX_FAILS 3
typedef enum {
    STATE_INIT,
    STATE_MAIN_MENU,
    STATE_CHECK_PASSWORD,
    STATE_OPEN_DOOR,
    STATE_CHANGE_PASSWORD,
    STATE_SET_TIMEOUT,
    STATE_TIMEOUT
} SystemState;

typedef enum {
    ACTION_OPEN_DOOR,
    ACTION_CHANGE_PASSWORD,
    ACTION_SET_TIMEOUT,
} SystemActions;

static inline void enable_gpio(unsigned int port_mask) {
    SYSCTL_RCGCGPIO_R |= port_mask;
    volatile unsigned int delay = SYSCTL_RCGCGPIO_R;
}

#endif
 