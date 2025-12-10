#ifndef POTENTIOMETER_H
#define POTENTIOMETER_H
unsigned int TIMEOUT;
void Pot_Init(void);
void Pot_Read_Timeout(unsigned int *timeout);

#endif