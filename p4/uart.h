// Cabecera con las direcciones de LPC2378
// licencia Apache2 Copyright 2026 Joaquín Cuéllar <p22cupaj@uco.es>

#ifndef _UART_H_
#define _UART_H_

enum{
    ERR_UART_OK = 0,
    ERR_UART_INVALID_UART,
    ERR_UART_INVALID_BAUDS,
    ERR_UNKNOWN
};

typedef struct lpcUart
{
    uint8_t uart_id;
    uint8_t configured;
    uint32_t bauds;
} lpc_uart;

static uint32_t validBauds[2] = {9600, 115200};
static uint8_t validBaudsSize = 2;

uint8_t uart_init(uint8_t uartId, uint32_t bauds, lpc_uart* myUart);

void uart_print(lpc_uart* myUart, const char *myString);
void uart_putChar(lpc_uart* myUart, char s);

static uint8_t checkBauds(uint32_t bauds)
{
    uint8_t ret = ERR_UART_INVALID_BAUDS;
    for(uint8_t index = 0; index < validBaudsSize; index++)
    {
        if(bauds == validBauds[index])
            ret = ERR_UART_OK;
    }
    return ret;
}

static uint8_t configureUart(lpc_uart* myUart)
{
    uint8_t ret = ERR_UART_OK;
    if(myUart->uart_id == 0)
    {
        ret = ERR_UART_INVALID_UART;
    }
    else if(myUart->uart_id == 1)
    {
        // comunicación de 8 bits
        // sin paridad ni bit de parada, etc.
        // y DLAB = 1 para configurar velocidad
        U1LCR = (3u << 0) | (1u << 7);
        // cálculo de los divisores para la fórmula de baudios
        // página 451
        if(myUart->bauds == 9600)
        {
            U1DLM = 0;
            U1DLL = 26u;
            U1FDR = 0x10;
        }
        else if(myUart->bauds == 115200)
        {
            U1DLM = 0;
            U1DLL = 2u;
            // 1/12
            U1FDR |= 0x01;
            U1FDR |= (12u << 4);
            U1FDR = 0x10;
        }
        // desactivamos DLAB
        U1LCR = (3u << 0);
        myUart->configured = 1;
    }
    else
        ret = ERR_UART_INVALID_UART;
    return ret;
}

#endif