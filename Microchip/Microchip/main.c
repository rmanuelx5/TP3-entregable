/*
 * Microchip.c
 *
 * Created: 28/5/2024 21:19:47
 * Author : manuc
 */ 

#define F_CPU 16000000UL
#include <avr/io.h>
#include <stdio.h>
#include "serialPort.h"
#include "DHT11.h"
#include "RTC.h"

#define BR9600 (0x67)	// 0x67=103 configura BAUDRATE=9600@16MHz


char  msg1[50];
uint8_t activo= 1; //usado como variable de control, si se pulsa 's' o 'S'
//comunicación con la ISR
volatile char RX_Buffer=0;


int main(void)	
{	
	//variables a utilizar
	uint8_t temperatura;
	uint8_t humedad;
	uint8_t verificacion;
	tiempo t;
	uint8_t ultImpr = -1;
	
	//inicializacion de periféricos
	SerialPort_Init(BR9600); 		// Inicializo formato 8N1 y BAUDRATE = 9600bps
	SerialPort_TX_Enable();			// Activo el Transmisor del Puerto Serie
	SerialPort_RX_Enable();			// Activo el Receptor del Puerto Serie
	SerialPort_RX_Interrupt_Enable();	// Activo Interrupción de recepcion.
	sei();
		
	DHT11_Init();
	rtcInit();	
	
	
	//bucle infinito
 	while(1){
		
		if(RX_Buffer){ // recepción no bloqueante de datos
			
			if(RX_Buffer == 's' || RX_Buffer == 'S'){
				if (activo){
					activo= 0;
					sprintf(msg1, "Desactivado\r\n");
				}
				else{
					activo=1;
					sprintf(msg1, "Activado\r\n");
				}
				
				RX_Buffer = UDR0; //la lectura del UDR borra flag RXC
						
				SerialPort_Wait_For_TX_Buffer_Free(); // Espero a que el canal de transmisión este libre (bloqueante)
				SerialPort_Send_String(msg1);
			}
		}
		
		if (activo){
			verificacion = DHTRead(&temperatura, &humedad);
			t = getTime();
			
			if(!(t.segundo % 2) && ultImpr != t.segundo){
					
				if (verificacion){
					ultImpr = t.segundo;
					sprintf(msg1, "TEMP: %02d °C HUM: %02d%% FECHA: %02d/%02d/%02d HORA:%02d:%02d:%02d\r\n", temperatura, humedad, t.dia, t.mes, t.anio, t.hora, t.minuto, t.segundo);
					SerialPort_Wait_For_TX_Buffer_Free(); // Espero a que el canal de transmisión este libre (bloqueante)
					SerialPort_Send_String(msg1);
				}
				else {
					sprintf(msg1, "Error en dht \r\n");
					SerialPort_Wait_For_TX_Buffer_Free();  // Espero a que el canal de transmisión esté libre (bloqueante)
					SerialPort_Send_String(msg1);
					
				}
			}
		}
	}
	return 0;
}

// Rutina de Servicio de Interrupción de Byte Recibido
ISR(USART_RX_vect){
	
	RX_Buffer = UDR0; //la lectura del UDR borra flag RXC
}
