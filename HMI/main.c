// New HMI main that initializes drivers and runs the Application state machine
#include "./Application/state_machine.h"
#include "./HW/LCD.h"
#include "./HW/keypad.h"
#include "./MC/uart.h"
#include "./MC/buffer.h"
#include "./HW/potentiometer.h"
#include "./MC/SysTick_Driver.h"
#include "tm4c123gh6pm.h"


#define STACK_TOP    0x20008000
#define STACK_SIZE   (4*1024)          // 4 KB
#define STACK_BASE   (STACK_TOP - STACK_SIZE)

void StackPaint(void)
{
    uint32_t *p = (uint32_t *)STACK_BASE;
    uint32_t *end = (uint32_t *)STACK_TOP;

    while (p < end)
    {
        *p++ = 0xAAAAAAAA;
    }
}





int main(void)
{
  StackPaint();
  SysTick_Init();      // Required for all delays
    UART0_Init();        // UART for communication
    
      keypad_init();       // Keypad input
    LCD_init();          // LCD display
  
    Pot_Init();          // Potentiometer input (if SET_AUTO_LOCK is used)
    ResetBuffer();       // Clear UART buffer

    // Initialize the state machine
    INIT_MACHINE();

    // Enable global interrupts if UART/other peripherals need them
     __asm("CPSIE I");

 
    // Main loop: run the HMI state machine which reacts to keypad and UART
  

     // UART0_Send_String('welcome');
  


      
     // LCD_writeOrMenu(0);
      // SysTick_DelayMs(6000);
 /*     
      if (bufferIndex >= 2)
{
           //LCD_command(0x01);
         
            GPIO_PORTF_DEN_R|=GREEN_LED;
        GPIO_PORTF_DIR_R|=GREEN_LED;
      GPIO_PORTF_DATA_R^=GREEN_LED;
              
              char word[16];
              for(int i=0;i<bufferIndex;i++)
              {
                word[i] = BUFFER[i];
               
              }
              word[bufferIndex] = '\0';
              LCD_command(0x01);
            SysTick_DelayMs(2);
            LCD_writeOrMenu(word);
               ResetBuffer();   
               SysTick_DelayMs(6000);

}


   */
      for (;;)
      
    { 
       STATE_MACHINE();
    }
}