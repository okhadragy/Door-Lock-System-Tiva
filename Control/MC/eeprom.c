#include "eeprom.h"

void EEPROM_Init(void)
{
    SYSCTL_RCGCEEPROM_R |= 1;
    while (EEPROM_EEDONE_R & 0x01)
        ; // Wait until not working
    
    uint8_t def = 5; // DEFAULT BEFORE FIRST SET: 5
    EEPROM_WriteTimeout(&def);
}

void EEPROM_WritePassword(volatile uint8_t *pass)
{
    EEPROM_EEBLOCK_R = 0;  // Use block 0
    EEPROM_EEOFFSET_R = 0; // Start at offset 0
    unsigned int b0 = pass[0];
    unsigned int b1 = pass[1];
    unsigned int b2 = pass[2];
    unsigned int b3 = pass[3];
    unsigned int b4 = pass[4];

    unsigned int word1 = (b3 << 24)|(b2 << 16)|(b1 << 8)|b0;
    unsigned int word2 = b4;

    EEPROM_EERDWR_R = word1;
    while (EEPROM_EEDONE_R & 0x01)
        ;
    EEPROM_EEOFFSET_R = 1;
    EEPROM_EERDWR_R = word2;
    while (EEPROM_EEDONE_R & 0x01)
    {
    };
}

void EEPROM_ReadPassword(uint8_t *pass)
{
    EEPROM_EEBLOCK_R = 0;
    EEPROM_EEOFFSET_R = 0;
    unsigned int word1 = EEPROM_EERDWR_R;
    EEPROM_EEOFFSET_R = 1;
    unsigned int word2 = EEPROM_EERDWR_R;

    pass[0] = word1 & 0xFF;
    pass[1] = (word1 >> 8) & 0xFF;
    pass[2] = (word1 >> 16) & 0xFF;
    pass[3] = (word1 >> 24) & 0xFF;
    pass[4] = word2 & 0xFF;
}

void convertTimeoutToSec(volatile uint8_t *timeoutStr, uint8_t *timeoutSec, uint8_t *result)
{
    *timeoutSec = 0;
    for (int i = 0; i < TIMEOUT_LENGTH; i++)
    {
        if (timeoutStr[i] < '0' || timeoutStr[i] > '9')
        {
            *timeoutSec = 0;
            *result = 0; 
            return;
        }

        *timeoutSec = (*timeoutSec * 10) + (timeoutStr[i] - '0');
    }
    *result = 1;
}

void EEPROM_WriteTimeout(const uint8_t *timeoutSec)
{
    EEPROM_EEBLOCK_R = 0;
    EEPROM_EEOFFSET_R = 2; // Offset 2 for timeout

    EEPROM_EERDWR_R = *timeoutSec;
    while (EEPROM_EEDONE_R & 0x01)
    {
    };
}

void EEPROM_ReadTimeout(uint8_t *timeoutSec)
{
    EEPROM_EEBLOCK_R = 0;
    EEPROM_EEOFFSET_R = 2; // Offset 2 for timeout

    *timeoutSec = EEPROM_EERDWR_R;
    while (EEPROM_EEDONE_R & 0x01)
    {
    };
}