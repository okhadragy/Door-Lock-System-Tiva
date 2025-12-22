#include "uart.h"
#include "buffer.h"
#include "SysTick_Driver.h"
#include <stdint.h>

/* ================= UART5 (PE4 RX, PE5 TX) ================= */

void UART0_Init(void)
{
    /* 1) Enable clocks */
    SYSCTL_RCGCUART_R |= (1 << 5);   // UART5
    SYSCTL_RCGCGPIO_R |= (1 << 4);   // GPIOE

    volatile int delay = SYSCTL_RCGCGPIO_R; // allow clock to settle

    /* 2) Configure PE4, PE5 for UART */
    GPIO_PORTE_AFSEL_R |= (1 << 4) | (1 << 5);
    GPIO_PORTE_PCTL_R  = (GPIO_PORTE_PCTL_R & 0xFF00FFFF) | 0x00110000;
    GPIO_PORTE_DEN_R   |= (1 << 4) | (1 << 5);
    GPIO_PORTE_AMSEL_R &= ~((1 << 4) | (1 << 5));

    /* 3) Disable UART5 */
    UART5_CTL_R &= ~UART_CTL_UARTEN;

    /* 4) Baud rate: 9600 @ 16 MHz */
    UART5_IBRD_R = 104;
    UART5_FBRD_R = 11;

    /* 5) 8-bit, even parity, FIFO enabled */
    // UART5_LCRH_R = 0x76;
    
    /* 5) 8-bit, even parity, FIFO disabled */
    UART5_LCRH_R = UART_LCRH_WLEN_8 | UART_LCRH_PEN | UART_LCRH_EPS;

    /* 6) Enable UART, TX, RX */
    UART5_CTL_R = UART_CTL_UARTEN | UART_CTL_TXE | UART_CTL_RXE;

    /* 7) Enable RX interrupt */
    UART5_ICR_R = UART_ICR_RXIC;   // clear RX interrupt
    UART5_IM_R  |= UART_IM_RXIM;   // RX interrupt enable

    /* 8) NVIC enable (UART5 IRQ = 61 ? EN1 bit 29) */
    NVIC_EN1_R |= (1 << 29);
}

/* ================= TRANSMIT ================= */

void UART0_Transmit(char data)
{
    
    SysTick_DelayMs(50);
    
    //while (UART5_FR_R & UART_FR_TXFF); // wait if FIFO full
    UART5_DR_R = data;
}

void UART0_Send_String(const char *str)
{
    while (*str)
    {
        UART0_Transmit(*str++);
    }
}

/* ================= INTERRUPT HANDLER ================= */

void UART0IntHandler(void)
{
    /* Check RX interrupt */
    if (UART5_MIS_R & UART_MIS_RXMIS)
    {
        /* Read ALL available bytes */
        // while (!(UART5_FR_R & UART_FR_RXFE))
        // {
            unsigned int  data = UART5_DR_R;

            /* Handle overrun error */
            if (data & UART_DR_OE)
            {
                UART5_ECR_R = 0xFF;    // Clear all UART errors
                //break;                // Exit FIFO read
            }

            /* Handle other UART errors */
            if (data & 0xF00)
            {
                // Error detected ? discard byte
               // continue;
            }

            /* Store received byte */
            if (bufferIndex < sizeof(BUFFER))
            {
              if (data == 'S' || data == 'F' || data =='!') {
                
                BUFFER[bufferIndex++] = (char)(data & 0xFF);
         
                
              } else {
                
              // IGNORE (UART NOISE)
              
              }
            }
            else
            {
                ResetBuffer();   // buffer overflow protection
            }
        }

        /* Clear RX interrupt ONCE */
        UART5_ICR_R = UART_ICR_RXIC;
    
}