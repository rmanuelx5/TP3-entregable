/*
 * dht11.h
 *
 * Created: 3/6/2024 14:11:42
 *  Author: Barcala
 */ 


#ifndef DHT11_H_
#define DHT11_H_

#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#define DHT11_PIN PC0
#include <stdio.h>

//prueba
#include "serialPort.h"
#define BR9600 (0x67)	// 0x67=103 configura BAUDRATE=9600@16MHz

uint8_t DHTRead(uint8_t *temperatura, uint8_t *humedad);


#endif /* DHT11_H_ */