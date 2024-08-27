/*
 * PWM1.c
 *
 * Created: 11/04/2024 22:15:54
 *  Author: diego
 */ 
#include "PWM1.h"
void update_duty_cycle0(uint8_t A1B0, uint8_t duty_cycle);
void update_duty_cycle1(uint8_t A1B0, uint8_t duty_cycle);
void update_duty_cycle2(uint8_t A1B0, uint8_t duty_cycle);
void salidaPWM0(uint8_t A1B0, uint16_t prescaler);
void salidaPWM1(uint8_t A1B0, uint16_t prescaler, uint16_t num_ICR1);
void salidaPWM2(uint8_t A1B0, uint16_t prescaler);

void update_duty_cycle0(uint8_t A1B0, uint8_t duty_cycle){//A=1, B=0
	if (A1B0 == 1){
		OCR0A = duty_cycle;
	}
	else if (A1B0 == 0){
		OCR0B = duty_cycle;
	}
}

void update_duty_cycle1(uint8_t A1B0, uint8_t duty_cycle){//A=1, B=0
	if (A1B0 == 1){
		OCR1A = duty_cycle;
	}
	else if (A1B0 == 0){
		OCR1B = duty_cycle;
	}
}

void update_duty_cycle2(uint8_t A1B0, uint8_t duty_cycle){//A=1, B=0
	if (A1B0 == 1){
		OCR2A = duty_cycle;
	}
	else if (A1B0 == 0){
		OCR2B = duty_cycle;
	}
}

void salidaPWM0(uint8_t A1B0, uint16_t prescaler){ //A -> D6, B -> D5
	TCCR0A = 0;
	TCCR0B = 0;
	if (A1B0 == 1){
		DDRD |= (1<<DDD6);
		TCCR0A |= (1<<COM0A1); //NO INVERTIDO
	}else if (A1B0 == 0){
		DDRD |= (1<<DDD5);
		TCCR0A |= (1<<COM0B1); //NO INVERTIDO
	} else if (A1B0 == 2){
		DDRD = 0;
		DDRD |= (1<<DDD5) | (1<<DDD6) | (1<<DDD1);
		TCCR0A |= (1<<COM0B1) | (1<<COM0A1); //NO INVERTIDO
	}
	TCCR0A |= (1<<WGM01) | (1<<WGM00);//fast PWM
	if (prescaler == 8){
		TCCR0B |= (1<<CS01);
		TCCR0B &= ~((1<<CS00)| (1<<CS02));
	}else if (prescaler == 64){
		TCCR0B |= (1<<CS00) | (1<<CS01);
		TCCR0B &= ~(1<<CS02);
	}else if (prescaler == 256){
		TCCR0B |= (1<<CS02);
		TCCR0B &= ~((1<<CS00) | (1<<CS01));
	}else if (prescaler == 1024){
		TCCR0B |= (1<<CS00) | (1<<CS02);
		TCCR0B &= ~(1<<CS01);
	}
}

void salidaPWM1(uint8_t A1B0, uint16_t prescaler, uint16_t num_ICR1){ //A -> D9, B -> D10
	TCCR1A = 0;
	TCCR1B = 0;
	if (A1B0 == 1){
		DDRB |= (1<<DDB1);
		TCCR1A |= (1<<COM1A1); //NO INVERTIDO
		} else if (A1B0 == 0){
		DDRB |= (1<<DDB2);
		TCCR1A |= (1<<COM1B1); //NO INVERTIDO
		} else if (A1B0 == 2){
		DDRB |= (1<<DDB1) | (1<<DDB2);
		TCCR1A |= (1<<COM1B1) | (1<<COM1A1); //NO INVERTIDO
	}
	TCCR1A |= (1<<WGM11);//TOP = ICR1
	TCCR1B |= (1<<WGM12) | (1<<WGM13);
	if (prescaler == 8){
		TCCR1B |= (1<<CS11);
		TCCR1B &= ~((1<<CS10)| (1<<CS12));
		}else if (prescaler == 64){
		TCCR1B |= (1<<CS10) | (1<<CS11);
		TCCR1B &= ~(1<<CS12);
		}else if (prescaler == 256){
		TCCR1B |= (1<<CS12);
		TCCR1B &= ~((1<<CS10) | (1<<CS11));
		}else if (prescaler == 1024){
		TCCR1B |= (1<<CS10) | (1<<CS12);
		TCCR1B &= ~(1<<CS11);
	}
	ICR1 = num_ICR1;
}

void salidaPWM2(uint8_t A1B0, uint16_t prescaler){ //A -> D11, B -> D3
	TCCR2A = 0;
	TCCR2B = 0;
	if (A1B0 == 1){
		DDRB |= (1<<DDB3);
		TCCR2A |= (1<<COM2A1); //NO INVERTIDO
		}else if (A1B0 == 0){
		DDRD |= (1<<DDD3);
		TCCR2A |= (1<<COM2B1); //NO INVERTIDO
		} else if (A1B0 == 2){
		DDRB |= (1<<DDB3);
		DDRD |= (1<<DDD3);
		TCCR2A |= (1<<COM2B1) | (1<<COM2A1); //NO INVERTIDO
	}
	TCCR2A |= (1<<WGM21) | (1<<WGM20);//fast PWM
	if (prescaler == 8){
		TCCR2B |= (1<<CS21);
		TCCR2B &= ~((1<<CS20)| (1<<CS22));
		}else if (prescaler == 32){
		TCCR2B |= (1<<CS20) | (1<<CS21);
		TCCR2B &= ~(1<<CS22);
		}else if (prescaler == 64){
		TCCR2B |= (1<<CS22);
		TCCR2B &= ~((1<<CS20) | (1<<CS21));
		}else if (prescaler == 128){
		TCCR2B |= (1<<CS20) | (1<<CS22);
		TCCR2B &= ~(1<<CS21);
		}else if (prescaler == 256){
		TCCR2B |= (1<<CS21) | (1<<CS22);
		TCCR2B &= ~(1<<CS20);
		}else if (prescaler == 1024){
		TCCR2B |= (1<<CS21) | (1<<CS22) | (1<<CS20);
	}
}