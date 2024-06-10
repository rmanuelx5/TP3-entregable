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
#include "RTC.h"

#define BR9600 (0x67)	// 0x67=103 configura BAUDRATE=9600@16MHz


char  msg1[50];
uint8_t activo= 1; //usado como variable de control, si se pulsa 's' o 'S'
//comunicaci�n con la ISR
volatile char RX_Buffer=0;


int main(void)	
{	
	uint8_t temperatura;
	uint8_t humedad;
	uint8_t verificacion;
	
	tiempo t;
	
	uint8_t ultImpr = -1;
	
	
	SerialPort_Init(BR9600); 		// Inicializo formato 8N1 y BAUDRATE = 9600bps
	SerialPort_TX_Enable();			// Activo el Transmisor del Puerto Serie
	SerialPort_RX_Enable();			// Activo el Receptor del Puerto Serie
	SerialPort_RX_Interrupt_Enable();	// Activo Interrupci�n de recepcion.
	sei();
		
	rtcInit();	
	t.hora = 2;
	t.minuto = 24;
	t.segundo = 23;
	t.anio = 2006;
	t.dia = 23;
	t.mes = 2;
	setTime(&t);
	
 	while(1){
		
		if(RX_Buffer){ // recepci�n NO Bloqueante
			// Si presionan 's' se termina el programa
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
						
				SerialPort_Wait_For_TX_Buffer_Free(); // Espero a que el canal de transmisi�n este libre (bloqueante)
				SerialPort_Send_String(msg1);
			}
		}
		
		if (activo){
			verificacion = DHTRead(&temperatura, &humedad);
			t = getTime();
			
			if (verificacion){
			
					if(!(t.segundo % 2) && ultImpr != t.segundo){
						ultImpr = t.segundo;
						sprintf(msg1, "TEMP: %02d �C HUM: %02d%% FECHA: %02d/%02d/%02d HORA:%02d:%02d:%02d\r\n", temperatura, humedad, t.dia, t.mes, t.anio, t.hora, t.minuto, t.segundo);

						SerialPort_Wait_For_TX_Buffer_Free(); // Espero a que el canal de transmisi�n este libre (bloqueante)
						SerialPort_Send_String(msg1);
					}
				}
			else {
				sprintf(msg1, "error en dht");
				SerialPort_Wait_For_TX_Buffer_Free();  // Espero a que el canal de transmisi�n est� libre (bloqueante)
				SerialPort_Send_String(msg1);
			}
		}
				
		//leo tecla en terminal
// 		SerialPort_Wait_Until_New_Data();	 
// 		dato = SerialPort_Recive_Data();

		// Si presionan 's' o 'S' se termina o reanuda el informe en consola
// 		if( dato == 's' || dato == 'S' )
// 		{
// 			activo = ~activo;
// 			// toggle activo
// 		}	
		/*else
		{				
			SerialPort_Wait_For_TX_Buffer_Free(); // Espero a que el canal de transmisi�n este libre (bloqueante)
			//cada 2 segundos mando a terminal
			
			SerialPort_Send_String(msg1);    
		}*/
	}
	return 0;

}

// Rutina de Servicio de Interrupci�n de Byte Recibido
ISR(USART_RX_vect){
	
	RX_Buffer = UDR0; //la lectura del UDR borra flag RXC
}
