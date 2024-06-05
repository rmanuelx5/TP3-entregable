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

uint8_t DHTRead(uint8_t *temperatura, uint8_t *humedad);


#endif /* DHT11_H_ */