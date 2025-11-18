#include "registers.h"

char buffer[PASSWORD_LENGTH];
unsigned int bufferIndex = 0;
int failCount = 0;
SystemState currentState = STATE_INIT;
enum SystemActions currentAction;

// Function prototypes
void StateMachine(void);
void ResetBuffer(void);
void comparePassword(const char *pass1, unsigned int pass1_length, const char *pass2, int *result);
void Buzzer_Init(void);
void Buzzer_Output(int state);
void Buzzer_ON(unsigned int durationMs);
void Timer0A_Init(void);
void Timer0A_DelayMs(unsigned int ms);
void Timer0AIntHandler(void);
void EEPROM_Init(void);
void EEPROM_WritePassword(const char *pass);
void EEPROM_ReadPassword(char *pass);
void convertTimeoutToSec(const char *timeoutStr, unsigned int *timeoutSec, unsigned int *result);
void EEPROM_WriteTimeout(const unsigned int *timeoutSec);
void EEPROM_ReadTimeout(unsigned int *timeoutSec);
void UART0_Init(void);
void UART0_Transmit(char data);
void UART0_SendString(const char *str);
void UART0IntHandler(void);
void open_Door(void);



void StateMachine(void) {
    switch (currentState) {
        case STATE_INIT:
        case STATE_CHANGE_PASSWORD:
            if (bufferIndex != PASSWORD_LENGTH) {
                return;
            }

            EEPROM_WritePassword(buffer);
            ResetBuffer();
            UART0_SendString("S");
            currentState = STATE_MAIN_MENU;
            break;

        case STATE_MAIN_MENU:
            if (bufferIndex != 1) {
                return;
            }
            
            switch(buffer[0]) {
                case '+':
                    currentAction = ACTION_OPEN_DOOR;
                    UART0_SendString("S");
                    currentState = STATE_CHECK_PASSWORD;
                    break;
                case '-':
                    currentAction = ACTION_CHANGE_PASSWORD;
                    UART0_SendString("S");
                    currentState = STATE_CHECK_PASSWORD;
                    break;
                case '*':
                    currentAction = ACTION_SET_TIMEOUT;
                    UART0_SendString("S");
                    currentState = STATE_CHECK_PASSWORD;
                    break;
                default:
                    UART0_SendString("!");
                    currentState = STATE_MAIN_MENU;
                    break;
            }
            ResetBuffer();
            break;
        case STATE_CHECK_PASSWORD:
            if (bufferIndex != PASSWORD_LENGTH) {
                return;
            }

            if (!(currentAction == ACTION_OPEN_DOOR ||
                currentAction == ACTION_CHANGE_PASSWORD ||
                currentAction == ACTION_SET_TIMEOUT)) {
                UART0_SendString("!");
                ResetBuffer();
                currentState = STATE_MAIN_MENU;
                return;
            }

            char correctPassword[PASSWORD_LENGTH];
            EEPROM_ReadPassword(correctPassword);
            unsigned int passwordMatch;
            comparePassword(buffer, bufferIndex, correctPassword, &passwordMatch);
            
            if (passwordMatch) {
                UART0_SendString("S");
                if (currentAction == ACTION_OPEN_DOOR) {
                    open_Door();
                    UART0_SendString("S");
                    currentState = STATE_MAIN_MENU;
                } else if (currentAction == ACTION_CHANGE_PASSWORD) {
                    currentState = STATE_CHANGE_PASSWORD;
                } else if (currentAction == ACTION_SET_TIMEOUT) {
                    currentState = STATE_SET_TIMEOUT;
                }
            } else {
                UART0_SendString("F");
                failCount++;
                if (failCount >= MAX_FAILS) {
                    unsigned int timeoutSec;
                    Buzzer_ON(5000);
                    EEPROM_ReadTimeout(&timeoutSec);
                    Timer0A_DelayMs(timeoutSec * 1000);
                    currentState = STATE_MAIN_MENU;
                    failCount = 0;
                }
            }
            ResetBuffer();
            break;
        case STATE_SET_TIMEOUT:
            if (bufferIndex != TIMEOUT_LENGTH) {
                return;
            }

            unsigned int timeoutSec;
            unsigned int conversionResult;
            convertTimeoutToSec(buffer, &timeoutSec, &conversionResult);
            if (!conversionResult) {
                UART0_SendString("!");
                ResetBuffer();
                currentState = STATE_MAIN_MENU;
                break;
            }

            EEPROM_WriteTimeout(&timeoutSec);
            ResetBuffer();
            UART0_SendString("S");
            currentState = STATE_MAIN_MENU;
            break;
        default:
            break;
    }
}

void ResetBuffer(void)
{
    for (int i = 0; i < bufferIndex; i++)
    {
        buffer[i] = 0;
    }
    bufferIndex = 0;
}

void comparePassword(const char *pass1, const unsigned int pass1_length, const char *pass2, int *result)
{
    if (pass1_length != PASSWORD_LENGTH)
    {
        *result = 0; // length mismatch
        return;
    }
    
    for (int i = 0; i < PASSWORD_LENGTH; i++)
    {
        if (pass1[i] != pass2[i])
        {
            *result = 0; // mismatch
            return;
        }
    }
    *result = 1; // match
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

void Buzzer_ON(unsigned int durationMs){
    Buzzer_Output(1);
    Timer0A_DelayMs(durationMs); // Buzzer ON for specified duration
    Buzzer_Output(0);
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
                UART0_SendString("!");
            }
        }
        else
        {
            if (bufferIndex < sizeof(buffer)) {
                buffer[bufferIndex++] = c;
                StateMachine();
            } else {
                UART0_SendString("!"); // Buffer overflow
                ResetBuffer();
            }
            
        }

        UART0_ICR_R = 0x10; // Clear RX interrupt flag
    }
}

int main()
{
    EEPROM_Init();
    UART0_Init();
    Timer0A_Init();
    Buzzer_Init();
    enable_gpio(GPIO_PORTF);
    __asm("CPSIE I");

    for (;;)
    {
        __asm("      wfi\n");
    }
}
