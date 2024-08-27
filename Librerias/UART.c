/*
 * UART.c
 *
 * Created: 23/07/2024 22:37:55
 *  Author: diego
 */ 
#define F_CPU 16000000
#include "UART.h"
#include <stdint.h>
#include <avr/io.h>

void init_uart(void){
	DDRD &= (1<<DDD0); //RX
	DDRD |= (1<<DDD1); //TX
	//Configurar UCSR0A
	UCSR0A = 0;
	//Configurar UCSR0B
	UCSR0B = 0;
	UCSR0B |= (1<<RXCIE0); //Habilitar interrupcion de recepcion
	UCSR0B |= (1<<TXCIE0); //Habilitar interrupcion de transmicion
	UCSR0B |= (1<<RXEN0); //Habilitar RX (recepcion)
	UCSR0B |= (1<<TXEN0); //Habilitar TX (transmición)
	//Configurar UCSR0C
	UCSR0C = 0;
	//UCSR0C &= ~((1<<UMSEL01) | (1<<UMSEL00)); //ASINCRONO
	//UCSR0C &= ~((1<<UPM01) | (1<<UPM00)); //NO PARIEDAD
	//UCSR0C &= ~(1<<USBS0); //UN BIT DE STOP, 2 BITS -> 1
	UCSR0C |= (1<<UCSZ01) | (1<<UCSZ00); //8 BITS DE ESPACIO
	UBRR0 = 103;//Configurar velocidad del baudrate 9600
}
void writetextUART(char* texto){
	uint8_t i;
	for (i=0; texto[i]!='\0';i++){
		while (!(UCSR0A & (1<<UDRE0)));//se queda en el while hasta que la bandera UDRE0 este en 1
		UDR0 = texto[i];
	}
}
void writeUART(char caracter){
	while (!(UCSR0A & (1<<UDRE0)));
	UDR0 = caracter;
}