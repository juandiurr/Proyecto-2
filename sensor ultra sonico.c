/*
 * proyacto master.c
 *
 * Created: 10/08/2024 18:31:15
 * Author : diego
 */ 
#define SlaveAddress 0x40
#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>
#include <avr/interrupt.h>

#include "Librerias/I2CS.h"
const uint8_t start_clk = 0x0A;
const uint8_t start_clk_n = 0x02;
const uint8_t stop_clk = 0x08;
uint8_t range = 0;
uint8_t distance_cm = 0;
volatile uint8_t buffersus = 0;
volatile uint8_t buffer2sus = 0;

void TIMER_init(void);
void US_trigger(void);
uint8_t US_echo(void);

// Función para inicializar el timer


int main(void) {
	cli();
	  // Variable para almacenar la distancia en cm
	DDRD |= (1<<DDD2);//pines para mover el motor dc
	PORTD &= ~(1<<PORTD2);
	DDRD |= (1<<DDD3);
	PORTD &= ~(1<<PORTD3);
	TIMER_init();  // Inicializar el timer
	I2C_slave_init(SlaveAddress);
	sei();
	while (1) {
		US_trigger();  // Mandar el pulso al trigger para activar el ultrasonido
		distance_cm = US_echo();  // Leer el valor medido por el ultrasonido y guardarlo en la variable
		
		if (buffersus == 3){//mover hacia atras
			PORTD &= ~(1<<PORTD3);
			PORTD |= (1<<PORTD2);
		}else if (buffersus == 2){//Mover  andelante
			PORTD &= ~(1<<PORTD2);
			PORTD |= (1<<PORTD3);
		}else if (buffersus == 4){//no moverse
			PORTD &= ~(1<<PORTD2);
			PORTD &= ~(1<<PORTD3);
		}
	}
}

void TIMER_init(void) {
	DDRB |= (1<<DDB5);  // Configurar PB5 como salida (para el trigger del ultrasonido)
	TCCR1A = 0x00;  // Activar modo CTC
	TCCR1B = 0x08;  // Limpiar el dato de OC1A/OC1B
}

// Función para mandar el pulso al trigger del ultrasonido
void US_trigger(void) {
	PORTB |= (1 << PORTB5);  // Enviar pulso alto
	_delay_us(10);
	PORTB &= ~(1 << PORTB5);  // Enviar pulso bajo
}

// Función para leer el dato del ultrasonido
uint8_t US_echo(void) {
	
	while (!(PINB & 0x01));  // Esperar hasta que PB0 esté en alto (inicio de medición)
	TCNT1 = 0x00;  // Reiniciar el timer
	TCCR1B = start_clk_n;  // Iniciar el timer con preescalador 1:8
	while ((PINB & 0x01));  // Esperar hasta que PB0 esté en bajo (fin de medición)
	TCCR1B = stop_clk;  // Detener el timer
	range = TCNT1 / 58;  // Convertir el valor del timer a distancia en cm
	return range;
}

ISR(TWI_vect) {
	uint8_t estado = TWSR & 0xF8; // Lee los 3 bits superiores del registro de estado
	buffer2sus = distance_cm;
	switch (estado){
		case 0x60: // SLA+W recibido, ACK enviado
		case 0x70: // SLA+W recibido en modo general, ACK enviado
		TWCR |= (1 << TWINT); // Borra el flag TWINT para continuar
		break;

		case 0x80: // Datos recibidos, ACK enviado
		case 0x90: // Datos recibidos en llamada general, ACK enviado
		buffersus = TWDR; // Lee el dato recibido del registro de datos
		TWCR |= (1 << TWINT); // Borra el flag TWINT para continuar
		PORTD |= (1<<PORTD2);
		break;

		case 0xA8: // SLA+R recibido, ACK enviado
		case 0xB8: // Dato transmitido y ACK recibido
		TWDR = distance_cm; // Carga el valor del ADC en el registro de datos
		TWCR |= (1 << TWINT) | (1 << TWEA); // Borra el flag TWINT y habilita ACK para el próximo byte
		break;

		case 0xC0: // Dato transmitido y NACK recibido
		case 0xC8: // Último dato transmitido y ACK recibido
		TWCR |= (1 << TWINT) | (1 << TWEA); // Borra el flag TWINT y habilita ACK para el próximo byte
		break;
		
		default: // Manejo de errores
		TWCR |= (1 << TWINT) | (1 << TWSTO); // Borra el flag TWINT y envía una condición de STOP
		break;
	}
}