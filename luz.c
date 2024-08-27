/*
 * GccApplication1.c
 *
 * Created: 10/08/2024 17:17:59
 * Author : diego
 */ 
#define SlaveAddress 0x30
#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>
#include <avr/interrupt.h>
#include "Librerias/ADC.h"
#include "Librerias/PWM1.h"
#include "Librerias/TIMERS.h"
#include "Librerias/UART.h"
#include "Librerias/I2CS.h"
volatile uint8_t luz = 0;
volatile uint8_t uart = 0;
volatile uint8_t duty = 15;
volatile uint8_t timer = 0;
volatile uint8_t estado = 0;
volatile uint8_t buffer = 0;//recibir del master
volatile uint8_t buffer2 = 0;//mandar al master
volatile uint8_t buff = 0;//0 cerrado, 1 cerrando, 2 abierto, 3 abriendo

void ver_luz(void);
int main(void)
{
	cli();
	ADC_(128);
	ADC2();
	salidaPWM1(0, 1024, 312);//periodo 20ms
	init_uart();
	timer0(1024);
	timer2(1024);
	DDRD |= (1<<DDD2);
	PORTD &= ~(1<<PORTD2);
	I2C_slave_init(SlaveAddress);
	TIMSK0 &= ~(1<<TOIE0);//deshabilitar int timer0
	TIMSK2 &= ~(1<<TOIE2);//deshabilitar int timer0
	sei();
    /* Replace with your application code */
    while (1) 
    {
		
		if(luz > 15){//hay luz
			buffer2 = 1;
			
			do{
				if (buffer == 1 && buff == 0){
					
					TIMSK0 |= (1<<TOIE0); //habilitar int timer0
					TCNT0 = 100;//10ms
					buff = 3;//panel abriendo
					buffer = 0;
					PORTD |= (1<<PORTD2);
				}
				
				ADCSRA |= (1<<ADSC);//HABILITAR CONVERSION
				update_duty_cycle1(0, duty);
				//PORTD |= (1<<PORTD2);
				ver_luz();
				
			}while(luz > 15);
			
		}else if(luz <= 15){//no luz
			buffer2 = 0;
			
			do{
				if (buff == 2){//abierto
					TIMSK2 |= (1<<TOIE2); //habilitar int timer2
					TCNT2 = 100;//10ms
					PORTD &= ~(1<<PORTD2);
					buff = 1;
				}
				ADCSRA |= (1<<ADSC);//HABILITAR CONVERSION
				update_duty_cycle1(0, duty);
				//PORTD &= ~(1<<PORTD2);
				ver_luz();
				
			}while(luz <= 15);
			
		}
    }
}
void ver_luz(void){
	if(uart == 98){
		writeUART(luz/100 + 48);
		writeUART((luz%100)/10 + 48);
		writeUART(luz%10 + 48);
		writeUART(10);
		uart = 0;
	}
}

ISR(TIMER2_OVF_vect){
	TCNT2 = 100;//10ms
	TIFR2 |= (1<<TOV2); //apagar bandera
	timer++;
	if(duty == 15){
		TIMSK2 &= ~(1<<TOIE2);//deshabilitar int timer0
		buff = 0;//cerrado
		buffer2 = 0;
		timer = 0;
	}
	if (timer == 15){
		duty--;
		timer = 0;
	}
}
ISR(TIMER0_OVF_vect){
	TCNT0 = 100;//10ms
	TIFR0 |= (1<<TOV0); //apagar bandera
	timer++;
	if(duty == 28){
		TIMSK0 &= ~(1<<TOIE0);//deshabilitar int timer0
		buff = 2;//abierto
		buffer2 = 2;
		timer = 0;
	}
	if (timer == 15){
		duty++;
		timer = 0;
	}
}
ISR(USART_RX_vect){
	if (UDR0 == 'b'){//b
		uart = 98;
	}
}
ISR(ADC_vect){
	luz = ADCH;
	ADCSRA |= (1<<ADIF); //apagar bandera
}




ISR(TWI_vect) {
	uint8_t estado = TWSR & 0xF8; // Lee los 3 bits superiores del registro de estado

	switch (estado){
		case 0x60: // SLA+W recibido, ACK enviado
		case 0x70: // SLA+W recibido en modo general, ACK enviado
		TWCR |= (1 << TWINT); // Borra el flag TWINT para continuar
		break;

		case 0x80: // Datos recibidos, ACK enviado
		case 0x90: // Datos recibidos en llamada general, ACK enviado
		buffer = TWDR; // Lee el dato recibido del registro de datos
		TWCR |= (1 << TWINT); // Borra el flag TWINT para continuar
		PORTD |= (1<<PORTD2);
		break;

		case 0xA8: // SLA+R recibido, ACK enviado
		case 0xB8: // Dato transmitido y ACK recibido
		TWDR = buffer2; // Carga el valor del ADC en el registro de datos
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
