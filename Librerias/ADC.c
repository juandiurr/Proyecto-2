/*
 * ADC.c
 *
 * Created: 12/04/2024 18:20:21
 *  Author: diego
 */ 
#include "ADC.h"

void ADC_(uint8_t prescaler);
void ADC0(void);
void ADC1(void);
void ADC2(void);
void ADC3(void);
void ADC4(void);
void ADC5(void);
void ADC6(void);
void ADC7(void);
void habilitar_conversion(void);

void ADC_(uint8_t prescaler){
	ADMUX = 0;
	ADMUX |= (1<<REFS0);
	//JUSTIFICACION A LA IZQUIERDA
	ADMUX |= (1<<ADLAR);
	//HABILITAR INTERRUPCION DEL ADC
	ADCSRA |= (1<<ADIE);
	if (prescaler == 2){
		ADCSRA |= (1<<ADPS0);
		} else if (prescaler == 4){
		ADCSRA |= (1<<ADPS1) ;
		}else if (prescaler == 8){
		ADCSRA |= (1<<ADPS1) | (1<<ADPS0);
		}else if (prescaler == 16){
		ADCSRA |= (1<<ADPS2);
		}else if (prescaler == 32){
		ADCSRA |= (1<<ADPS2) | (1<<ADPS0);
		}else if (prescaler == 128){
		ADCSRA |= (1<<ADPS2) | (1<<ADPS1) | (1<<ADPS0);
		}else{
		ADCSRA |= (1<<ADPS2) | (1<<ADPS1) | (1<<ADPS0);
	}
	//HABILITAR ADC
	ADCSRA |= (1<<ADEN);
}

void ADC0(void){
	ADMUX &= ~((1<<MUX1) | (1<<MUX2) | (1<<MUX3) | (1<<MUX0));
}

void ADC1(void){
	//ADC1
	ADMUX &= ~((1<<MUX1) | (1<<MUX2) | (1<<MUX3) | (1<<MUX0));
	ADMUX |= (1<<MUX0);
}
void ADC2(void){
	//ADC2
	ADMUX &= ~((1<<MUX1) | (1<<MUX2) | (1<<MUX3) | (1<<MUX0));
	ADMUX |= (1<<MUX1);
}
void ADC3(void){
	//ADC2
	ADMUX &= ~((1<<MUX1) | (1<<MUX2) | (1<<MUX3) | (1<<MUX0));
	ADMUX |= (1<<MUX1) | (1<<MUX0) ;
}
void ADC4(void){
	//ADC4
	ADMUX &= ~((1<<MUX1) | (1<<MUX2) | (1<<MUX3) | (1<<MUX0));
	ADMUX |= (1<<MUX2);
}
void ADC5(void){
	//ADC5
	ADMUX &= ~((1<<MUX1) | (1<<MUX2) | (1<<MUX3) | (1<<MUX0));
	ADMUX |= (1<<MUX2) | (1<<MUX0);
	
}
void ADC6(void){
	//ADC5
	ADMUX &= ~((1<<MUX1) | (1<<MUX2) | (1<<MUX3) | (1<<MUX0));
	ADMUX |= (1<<MUX2) | (1<<MUX1) ;
	
}
void ADC7(void){
	//ADC7
	ADMUX &= ~((1<<MUX1) | (1<<MUX2) | (1<<MUX3) | (1<<MUX0));
	ADMUX |= (1<<MUX2) | (1<<MUX1) | (1<<MUX0);
}

void habilitar_conversion(void){
	ADCSRA |= (1<<ADSC);//HABILITAR CONVERSION
}
