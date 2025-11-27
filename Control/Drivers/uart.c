#include "uart.h"
#include "../logic/buffer.h"

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
            } else {
                UART0_SendString("!"); // Buffer overflow
                ResetBuffer();
            }
            
        }
        UART0_ICR_R = 0x10; // Clear RX interrupt flag
    }
}