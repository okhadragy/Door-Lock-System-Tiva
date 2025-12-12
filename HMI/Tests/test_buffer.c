#include "test_functions.h"
#include "../MC/buffer.h"

extern char BUFFER[]; 
extern unsigned int bufferIndex;

void Test_Buffer_Logic(void) {
    BUFFER[0] = 'A';
    BUFFER[1] = 'B';
    bufferIndex = 2;

    ResetBuffer();

    ASSERT_TRUE(bufferIndex == 0);
    ASSERT_TRUE(BUFFER[0] == 0);
}