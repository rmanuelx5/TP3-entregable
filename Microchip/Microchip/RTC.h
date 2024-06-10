/*
 * RTC.h
 *
 * Created: 3/6/2024 14:12:11
 *  Author: Barcala
 */ 
// RTC.h

#ifndef RTC_H
#define RTC_H
#define F_CPU 16000000UL
// Direcciones de registro del DS3232
#define DS3232_ADDR 0x68
#define DS3232_TIME 0x00
#define DS3232_CONTROL 0x0E
#define DS3232_STATUS 0x0F

#include <stdint.h>
#include <util/twi.h>


// Definición de la estructura para almacenar la hora
typedef struct {
	uint8_t segundo;
	uint8_t minuto;
	uint8_t hora;
	uint8_t dia;
	uint8_t mes;
	uint16_t anio;
} tiempo;

// Prototipos de funciones
void rtcInit(void);
void setTime(const tiempo *t);
tiempo getTime(void);

#endif

