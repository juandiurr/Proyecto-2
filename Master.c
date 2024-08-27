/*
 * proyacto master.c
 *
 * Created: 10/08/2024 18:31:15
 * Author : diego
 */ 

#define SlaveLUZ 0x30
#define SlaveSUS 0x40
#define SlaveTEM 0x48
#define F_CPU 16000000UL
#include "Librerias/I2CM.h"
#include "Librerias/LCD8b.h"
#include "Librerias/TIMERS.h"
#include "Librerias/UART.h"
#include <avr/io.h>
#include <stdint.h>
#include <stdio.h>
#include <avr/interrupt.h>
#include <util/delay.h>
uint8_t *buffer_luz = 0;
uint8_t *buffer_tem = 0;
uint8_t *buffer_sus = 0;
uint16_t tura = 0;
uint8_t temp = 0;
uint8_t estado = 0;
volatile uint8_t buffer_luz2 = 50;
volatile uint8_t toggle = 0;
volatile uint8_t timer = 0;
volatile uint8_t delay = 0;
volatile uint8_t timer_2 = 0;
volatile uint8_t uartt = 0;
volatile uint8_t uartt2 = 0;
volatile uint8_t distancia = 0;
char tempca[4];
char susca[4];
char luzca[4];




void ver_luz(void);
void ver_temp(void);
void ver_sus(void);
void esp32(void);
void delayy(void);
int main(void)
{
	cli();
	init_uart();
	initLCD8bits();
	timer1(1024);
	LCD_Set_Cursor(1,1);
	LCD_Write_String("LUZ: SUS: TEM:");
	//_delay_ms(1000);
	LCD_Set_Cursor(13,2);
	LCD_WriteCaracter(0xDF);
	LCD_Set_Cursor(14,2);
	LCD_WriteCaracter('C');
	I2C_master_init(100000, 1);//inicializar master con fsc 100k Hz, prescaler 1
	sei();
	
//******************************************************************************
//								--LOOP PRINCIPAL--
//******************************************************************************
    while (1) 
    {
		
		
	//******************************************************************************
	//								--ESCLAVO LUZ--
	//******************************************************************************
		
		temp = I2C_master_read(SlaveLUZ, buffer_luz);//leer estado de luz
		if (temp != 1){//error
			LCD_Set_Cursor(5,2);
			LCD_WriteCaracter(temp);
			/*writetextUART("error con esclavo luz: ");
			writeUART(temp);
			writeUART(10);*/
		}else{
			LCD_Set_Cursor(5,2);
			LCD_WriteCaracter(' ');
		}
		if (*buffer_luz == 0){//no hay luz
			estado = 0;
			LCD_Set_Cursor(5,1);
			LCD_WriteCaracter(' ');
			if (uartt2 == 1){
				uartt2 = 0;
			}
		}else if(*buffer_luz == 1){//hay luz y cerrado
			estado = 1;
			/*if (toggle == 1){
				LCD_Set_Cursor(1,1);
				LCD_Write_String("LUZ:");
				}else if(toggle == 0){
				LCD_Set_Cursor(1,1);
				LCD_Write_String("    ");
			}*/
			LCD_Set_Cursor(5,1);
			LCD_WriteCaracter(0x7F);
			if (uartt2 == 1){
				temp = I2C_master_write(1, SlaveLUZ);//le indica al esclavo luz de mover el servo
				if (temp != 1){
					LCD_Set_Cursor(5,1);
					LCD_WriteCaracter('e');
					LCD_Set_Cursor(5,2);
					LCD_WriteCaracter(temp);
					/*writetextUART("error con esclavo luz 2: ");
					writeUART(temp);
					writeUART(10);*/
				}
				uartt2 = 0;
			}
			
		}else if(*buffer_luz == 2){//abierto
			estado = 2;
			if (uartt2 == 1){
				uartt2 = 0;
			}
			LCD_Set_Cursor(5,1);
			LCD_WriteCaracter(' ');
		}
		if (estado == 0){
			if(timer == 2){
				if(buffer_luz2 != 0){
					buffer_luz2--;
					timer = 0;
				}
				
			}
			
		}else if(estado == 1){
			if(timer == 2){
				if(buffer_luz2 != 0){
					buffer_luz2--;
					timer = 0;
				}
			}
		}else if(estado == 2){
			if(timer == 2){
				if(buffer_luz2 != 100){
					buffer_luz2++;
					timer = 0;
				}
			}
		}
		if(buffer_luz2 < 11){
			LCD_Set_Cursor(5,2);
			LCD_WriteCaracter('!');
		}else{
			LCD_Set_Cursor(5,2);
			LCD_WriteCaracter(' ');
		}
		LCD_Set_Cursor(1,2);
		sprintf(luzca, "%u", buffer_luz2);
		LCD_Write_String(luzca);
		
		delayy();
		
		ver_temp();
		LCD_Set_Cursor(11,2);
		sprintf(tempca, "%u", tura);
		LCD_Write_String(tempca);
		
		
		delayy();
		
		ver_sus();
		
		
		
		delayy();
		
		esp32();
		writeUART('e');
		sprintf(luzca, "%u", buffer_luz2);
		writetextUART(luzca);
		//mandar dato de sensor de temperatura
		writeUART('f');
		sprintf(tempca, "%u", tura);
		writetextUART(tempca);
		//mandar dato de sensor de ultrasónico
		writeUART('g');
		if(distancia >= 100){
			writetextUART("---");
			}else{
			sprintf(susca, "%u", distancia);
			writetextUART(susca);
		}
		timer++;
		
    }
}


