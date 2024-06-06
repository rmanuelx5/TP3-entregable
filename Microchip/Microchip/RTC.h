/*
 * RTC.h
 *
 * Created: 3/6/2024 14:12:11
 *  Author: Barcala
 */ 


#ifndef RTC_H_
#define RTC_H_

#include <avr/io.h>
#define SDA_RTC port
#include <stdio.h>

//prueba
#include "serialPort.h"
#define BR9600 (0x67)	// 0x67=103 configura BAUDRATE=9600@16MHz

typedef struct{
	uint8_t anio;      // Años
	uint8_t mes;       // Mes [1,12]
	uint8_t dia;       // Día del mes [1,31]
	uint8_t hora;      // Horas [0,23]
	uint8_t minuto;    // Minutos [0,59]
	uint8_t segundo;   // Segundos [0,59]
} tiempo;

void RTC_init(void);
tiempo getTime(void);

#endif /* RTC_H_ */