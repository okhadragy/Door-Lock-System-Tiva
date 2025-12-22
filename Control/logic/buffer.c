#include "buffer.h"

volatile uint8_t BUFFER[2*PASSWORD_LENGTH];
volatile uint8_t bufferIndex = 0;

void ResetBuffer(void)
{
    __asm(" CPSID I");
    bufferIndex = 0;
    __asm(" CPSIE I");
}
