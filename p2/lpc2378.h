#ifndef LPC2378_H
#define LPC2378_H

// mirad el manual de usuario keil LPC2378
//PINSEL4 en el LPC2378. Página: 162 de 706
// emplea 2 bits de configuración 00 01 10 11
#define PINSEL4     (*(volatile uint32_t *)0xE002C010u)
#define PINSEL0     (*(volatile uint32_t *)0xE002C000u) // <--- Agregado (Página 148 del manual)
#define PINSEL1     (*(volatile uint32_t *)0xE002C004u) // <--- Agregado (Página 150 del manual)
// mirad el manual de usuario, página: 173 de 706
// Direccionamos las GPIO de forma indirecta
// Se pueden incluso emplear máscaras
#define FIO2DIR     (*(volatile uint32_t *)0x3FFFC040u)
#define FIO2DIR0    (*(volatile uint8_t *)0x3FFFC040u)
#define FIO2DIR1    (*(volatile uint8_t *)0x3FFFC041u)

#define FIO2SET     (*(volatile uint32_t *)0x3FFFC058u)
#define FIO2SET0     (*(volatile uint8_t *)0x3FFFC058u)

#define FIO2CLR     (*(volatile uint32_t *)0x3FFFC05Cu)
#define FIO2CLR0     (*(volatile uint8_t *)0x3FFFC05Cu)

#define FIO2PIN  (*(volatile uint32_t *) 0x3FFFC054)
#define PINMODE4 (*(volatile uint32_t*) 0xE002C050)

//pagina 441 
#define U1CLR (*(volatile uint8_t *)0xE0010018u)
//pagina 62
#define PCLKSEL0 (*(volatile uint8_t *)0xE01FC1A8)
//437
#define U1DLL (*(volatile uint8_t *) 0xE0010000u)
#define U1DLM (*(volatile uint8_t *) 0xE0010004u)
//451
#define U1FDR (*(volatile uint8_t *) 0xE0010028u)
//página 436
#define U1RBR (*(volatile uint8_t *) 0xE0010000u)

#endif
