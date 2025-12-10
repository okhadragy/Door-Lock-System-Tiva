#include "buffer.h"

char BUFFER[BUFFER_SIZE];
unsigned int bufferIndex = 0;

void ResetBuffer(void)
{
    for (unsigned int i = 0; i < bufferIndex; i++)
        BUFFER[i] = 0;

    bufferIndex = 0;
}
