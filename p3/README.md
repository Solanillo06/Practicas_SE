Práctica 3: Empleamos interrupciones en vez de polling para el botón EINT0.
===

# Objetivo de la práctica  

+ Partiendo de lo hecho en la práctica anterior...
    - Donde hemos separado la UART en su propio código fuente uart.h/uart.c
    - Donde hemos mejorado el Makefile para poder hacer un *make flash*
+ Vamos a emplear interrupciones para detectar un flanco de bajada en el botón EINT0.
+ Mediante la UART 1, mandaremos un mensaje de bienvenida con nuestra versión del código (nos lo inventamos, es por aseguraros que ejecutáis lo que queréis realmente), Y mandaremos un mensaje cada vez que incrementemos el contador con el número de cuenta.

+ EXTRA: podríamos rizar el rizo y dormir la placa en modo sueño!

# Recordad

## Makefile

Editad el Makefile si añadís más ficheros de código de fuente o los renombrais.

### Para flashear mediante el Makefile:
```
make flash
```

Algunas veces la placa falla en programarse, resetearse, etc. Leed el mensaje de vuelta bien y repetid el comando que haya fallado si es necesario.

y recordad el tipo de programador que tenéis, cambiad los comandos según sea el ulink2 o el jlink, en el Makefile:

(si tenéis el programador Keil ulink 2)
```
$ openocd -f interface/cmsis-dap.cfg -f mcb2300.cfg [...]
```

(si tenéis el programador jlink, el de color negro)
```
$ openocd -f interface/jlink.cfg -f mcb2300.cfg [...]
```

### Para borrar todos los binarios y código objeto:
```
make clean
```

## Dependencias nuevas para esta práctica

Necesitaréis instalar un terminal serie, uno clásico que funciona muy bien es minicom:
```
# emerge --ask minicom
```
```
$ sudo apt install minicom
```

Para conectarnos con minicom, habrá que detectar el puerto usb empleado por el conversor, estar en el grupo dialout de usuarios, y el comando será:
```
minicom -D /dev/ttyUSB0 -b 9600
```
\* /dev/ttyUSB= cambiará según como se mapeé el dispositivo

# Qué vamos a hacer?

Queremos que al pulsar el botón INT0 el contador BCD se incremente y el microcontrolador nos comunique un mensaje por el puerte serie el valor numérico que tiene.

Queremos además que no sea por **polling** si no por interrupción producida por el la entrada EINT0 que usamos para el botón.

Habrá que:

+ Partiendo de lo hecho en la práctica anterior...
    - Donde hemos separado la UART en su propio código fuente uart.h/uart.c
    - Donde hemos mejorado el Makefile para poder hacer un *make flash*
+ Vamos a emplear interrupciones para detectar un flanco de bajada en el botón EINT0. 
+ Queremos que el contador se incremente sólo cuando se reciba la interrupción del botón.
Emplearemos los registros:
```
// habrá que configurar el pin 2.10 no como GPIO si no como EINT0!!
PINSEL4

// Para indicar que empleamos IRQ para la interrupción EINT0
VICIntSelect

// Para habilitar la interrupción EINT0 (esto se debe de hacer ya lo último!)
VICIntEnable

// para saber si la fuente de una interrupción IRQ es nuestro EINT0, analizaremos este registro
VICIRQStatus

/// PAra configurar el comportamiento de EINT0:
// Para que EINT0 detecte flancos y no niveles
EXTMODE

// Para que EINT0 detecte un flanco descendente
EXTPOLAR

// Tanto para empezar, tanto cuando procesemos la interrupción.
// Para limpiar cualquier interrupción pendiente
EXTINT

```

+ Mediante la UART 1, mandaremos un mensaje de bienvenida con nuestra versión del código (nos lo inventamos, es por aseguraros que ejecutáis lo que queréis realmente), Y mandaremos un mensaje cada vez que incrementemos el contador con el número de cuenta.

+ EXTRA: podríamos rizar el rizo y dormir la placa en modo sueño, de forma que:
    - se inicie, habilite interrupciones, uart, mensaje de bienvenida, etc.
    - mensaje por uart de que nos vamos a modo sueño.
    - modo sueño.
    (vemos que la ejecución del while se para y no se incrementa el contador, tampoco recibimos los mensajes de la uart)
    - pulsamos botón, se produce la interrupción, se saca al microcontrolador del modo sueño.
    - contamos, imprimimos que nos vamos a dormir y volvemos a entrar en modo sueño.
    - etc.
```
// Se emplea el registro:
PCON
// Ojo! en el momento que se modifican sus bits PM2, PM1, y PM0
// el micro entra en ese modo de energía!
```

# Licencia

Copyright 2026 Joaquín Cuéllar <p22cupaj@uco.es>

Este código está licenciado bajo Apache2, podéis hacer lo que queráis con él, es vuestro.