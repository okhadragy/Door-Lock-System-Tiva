#ifndef STATE_MACHINE_H_
#define STATE_MACHINE_H_

#include <stdint.h>

#define PASSWORD_LEN 5

typedef enum {
    INITIAL_STATE,
    WAIT_INITIAL_PASSWORD_RESPONSE,
    MAIN_MENU,
    SEND_ACTION_WAIT_ACK,
    ENTER_PASSWORD,
    WAIT_PASSWORD_RESPONSE,
    SET_AUTO_LOCK,
    NEW_PASSWORD,
    WAIT_SAVE_RESPONSE,
    WAIT_DOOR_ACK
} STATE;

extern STATE CURRENT_STATE;
extern unsigned char PASSWORD[PASSWORD_LEN];
extern uint32_t auto_lock_timeout;

void INIT_MACHINE(void);
void STATE_MACHINE(void);

#endif // STATE_MACHINE_H_
