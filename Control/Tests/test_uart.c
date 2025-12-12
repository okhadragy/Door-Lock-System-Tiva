#include "../Drivers/uart.h"
#include "test_functions.h"
#include "../logic/buffer.h"
#include <stdint.h>
#include <stdio.h>

void UART0IntHandler_Test(uint32_t);
// Simulate RX interrupt handling for testing only
 void UART0IntHandler_Test(uint32_t fake_data)
{
    // Simulate MIS flag
    UART0_MIS_R = 0x10; // RX interrupt flag

    // Simulate DR register value (data + error bits)
    UART0_DR_R = fake_data;

    // Call the real handler (it reads MIS and DR)
    UART0IntHandler();
}

// Helper to print BUFFER contents
void Print_Buffer(void)
{
    UART0_Send_String("BUFFER: ");
    for (int i = 0; i < bufferIndex; i++)
        UART0_Transmit(BUFFER[i]);
    UART0_Send_String("\r\n");
}

// Run all test cases
void UART0_Handler_Tests(void)
{
    UART0_Send_String("\r\n--- UART0IntHandler Test Start ---\r\n");

    // 1) Normal RX, single byte
    bufferIndex = 0;
    UART0IntHandler_Test('A'); 
    Print_Buffer(); // Should show BUFFER: A

    // 2) Normal RX, multiple bytes
    bufferIndex = 0;
    UART0IntHandler_Test('H'); 
    UART0IntHandler_Test('i'); 
    Print_Buffer(); // BUFFER: Hi

    // 3) Simulate buffer overflow
    bufferIndex = sizeof(BUFFER);  // force full buffer
    UART0IntHandler_Test('X'); // Should trigger "!" and reset buffer
    Print_Buffer(); // BUFFER empty

    UART0_Send_String("--- UART0IntHandler Test End ---\r\n");
}

void UART_Debug_Registers(void)
{
    char s[80];
    sprintf(s,
            "FR=0x%02X  DR=0x%02X  IBRD=%d  FBRD=%d\r\n",
            UART0_FR_R, UART0_DR_R, UART0_IBRD_R, UART0_FBRD_R);
    UART0_Send_String(s);
}
void UART_Test_Transmit(void)
{
    UART0_Send_String("TX Test: Hello from Tiva!\r\n");
}

void UART_Test_Echo(void)
{
    UART0_Send_String("RX Test: Type something...\r\n");
}
void UART_Test_BufferOverflow(void)
{
    UART0_Send_String("Buffer Overflow Test\r\n");

    // Simulate filling the buffer completely
    for (int i = 0; i < sizeof(BUFFER); i++)
    {
        UART0IntHandler_Test('A');  // fake incoming byte
    }

    // Now buffer is FULL, next byte should trigger overflow
    UART0IntHandler_Test('B');      // should print "!" and ResetBuffer()

    // Show buffer state after overflow
    UART0_Send_String("\r\nAfter overflow, bufferIndex = ");
    
    char msg[10];
    sprintf(msg, "%d\r\n", bufferIndex);
    UART0_Send_String(msg);
}

void UART_Run_All_Tests(void)
{
    UART0_Send_String("\r\n--- UART SELF TEST START ---\r\n");

    UART_Debug_Registers();
    UART_Test_Transmit();
    UART_Test_Echo();
    UART_Test_BufferOverflow();

    UART0_Send_String("\r\n--- UART SELF TEST END ---\r\n");
}