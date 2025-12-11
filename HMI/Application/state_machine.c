#include "state_machine.h"
#include "../HW/LCD.h"
#include "../HW/keypad.h"
#include "../MC/uart.h"
#include "../MC/buffer.h"
#include "../HW/potentiometer.h"
#include "../MC/SysTick_Driver.h"
#include <stdio.h>
#include <string.h>

STATE CURRENT_STATE;
unsigned char PASSWORD[PASSWORD_LEN];
unsigned int auto_lock_timeout = 5;

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
        SysTick_DelayMs(20);
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
static void enterPassword()
{
    int i = 0;

    LCD_command(0x01);
    SysTick_DelayMs(2);
    LCD_writeOrMenu("Enter Password:");

    char PASSWORD[PASSWORD_LENGTH];

    while(i < PASSWORD_LEN)
    {
        char k = readKey();
        if(k != 0)
        {
            PASSWORD[i] = k;
            LCD_writeOrMenu("*");
            i++;
        }
    }

    UART0_Send_String(PASSWORD);

    ResetBuffer();
}

/*----------------------------------------
         INIT
----------------------------------------*/
void INIT_MACHINE()
{
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
        enterPassword();
        CURRENT_STATE = WAIT_INITIAL_PASSWORD_RESPONSE;
        break;

    case WAIT_INITIAL_PASSWORD_RESPONSE:
        if(bufferIndex > 0)
        {
            resp = BUFFER[0];
            ResetBuffer();

            if(resp == 'S')
            {
                LCD_command(0x01);
                LCD_writeOrMenu("Saved");
                SysTick_DelayMs(500);
                CURRENT_STATE = MAIN_MENU;
            }
            else
            {
                LCD_command(0x01);
                LCD_writeOrMenu("Wrong");
                SysTick_DelayMs(600);
                CURRENT_STATE = INITIAL_STATE;
            }
        }
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
                // ACK received. For all actions the controller expects a password
                // to be entered next (controller moves to check-password state).
                // Preserve lastAction so we can branch after password validation.
                CURRENT_STATE = ENTER_PASSWORD;
            }
            else
            {
                LCD_command(0x01);
                LCD_writeOrMenu("Error!");
                SysTick_DelayMs(700);
                CURRENT_STATE = MAIN_MENU;
            }
        }
        break;

    /*----------------------------------------
               4) ENTER PASSWORD
    ----------------------------------------*/
    case ENTER_PASSWORD:
        enterPassword();
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
                // Password matched. Branch based on the original action.
                if(lastAction == '+')
                {
                    // Door open flow: controller will send another 'S' after opening.
                    LCD_command(0x01);
                    LCD_writeOrMenu("Password OK");
                    SysTick_DelayMs(400);
                    CURRENT_STATE = WAIT_DOOR_ACK;
                }
                else if(lastAction == '-')
                {
                    // Change password: prompt for new password
                    CURRENT_STATE = NEW_PASSWORD;
                }
                else if(lastAction == '*')
                {
                    // Set timeout: controller will now accept timeout digits
                    CURRENT_STATE = SET_AUTO_LOCK;
                }
                else
                {
                    LCD_command(0x01);
                    LCD_writeOrMenu("Correct");
                    SysTick_DelayMs(600);
                    CURRENT_STATE = MAIN_MENU;
                }
                // we'll clear lastAction later after completing the action
            }
            else
            {
                LCD_command(0x01);
                LCD_writeOrMenu("Wrong");
                SysTick_DelayMs(600);
                CURRENT_STATE = MAIN_MENU;
                lastAction = 0;
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
            // Send two ASCII digits (tens and units). Adjust if tt can exceed 99.

            // UART0_Transmit((tt/1000)+ '0'); // OLD
            // UART0_Transmit((tt/100)+ '0'); // OLD
            // UART0_Transmit((tt/10) + '0'); // OLD
            // UART0_Transmit((tt%1000) + '0'); // OLD

            UART0_Transmit((tt/10) + '0');
            UART0_Transmit((tt%10)+ '0');

  
            ResetBuffer();
            // After sending timeout digits controller will respond with 'S' or '!'
            CURRENT_STATE = WAIT_SAVE_RESPONSE;
            // clear lastAction now that we've finished the action
            lastAction = 0;
        }
        break;
    }

    /*----------------------------------------
           NEW PASSWORD ENTRY
    ----------------------------------------*/
    case NEW_PASSWORD:
        // Prompt and collect new password of length PASSWORD_LEN
        LCD_command(0x01);
        SysTick_DelayMs(2);
        LCD_writeOrMenu("Enter New Password:");

        char PASSWORD[PASSWORD_LENGTH];

        {
            int i = 0;
            while(i < PASSWORD_LEN)
            {
                char k2 = readKey();
                if(k2 != 0)
                {
                    SysTick_DelayMs(20);
                    if(readKeypad() == k2)
                    {
                        PASSWORD[i] = k2;
                        LCD_data('*');
                        i++;
                        while(readKeypad() != 0) SysTick_DelayMs(5);
                    }
                }
            }
        }
        
        UART0_Send_String(PASSWORD);

        ResetBuffer();
        CURRENT_STATE = WAIT_SAVE_RESPONSE;
        break;

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
                LCD_command(0x01);
                LCD_writeOrMenu("Saved");
                SysTick_DelayMs(500);
            }
            else
            {
                LCD_command(0x01);
                LCD_writeOrMenu("Error!");
                SysTick_DelayMs(700);
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
                LCD_command(0x01);
                LCD_writeOrMenu("Door Opened");
                SysTick_DelayMs(700);
            }
            else
            {
                LCD_command(0x01);
                LCD_writeOrMenu("Error!");
                SysTick_DelayMs(700);
            }
            CURRENT_STATE = MAIN_MENU;
            lastAction = 0;
        }
        break;

    } // switch end
}