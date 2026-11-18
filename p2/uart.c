#include <stdint.h>
#include "lpc2378.h"
#include "uart.h" 




uint8_t uart_init(uint8_t uartId, uint32_t bauds, lpc_uart* myUart)
{
    uint8_t ret = ERR_UART_OK;
    myUart->configured = 0;
    myUart->uart_id = uartId;
    myUart->bauds = bauds;

    ret = checkBauds(myUart->bauds);

    if(ret == ERR_UART_OK)
    {
        if(myUart->uart_id == 1) //uart 1
        {
            // puerto P0,15 a TXD1, valor 01
            PINSEL0 &= ~(1u << 31);
            PINSEL0 |= (1u << 30);
            // y P0.16 a RXD1, valor 01
            PINSEL1 &= ~(1u << 1);
            PINSEL1 |= (1u << 0);
        }
        else
        {
            // error, invalid UART!
            ret = ERR_UART_INVALID_UART;
        }
    }
    if(ret == ERR_UART_OK)
    {
        // queremos que a la UART lleguen 4MHz
        PCLKSEL0 &= ~(1u << 9);
        PCLKSEL0 |= (1u << 8);
        
        ret = configureUart(myUart);
    }

    return ret;
}

void uart_print(lpc_uart* myUart, const char *myString)
{
    while (*myString) {
        uart_putChar(myUart, *myString++);
    }

    return;
}
void uart_putChar(lpc_uart* myUart, char s)
{
    if(myUart->uart_id == 1)
    {
        while ((U1LSR & (1u << 5)) == 0u) {}
        U1THR = s;
    }
    return;
}