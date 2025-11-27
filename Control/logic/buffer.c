#include "buffer.h"

char buffer[PASSWORD_LENGTH];
unsigned int bufferIndex = 0;

void ResetBuffer(void)
{
    for (int i = 0; i < bufferIndex; i++)
    {
        buffer[i] = 0;
    }
    bufferIndex = 0;
}
