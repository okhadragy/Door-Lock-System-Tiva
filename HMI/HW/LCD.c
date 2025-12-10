#include "tm4c123gh6pm.h"
#include <stdint.h>
#include "SysTick_Driver.h"



void LCD_enablePulse() {
    // Generate Enable pulse for LCD
    GPIO_PORTA_DATA_R |= (1<<3);   // EN = 1
    
    // CHANGED: 1us is very short for a function wrapper. 
    // Used 10us to be safe and ensure stability.
    SysTick_DelayUs(10);           
    
    GPIO_PORTA_DATA_R &= ~(1<<3);  // EN = 0
    
    // Wait before next command/data
    SysTick_DelayUs(40);           
}

void LCD_send4Bits(uint8_t data) {
    // Send lower 4 bits of data to PB0-PB3
    // Clear PB0-3 then set new data
    GPIO_PORTB_DATA_R = (GPIO_PORTB_DATA_R & ~0x0F) | (data & 0x0F);
}

void LCD_command(uint8_t cmd) {
    GPIO_PORTA_DATA_R &= ~(1<<2);  // RS = 0 for command

    LCD_send4Bits(cmd >> 4);       // Send upper nibble
    LCD_enablePulse();

    LCD_send4Bits(cmd & 0x0F);     // Send lower nibble
    LCD_enablePulse();

    // Command processing delay (Clear command needs ~2ms)
    if (cmd < 4) {
        SysTick_DelayMs(2);
    } else {
        SysTick_DelayUs(40);
    }
}

void LCD_data(uint8_t data) {
    GPIO_PORTA_DATA_R |= (1<<2);   // RS = 1 for data

    LCD_send4Bits(data >> 4);      // Upper nibble
    LCD_enablePulse();

    LCD_send4Bits(data & 0x0F);    // Lower nibble
    LCD_enablePulse();
    
    // Write delay
    SysTick_DelayUs(40);        
}



void LCD_printString(char *str)
{
    uint8_t count = 0;   

    LCD_command(0x80);   

    while (*str != '\0')
    {
        LCD_data(*str);
        count++;

        if (count == 16 || *str == ':')
        {
            LCD_command(0xC0);  // Move to line 2
            SysTick_DelayMs(4);
        }

        str++;
    }
}
void LCD_writeOrMenu(char *data) {
    if (data != 0) {
        // If string is provided, write it
        LCD_printString(data);
    } else {

        LCD_command(0x01);       // Clear display
        SysTick_DelayMs(2);

        // First line
        LCD_printString("MENU:");

        // Move to second line
        LCD_command(0xC0);       // 0xC0 = address of second line

        LCD_data('+');
        LCD_command(0xC5);
        LCD_data('-');
        LCD_command(0xCA);
        LCD_data('*');
    }
}
// ---------- INITIALIZATION ----------

void LCD_init() {
    // Enable GPIOA and GPIOB
    SYSCTL_RCGCGPIO_R |= (1<<0) | (1<<1);
    while((SYSCTL_PRGPIO_R & ((1<<0)|(1<<1))) == 0);

    // Configure pins
    GPIO_PORTA_DIR_R |= (1<<2) | (1<<3); // PA2, PA3 Output
    GPIO_PORTA_DEN_R |= (1<<2) | (1<<3);

    GPIO_PORTB_DIR_R |= 0x0F;            // PB0-PB3 Output
    GPIO_PORTB_DEN_R |= 0x0F;

    // Wait for LCD power-up (>15ms required)
    SysTick_DelayMs(20);          

    // Initialization sequence (as per datasheet for 4-bit mode)
    LCD_send4Bits(0x03);
    LCD_enablePulse();
    SysTick_DelayMs(5);

    LCD_send4Bits(0x03);
    LCD_enablePulse();
    SysTick_DelayUs(150);

    LCD_send4Bits(0x03);
    LCD_enablePulse();

    LCD_send4Bits(0x02);           // Switch to 4-bit mode
    LCD_enablePulse();

    // Now in 4-bit mode, use LCD_command
    LCD_command(0x28);             // 4-bit, 2 lines, 5x8 font
    LCD_command(0x0F);             // Display ON, cursor blink
    LCD_command(0x01);             // Clear display
    SysTick_DelayMs(2);            // Clear requires ~2ms
    LCD_command(0x06);             // Entry mode, cursor moves right
}

// ---------- MAIN ----------

int main(void) {
    // 1. INITIALIZE SYSTICK FIRST!
    SysTick_Init(); 

    // 2. Initialize LCD
    LCD_init();

    // 3. Main Loop
    LCD_command(0x01);       // Clear
    SysTick_DelayMs(2);
    //LCD_printString("Enter Password:");
    // LCD_printString("123444444444");
    //LCD_writeOrMenu(0);
    LCD_writeOrMenu("Enter Password:");
    LCD_writeOrMenu("1234567891234567");
    while(1) {
        // Empty loop
    }
}