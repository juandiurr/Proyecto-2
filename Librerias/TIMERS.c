/*
 * TIMERS.c
 *
 * Created: 16/04/2024 17:55:52
 *  Author: diego
 */ 
//Habilitar timers con prescalers e interrupcion
#include "TIMERS.h"
void timer0(uint16_t prescaler);
void timer1(uint16_t prescaler);
void timer2(uint16_t prescaler);

void timer0(uint16_t prescaler){
	//TIMSK0 |= (1<<TOIE0); //habilitar int timer0
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
void timer1(uint16_t prescaler){
	TIMSK1 |= (1<<TOIE1); //habilitar int timer1
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
}
void timer2(uint16_t prescaler){
	//TIMSK2 |= (1<<TOIE2); //habilitar int timer2
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