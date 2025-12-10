#include "tm4c123gh6pm.h"
#include "Common_Macros.h"
#include "buffer.h"
void UART0_INIT(void)
{
    SET_BIT(SYSCTL_RCGCUART_R, 0);   // enable UART0 clock
    SET_BIT(SYSCTL_RCGCGPIO_R, 0);   // enable GPIOA clock

    while ((SYSCTL_PRUART_R & 0x01) == 0);
    while ((SYSCTL_PRGPIO_R & 0x01) == 0);

    // Configure PA0, PA1 as UART pins
    GPIO_PORTA_AFSEL_R |= 0x03;
    GPIO_PORTA_DEN_R   |= 0x03;

    // PCTL selects UART function (0x1 for A0, 0x1 for A1)
    GPIO_PORTA_PCTL_R = (GPIO_PORTA_PCTL_R & ~0xFF) | 0x11;

    // Disable UART before setup
    UART0_CTL_R &= ~(1 << 0);

    // Baud Rate = 9600 @ 16MHz
    UART0_IBRD_R = 104;
    UART0_FBRD_R = 11;

    // 8-bit, no parity, FIFO enabled
    UART0_LCRH_R = 0x70;

    // Enable RX interrupt
    UART0_IM_R |= (1 << 4);

    // Enable UART0 interrupt in NVIC
    NVIC_EN0_R |= (1 << 5);

    // Enable UART, RXE, TXE
    UART0_CTL_R |= 0x301;
}




void UART0_SEND_CHAR(unsigned char data){
    while(READ_BIT(UART0_FR_R,5)==1);
    UART0_DR_R = data; 
}

void UART0IntHandler(void)
{
    if (UART0_MIS_R & (1 << 4))
    {
        unsigned int data = UART0_DR_R;
        char c = data & 0xFF;

        if (data & 0xF00)
        {
            UART0_SEND_STRING("!");
        }
        else
        {
            if (bufferIndex < BUFFER_SIZE)
            {
                BUFFER[bufferIndex++] = c;
            }
            else
            {
                ResetBuffer();
            }
        }

        UART0_ICR_R = (1 << 4);
    }
}

void UART0_SEND_STRING(const char* str){
    while(*str){
        UART0_SEND_CHAR(*str);
        str++;
    }
}