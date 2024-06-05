/*
 * Microchip.c
 *
 * Created: 28/5/2024 21:19:47
 * Author : manuc
 */ 
/*
#include <avr/io.h>
#include "serialPort.h"
#include "RTC.h"
#include "DHT11.h"
//#include "timer.h" esta en duda su uso
int main(void)
{
	SerialPort_Init();
    
    while (1) 
    {
		
    }
}

*/
#define F_CPU 16000000UL
#include <avr/io.h>
#include <stdio.h>
#include "serialPort.h"
#include "DHT11.h"

#define BR9600 (0x67)	// 0x67=103 configura BAUDRATE=9600@16MHz


//char msg1[] = "TEMP: %02d °C HUM: %02d\% FECHA: %02d/%02d/%02d HORA:%02d:%02d:%03d\r\n"; //temp, hum, dd/mm/aa, hh/mm/sss
char  msg1[50];
uint8_t activo= 0; //usado como variable de control, si se pulsa 's' o 'S'
volatile char RX_Buffer=0;


int main(void)	
{	
	uint8_t temperatura;
	uint8_t humedad;
	uint8_t verificacion;
	
	volatile char dato = 0;
	
	SerialPort_Init(BR9600); 		// Inicializo formato 8N1 y BAUDRATE = 9600bps
	SerialPort_TX_Enable();			// Activo el Transmisor del Puerto Serie
	SerialPort_RX_Enable();			// Activo el Receptor del Puerto Serie
	
	while(1)
	{
		verificacion = DHTRead(&temperatura, &humedad);
		//verif = 0 hay error, verif=1 se lee correctamente
		if (verificacion){
				//sprintf(msg1, "TEMP: %02d °C HUM: %02d\% FECHA: %02d/%02d/%02d HORA:%02d:%02d:%03d\r\n", temperatura, humedad, )
				sprintf(msg1, "TEMP: %02d °C HUM: %02d%% \r \n ", temperatura, humedad);
			}
		else {
			sprintf(msg1, "error en dht");
		}
		
		SerialPort_Wait_For_TX_Buffer_Free(); // Espero a que el canal de transmisión este libre (bloqueante)
		//cada 2 segundos mando a terminal		
		SerialPort_Send_String(msg1);	
		
		SerialPort_Wait_Until_New_Data();	  // Polling - Bloqueante, puede durar indefinidamente.
		dato = SerialPort_Recive_Data();

		// Si presionan 's' se termina o reanuda
		if( dato == 's' || dato == 'S' )
		{
			activo = ~activo;
			// toggle activo
		}	
		/*else
		{				
			SerialPort_Wait_For_TX_Buffer_Free(); // Espero a que el canal de transmisión este libre (bloqueante)
			//cada 2 segundos mando a terminal
			
			SerialPort_Send_String(msg1);    
		}*/
	}
	return 0;

}

// Rutina de Servicio de Interrupción de Byte Recibido
ISR(USART_RX_vect){
	if (activo){
		//imprimirMensaje();
		SerialPort_Wait_For_TX_Buffer_Free(); // Espero a que el canal de transmisión este libre (bloqueante)
		//cada 2 segundos mando a terminal
		
		SerialPort_Send_String(msg1);
	}
	RX_Buffer = UDR0; //la lectura del UDR borra flag RXC
}
