#include "tm4c123gh6pm.h"
#include "stdint.h"
#include "keypad.h"
#include <stdbool.h>

void keypad_init(void)
{
    SYSCTL_RCGCGPIO_R |= (1 << 2) | (1 << 4) | (1 << 5); // Enable Port C, E, F
    while ((SYSCTL_PRGPIO_R & ((1 << 2) | (1 << 4) | (1 << 5))) == 0);

    // Rows (PC4–PC7) ? Output
    GPIO_PORTC_DIR_R |= 0xF0;
    GPIO_PORTC_DEN_R |= 0xF0;

    // Columns (PE0–PE3) ? Input + Pull-up
    GPIO_PORTE_DIR_R &= ~0x0F;
    GPIO_PORTE_DEN_R |= 0x0F;
    GPIO_PORTE_PUR_R |= 0x0F;
    //GPIO_PORTE_PDR_R &= ~0x0F;

    // LED (PF3) ? Output
   // GPIO_PORTF_DIR_R |= 0x08;
   // GPIO_PORTF_DEN_R |= 0x08;
}

// =========================
//  law 7ad mohtam yefham : be3ml loop 3la el keypad kolha l7ad ma yshoof el button el 2nt dost 3leh , w beraga3 string
// =========================
unsigned char readKeypad(void)
{
    const unsigned char keys[4][4] = {
        {'1','2','3','+'},
        {'4','5','6','-'},
        {'7','8','9','C'},
        {'*','0','#','D'}
    };

    const uint8_t row_mask[4] = {0xE0, 0xD0, 0xB0, 0x70}; // Active-low rows

    for (int row = 0; row < 4; row++)
    {
        // Write only R4-R7 bits without touching other bits
        GPIO_PORTC_DATA_R = (GPIO_PORTC_DATA_R & ~0xF0) | row_mask[row];

        delayMs(2);

        uint8_t col = GPIO_PORTE_DATA_R & 0x0F;

        if (col != 0x0F) // One column pulled LOW
        {
            if (!(col & 0x01)) return keys[row][0];
            if (!(col & 0x02)) return keys[row][1];
            if (!(col & 0x04)) return keys[row][2];
            if (!(col & 0x08)) return keys[row][3];
        }
    }

    return 0;

}



bool isAction(unsigned char press){
    return((press=='A')||(press=='B')||(press=='*'));
}