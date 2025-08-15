README.md
# Control de LEDs RGB vía Bluetooth con PIC16F877A y Flutter
Este proyecto permite controlar un LED RGB conectado a un **PIC16F877A** mediante comandos enviados desde una aplicación móvil desarrollada en **Flutter**.  
La comunicación entre la aplicación y el microcontrolador se realiza mediante el módulo **Bluetooth HC-05** usando UART.

##  Características principales

- **Microcontrolador:** PIC16F877A
- **Comunicación:** UART 9600 baudios (HC-05)
- **Control vía App:** Aplicación Flutter con interfaz simple para enviar comandos
- **LED RGB:** Control individual de colores Rojo, Verde y Azul
- **Modo encendido/apagado:** Active Low (0 = encendido, 1 = apagado)

##  Hardware necesario

- PIC16F877A
- Módulo Bluetooth HC-05
- LED RGB (cátodo común)
- Resistencias limitadoras para cada color
- Fuente de alimentación 5V
- Cables de conexión
- Placa de pruebas o PCB



##  Comandos disponibles

| Comando | Acción                          |
|---------|---------------------------------|
| `R`     | Encender LED rojo               |
| `V`     | Encender LED verde              |
| `A`     | Encender LED azul               |
| `T`     | Encender todos los colores      |
| `0`     | Apagar todos los LEDs           |


##  Conexiones de hardware

| PIC16F877A Pin | Función              |
|----------------|----------------------|
| RC6 (TX)       | Transmisión UART      |
| RC7 (RX)       | Recepción UART        |
| RB0            | LED Rojo              |
| RB1            | LED Verde             |
| RB2            | LED Azul              |
| VCC (5V)       | Alimentación          |
| GND            | Tierra común          |

> **Nota:** Configurar el HC-05 en modo esclavo y a 9600 baudios.

---

##  Código en C para PIC (MPLAB X + XC8)

El microcontrolador inicializa el puerto UART, configura PORTB como salida y usa interrupciones para recibir datos.  
Dependiendo del comando recibido, activa los pines correspondientes al LED RGB.

##  Aplicación Flutter

La app Flutter se encarga de:
1. Escanear dispositivos Bluetooth cercanos
2. Conectarse al HC-05
3. Enviar los comandos definidos (`R`, `V`, `A`, `T`, `0`)

Dependencias principales:
```yaml
dependencies:
  flutter:
    sdk: flutter
  flutter_bluetooth_serial: ^0.4.0

