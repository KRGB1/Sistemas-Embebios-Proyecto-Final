/* 
 * File:   Bluetooth_Micro.c
 * Author: Zbook
 *
 * Created on 8 de agosto de 2025, 05:40 PM
 */

#include <xc.h> // Se incluye la librería principal para los PICs
#include <stdint.h> // Para usar tipos de datos como uint8_t (byte sin signo)

// Definir la frecuencia del cristal del oscilador.
// El código utiliza SPBRG = 25, que es para 9600 baudios
// con un cristal de 4MHz. Mantenemos esta frecuencia para compatibilidad.
#define _XTAL_FREQ 4000000UL

// --- CONFIGURACIÓN DEL MICROCONTROLADOR (Fuses/Pragmas) ---
// Estas configuraciones corresponden a las del código en ensamblador original.
#pragma config FOSC = XT        // Oscillator Selection bits (Oscilador XT para 4MHz)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT deshabilitado)
#pragma config PWRTE = ON       // Power-up Timer Enable bit (PWRT habilitado)
#pragma config BOREN = OFF      // Brown-out Reset Enable bit (BOR deshabilitado)
#pragma config LVP = OFF        // Low-Voltage (Single-Supply) In-Circuit Serial Programming Enable bit (LVP deshabilitado)
#pragma config CPD = OFF        // Data EEPROM Memory Code Protection bit (Protección de código de la EEPROM deshabilitada)
#pragma config WRT = OFF        // Flash Program Memory Write Enable bits (Protección de escritura de la memoria de programa deshabilitada)
#pragma config CP = OFF         // Flash Program Memory Code Protection bit (Protección de código de la memoria de programa deshabilitada)

// --- VARIABLES GLOBALES ---
// Esta variable almacenará el byte recibido a través del módulo UART (Bluetooth).
// 'volatile' es importante porque la variable es modificada por la interrupción.
volatile uint8_t received_command; // Renombrada para mayor claridad, ya que es un "comando"

// --- FUNCIONES DE INICIALIZACIÓN ---

/**
 * @brief Inicializa el módulo UART (USART) del PIC16F877A para comunicación serial.
 * Configura la velocidad de transmisión/recepción (baud rate) a 9600.
 */
void UART_Init(void) {
    // Configuración del Baud Rate a 9600 con cristal de 4MHz.
    // SPBRG = (Fosc / (16 * Baud Rate)) - 1 = (4000000 / (16 * 9600)) - 1 = 25.04.
    SPBRG = 25; // Baud Rate Generator Register

    // Configuración del Transmit Status and Control Register (TXSTA)
    TXSTAbits.TXEN = 1;     // Habilitar transmisión (TX pin)
    TXSTAbits.BRGH = 1;     // Seleccionar High Baud Rate
    TXSTAbits.SYNC = 0;     // Configurar modo asíncrono (UART)

    // Configuración del Receive Status and Control Register (RCSTA)
    RCSTAbits.SPEN = 1;     // Habilitar el puerto serial (configura RC7/RX y RC6/TX)
    RCSTAbits.CREN = 1;     // Habilitar recepción continua (recibe datos continuamente)
    RCSTAbits.RX9 = 0;      // Seleccionar recepción de 8 bits

    // Configuración de las interrupciones para el UART
    PIE1bits.RCIE = 1;      // Habilitar la interrupción por recepción UART
    INTCONbits.PEIE = 1;    // Habilitar las interrupciones de periféricos
    INTCONbits.GIE = 1;     // Habilitar las interrupciones globales
}

// --- RUTINA DE SERVICIO DE INTERRUPCIÓN (ISR) ---
/**
 * @brief Esta función se ejecuta automáticamente cuando ocurre una interrupción.
 * Se encarga de manejar la interrupción de recepción UART y controlar los LEDs.
 */
void __interrupt() isr(void) {
    // Comprobar si la interrupción fue causada por la recepción UART (USART)
    if (PIR1bits.RCIF) {
        // Borrar la bandera de interrupción de recepción.
        PIR1bits.RCIF = 0;

        // Leer el dato recibido del registro de recepción (RCREG).
        received_command = RCREG;

        // --- Lógica de control de LEDs adaptada a tus conexiones (RB0, RB1, RB2) ---
        // Se utilizan operadores a nivel de bit para controlar solo los pines RB0, RB1, RB2
        // y asegurar que los demás pines del PORTB (RB3-RB7) permanezcan apagados.

        // Primero, apagar todos los LEDs. Como son Active Low, esto significa ponerlos en ALTO.
        PORTBbits.RB0 = 1; // Apaga RB0 (Rojo)
        PORTBbits.RB1 = 1; // Apaga RB1 (Verde)
        PORTBbits.RB2 = 1; // Apaga RB2 (Azul)

        switch (received_command) {
            case 'R': // Comando para encender solo el LED Rojo (RB0)
                PORTBbits.RB0 = 0; // Enciende RB0 (poniendo a BAJO)
                break;
            case 'V': // Comando para encender solo el LED Verde (RB1)
                PORTBbits.RB1 = 0; // Enciende RB1 (poniendo a BAJO)
                break;
            case 'A': // Comando para encender solo el LED Azul (RB2)
                PORTBbits.RB2 = 0; // Enciende RB2 (poniendo a BAJO)
                break;
            case 'T': // Comando para encender los TRES LEDs (Rojo, Verde, Azul)
                PORTBbits.RB0 = 0; // Enciende RB0
                PORTBbits.RB1 = 0; // Enciende RB1
                PORTBbits.RB2 = 0; // Enciende RB2
                break;
            case '0': // Comando para APAGAR TODOS los LEDs (ya están apagados por la lógica inicial)
                // No se necesita acción adicional aquí, ya que se apagan al inicio del switch.
                break;
            default:
                // Si el carácter recibido no es un comando conocido,
                // los LEDs permanecerán apagados (estado inicial del switch).
                break;
        }
    }
}

// --- FUNCIÓN PRINCIPAL DEL PROGRAMA ---
/**
 * @brief Función principal que se ejecuta al iniciar el microcontrolador.
 * Configura los puertos y el UART, y luego entra en un bucle infinito.
 */
void main(void) {
    // --- Configuración de los puertos I/O ---
    // Configurar PORTB como salida (todos los 8 pines).
    TRISB = 0x00;
    // Al iniciar, apagar todos los LEDs. Como son Active Low, esto significa poner los pines en ALTO.
    PORTB = 0xFF;           // Poner todos los pines del PORTB a 1 (todos los LEDs apagados inicialmente)

    // Configurar los pines RC6 (TX) y RC7 (RX) del UART.
    TRISCbits.TRISC6 = 0;   // RC6 (TX) se configura como salida
    TRISCbits.TRISC7 = 1;   // RC7 (RX) se configura como entrada

    // Inicializar el módulo UART.
    UART_Init();

    // --- Bucle principal infinito ---
    // El programa entra en este bucle y permanece aquí indefinidamente.
    // Toda la lógica de procesamiento de los datos recibidos por UART se
    // maneja dentro de la Rutina de Servicio de Interrupción (ISR).
    while (1) {
        // Este bucle puede estar vacío ya que la mayor parte de la lógica
        // se maneja por interrupciones. Aquí podrías añadir tareas
        // que se ejecuten continuamente pero que no requieran interrupciones.
    }
}
