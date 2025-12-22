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
    UART0_ICR_R |= 0x10;
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

void UART0_Send_String(const char *str)
{
    while (*str)
    {
        UART0_Transmit(*str++);
    }
}

void UART0IntHandler(void)
{
    /* Check RX interrupt */
    if (UART5_MIS_R & UART_MIS_RXMIS)
    {
        /* Read ALL available bytes */
        while (!(UART5_FR_R & UART_FR_RXFE))
        {
            unsigned int  data = UART5_DR_R;

            /* Handle overrun error */
            if (data & UART_DR_OE)
            {
                UART5_ECR_R = 0xFF;    // Clear all UART errors
                break;                // Exit FIFO read
            }

            /* Handle other UART errors */
            if (data & 0xF00)
            {
                // Error detected ? discard byte
                continue;
            }

            /* Store received byte */
            if (bufferIndex < sizeof(BUFFER))
            {
                BUFFER[bufferIndex++] = (char)(data & 0xFF);
            }
            else
            {
                ResetBuffer();   // buffer overflow protection
            }
        }

        /* Clear RX interrupt ONCE */
        UART5_ICR_R = UART_ICR_RXIC;
    }
}
