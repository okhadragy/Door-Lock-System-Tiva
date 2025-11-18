#include "registers.h"

#define PASSWORD_LENGTH 8

char passwordBuffer[PASSWORD_LENGTH];
unsigned int passwordIndex = 0;
char correctPassword[PASSWORD_LENGTH];

int comparePassword(void)
{
    for (int i = 0; i < PASSWORD_LENGTH; i++)
    {
        if (passwordBuffer[i] != correctPassword[i])
        {
            return 0; // mismatch
        }
    }
    return 1; // match
}

void Buzzer_Init(void)
{
    enable_gpio(GPIO_PORTE);          // Enable clock for Port E
    GPIO_PORTE_DIR_R |= BUZZER_PIN;   // Set PE0 as output
    GPIO_PORTE_DEN_R |= BUZZER_PIN;   // Enable digital function
    GPIO_PORTE_DATA_R &= ~BUZZER_PIN; // Initialize Buzzer OFF
}

/**
 * @param state 1 to turn ON, 0 to turn OFF.
 */
void Buzzer_Output(int state)
{
    if (state)
    {
        GPIO_PORTE_DATA_R |= BUZZER_PIN; // Turn ON the buzzer
    }
    else
    {
        GPIO_PORTE_DATA_R &= ~BUZZER_PIN;
    }
}

void Timer0A_Init(void)
{
    SYSCTL_RCGCTIMER_R |= 0x01;
    while ((SYSCTL_PRTIMER_R & 0x01) == 0)
    {
    }
    TIMER0_CTL_R = 0x00000000;
    TIMER0_CFG_R = 0x00000000;
    TIMER0_TAMR_R = 0x00000001; // one-shot mode
    TIMER0_ICR_R = 0x00000001;
    TIMER0_IMR_R = 0x00000001;
    NVIC_PRI4_R = (NVIC_PRI4_R & 0x00FFFFFF) | 0x80000000;
    NVIC_EN0_R = 1 << 19;
}

void Timer0A_DelayMs(unsigned int ms)
{
    TIMER0_CTL_R = 0x00000000;   // disable during setup
    TIMER0_TAILR_R = 16000 * ms; // set reload value
    TIMER0_ICR_R = 0x00000001;   // clear any old flag
    TIMER0_CTL_R = 0x00000001;   // start one-shot
}

void Timer0AIntHandler(void)
{
    TIMER0_ICR_R = TIMER_ICR_TATOCINT; // clear flag
    GPIO_PORTF_DATA_R ^= RED_LED;
}

void EEPROM_Init(void)
{
    SYSCTL_RCGCEEPROM_R |= 1;
    while (EEPROM_EEDONE_R & 0x01)
        ; // Wait until not working
}

void EEPROM_WritePassword(const char *pass)
{
    EEPROM_EEBLOCK_R = 0;  // Use block 0
    EEPROM_EEOFFSET_R = 0; // Start at offset 0

    unsigned int word1 = (pass[3] << 24) | (pass[2] << 16) | (pass[1] << 8) | pass[0];
    unsigned int word2 = (pass[7] << 24) | (pass[6] << 16) | (pass[5] << 8) | pass[4];

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
    pass[5] = (word2 >> 8) & 0xFF;
    pass[6] = (word2 >> 16) & 0xFF;
    pass[7] = (word2 >> 24) & 0xFF;
}

void UART0_Init(void)
{
    // clock
    SYSCTL_RCGCUART_R |= (1 << 0);
    enable_gpio((GPIO_PORTA | GPIO_PORTF));

    // configure A0 as RX and A1 as TX
    GPIO_PORTA_AFSEL_R |= 0x03;
    GPIO_PORTA_PCTL_R = (GPIO_PORTA_PCTL_R & ~0xFF) | 0x11;
    GPIO_PORTA_DEN_R |= 0x03;

    // disable UART to reconfigure
    UART0_CTL_R &= ~0x01;

    // configure 9600 baud rate
    UART0_IBRD_R = 104;
    UART0_FBRD_R = 11;

    UART0_LCRH_R = 0x76;  // 8-bit, even parity, FIFO enabled
    UART0_CTL_R |= 0x301; // Enable RXE, TXE, UART

    // Enable UART0 interrupt
    UART0_IM_R |= 0x10;     // RX interrupt enable
    NVIC_EN0_R |= (1 << 5); // NVIC interrupt enable for UART0
}

void UART0_Transmit(char data)
{
    while (UART0_FR_R & 0x20)
    {
    }; // Wait while TX FIFO full
    UART0_DR_R = data;
}

void UART0_SendString(const char *str)
{
    while (*str)
    {
        UART0_Transmit(*str++);
    }
}

void UART0IntHandler(void)
{
    if (UART0_MIS_R & 0x10)
    {                                   // check RX interrupt flag = bit 4
        unsigned int data = UART0_DR_R; // Read data + error bits
        char c = data & 0xFF;           // Extract received byte (lowest 8 bits)

        if (data & 0xF00)
        { // Check for any error flags in bits [11:8] OE = Overrun, BE = Break, PE = Parity, FE = Framing)
            if (data & 0x200)
            { // Parity error bit (PE)
                UART0_Transmit('!');
            }
        }
        else
        {
            passwordBuffer[passwordIndex++] = c;
            if (passwordIndex == PASSWORD_LENGTH)
            {
                if (comparePassword())
                {
                    UART0_Transmit('1');
                    GPIO_PORTF_DATA_R |= BLUE_LED;
                }
                else
                {
                    UART0_Transmit('0');
                    GPIO_PORTF_DATA_R |= RED_LED;
                }
                passwordIndex = 0;
                Timer0A_DelayMs(2000);
            }
            else
            {
                UART0_Transmit('#');
            }
        }

        UART0_ICR_R = 0x10; // Clear RX interrupt flag
    }
}

int main()
{
    const char newPassword[] = "12345678";
    EEPROM_Init();
    EEPROM_WritePassword(newPassword);
    EEPROM_ReadPassword(correctPassword);
    UART0_Init();
    Timer0A_Init();
    Buzzer_Init();
    enable_gpio(GPIO_PORTF);
    __asm("CPSIE I");

    GPIO_PORTF_DIR_R |= 0x0E;
    GPIO_PORTF_LOCK_R = 0x4C4F434B;
    GPIO_PORTF_CR_R |= 0x01;
    GPIO_PORTF_DEN_R |= 0x1F;
    GPIO_PORTF_PUR_R |= 0x11;

    int counter = 0;
    for (char i = '1'; i <= '8'; i++)
    {
        passwordBuffer[counter++] = i;
    }

    if (comparePassword())
    {
        GPIO_PORTF_DATA_R |= BLUE_LED;
    }
    else
    {
        GPIO_PORTF_DATA_R |= RED_LED;
    }

    Timer0A_DelayMs(1000);

    for (;;)
    {
        unsigned int input = GPIO_PORTF_DATA_R;
        GPIO_PORTF_DATA_R &= 0;

        if (!(input & SW1))
        {
            GPIO_PORTF_DATA_R |= RED_LED;
        }

        if (!(input & SW2))
        {
            GPIO_PORTF_DATA_R |= BLUE_LED;
        }

        // Timer0A_DelayMs(1000);

        // __asm("      wfi\n");
    }
}
