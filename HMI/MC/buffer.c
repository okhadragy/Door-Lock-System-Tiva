#include "buffer.h"

volatile char BUFFER[PASSWORD_LENGTH];
volatile unsigned int bufferIndex = 0;

void ResetBuffer(void)
{
    for (int i = 0; i < bufferIndex; i++)
    {
        BUFFER[i] = 0;
    }
    bufferIndex = 0;
}
