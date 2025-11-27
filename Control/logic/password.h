#ifndef PASSWORD_H
#define PASSWORD_H
#include "../config.h"

void comparePassword(const char *pass1, unsigned int pass1_length, const char *pass2, unsigned int *result);

#endif