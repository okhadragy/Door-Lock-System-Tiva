#include "statemachine.h"
#include "password.h"
#include "buffer.h"
#include "../Drivers/timer.h"
#include "../Drivers/eeprom.h"
#include "../Drivers/motor.h"
#include "../Drivers/buzzer.h"
#include "../Drivers/uart.h"

int failCount = 0;
SystemState currentState = STATE_CHECK_PASSWORD;
SystemActions currentAction = ACTION_INIT;
uint8_t correctPassword[PASSWORD_LENGTH];
int isChecking = 0;

void StateMachine(void) {
    switch (currentState) {
        case STATE_CHANGE_PASSWORD:
            if (bufferIndex != PASSWORD_LENGTH) {
                return;
            }

            EEPROM_WritePassword(BUFFER);
            ResetBuffer();
            UART0_Send_String("S");
            currentState = STATE_MAIN_MENU;
            break;

        case STATE_MAIN_MENU:
            if (bufferIndex != 1) {
                return;
            }
            
            switch(BUFFER[0]) {
                case '+':
                    currentAction = ACTION_OPEN_DOOR;
                    UART0_Send_String("S");
                    currentState = STATE_CHECK_PASSWORD;
                    break;
                case '-':
                    currentAction = ACTION_CHANGE_PASSWORD;
                    UART0_Send_String("S");
                    currentState = STATE_CHECK_PASSWORD;
                    break;
                case '*':
                    currentAction = ACTION_SET_TIMEOUT;
                    UART0_Send_String("S");
                    currentState = STATE_CHECK_PASSWORD;
                    break;
                default:
                    UART0_Send_String("!");
                    currentState = STATE_MAIN_MENU;
                    break;
            }
            ResetBuffer();
            break;
            
        case STATE_CHECK_PASSWORD:
            if (bufferIndex != PASSWORD_LENGTH) {
                return;
            } 
              
            if (!(currentAction == ACTION_OPEN_DOOR ||
                currentAction == ACTION_CHANGE_PASSWORD ||
                currentAction == ACTION_SET_TIMEOUT ||
                currentAction == ACTION_INIT  )) {
                UART0_Send_String("!");
                ResetBuffer();
                currentState = STATE_CHECK_PASSWORD; //
                currentAction = ACTION_INIT; //
                return;
            }
            
            if(currentAction == ACTION_INIT && !isChecking){
              GPIO_PORTF_DEN_R|=RED_LED;
              GPIO_PORTF_DIR_R|=RED_LED;
              GPIO_PORTF_DATA_R=RED_LED;
              isChecking = 1;
              for(int i =0; i<PASSWORD_LENGTH; i++){
               correctPassword[i] = BUFFER[i];
              }
              ResetBuffer();
              return;
            }else if(currentAction != ACTION_INIT){
              EEPROM_ReadPassword(correctPassword);
           
            }
            uint8_t passwordMatch;
            comparePassword(BUFFER, bufferIndex, correctPassword, &passwordMatch);
            
            if (passwordMatch) {
                UART0_Send_String("S");
                failCount = 0;
                if (currentAction == ACTION_INIT){
                  EEPROM_WritePassword(BUFFER);
                  currentState = STATE_MAIN_MENU;
                }
                else if (currentAction == ACTION_OPEN_DOOR) {
                    open_Door();
                    Timer0A_DelayMs(3000); // wait 3 seconds to close door again
                    close_Door();
                    UART0_Send_String("S");
                    currentState = STATE_MAIN_MENU;
                } else if (currentAction == ACTION_CHANGE_PASSWORD) {
                    currentState = STATE_CHANGE_PASSWORD;
                } else if (currentAction == ACTION_SET_TIMEOUT) {
                    currentState = STATE_SET_TIMEOUT;
                } 
            } else {
                UART0_Send_String("F");
                failCount++;
                if (failCount >= MAX_FAILS) {
                    uint8_t timeoutSec;
                    Buzzer_ON(5000);
                    EEPROM_ReadTimeout(&timeoutSec);
                    Timer0A_DelayMs(timeoutSec * 1000);
                    UART0_Send_String("S");
                    currentState = STATE_MAIN_MENU;
                    failCount = 0;
                }
            }
            isChecking = 0;
            ResetBuffer();
            break;
            
        case STATE_SET_TIMEOUT:
            if (bufferIndex != TIMEOUT_LENGTH) {
                return;
            }

            uint8_t timeoutSec;
            uint8_t conversionResult;
            convertTimeoutToSec(BUFFER, &timeoutSec, &conversionResult);
            if (!conversionResult) {
                UART0_Send_String("!");
                ResetBuffer();
                currentState = STATE_MAIN_MENU;
                break;
            }

            EEPROM_WriteTimeout(&timeoutSec);
            ResetBuffer();
            UART0_Send_String("S");
            currentState = STATE_MAIN_MENU;
            break;
        default:
            break;
    }
}