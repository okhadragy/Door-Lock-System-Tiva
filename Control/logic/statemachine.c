#include "statemachine.h"
#include "password.h"
#include "buffer.h"
#include "../Drivers/timer.h"
#include "../Drivers/eeprom.h"
#include "../Drivers/motor.h"
#include "../Drivers/buzzer.h"
#include "../Drivers/uart.h"

int failCount = 0;
SystemState currentState = STATE_INIT;
SystemActions currentAction;

void StateMachine(void) {
    switch (currentState) {
        case STATE_INIT:
        case STATE_CHANGE_PASSWORD:
            if (bufferIndex != PASSWORD_LENGTH) {
                return;
            }

            EEPROM_WritePassword(buffer);
            ResetBuffer();
            UART0_SendString("S");
            currentState = STATE_MAIN_MENU;
            break;

        case STATE_MAIN_MENU:
            if (bufferIndex != 1) {
                return;
            }
            
            switch(buffer[0]) {
                case '+':
                    currentAction = ACTION_OPEN_DOOR;
                    UART0_SendString("S");
                    currentState = STATE_CHECK_PASSWORD;
                    break;
                case '-':
                    currentAction = ACTION_CHANGE_PASSWORD;
                    UART0_SendString("S");
                    currentState = STATE_CHECK_PASSWORD;
                    break;
                case '*':
                    currentAction = ACTION_SET_TIMEOUT;
                    UART0_SendString("S");
                    currentState = STATE_CHECK_PASSWORD;
                    break;
                default:
                    UART0_SendString("!");
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
                currentAction == ACTION_SET_TIMEOUT)) {
                UART0_SendString("!");
                ResetBuffer();
                currentState = STATE_MAIN_MENU;
                return;
            }

            char correctPassword[PASSWORD_LENGTH];
            EEPROM_ReadPassword(correctPassword);
            unsigned int passwordMatch;
            comparePassword(buffer, bufferIndex, correctPassword, &passwordMatch);
            
            if (passwordMatch) {
                UART0_SendString("S");
                if (currentAction == ACTION_OPEN_DOOR) {
                    open_Door();
                    UART0_SendString("S");
                    currentState = STATE_MAIN_MENU;
                } else if (currentAction == ACTION_CHANGE_PASSWORD) {
                    currentState = STATE_CHANGE_PASSWORD;
                } else if (currentAction == ACTION_SET_TIMEOUT) {
                    currentState = STATE_SET_TIMEOUT;
                }
            } else {
                UART0_SendString("F");
                failCount++;
                if (failCount >= MAX_FAILS) {
                    unsigned int timeoutSec;
                    Buzzer_ON(5000);
                    EEPROM_ReadTimeout(&timeoutSec);
                    Timer0A_DelayMs(timeoutSec * 1000);
                    currentState = STATE_MAIN_MENU;
                    failCount = 0;
                }
            }
            ResetBuffer();
            break;
        case STATE_SET_TIMEOUT:
            if (bufferIndex != TIMEOUT_LENGTH) {
                return;
            }

            unsigned int timeoutSec;
            unsigned int conversionResult;
            convertTimeoutToSec(buffer, &timeoutSec, &conversionResult);
            if (!conversionResult) {
                UART0_SendString("!");
                ResetBuffer();
                currentState = STATE_MAIN_MENU;
                break;
            }

            EEPROM_WriteTimeout(&timeoutSec);
            ResetBuffer();
            UART0_SendString("S");
            currentState = STATE_MAIN_MENU;
            break;
        default:
            break;
    }
}