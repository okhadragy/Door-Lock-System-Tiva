#ifndef EEPROM_H
#define EEPROM_H
#include "../config.h"

void EEPROM_Init(void);
void EEPROM_WritePassword(const char *pass);
void EEPROM_ReadPassword(char *pass);
void convertTimeoutToSec(const char *timeoutStr, unsigned int *timeoutSec, unsigned int *result);
void EEPROM_WriteTimeout(const unsigned int *timeoutSec);
void EEPROM_ReadTimeout(unsigned int *timeoutSec);

#endif