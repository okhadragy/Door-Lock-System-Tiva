#ifndef UART_H
#define UART_H
#include "../config.h"

void UART0_Init(void);
void UART0_Transmit(char data);
void UART0_Send_String(const char *str);
void UART0IntHandler(void);

#endif