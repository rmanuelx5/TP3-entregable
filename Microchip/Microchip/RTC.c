/*
 * RTC.c
 *
 * Created: 3/6/2024 14:12:37
 *  Author: Barcala
 */ 

#include "RTC.h"


//prototipos de funciones internas
unsigned char RTC_read(unsigned char);
void RTC_start(void);
void RTC_stop(void);
void mandarACK(unsigned char);




void RTC_init(void){
	TWSR=0x00;	//prescaler en 0
	TWBR=152;	//frecuencia de scl
	TWCR=0x04;	//enciende TWI
	
	//prueba
	
	SerialPort_Init(BR9600);  // Inicializo formato 8N1 y BAUDRATE = 9600bps
	SerialPort_TX_Enable();   // Activo el Transmisor del Puerto Serie
	SerialPort_RX_Enable();   // Activo el Receptor del Puerto Serie
}
//prueba
void DebbugT(tiempo t){
	char  msg1[50];
	sprintf(msg1, "Datos obtenidos: %02d, %02d, %04d, %02d, %02d, %02d // ", t.dia, t.mes, t.anio, t.hora, t.minuto, t.segundo);
	SerialPort_Wait_For_TX_Buffer_Free();
	SerialPort_Send_String(msg1);
}

void RTC_start(void){
	TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
	while ((TWCR & (1<<TWINT)) == 0);
}

unsigned char RTC_read (uint8_t ultimo){
	
	mandarACK(ultimo);
	
	while ((TWCR & (1 << TWINT)) == 0);
	return TWDR;
}

void RTC_stop (){
	TWCR= (1 << TWINT)|(1 << TWEN) |(1 << TWSTO) ;
}

tiempo getTime(){
	tiempo t;
	RTC_start();
	
	t.segundo = RTC_read(0);
	t.minuto = RTC_read(1);
	t.hora = RTC_read(2);
	//read(3) es el dia de la semana
	t.dia = RTC_read(4);
	t.mes = RTC_read(5);
	t.anio = RTC_read(6);
	
	DebbugT(t);
	RTC_stop();
	
	return t;
}

void mandarACK(uint8_t ultimo){
	if (ultimo < 6) //send ACK
		TWCR=(1 << TWINT)|(1 << TWEN) |(1 << TWEA) ; //send ACK
	else
		TWCR= (1 << TWINT) |(1 << TWEN) ; //send NACK
}