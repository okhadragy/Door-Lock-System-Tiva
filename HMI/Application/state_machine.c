#include "state_machine.h"
#include "../HW/LCD.h"
#include "../HW/keypad.h"
#include "../MC/uart.h"
#include "../MC/buffer.h"
#include "../HW/potentiometer.h"
#include "../MC/SysTick_Driver.h"
#include <string.h>

STATE CURRENT_STATE;
unsigned char PASSWORD[PASSWORD_LENGTH+1];
unsigned int auto_lock_timeout = 5;
int failCount = 0;

// Tracks the last menu action sent to the controller so we can decide
// subsequent behavior after an ACK is received.
static char lastAction = 0;

/*----------------------------------------
  Helper: Read stable key
----------------------------------------*/
 static char readKey()
{
    char k = readKeypad();
    if (k != 0)
    {
       //SysTick_DelayMs(20);
        if (readKeypad() == k)
        {
            while(readKeypad() != 0) SysTick_DelayMs(5);

            return k;
        }
    }
    return 0;
}

/*----------------------------------------
  Helper: Get 5-digit password and send it
----------------------------------------*/
/*static void enterPassword()
{
    int i = 0;

    LCD_command(0x01);
   SysTick_DelayMs(2);
   
       LCD_command(0x80);
    LCD_writeOrMenu("Enter Password:");
       LCD_command(0xC0);    //must be added to move cursor down 

    char PASSWORD[PASSWORD_LENGTH];

    while(i < PASSWORD_LEN)
    {
        char k = readKey();
        if(k != 0)
        {
            PASSWORD[i] = k;
            LCD_data('*');
            i++;
           if(k=='#')
            {
              if(i>0)
              {
                i--;
              }
              PASSWORD[i]='\0';
                LCD_data('*');
            }
        }
    }

    UART0_Send_String(PASSWORD);

    ResetBuffer();
}*/

static void enterPassword(int type )
{
    
    int i = 0;
    
    LCD_command(0x01);          // Clear LCD
    SysTick_DelayMs(2);
   
    LCD_command(0x80);

    if(type==0){
       LCD_writeOrMenu("Enter Password:");
    }
    else if(type ==1){
       LCD_writeOrMenu("Confirm Password:");
    }
    else if(type==2){
      LCD_writeOrMenu("Enter New Password:");
    }
    else if(type==3){
      LCD_writeOrMenu("Confirm New Password:");
    }
    
    
    
    LCD_command(0xC0);          // Move cursor to next line

    char localPassword[PASSWORD_LENGTH + 1] = {0};

    while(i < PASSWORD_LENGTH)
    {
        char k = readKey();
        if(k != 0 && k != '+' && k != '-' && k != '*' && k != 'C' && k != 'D')
        {
            if(k == '#')  // Backspace key
            {
                if(i > 0)
                {
                    i--;                
                    PASSWORD[i] = '\0'; 
                    LCD_command(0x10); // Move cursor back
                    LCD_data(' ');     // Overwrite with space
                    LCD_command(0x10); // Move cursor back again
                }
            }
            else // Normal character
            {
                PASSWORD[i] = k;
                LCD_data('*');
                i++;
            }
        }
    }
    PASSWORD[PASSWORD_LENGTH] = '\0';
    
    UART0_Send_String(PASSWORD); // Send entered password
    ResetBuffer();
    //SysTick_DelayMs(10000);
}


/*----------------------------------------
         INIT
----------------------------------------*/
void INIT_MACHINE()

{
      GPIO_PORTF_DEN_R|=RED_LED;
      GPIO_PORTF_DIR_R|=RED_LED;
      
      GPIO_PORTF_DEN_R|=GREEN_LED;
      GPIO_PORTF_DIR_R|=GREEN_LED;
     
    CURRENT_STATE = INITIAL_STATE;
}

