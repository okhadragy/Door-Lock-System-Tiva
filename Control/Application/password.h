#ifndef PASSWORD_H
#define PASSWORD_H
#include "../config.h"

void comparePassword(volatile uint8_t *pass1, const uint8_t pass1_length, const uint8_t *pass2, uint8_t *result);

#endif