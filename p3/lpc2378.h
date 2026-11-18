// Cabecera con las direcciones de LPC2378
// licencia Apache2 Copyright 2026 Joaquín Cuéllar <p22cupaj@uco.es>

#ifndef _LPC2378_H_
#define _LPC2378_H_
// necesario para el uso de tipos de datos
#include <stdint.h>

// pág 157
#define PINSEL0     (*(volatile uint32_t *)0xE002C000u)
#define PINSEL1     (*(volatile uint32_t *)0xE002C004u)   
// mirad el manual de usuario, PINSEL4 en el LPC2378. Página: 162 de 706
// emplea 2 bits de configuración 00 01 10 11
#define PINSEL4     (*(volatile uint32_t *)0xE002C010u)

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

// UARTS
// página 62, registro que establece la división del IRC interno en cada periférico
#define PCLKSEL0    (*(volatile uint32_t*) 0xE01FC1A8u)

/// UART 1
// página 441, registro de control de la línea
#define U1LCR       (*(volatile uint8_t*) 0xE001000Cu)
// divisores para la frecuencia y el cálculo de baudios
// pág 437
#define U1DLL       (*(volatile uint8_t*) 0xE0010000u)
#define U1DLM       (*(volatile uint8_t*) 0xE0010004u)
// pág 451
#define U1FDR       (*(volatile uint32_t*) 0xE0010028u)
// transmisión, pág 436
#define U1THR       (*(volatile uint8_t*) 0xE0010000u)
// estado de la línea pág 446
#define U1LSR       (*(volatile uint8_t*) 0xE0010014u)
// interrupciones externas, flanco o nivel pag: 37
#define EXTMODE     (*(volatile uint8_t*) 0xE01FC148u)
//
#define EXTPOLAR    (*(volatile uint8_t*) 0xE01FC14Cu)
//
#define EXTINT      (*(volatile uint8_t*) 0xE01FC140u)
// 
#define VICIntSelect    (*(volatile uint32_t*)0xFFFFF00Cu)
//
#define VICIntEnable    (*(volatile uint32_t*)0xFFFFF010u)
// lee el estado de las interrupciones, pag 90
#define VICIRQStatus    (*(volatile uint32_t*)0xFFFFF000u)




#endif