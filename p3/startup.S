// LPC2378 - startup bare-metal para GCC/Newlib
// IRQ/FIQ disponibles al entrar en main; fuentes VIC deshabilitadas al arrancar.
// Basado en el startup original del usuario (2026).

.syntax unified
.cpu arm7tdmi
.arm

/* Modos ARM7 y bits de inhibicion global de interrupciones. */
.equ MODE_FIQ, 0x11
.equ MODE_IRQ, 0x12
.equ MODE_SVC, 0x13
.equ MODE_ABT, 0x17
.equ MODE_UND, 0x1B
.equ MODE_SYS, 0x1F
.equ I_BIT,    0x80
.equ F_BIT,    0x40

/* Registros VIC del LPC2378. */
.equ VIC_INT_SELECT,      0xFFFFF00C
.equ VIC_INT_EN_CLR,      0xFFFFF014
.equ VIC_SOFT_INT_CLEAR,  0xFFFFF01C
.equ VIC_ADDRESS,         0xFFFFFF00

/* Tabla de vectores: debe quedar en 0x00000000 segun linker.ld. */
.section .vectors, "ax", %progbits
.align 2
.global _start
_start:
    b Reset_Handler              /* 0x00 Reset */
    b Undefined_Handler          /* 0x04 Undefined instruction */
    b SWI_Handler                /* 0x08 SWI */
    b PrefetchAbort_Handler      /* 0x0C Prefetch abort */
    b DataAbort_Handler          /* 0x10 Data abort */
    .word 0                      /* 0x14 checksum LPC, lo completa herramienta de flash */
    b IRQ_Handler                /* 0x18 IRQ */
    b FIQ_Handler                /* 0x1C FIQ */

.section .text.Reset_Handler, "ax", %progbits
.align 2
.global Reset_Handler
.type Reset_Handler, %function
Reset_Handler:
    /* Configurar pila de cada modo. Mientras tanto, IRQ/FIQ inhibidas. */
    msr CPSR_c, #(MODE_UND | I_BIT | F_BIT)
    ldr sp, =_stack_und_top

    msr CPSR_c, #(MODE_ABT | I_BIT | F_BIT)
    ldr sp, =_stack_abt_top

    msr CPSR_c, #(MODE_FIQ | I_BIT | F_BIT)
    ldr sp, =_stack_fiq_top

    msr CPSR_c, #(MODE_IRQ | I_BIT | F_BIT)
    ldr sp, =_stack_irq_top

    msr CPSR_c, #(MODE_SVC | I_BIT | F_BIT)
    ldr sp, =_stack_svc_top

    /* VIC en estado conocido. Sin fuentes habilitadas; todas seleccionadas IRQ. */
    ldr r0, =VIC_INT_EN_CLR
    mvn r1, #0                  /* r1 = 0xFFFFFFFF */
    str r1, [r0]                 /* inhibir las 32 fuentes VIC */
    ldr r0, =VIC_SOFT_INT_CLEAR
    str r1, [r0]                 /* borrar peticiones de software pendientes */
    ldr r0, =VIC_INT_SELECT
    mov r1, #0
    str r1, [r0]                 /* valor por defecto: todas las fuentes son IRQ */

    /* Copiar .data desde su imagen en FLASH hasta su direccion en RAM. */
    ldr r0, =_sidata
    ldr r1, =_sdata
    ldr r2, =_edata
.L_copy_data:
    cmp r1, r2
    bhs .L_zero_bss
    ldr r3, [r0], #4
    str r3, [r1], #4
    b .L_copy_data

    /* Poner a cero .bss. El linker alinea sus extremos a 4 bytes. */
.L_zero_bss:
    ldr r1, =_sbss
    ldr r2, =_ebss
    mov r3, #0
.L_zero_bss_loop:
    cmp r1, r2
    bhs .L_enter_system
    str r3, [r1], #4
    b .L_zero_bss_loop

    /* System: privilegios de SVC y banco de registros de User.
       I=0 y F=0: el nucleo acepta IRQ y FIQ desde este punto.
       Es seguro porque, hasta que main habilite fuentes, el VIC no genera peticiones. */
.L_enter_system:
    msr CPSR_c, #MODE_SYS
    ldr sp, =_stack_sys_top
    bl main
.L_main_returned:
    b .L_main_returned
.size Reset_Handler, . - Reset_Handler

/* Excepciones no implementadas: detener para facilitar depuracion. */
Undefined_Handler:
    b Undefined_Handler
SWI_Handler:
    b SWI_Handler
PrefetchAbort_Handler:
    b PrefetchAbort_Handler
DataAbort_Handler:
    b DataAbort_Handler

/* IRQ: LR_irq contiene PC de reanudacion + 4.
   El hardware ha copiado CPSR->SPSR_irq, seleccionado IRQ y puesto I=1.
   Salvamos registros volatiles ABI, SPSR y alineamos la pila para llamar a C. */
.global IRQ_Handler
.type IRQ_Handler, %function
IRQ_Handler:
    sub lr, lr, #4
    stmdb sp!, {r0-r3, r12, lr}     /* 6 palabras = 24 bytes */
    mrs r0, SPSR
    stmdb sp!, {r0, r1}            /* 2 palabras; total 32 bytes, alineado a 8 */

    bl irq_handler_c                /* C debe limpiar el flag del periferico */

    /* Finalizar en VIC la IRQ actual: VICAddress, NO VICVectAddrN.
       No borra el flag del periferico ni deshabilita su fuente. */
    ldr r0, =VIC_ADDRESS
    mov r1, #0
    str r1, [r0]

    ldmia sp!, {r0, r1}
    msr SPSR_cxsf, r0
    ldmia sp!, {r0-r3, r12, lr}
    subs pc, lr, #0                /* PC=LR corregido; CPSR <- SPSR_irq */

/* FIQ: banco privado r8-r14; guardamos r0-r7 y LR, mas SPSR_fiq.
   Total 10 palabras = 40 bytes; mantenemos alineacion a 8 para C.
   FIQ NO usa reconocimiento de fin de servicio en VICAddress. */
.global FIQ_Handler
.type FIQ_Handler, %function
FIQ_Handler:
    sub lr, lr, #4
    stmdb sp!, {r0-r7, lr}        /* 9 palabras */
    mrs r0, SPSR
    stmdb sp!, {r0}               /* 1 palabra; total 40 bytes */

    bl fiq_handler_c               /* C identifica fuente y limpia flag del periferico */

    ldmia sp!, {r0}
    msr SPSR_cxsf, r0
    ldmia sp!, {r0-r7, lr}
    subs pc, lr, #0

/* Implementaciones debiles: apps sin interrupciones no necesitan definir handlers.
   Una implementacion C con el mismo nombre reemplaza el simbolo debil.
   Si se habilita por error una fuente sin handler, se detiene para depurar. */
.weak irq_handler_c
.type irq_handler_c, %function
irq_handler_c:
    b irq_handler_c

.weak fiq_handler_c
.type fiq_handler_c, %function
fiq_handler_c:
    b fiq_handler_c

