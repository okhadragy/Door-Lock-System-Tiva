#ifndef UART_DRIVER
#define UART_DRIVER
void UART0_INIT();
void UART0_SEND_CHAR(unsigned char data);
unsigned char UART0IntHandler();
void UART0_SEND_STRING(const char* str);
#endif