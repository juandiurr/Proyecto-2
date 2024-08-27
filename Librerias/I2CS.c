/*
 * I2C.c
 *
 * Created: 1/08/2024 20:52:50
 *  Author: diego
 */ 
#include "I2CS.h"

void I2C_slave_init(uint8_t address){
	DDRC &= ~((1<<DDC4)|(1<<DDC5));
	TWAR = (address << 1) & 0b11111110;//se asigna la dirección de entrada
	//TWAR = (address << 1| 0x01); //asigna dirección y habilita llamada general
	//se habilita interfaz, ACK automatico, se habilita ISR
	TWCR = (1<<TWEA)|(1<<TWEN)|(1<<TWIE);
}