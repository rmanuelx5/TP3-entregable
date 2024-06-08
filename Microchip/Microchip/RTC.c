/*
 * RTC.c
 *
 * Created: 3/6/2024 14:12:37
 *  Author: Barcala
 */ 


#include "rtc.h"

// Funciones auxiliares (estáticas, solo accesibles dentro de este archivo)
static uint8_t bcd_to_dec(uint8_t val) {
	return ((val / 16 * 10) + (val % 16));
}

static uint8_t dec_to_bcd(uint8_t val) {
	return ((val / 10 * 16) + (val % 10));
}

static void rtc_write_register(uint8_t reg, uint8_t value) {
	TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN); // START
	while (!(TWCR & (1 << TWINT)));
	TWDR = DS3232_ADDR << 1; // Dirección de escritura
	TWCR = (1 << TWINT) | (1 << TWEN);
	while (!(TWCR & (1 << TWINT)));
	TWDR = reg; // Registro a escribir
	TWCR = (1 << TWINT) | (1 << TWEN);
	while (!(TWCR & (1 << TWINT)));
	TWDR = value; // Valor a escribir
	TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO); // STOP
}

static uint8_t rtc_read_register(uint8_t reg) {
	rtc_write_register(reg, 0); // Dummy write para seleccionar el registro
	TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN); // Repetir START
	while (!(TWCR & (1 << TWINT)));
	TWDR = (DS3232_ADDR << 1) | 1; // Dirección de lectura
	TWCR = (1 << TWINT) | (1 << TWEN);
	while (!(TWCR & (1 << TWINT)));
	TWCR = (1 << TWINT) | (1 << TWEN); // ACK después de la dirección
	while (!(TWCR & (1 << TWINT)));
	TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO); // NACK y STOP
	while ((TWCR & (1 << TWINT)));
	return TWDR;
}

// Funciones públicas de la biblioteca
void rtcInit(void) {
	TWBR = 12; // Ajustar según la velocidad de reloj (400kHz en este caso)
	TWCR = (1 << TWEN); // Habilitar TWI
}

void setTime(const tiempo *t) {
	rtc_write_register(DS3232_TIME, dec_to_bcd(t->segundo));
	rtc_write_register(DS3232_TIME + 1, dec_to_bcd(t->minuto));
	rtc_write_register(DS3232_TIME + 2, dec_to_bcd(t->hora));
	rtc_write_register(DS3232_TIME + 4, dec_to_bcd(t->dia));
	rtc_write_register(DS3232_TIME + 5, dec_to_bcd(t->mes));
	rtc_write_register(DS3232_TIME + 6, dec_to_bcd(t->anio));
}

tiempo getTime(void) {
	tiempo t;
	t.segundo = bcd_to_dec(rtc_read_register(DS3232_TIME));
	t.minuto = bcd_to_dec(rtc_read_register(DS3232_TIME + 1));
	t.hora   = bcd_to_dec(rtc_read_register(DS3232_TIME + 2));
	t.dia   = bcd_to_dec(rtc_read_register(DS3232_TIME + 4));
	t.mes   = bcd_to_dec(rtc_read_register(DS3232_TIME + 5));
	t.anio   = bcd_to_dec(rtc_read_register(DS3232_TIME + 6));
	return t;
	
}