/*
 * RTC.c
 *
 * Created: 3/6/2024 14:12:37
 *  Author: Barcala
 */ 


#include "rtc.h"

// Funciones auxiliares (estáticas, solo accesibles dentro de este archivo)
//convertir a decimal
static uint8_t convertirDec(uint8_t val) {
	return ((val / 16 * 10) + (val % 16));
}

static void rtcWrite(uint8_t reg, uint8_t valor) {
	TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN); // START
	while (!(TWCR & (1 << TWINT)));
	TWDR = DS3232_ADDR << 1; // Dirección de escritura
	TWCR = (1 << TWINT) | (1 << TWEN);
	while (!(TWCR & (1 << TWINT)));
	TWDR = reg; // Registro a escribir
	TWCR = (1 << TWINT) | (1 << TWEN);
	while (!(TWCR & (1 << TWINT)));
	TWDR = valor; // Valor a escribir
	TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO); // STOP
}

static uint8_t rtcRead(uint8_t reg) {
	rtcWrite(reg, 0);
	TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN); // Repetir START
	while (!(TWCR & (1 << TWINT)));
	TWDR = (DS3232_ADDR << 1) | 1; // Dirección de lectura
	TWCR = (1 << TWINT) | (1 << TWEN);
	while (!(TWCR & (1 << TWINT)));
	TWCR = (1 << TWINT) | (1 << TWEN); // mando ACK
	while (!(TWCR & (1 << TWINT)));
	TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO); // NACK y STOP
	while ((TWCR & (1 << TWINT)));
	return TWDR;
}

void rtcInit(void) {
	TWBR = 12; // velocidad de reloj
	TWCR = (1 << TWEN); // Habilitar TWI
}

//orden 0 seg, 1 min, 2 hora, 3 dia (no usamos), 4 dia, 5 mes, 6 año

tiempo getTime(void) {
	tiempo t;
	t.segundo = convertirDec(rtcRead(DS3232_TIME));
	t.minuto = convertirDec(rtcRead(DS3232_TIME + 1));
	t.hora = convertirDec(rtcRead(DS3232_TIME + 2));
	t.dia = convertirDec(rtcRead(DS3232_TIME + 4));
	t.mes = convertirDec(rtcRead(DS3232_TIME + 5));
	t.anio = convertirDec(rtcRead(DS3232_TIME + 6));
	return t;
	
}