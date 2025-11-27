#include "password.h"

void comparePassword(const char *pass1, const unsigned int pass1_length, const char *pass2, unsigned int *result)
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