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
 
#include "DHT11.h"
 
//prototipos de funciones internas
uint8_t pulsoInicio(void);
uint8_t leerBit(void);


void DHT11_Init() {
	DDRC |= (1 << DHT11_PIN); // Configurar el pin como salida
	PORTC |= (1 << DHT11_PIN); // Activar pull-up
}

 
uint8_t DHTRead(uint8_t *temperatura, uint8_t *humedad){
	uint8_t lectura[5] = {0};

	if (pulsoInicio()){
		for (uint8_t i = 0; i < 5; i++) {
			lectura[i]=0;
			for (uint8_t j = 0; j < 8; j++) {
				uint8_t bit = leerBit();
				if (bit != 2) //si bit es 2 es error
					lectura[i] |= (bit << (7 - j));
				else return 0; //codigo de error
			}
		}
	} else {
		return 0;  
	}

	uint8_t checksum = lectura[0] + lectura[1] + lectura[2] + lectura[3];
	
	if (checksum != lectura[4]) {
		return 0; // Error en la lectura
	}

	*temperatura = lectura[2];
	*humedad = lectura[0];

	return 1;
}

uint8_t pulsoInicio(){
	// Envío señal de inicio y espero 18ms hasta que DHT la detecte
	DDRC |= (1 << DHT11_PIN);   // Configura el pin como salida
	PORTC &= ~(1 << DHT11_PIN); // Baja el pin
	_delay_ms(18);              // Mantén el pin bajo durante 18ms

	// Sube el pin y espera 20-40us
	PORTC |= (1 << DHT11_PIN);
	
	//_delay_us(20);
	DDRC &= ~(1 << DHT11_PIN);  // Configura el pin como entrada

	// Esperar respuesta del sensor
	_delay_us(40);
	if (PINC & (1 << DHT11_PIN)) {
		return 0; // Si el pin sigue alto, hay un error
	}
	_delay_us(85);
	if (!(PINC & (1 << DHT11_PIN))) {
		return 0; // Si el pin sigue bajo, hay un error
	}
	_delay_us(85);
	if ((PINC & (1 << DHT11_PIN))) {
		return 0; // Si el pin sigue alto, hay un error
	}
	// Se inició correctamente el sensor
	return 1;
}


uint8_t leerBit(){
	uint8_t contador = 0;
	while (!(PINC & (1 << DHT11_PIN))); // Esperar a que el pin se ponga alto
	_delay_us(35);  // Esperar 30us para estar en la mitad del pulso alto
	
	if (PINC & (1 << DHT11_PIN)) {
		// Si el pin sigue alto, el bit es 1
		while (PINC & (1 << DHT11_PIN)) {
			contador++;
			_delay_us(1);
		}
		return 1;
	}
	// Si el pin está bajo, el bit es 0
	return 0;
}


