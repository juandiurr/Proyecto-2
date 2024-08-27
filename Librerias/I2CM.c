/*
 * I2C.c
 *
 * Created: 1/08/2024 20:52:50
 *  Author: diego
 */ 
#include "I2CM.h"

void I2C_master_init(unsigned long scl_clk, uint8_t prescaler){
	DDRC |= (1<<DDC4)|(1<<DDC5);//pines I2C como salidas
	switch(prescaler){
		case 1:
			TWSR &= ~((1<<TWPS1)|(1<<TWPS0));
		break;
		case 4:
			TWSR &= ~(1<<TWPS1);
			TWSR |= (1<<TWPS0);
		break;
		case 16:
			TWSR &= ~(1<<TWPS0);
			TWSR |= (1<<TWPS1);
		break;
		case 64:
			TWSR |= (1<<TWPS1)|(1<<TWPS0);
		break;
		default:
			prescaler = 1;
			TWSR &= ~((1<<TWPS1)|(1<<TWPS0));
		break;
	}	
	TWBR = ((F_CPU/scl_clk)-16)/(2*prescaler);
	TWCR |= (1<<TWEN);//habilitamos interfaz
}

uint8_t I2C_master_start(void){
	uint8_t estado;
	TWCR |= (1<<TWINT)|(1<<TWSTA)|(1<<TWEN);//Iniciar condici�n de start
	while(!(TWCR&(1<<TWINT)));//ESPERA A QUE TERMINE FLAG TWINT
	estado = TWSR & 0xF8;//VERIFICAR ESTADO
	if ((estado == 0x08) || (estado == 0x10)){
		return 1;
	}else{
		return estado; //si devuelve 0 no se envi� el start
	}
}
void I2C_master_stop(void){
	TWCR = (1<<TWEN)|(1<<TWINT)|(1<<TWSTO);//inicia envio de secuencia de parada
	while(TWCR & (1<<TWSTO));//esperamos que el bit se limpie
}

//Funci�n de transimici�n de datos del maestro al esclavo 
//Devolvera 1 si el esclavo a recibido el dato
uint8_t I2C_master_write(uint8_t dato, uint8_t direccion_esclavo){
	uint8_t estado;
	uint8_t buff;
	// Iniciar condici�n de START
	TWCR = (1 << TWSTA) | (1 << TWEN) | (1 << TWINT);
	while (!(TWCR & (1 << TWINT))); // Esperar a que se complete

	// Verificar estado de START
	estado = TWSR & 0xF8;
	if (estado != 0x08 && estado != 0x10){
		return estado;// Error: No se pudo iniciar la condici�n de START
		I2C_master_stop();
	}
	// Enviar direcci�n del esclavo con bit de lectura
	buff = (direccion_esclavo << 1); // SLA + W (bit de escritura)
	buff &= ~(1<<0);
	TWDR = buff;
	TWCR = (1 << TWEN) | (1 << TWINT);
	while (!(TWCR & (1 << TWINT))); // Esperar a que se complete
	
	// Verificar estado de la direcci�n del esclavo
	estado = TWSR & 0xF8;
	if (estado != 0x18){
		I2C_master_stop();
		return estado;
	}
	
	TWDR = dato; //cargar dato
	TWCR = (1<<TWEN)|(1<<TWINT);//INICIA ENVIO
	while(!(TWCR&(1<<TWINT)));//ESPERA EL FLAG TWINT
	estado = TWSR & 0xF8;//verificar estado
	//verificar si se transmiti� una SLA + W con ACK, SLA + R con ACK, o un dato sin ACK
	//completo el envio de info con o sin acknowladge
	if (estado == 0x28){
		return 1;
		I2C_master_stop();
	}else{
		return estado;
		I2C_master_stop();
	}
}


uint8_t I2C_master_read(uint8_t direccion_esclavo, uint8_t *dato) {
	uint8_t estado;
	// Iniciar condici�n de START
	TWCR = (1 << TWSTA) | (1 << TWEN) | (1 << TWINT);
	while (!(TWCR & (1 << TWINT))); // Esperar a que se complete

	// Verificar estado de START
	estado = TWSR & 0xF8;
	if (estado != 0x08 && estado != 0x10){
		return estado;// Error: No se pudo iniciar la condici�n de START
		I2C_master_stop();
	}
	

	// Enviar direcci�n del esclavo con bit de lectura
	TWDR = (direccion_esclavo << 1) | 1; // SLA + R (bit de lectura)
	TWCR = (1 << TWEN) | (1 << TWINT);
	while (!(TWCR & (1 << TWINT))); // Esperar a que se complete

	// Verificar estado de la direcci�n del esclavo
	estado = TWSR & 0xF8;
	if (estado != 0x40){
		I2C_master_stop();
		return estado;
	}
	//TWCR |= (1<<TWEA);//LECTURA CON ACK
	// Leer el dato del esclavo
	TWCR = (1 << TWINT) | (1 << TWEN);
	while (!(TWCR & (1 << TWINT))); // Esperar a que se complete

	// Verificar estado del dato recibido
	estado = TWSR & 0xF8;
	if(estado != 0x58 && estado != 0x50){
		I2C_master_stop();
		return estado;
	}else{
		*dato = TWDR; // Leer el dato recibido
		I2C_master_stop();
		return 1; // �xito
	}
}
// Escribe un byte en el bus I2C
uint8_t I2C_Master_Write(uint8_t dato) {
	TWDR = dato; // Carga el dato en el registro de datos
	TWCR = (1 << TWINT) | (1 << TWEN); // Inicia la transmisi�n
	while (!(TWCR & (1 << TWINT))); // Espera a que la transmisi�n se complete

	uint8_t estado = TWSR & 0xF8; // Lee el estado del TWI
	if (estado != 0x18 && estado != 0x28 && estado != 0x40) {
		return estado; // Error
	}
	return 1; // �xito
}