/*----------------------------------------
       STATE MACHINE LOGIC
----------------------------------------*/
void STATE_MACHINE()
{
    char k; // keypress
    char resp; // UART response

    switch(CURRENT_STATE)
    {
    /*----------------------------------------
          1) FIRST PASSWORD STATE
    ----------------------------------------*/
    case INITIAL_STATE:
        lastAction = 'I';
        enterPassword(0);
        enterPassword(1);
        CURRENT_STATE = WAIT_INITIAL_PASSWORD_RESPONSE;
        break;

    case WAIT_INITIAL_PASSWORD_RESPONSE:
     // LCD_command(0x01);
     // SysTick_DelayMs(2);
     // LCD_command(0x80);
      //LCD_writeOrMenu("Wait for Response");
     // LCD_command(0xC0);
      //__asm volatile ("cpsid i" : : : "memory");
        SysTick_DelayMs(100);
        if(bufferIndex > 0)
        {
          /*  resp = BUFFER[0];
            char res[2] = {resp, '\0'};
            LCD_writeOrMenu(res);
            SysTick_DelayMs(500);
            //ResetBuffer();*/
    // __asm volatile ("cpsid i" : : : "memory");
        LCD_command(0x01);
        SysTick_DelayMs(2);
        resp = BUFFER[0];
        
        //char res[2] = {resp, '\0'};
        // LCD_writeOrMenu(res);
        // SysTick_DelayMs(3000);
        
   // __asm volatile ("cpsie i" : : : "memory");
            
            if(resp == 'S')
            {
                failCount = 0;
                LCD_command(0x01);
                LCD_writeOrMenu("Saved");
                GPIO_PORTF_DATA_R|=GREEN_LED;
                SysTick_DelayMs(1000);
  
                GPIO_PORTF_DATA_R&=~GREEN_LED;
                CURRENT_STATE = MAIN_MENU;
            }
            else if (resp == 'F')
            {
                failCount++;
              
                if (failCount >= MAX_FAILS) {
                    failCount = 0;
                    GPIO_PORTF_DATA_R|=RED_LED;
                    CURRENT_STATE = TIMEOUT;
                }
                
                else{
                    LCD_command(0x01);
                    LCD_writeOrMenu("Wrong");
                    GPIO_PORTF_DATA_R|=RED_LED;
                    SysTick_DelayMs(600);
                    GPIO_PORTF_DATA_R&=~RED_LED;
                    if(lastAction=='I'){
                      CURRENT_STATE = INITIAL_STATE;
                    }
                    else if(lastAction=='-'){
                      CURRENT_STATE = MAIN_MENU;
                    }
                }
            }
            ResetBuffer();
        }
        //__asm volatile ("cpsie i" : : : "memory");
        break;

    /*----------------------------------------
                 2) MAIN MENU
    ----------------------------------------*/
    case MAIN_MENU:
        LCD_writeOrMenu(0); // print menu screen

        k = readKey();
        if(k == '+')
        {
            lastAction = '+';
            UART0_Send_String("+");
            ResetBuffer();
            CURRENT_STATE = SEND_ACTION_WAIT_ACK;
        }
        else if(k == '-')
        {
            lastAction = '-';
            UART0_Send_String("-");
            ResetBuffer();
            CURRENT_STATE = SEND_ACTION_WAIT_ACK;
        }
        else if(k == '*')
        {
            lastAction = '*';
            UART0_Send_String("*");
            ResetBuffer();
            CURRENT_STATE = SEND_ACTION_WAIT_ACK;
        }
        break;

    /*----------------------------------------
           3) WAIT FOR ACK FROM CONTROL
    ----------------------------------------*/
    case SEND_ACTION_WAIT_ACK:
        if(bufferIndex > 0)
        {
            resp = BUFFER[0];
            ResetBuffer();

            if(resp == 'S')
            {
            
                CURRENT_STATE = ENTER_PASSWORD;
            }
            else
            {
                LCD_command(0x01);
                LCD_writeOrMenu("Error!");
                GPIO_PORTF_DATA_R|=RED_LED;
                SysTick_DelayMs(700);
                GPIO_PORTF_DATA_R&=~RED_LED;
                CURRENT_STATE = MAIN_MENU;
            }
        }
        break;

    /*----------------------------------------
               4) ENTER PASSWORD
    ----------------------------------------*/
    case ENTER_PASSWORD:
        enterPassword(0);
        CURRENT_STATE = WAIT_PASSWORD_RESPONSE;
        break;

    /*----------------------------------------
         5) WAIT PASSWORD RESULT
    ----------------------------------------*/
    case WAIT_PASSWORD_RESPONSE:
        if(bufferIndex > 0)
        {
            resp = BUFFER[0];
            ResetBuffer();

            if(resp == 'S')
            {
                failCount = 0;
                // Password matched. Branch based on the original action.
                if(lastAction == '+')
                {
                    // Door open flow: controller will send another 'S' after opening.
                    LCD_command(0x01);
                    LCD_writeOrMenu("Password OK");
                    GPIO_PORTF_DATA_R|=GREEN_LED;
                    SysTick_DelayMs(200);
                    LCD_command(0x01);
                    LCD_writeOrMenu("Door Opened");
                    GPIO_PORTF_DATA_R|=GREEN_LED;
                    CURRENT_STATE = WAIT_DOOR_ACK;
                    GPIO_PORTF_DATA_R&=~GREEN_LED;
                }
                else if(lastAction == '-')
                {
                    LCD_command(0x01);
                    LCD_writeOrMenu("Password OK");
                    GPIO_PORTF_DATA_R|=GREEN_LED;
                    SysTick_DelayMs(400);
                    GPIO_PORTF_DATA_R&=~GREEN_LED;
                    CURRENT_STATE = NEW_PASSWORD;
                }
                else if(lastAction == '*')
                {
                    // Set timeout: controller will now accept timeout digits
                    LCD_command(0x01);
                    LCD_writeOrMenu("Password OK");
                    GPIO_PORTF_DATA_R|=GREEN_LED;
                    SysTick_DelayMs(400);
                    GPIO_PORTF_DATA_R&=~GREEN_LED;
                    CURRENT_STATE = SET_AUTO_LOCK;
                }
                else
                {
                    
                    LCD_command(0x01);
                    LCD_writeOrMenu("Correct");
                    GPIO_PORTF_DATA_R|=GREEN_LED;
                    SysTick_DelayMs(600);
                    GPIO_PORTF_DATA_R&=~GREEN_LED;
                    CURRENT_STATE = MAIN_MENU;
                }
                // we'll clear lastAction later after completing the action
            }
            else
            {
                failCount++;
              
                if (failCount >= MAX_FAILS) {
                    failCount = 0;
                    GPIO_PORTF_DATA_R|=RED_LED;
                    CURRENT_STATE = TIMEOUT;
                }
                
                else{
                    LCD_command(0x01);
                    LCD_writeOrMenu("Wrong");
                    GPIO_PORTF_DATA_R|=RED_LED;
                    SysTick_DelayMs(600);
                    GPIO_PORTF_DATA_R&=~RED_LED;
                    CURRENT_STATE = MAIN_MENU;
                    lastAction = 0;
                }
            }
        }
        break;

    /*----------------------------------------
             6) SET AUTO LOCK
    ----------------------------------------*/
    case SET_AUTO_LOCK:
    {
        unsigned int tt;
        Pot_Read_Timeout(&tt);

        LCD_command(0x01);
        char msg[20];
        sprintf(msg, "Timeout: %u", tt);
        LCD_writeOrMenu(msg);

        char kk = readKey();
        if(kk == '*')
        { 
            UART0_Transmit((tt/10) + '0');
            UART0_Transmit((tt%10) + '0');
  
            ResetBuffer();
            // After sending timeout digits controller will respond with 'S' or '!'
            CURRENT_STATE = WAIT_SAVE_RESPONSE;
            // clear lastAction now that we've finished the action
            lastAction = 0;
        }
        break;
    }
    
    /*----------------------------------------
             7) TIMEOUT
    ----------------------------------------*/
    case TIMEOUT:
    {
        LCD_command(0x01);
        LCD_writeOrMenu("!!! LOCKOUT !!!");
        //GPIO_PORTF_DATA_R|=RED_LED;

        if(bufferIndex > 0)
        {
            resp = BUFFER[0];
            ResetBuffer();

            if(resp == 'S')
            {
                if(lastAction == 'I'){
                    CURRENT_STATE = INITIAL_STATE;
                }
                
                else{
                    CURRENT_STATE = MAIN_MENU;
                }
                GPIO_PORTF_DATA_R&=~RED_LED;
                lastAction = 0;
                    
            }
            else
            {
                LCD_command(0x01);
                LCD_writeOrMenu("Error!");
                SysTick_DelayMs(700);
                CURRENT_STATE = MAIN_MENU;
                GPIO_PORTF_DATA_R&=~RED_LED;
            }
        }
        
        break;
    }

    /*----------------------------------------
           NEW PASSWORD ENTRY
    ----------------------------------------*/
    /*
    case NEW_PASSWORD:
        // Prompt and collect new password of length PASSWORD_LEN
          LCD_command(0x01);
   SysTick_DelayMs(2);
       LCD_command(0x80);
    LCD_writeOrMenu("Enter new Password:");
       LCD_command(0xC0);  

        char PASSWORD[PASSWORD_LENGTH];
        char CONFRIM_PASSWORD[PASSWORD_LENGTH];

        {
            int i1 = 0;
            while(i1 < PASSWORD_LEN )
            {
                char k2 = readKey();
                if(k2 != 0 && k2!='*')
                {
                    SysTick_DelayMs(20);

                        PASSWORD[i1] = k2;
                        LCD_data('*');
                        i1++;
                        while(readKeypad() != 0) SysTick_DelayMs(5);
                    
                }
            }
            int i2 = 0;
   
            while(i2 < PASSWORD_LEN )
            {
                char k3 = readKey();
                if(k3 != 0 && k3!='*')
                {
                    SysTick_DelayMs(20);

                        CONFRIM_PASSWORD[i2] = k3;
                        LCD_data('*');
                        i2++;
                        while(readKeypad() != 0) SysTick_DelayMs(5);
                    
                }
            }
            if(strcmp(PASSWORD, CONFRIM_PASSWORD) == 0)
            {
                 UART0_Send_String(PASSWORD);
                   ResetBuffer();
            CURRENT_STATE = WAIT_SAVE_RESPONSE;
            break;

            }
        }
        
        UART0_Send_String(PASSWORD);

        ResetBuffer();
        CURRENT_STATE = WAIT_SAVE_RESPONSE;
        break;*/
/*
    case NEW_PASSWORD:
{
    LCD_command(0x01);
    SysTick_DelayMs(2);
    LCD_command(0x80);
    LCD_writeOrMenu("Enter New Password:");
    LCD_command(0xC0);

    char PASSWORD[PASSWORD_LENGTH + 1] = {0};
    char CONFIRM_PASSWORD[PASSWORD_LENGTH + 1] = {0};

    int i = 0;

    // ===== Enter new password =====
    while(i < PASSWORD_LEN)
    {
        char k = readKey();
        if(k != 0)
        {
            if(k == '*')   // Exit state and go to confirm
            {
            
                 i = 0;

    // ===== Confirm password =====
    while(i < PASSWORD_LEN)
    {
        char k = readKey();
        if(k != 0)
        {
            if(k == '*')   // Exit state
            {
                CURRENT_STATE = MAIN_MENU;
                break;
            }

            CONFIRM_PASSWORD[i++] = k;
            LCD_data('*');
            while(readKeypad() != 0);
        }
    }
            }

            PASSWORD[i++] = k;
            LCD_data('*');
            while(readKeypad() != 0);
        }
    }

    if(CURRENT_STATE != NEW_PASSWORD)
        break;

    LCD_command(0x01);
    LCD_writeOrMenu("Confirm Password:");
    LCD_command(0xC0);

  

    if(CURRENT_STATE != NEW_PASSWORD)
        break;

    // ===== Compare passwords =====
    if(strcmp(PASSWORD, CONFIRM_PASSWORD) == 0)
    {
        UART0_Send_String(PASSWORD);
        ResetBuffer();
        CURRENT_STATE = WAIT_SAVE_RESPONSE;
    }
    else
    {
        LCD_command(0x01);
        LCD_writeOrMenu("Mismatch!");
        SysTick_DelayMs(1000);
        CURRENT_STATE = NEW_PASSWORD;
    }

    break;
}
*/

case NEW_PASSWORD:
{
  
  lastAction='-';
  enterPassword(2);
  enterPassword(3);
  CURRENT_STATE = WAIT_INITIAL_PASSWORD_RESPONSE;
   break;
  
  /*


    char PASSWORD[PASSWORD_LENGTH + 1] = {0};
    char CONFIRM_Password[PASSWORD_LENGTH + 1] = {0};

    int i = 0;
    LCD_command(0x01);
    SysTick_DelayMs(2);
    LCD_writeOrMenu("Enter New Password:");
    LCD_command(0xC0);

    while (i < PASSWORD_LENGTH)
    {
        char k = readKey();
        if (k != 0)
        {
            if (k == '*')   // Go to confirm immediately
                break;

            PASSWORD[i++] = k;
            LCD_data('*');
            while (readKeypad() != 0);
        }
    }
    
    
    

    PASSWORD[i] = '\0';  // Null-terminate


    LCD_command(0x01);
    SysTick_DelayMs(2);
    LCD_writeOrMenu("Confirm Password:");
    LCD_command(0xC0);

    int i2 = 0;
    while (i2 < PASSWORD_LENGTH)
    {
        char k = readKey();
        if (k != 0)
        {
            if (k == '*')   // Allow early confirm
                break;

            CONFIRM_Password[i2++] = k;
            LCD_data('*');
            while (readKeypad() != 0);
        }
    }

    CONFIRM_Password[i2] = '\0';

  
    if (strcmp(PASSWORD, CONFIRM_Password) == 0)
    {
        UART0_Send_String(PASSWORD);
        ResetBuffer();
        CURRENT_STATE = WAIT_SAVE_RESPONSE;
    }
    else
    {
        LCD_command(0x01);
        LCD_writeOrMenu("Password Mismatch");
        SysTick_DelayMs(1000);
        CURRENT_STATE = MAIN_MENU;
    }
  */
   
}

    /*----------------------------------------
           WAIT FOR SAVE/SETTIME RESPONSE
    ----------------------------------------*/
    case WAIT_SAVE_RESPONSE:
        if(bufferIndex > 0)
        {
            resp = BUFFER[0];
            ResetBuffer();

            if(resp == 'S')
            {
                failCount = 0;
                LCD_command(0x01);
                LCD_writeOrMenu("Saved");
                GPIO_PORTF_DATA_R|=GREEN_LED;
                SysTick_DelayMs(500);
                GPIO_PORTF_DATA_R&=~GREEN_LED;
            }
            else
            {
                LCD_command(0x01);
                LCD_writeOrMenu("Error!");
                GPIO_PORTF_DATA_R|=RED_LED;
                SysTick_DelayMs(700);
                GPIO_PORTF_DATA_R&=~RED_LED;
            }
            CURRENT_STATE = MAIN_MENU;
            lastAction = 0;
        }
        break;

    /*----------------------------------------
           WAIT FOR DOOR OPEN ACK
    ----------------------------------------*/
    case WAIT_DOOR_ACK:
        if(bufferIndex > 0)
        {
            resp = BUFFER[0];
            ResetBuffer();
            if(resp == 'S')
            {
                GPIO_PORTF_DATA_R&=~GREEN_LED;
            }
            else
            {
                LCD_command(0x01);
                LCD_writeOrMenu("Error!");
                GPIO_PORTF_DATA_R|=RED_LED;
                SysTick_DelayMs(700);
                GPIO_PORTF_DATA_R&=~RED_LED;
            }
            CURRENT_STATE = MAIN_MENU;
            lastAction = 0;
        }
        break;        
    } // switch end
}