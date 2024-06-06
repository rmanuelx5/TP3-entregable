/*
 * dht11.c
 *
 * Created: 3/6/2024 14:11:26
 *  Author: Barcala
 */ 
 
 /*	INFO DE TIEMPOS DEL DATASHEET
	bajo 18ms envio se�al
	alto 40us esperar respuesta del sensor
	bajo 80us dht envia respuesta
	alto 80us
	empieza transmision
 */
 
#define DHT11_PIN PC0
#include "DHT11.h"
#define F_CPU 16000000UL
#include <avr/io.h>
#include <stdio.h>
#include "serialPort.h"
#define BR9600 (0x67) // 0x67=103 configura BAUDRATE=9600@16MHz


//prototipos de funciones internas
uint8_t pulsoInicio(void);
uint8_t leerBit(void);


void DHT11_Init() {
	DDRC |= (1 << DHT11_PIN); // Configurar el pin como salida
	PORTC |= (1 << DHT11_PIN); // Activar pull-up

	SerialPort_Init(BR9600);  // Inicializo formato 8N1 y BAUDRATE = 9600bps
	SerialPort_TX_Enable();   // Activo el Transmisor del Puerto Serie
	SerialPort_RX_Enable();   // Activo el Receptor del Puerto Serie
}

void Debbug(uint8_t lectura[5]){
	char  msg1[50];
	sprintf(msg1, "Datos obtenidos: %02d, %02d, %02d, %02d, %02d // ", lectura[0], lectura[1], lectura[2], lectura[3], lectura[4]);
	SerialPort_Wait_For_TX_Buffer_Free();
	SerialPort_Send_String(msg1);
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
		return 0; // Si pulsoInicio falla, retorna 0
	}

	//Debbug(lectura);
	
	uint8_t checksum = lectura[0] + lectura[1] + lectura[2] + lectura[3];
	if (checksum != lectura[4]) {
		return 0; // Error en la lectura
	}

	*temperatura = lectura[2];
	*humedad = lectura[0];

	return 1;
}



uint8_t pulsoInicio(){
	// Env�o se�al de inicio y espero 18ms hasta que DHT la detecte
	DDRC |= (1 << DHT11_PIN);   // Configura el pin como salida
	PORTC &= ~(1 << DHT11_PIN); // Baja el pin
	_delay_ms(18);              // Mant�n el pin bajo durante 18ms

	// Sube el pin y espera 20-40us
	PORTC |= (1 << DHT11_PIN);
	
	//_delay_us(20);
	DDRC &= ~(1 << DHT11_PIN);  // Configura el pin como entrada

	// Esperar respuesta del sensor
	_delay_us(40);
	if (PINC & (1 << DHT11_PIN)) {
		return 0; // Si el pin sigue alto, hay un error
	}
	_delay_us(80);
	if (!(PINC & (1 << DHT11_PIN))) {
		return 0; // Si el pin sigue bajo, hay un error
	}
	_delay_us(80);
	if ((PINC & (1 << DHT11_PIN))) {
		return 0; // Si el pin sigue alto, hay un error
	}
	// Se inici� correctamente el sensor
	return 1;
}


uint8_t leerBit(){
	uint8_t contador = 0;
	while (!(PINC & (1 << DHT11_PIN))); // Esperar a que el pin se ponga alto
	_delay_us(30);  // Esperar 30us para estar en la mitad del pulso alto
	
	if (PINC & (1 << DHT11_PIN)) {
		// Si el pin sigue alto, el bit es 1
		while (PINC & (1 << DHT11_PIN)) {
			contador++;
			_delay_us(1);
		}
		return 1;
	}
	// Si el pin est� bajo, el bit es 0
	return 0;
}


