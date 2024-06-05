/*
 * dht11.c
 *
 * Created: 3/6/2024 14:11:26
 *  Author: Barcala
 */ 
 
 /*	INFO DE TIEMPOS DEL DATASHEET
	bajo 18ms envio señal
	alto 40us esperar respuesta del sensor
	bajo 80us dht envia respuesta
	alto 80us
	empieza transmision
 */
 
#define DHT11_PIN PC0
#include "DHT11.h"


//prototipo funciones internas
//void pulsoInicio(uint8_t);


void DHT11_Init() {
	DDRC |= (1 << DHT11_PIN); // Configurar el pin como salida
	PORTC |= (1 << DHT11_PIN); // Activar pull-up
}
 
uint8_t DHTRead(uint8_t *temperatura, uint8_t *humedad){
	uint8_t control = 0;
	uint8_t lectura[5] = {0};
	//funcion interna señal inicio - esperar   
	pulsosInicio(control);
	
	// Leer 40 bits de datos
	if (control){
		for (uint8_t i = 0; i < 5; i++) {
			lectura[i]=0; //inicializo para realizar el OR con los datos
			for (uint8_t j = 0; j < 8; j++) {
				while (!(PINC & (1 << DHT11_PIN))); // Esperar el comienzo del bit
					uint8_t bit = reciboBit()
				_delay_us(30); //verifico si se mantiene el valor
				if (PINC & (1 << DHT11_PIN))
					lectura[i] |= (1 << (7 - j));
				while (PINC & (1 << DHT11_PIN)); // Esperar el fin del bit
			}	
		}
	}
	
	// verifico que la lectura sea válida
	//segun la datasheet lectura[4] es igual a la sumatoria de lectura[0]..[3]
	uint8_t checksum = lectura[0] + lectura[1] + lectura[2] + lectura[3];
	if (checksum != lectura[4])
		//hubo un error de lectura
		return 0;
	
	*temperatura = (uint8_t) lectura[2];
	*humedad = (uint8_t) lectura[0];
	//redondeo decimales
	if(lectura[1] >= 50)
		*humedad++;
	if(lectura[3] >= 50)
		*temperatura++;
	return 1;
}


void pulsosInicio(uint8_t control){
	 // Envío señal de inicio y espero 18ms hasta que DHT la detecte
	 DDRC |= (1 << DHT11_PIN);
	 PORTC &= ~(1 << DHT11_PIN);
	 _delay_ms(18);	 
	
	 //Señal en alto y espero 40ms	
	 PORTC |= (1 << DHT11_PIN);
	 _delay_us(40);
	 DDRC &= ~(1 << DHT11_PIN);
	 
	 // Esperar respuesta del sensor
	 _delay_us(80);
	 if (PINC & (1 << DHT11_PIN)){
		control = 0; // Si el pin sigue alto, hay un error
		return;
	 }
	 _delay_us(80);
	 if (!(PINC & (1 << DHT11_PIN))){
		control = 0; // Si el pin sigue bajo, hay un error
		return;
	 }
	 //Se inicio correctamente el sensor
	 control = 1;	
}

/*uint8_t leerBit(){
	if
	
}
*/
