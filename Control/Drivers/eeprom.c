#include "eeprom.h"

void EEPROM_Init(void)
{
    SYSCTL_RCGCEEPROM_R |= 1;
    while (EEPROM_EEDONE_R & 0x01)
        ; // Wait until not working

    unsigned int def = 5; // DEFAULT BEFORE FIRST SET: 5
    EEPROM_WriteTimeout(&def);
}

void EEPROM_WritePassword(const char *pass)
{
    EEPROM_EEBLOCK_R = 0;  // Use block 0
    EEPROM_EEOFFSET_R = 0; // Start at offset 0

    unsigned int word1 = (pass[3] << 24) | (pass[2] << 16) | (pass[1] << 8) | pass[0];
    unsigned int word2 = (0x00 << 24) | (0x00 << 16) | (0x00 << 8) | pass[4];

    EEPROM_EERDWR_R = word1;
    while (EEPROM_EEDONE_R & 0x01)
        ;
    EEPROM_EEOFFSET_R = 1;
    EEPROM_EERDWR_R = word2;
    while (EEPROM_EEDONE_R & 0x01)
    {
    };
}

void EEPROM_ReadPassword(char *pass)
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

void convertTimeoutToSec(const char *timeoutStr, unsigned int *timeoutSec, unsigned int *result)
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

void EEPROM_WriteTimeout(const unsigned int *timeoutSec)
{
    EEPROM_EEBLOCK_R = 0;
    EEPROM_EEOFFSET_R = 2; // Offset 2 for timeout

    EEPROM_EERDWR_R = *timeoutSec;
    while (EEPROM_EEDONE_R & 0x01)
    {
    };
}

void EEPROM_ReadTimeout(unsigned int *timeoutSec)
{
    EEPROM_EEBLOCK_R = 0;
    EEPROM_EEOFFSET_R = 2; // Offset 2 for timeout

    *timeoutSec = EEPROM_EERDWR_R;
    while (EEPROM_EEDONE_R & 0x01)
    {
    };
}