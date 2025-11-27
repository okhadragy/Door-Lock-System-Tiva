#ifndef BUFFER_H
#define BUFFER_H
#include "../config.h"

extern char buffer[PASSWORD_LENGTH];
extern unsigned int bufferIndex;

void ResetBuffer(void);

#endif