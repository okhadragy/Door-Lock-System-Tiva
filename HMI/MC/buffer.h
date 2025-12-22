#ifndef BUFFER_H
#define BUFFER_H
#include "../config.h"
//PREVENT FLUSHING
volatile extern char BUFFER[PASSWORD_LENGTH];
 volatile extern unsigned int bufferIndex;

void ResetBuffer(void);

#endif