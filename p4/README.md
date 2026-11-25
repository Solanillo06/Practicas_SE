Práctica 4: Empleamos timers! bloqueantes (delay) y con interrupciones
===

# Objetivo de la práctica  

+ Partiendo de lo hecho en la práctica anterior...
    - Hemos sabido configurar la interrupción externa EINT0
+ Vamos a configurar el TIMER0 (hay cuatro, pero cogemos ese por ejemplo).
+ Vamos a implementar una función delay en ms, que bloquée la ejecución del programa.
+ Vamos a aprender a configurar el timer también como interrupción

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

Queremos implementar el timer0 en una función delay, empleando ms como unidad de tiempo.
Tendréis que configurar el timer0, siguiendo estos pasos:
## 1. Configurar su fuente de reloj
El problema con los timers y las temporizaciones en un sistema empotrado es que trabajamos con frecuencias (Hz), habrá que saber traducir esas frecuencias en incrementos de tiempo (s, ms, ns, etc.)

Vamos a partir de una frecuencia conocida, como hicimos con la configuración de la UART1, necesitamos modificar el registro PCLKSEL0 y que los bits que afecten al timer 0 no dividan por anda el oscilador principal de 4MHz, así nos aseguramos un valor conocido para todos los cálculos del timer0 de 4 MHz!
(revisad la práctica 2 y modificad PCLKSEL0 para los bits del timer0)

## 2. debemos configurar nuestro cálculo del timer, para adaptar esos 4MHz a la frecuencia con la que el contador se incrementa
El registro T0TC (pág 556) del timer 0 se incrementa por unidad de tiempo que definamos, vamos a decidir que ese incremento es de 1 ms. Cuando pase 1ms ese registro aumenta en 1.

La frecuencia entonces Ftc será de 1000Hz: 1/1000Hz = 1ms 

Para que el registro T0TC se incremente cada 1000 Hz, hay un registro preescalador llamado TOPR que es el que se incrementa cada Hz que nos dar el oscilador interno.
Debemos de darle un valor para que incremente T0TC cuando lo supere, la fórmula es:

Ftc = PCLKSEL_timer / (TOPR +1)


Lo primero a configurar será la fuente de reloj del timer0, así como sus divisores para pasar frecuencia a tiempo.

Emplearemos así los registros:
```
T0TCR	0xE0004004	Arrancar, detener y reiniciar
T0TC	0xE0004008	Valor actual del contador
T0PR	0xE000400C	Valor del prescaler
// este no hace falta, pero es por si no lo entendéis bien y queréis curiosearlo.
T0PC	0xE0004010	Contador interno del prescaler
```

## 3. probais a hacer un delay!

Si habéis configurado bien la frecuencia de trabajo
Si habéis configurado bien el registro preescalador del timer0
éste por defecto trabaja como timer, no como contaador (lo miramos en el manual)
sólo hay que reiniciar el timer:
T0TCR = 2u;
iniciarlo:
T0TCR = 1u;
y comparar T0TC:

una espera activa de 500ms es
while (T0TC < 500) {};

**Probad a hacer el contador binario cada segundo, 5 segundos, etc. Y comprobad (más o menos) que el tiempo está bien calculado**

## 4. Lo siguiente sería con interrupciones

El delay está bien, tenemos una medida **bloqueante** de tiempo.
Pero una aproximación que será más útil es programar una interrupción cuando el conteo llegue al tiempo esperado.
De esta forma podemos tener el microcontrolador haciendo otras tareas mientras tanto, o en ahorro energético.

Emplearemos los registros:
```
T0MR0 - 0xE0004018 // es donde almacenamos el tiempo con el que vamos a comparar el temporizador. 32bits
T0MCR - 0xE0004014 // este registro de control sirve para configurar como queremos que se comporte, (usamos los bits 0 y 1)
// en este caso queremos disparar interrupción y resetear el timer!
// ojo! es un registro de 16 bits
T0IR - 0xE0004000 // nos sirve para resetear el flag de disparo de interrupción del timer
// como con la interrupción externa, después de configurarlo todo, lo reseteamos.
// me ha pasado haciendo la práctica que aún reseteándolo se quedaba pillado en las interrupciones. 
// y es que no pongais sólo un 1 en el bit del canal match 0. 
// reseteadlo al completo porque puede tener disparos asociados a otros canales o contadores, modos, etc.
// T0IR = 255u;
VICIntSelect
VICIntEnable
VICIntStatus
// en este caso, buscad el bit del timer0
```

## 5. Mediante interrupciones...
**Programad un sistema que cuente bits por cada pulsación de EINT0 mediante interrupción, o cada 5 segundos.**
**Si pulsamos el botón, reseteamos el temporizador de los 5 segundos.**
**Comprobad en el bucle mediante un flag, si el conteo se ha producido por tiempo o por botón pulsado**

# Licencia

Copyright 2026 Joaquín Cuéllar <p22cupaj@uco.es>

Este código está licenciado bajo Apache2, podéis hacer lo que queráis con él, es vuestro.