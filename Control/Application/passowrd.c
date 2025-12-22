#include "password.h"

void comparePassword(volatile uint8_t *pass1, const uint8_t pass1_length, const uint8_t *pass2, uint8_t *result)
{
    if (pass1_length != PASSWORD_LENGTH)
    {
        *result = 0; // length mismatch
        return;
    }
    
    for (int i = 0; i < PASSWORD_LENGTH; i++)
    {
        if (pass1[i] != pass2[i])
        {
            *result = 0; // mismatch
            return;
        }
    }
    *result = 1; // match
}