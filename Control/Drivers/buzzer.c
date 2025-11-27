#include "buzzer.h"
#include "timer.h"


void Buzzer_Init(void)
{
    enable_gpio(GPIO_PORTE);          // Enable clock for Port E
    GPIO_PORTE_DIR_R |= BUZZER_PIN;   // Set PE0 as output
    GPIO_PORTE_DEN_R |= BUZZER_PIN;   // Enable digital function
    GPIO_PORTE_DATA_R &= ~BUZZER_PIN; // Initialize Buzzer OFF
}

/**
 * @param state 1 to turn ON, 0 to turn OFF.
 */
void Buzzer_Output(int state)
{
    if (state)
    {
        GPIO_PORTE_DATA_R |= BUZZER_PIN; // Turn ON the buzzer
    }
    else
    {
        GPIO_PORTE_DATA_R &= ~BUZZER_PIN;
    }
}

void Buzzer_ON(unsigned int durationMs){
    Buzzer_Output(1);
    Timer0A_DelayMs(durationMs); // Buzzer ON for specified duration
    Buzzer_Output(0);
}