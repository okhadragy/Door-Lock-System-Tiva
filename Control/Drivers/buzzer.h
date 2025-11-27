#ifndef BUZZER_H
#define BUZZER_H
#include "../config.h"

void Buzzer_Init(void);
void Buzzer_Output(int state);
void Buzzer_ON(unsigned int durationMs);

#endif