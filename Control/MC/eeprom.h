#ifndef EEPROM_H
#define EEPROM_H
#include "../config.h"

void EEPROM_Init(void);
void EEPROM_WritePassword(volatile uint8_t *pass);
void EEPROM_ReadPassword(uint8_t *pass);
void convertTimeoutToSec(volatile uint8_t *timeoutStr, uint8_t *timeoutSec, uint8_t *result);
void EEPROM_WriteTimeout(const uint8_t *timeoutSec);
void EEPROM_ReadTimeout(uint8_t *timeoutSec);

#endif