#include <stdint.h>
#include "lpc2378.h"

#define LED10 (1u << 10)

static void delay(void)
{
    for (volatile uint32_t i = 0; i < 5000u; ++i) {
        __asm__ volatile ("nop");
    }
}

/* Required by startup.S */
void irq_handler_c(void)
{
    for (;;) {
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
    //puertos P0,15 a TXD1 , valor 01
    PINSEL0 &= ~(1u << 31);
    PINSEL0 |= (1u<<30);
    // y P0.16 a RDX1 , valor 01
    PINSEL1 &=~(1u << 1);
    PINSEL1 |=(1u << 0);
    // 8 bits bajos como salidas
    FIO2DIR0 |= 255u;
    // bit 10 como entrada
    FIO2DIR1 &= ~(1 << 2);

    // vamos a fijar la entrada con resistencia pull-up
    // página  156!
    PINMODE4 &= ~(3u << 20);

    //configuracion de la UART1
    //queremos que UART llegen 4MHz
    PCLKSEL0 &= ~(1u <<9);
    PCLKSEL0 |= (1u << 8); 
    //comunicaion 8 bits
    //sin paridad ni bit de parada,etc.
    //y DLAB=1  para configurar velocidad 
    U1LCR = (3u << 0) | (1u << 7);
    //calculo de los divisores para la formula de baudios
    U1DLM =0;
    U1DLL= 26u;
    U1FDR =0x10;
    //desactivamos DLAB
    U1LCR = (3u << 0);

    U1THR ='h';
    delay();
    U1THR = 'o';
    delay();
    U1THR = 'l';
    delay();
    U1THR ='a';
    delay();
    U1THR ='\r';
    delay();
    U1THR='\n';


    uint8_t counter = 0;
    FIO2CLR0 = ~counter;    
    for (;;) {
        if(!(FIO2PIN & LED10))
        {
            delay();
            FIO2SET0 += counter;
            FIO2CLR0 = ~counter; 
            counter++;
            while(!(FIO2PIN & LED10))
            {}
        }
    }
    return 0;
}