//******************************************************************************
//								--TEMPERATURA--
//******************************************************************************
void ver_temp(void){
	temp = I2C_master_read(SlaveTEM, buffer_tem);
	if (temp != 1){//error
		LCD_Set_Cursor(15,2);
		LCD_WriteCaracter(temp);
		/*writetextUART("error con esclavo temperatura: ");
		writeUART(temp);
		writeUART(10);*/
		}else{
		LCD_Set_Cursor(15,2);
		LCD_WriteCaracter(' ');
	}
	tura = (*buffer_tem<<8);
	tura = tura/256;
	
}
//******************************************************************************
//							--SENSOR ULTRASONICO--
//******************************************************************************
void ver_sus(void){
	temp = I2C_master_read(SlaveSUS, buffer_sus);
	if (temp != 1){
		LCD_Set_Cursor(10,2);
		LCD_WriteCaracter(temp);
		/*writetextUART("error al leer esclavo ultrasonico: ");
		writeUART(temp);
		writeUART(10);*/
	}else{
		LCD_Set_Cursor(10,2);
		LCD_WriteCaracter(' ');
	}
	distancia = *buffer_sus;
	if (distancia < 30){
		LCD_Set_Cursor(10,1);
		LCD_WriteCaracter('R');
		temp = I2C_master_write(3, SlaveSUS);//mover hacia atras
		
		if (temp != 1){
			LCD_Set_Cursor(10, 1);
			LCD_WriteCaracter('e');
			LCD_Set_Cursor(10, 2);
			LCD_WriteCaracter(temp);
			/*writetextUART("error al escribir 3 esclavo ultrasonico: ");
			writeUART(temp);
			writeUART(10);*/
		}else{
			LCD_Set_Cursor(10, 1);
			LCD_WriteCaracter(' ');
			LCD_Set_Cursor(10, 2);
			LCD_WriteCaracter(' ');
		}
		LCD_Set_Cursor(6,2);
		sprintf(susca, "%u", distancia);
		LCD_Write_String(susca);
	}else if ((distancia >= 30) && (distancia < 40)){
		LCD_Set_Cursor(10,1);
		LCD_WriteCaracter('P');
		temp = I2C_master_write(4, SlaveSUS);//no moverse
		if (temp != 1){
			LCD_Set_Cursor(10, 1);
			LCD_WriteCaracter('e');
			LCD_Set_Cursor(10, 2);
			LCD_WriteCaracter(temp);
			}else{
			LCD_Set_Cursor(10, 1);
			LCD_WriteCaracter(' ');
			LCD_Set_Cursor(10, 2);
			LCD_WriteCaracter(' ');
		}
		LCD_Set_Cursor(6,2);
		sprintf(susca, "%u", distancia);
		LCD_Write_String(susca);
	}
	
}
//******************************************************************************
//								--COMUNICACIÓN CON ESP32--
//******************************************************************************
void esp32(void){
	//Comunicacion con motores
	if (uartt == 2){//mover adelante
		LCD_Set_Cursor(10,1);
		LCD_WriteCaracter('D');
		temp = I2C_master_write(2, SlaveSUS);
		if (temp != 1){
			LCD_Set_Cursor(10,1);
			LCD_WriteCaracter('e');
			LCD_Set_Cursor(10,2);
			LCD_WriteCaracter(temp);
		}else{
			LCD_Set_Cursor(10,1);
			LCD_WriteCaracter(' ');
			LCD_Set_Cursor(10,2);
			LCD_WriteCaracter(' ');
		}
	}else if(uartt == 4){//no moverse
		LCD_Set_Cursor(10,1);
		LCD_WriteCaracter('P');
		temp = I2C_master_write(4, SlaveSUS);
		if (temp != 1){
			LCD_Set_Cursor(10,1);
			LCD_WriteCaracter('e');
			LCD_Set_Cursor(10,2);
			LCD_WriteCaracter(temp);
			}else{
			LCD_Set_Cursor(10,1);
			LCD_WriteCaracter(' ');
			LCD_Set_Cursor(10,2);
			LCD_WriteCaracter(' ');
		}
	}else if(uartt == 3){//mover hacia atras
		LCD_Set_Cursor(10,1);
		LCD_WriteCaracter('R');
		temp = I2C_master_write(3, SlaveSUS);
		if (temp != 1){
			LCD_Set_Cursor(10,1);
			LCD_WriteCaracter('e');
			LCD_Set_Cursor(10,2);
			LCD_WriteCaracter(temp);
			}else{
			LCD_Set_Cursor(10,1);
			LCD_WriteCaracter(' ');
			LCD_Set_Cursor(10,2);
			LCD_WriteCaracter(' ');
		}
	}
	
	
}
void delayy(void){
	tura = tura;
	//distancia = distancia;
	buffer_luz2 = buffer_luz2;
	do{
		LCD_Set_Cursor(11,2);
		sprintf(tempca, "%u", tura);
		LCD_Write_String(tempca);
		LCD_Set_Cursor(6,2);
		sprintf(susca, "%u", distancia);
		LCD_Write_String(susca);
		LCD_Set_Cursor(1,2);
		sprintf(luzca, "%u", buffer_luz2);
		LCD_Write_String(luzca);
		
	}while(delay == 0);
	delay = 0;
}
//******************************************************************************
//								--INTERRUPCIONES--
//******************************************************************************
ISR(TIMER1_OVF_vect){
	TIFR1 |= (1<<TOV1); //apagar bandera
	TCNT1 = 57724;//0.5 s
	delay = 1;
}
ISR(USART_RX_vect){
	if (UDR0 == 'a'){//mover servo
		uartt2 = 1;
	}else if (UDR0 == 'b'){//mover adelante
		uartt = 2;
	}else if (UDR0 == 'c'){//mover hacia atras
		uartt = 3;
	}else if (UDR0 == 'd'){//no moverse
		uartt = 4;
	}
}
ISR(USART_TX_vect){
	buffer_luz2 = buffer_luz2;
	//mandar dato de sensor de luz
	
}