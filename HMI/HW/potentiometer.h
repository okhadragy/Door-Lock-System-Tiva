#ifndef POTENTIOMETER_H
#define POTENTIOMETER_H
#include "../config.h"

void Pot_Init(void);
void Pot_Read_Timeout(unsigned int *timeout);

#endif