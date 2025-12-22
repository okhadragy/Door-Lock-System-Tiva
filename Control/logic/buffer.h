#ifndef BUFFER_H
#define BUFFER_H
#include "../config.h"
//PREVENT FLUSHING
volatile extern uint8_t BUFFER[2*PASSWORD_LENGTH];
volatile extern uint8_t bufferIndex;

void ResetBuffer(void);

#endif