#include <stdint.h>
#include "lpc2378.h" 
#include "uart.h"


#define LED10 (1u << 10)

volatile uint8_t boton_pulsado;


const char* uint8_tToString(uint8_t number)
{
    static char sNumber[3];
    if(number >= 100)
    {
        sNumber[0] = (number / 100) + '0';
        number %= 100;
        if(number >= 10)
            sNumber[1] = (number / 10) + '0';
        else
            sNumber[1] = '0'; 
        number %= 10;
        sNumber[2] = number + '0'; 
    }
    else if(number >= 10)
    {
        sNumber[0] = (number / 10) + '0';
        number %= 10;
        sNumber[1] = number + '0';
        sNumber[2] = 0;
    }
    else
    {
        sNumber[0] = number + '0';
        sNumber[1] = 0;
        sNumber[2] = 0;
    }

    return sNumber;
}

/* Required by startup.S */
void irq_handler_c(void)
{
    // eint0 ??
    if (VICIRQStatus & (1u << 14)){ // comprobamos que el bit 14 vale 1
        boton_pulsado = 1;
        EXTINT |= (1u << 0); // reiniciamos la flag de interrupcion.
    }
}

/* Required by startup.S */
void fiq_handler_c(void)
{
    for (;;) {
    }
}

int main(void)
{
    // configuramos el puerto como GPIO, valor 00. Página: 
    PINSEL4 &= ~(0b1100001111111111111111);
    PINSEL4 &= ~(1U << 21);
    PINSEL4 |= (1U << 20);

    // 8 bits bajos como salidas
    FIO2DIR0 |= 255u;
    // bit 10 como entrada
    FIO2DIR1 &= ~(1 << 2);

    // vamos a fijar la entrada con resistencia pull-up
    // página  156!
    PINMODE4 &= ~(3u << 20);

    // configuración de la UART1
    lpc_uart myUart1;
    uint8_t err = uart_init(1, 9600, &myUart1);
    if(err == ERR_UART_OK)
    {
        FIO2SET0 = (1u << 0);
    }
    else if (err == ERR_UART_INVALID_UART)
    {
        FIO2SET0 = (1u << 1);
    }
    else if (err == ERR_UART_INVALID_BAUDS)
    {
        FIO2SET0 = (1u << 2);
    }
    else
        FIO2SET0 = (1u << 3);
    if(myUart1.configured)
        uart_print(&myUart1, "Practica 3 v2\r\n");
    // interrupcion EINT0 por flanco
    EXTMODE |= (1u << 0);
    // como flanco de bajada
    EXTPOLAR &= ~(1u << 0);
    // reset causa de disparo
    EXTINT |= (1u << 0);
    boton_pulsado = 0;
    // eint0 como IRQ no FIQ
    VICIntSelect &= ~(1u << 14);

    
    uint8_t counter = 0;
    VICIntEnable |=(1u << 14);  
    for (;;) {
        /*
        if(!(FIO2PIN & LED10))
        {
            counter++;
            FIO2CLR0 = ~counter; 
            FIO2SET0 = counter;
            uart_print(&myUart1, "printing number: ");
            uart_print(&myUart1, uint8_tToString(counter));
            uart_print(&myUart1, "\r\n");
            while(!(FIO2PIN & LED10))
            {}
        }
        */
        if (boton_pulsado){
            uart_print(&myUart1, "botón pulsado \r\n");
            boton_pulsado = 0;
        }
    }
    return 0;
}