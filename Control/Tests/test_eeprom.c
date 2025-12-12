#include "test_functions.h"
#include "../Drivers/eeprom.h"

void test_EEPROM_Init(void)
{
    // Verify EEPROM Clock Gating (same check as original)
    ASSERT_TRUE((SYSCTL_RCGCEEPROM_R & 0x01) == 0x01);
}

void test_EEPROM_PasswordPersistence(void)
{
    EEPROM_Init();

    // Test Password Persistence (exactly the same as original)
    char writePass[6] = "12345";
    char readPass[6]  = {0};

    EEPROM_WritePassword(writePass);
    EEPROM_ReadPassword(readPass);

    unsigned int match = 1;
    for (int i = 0; i < 5; i++) {
        if (writePass[i] != readPass[i]) match = 0;
    }
    ASSERT_FALSE(match == 0);
}

void test_convertTimeoutToSec_valid(void)
{
    unsigned int sec = 0, res = 0;

    // Valid Case (Two digits as received from the potentiometer)
    convertTimeoutToSec("22", &sec, &res);
    ASSERT_TRUE(sec == 22);
    ASSERT_TRUE(res == 1);
}

void test_convertTimeoutToSec_invalid(void)
{
    unsigned int sec = 0, res = 0;

    // Invalid Case (Error Handling) (same as original)
    convertTimeoutToSec("9A", &sec, &res);
    ASSERT_TRUE(res == 0);
}

void test_EEPROM_TimeoutPersistence(void)
{
    EEPROM_Init();

    // Test Timeout Persistence (same values as original)
    unsigned int writeTime = 30;
    unsigned int readTime = 0;

    EEPROM_WriteTimeout(&writeTime);
    EEPROM_ReadTimeout(&readTime);

    ASSERT_TRUE(writeTime == readTime);
}

void Run_All_EEPROM_Tests(void)
{
    test_EEPROM_Init();
    test_EEPROM_PasswordPersistence();
    test_convertTimeoutToSec_valid();
    test_convertTimeoutToSec_invalid();
    test_EEPROM_TimeoutPersistence();
}
