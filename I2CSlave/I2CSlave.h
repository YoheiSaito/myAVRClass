#ifndef I2C_H
#define I2C_H
#include "avr/io.h"
#include "avr/interrupt.h"
#include "util/delay.h"

static void (*TWIInterruptFuntction)(void);
class I2CSlave{
	public:
	I2CSlave(unsigned char ADDRESS);
	void setTransferData(unsigned char *buf);
	void setRecieveData(unsigned char *buf);
	void enableInterrupt(void (*function)(void));
	void disableInterrupt();
	unsigned char* getTransferAddress(void);
	unsigned char* getRecieveAddress(void);
	int  check(void);
	int	 communicate(void);
	private:
	unsigned char *sBuf;
	unsigned char *rBuf;
	int twiSend(int n);
	int twiRecieve(int n);

};
I2CSlave::I2CSlave(unsigned char ADDRESS){
	TWBR = 2;
	TWSR = 0x00;
	TWCR = (1<<TWEN)|(1<<TWEA)|(1<<TWINT);
	TWAR = ADDRESS;
}
int I2CSlave::twiSend(int n){
	int r = -1;
	TWDR = sBuf[n];
	TWCR |= (1<<TWINT) | (1<<TWEA);
	while(  ! ( TWCR & ( 1<<TWINT ) ) );
	if((TWSR & 0xF8 ) != 0xC0){
		r = twiSend(n+1)-1;
	}
	return r;
}
int I2CSlave::twiRecieve(int n){
	int r = 1;
	rBuf[n] = TWDR;
	TWCR |= ( 1<<TWINT ) | ( 1<<TWEA);
	while( !( TWCR & (1<<TWINT) ) );
	if( (TWSR & 0xF8 ) != 0xA0){
		r  = twiRecieve(n+1)+1;
	}
	return r;
}
void I2CSlave::enableInterrupt(void (*function)(void)){
	TWCR |= (1<<TWIE);
	TWIInterruptFuntction = function;
}
void I2CSlave::disableInterrupt(){
	TWCR &= ~(1<<TWIE);
}

void I2CSlave::setTransferData(unsigned char *buf){
	sBuf = buf;
}
void I2CSlave::setRecieveData(unsigned char *buf){
	rBuf = buf;
}
unsigned char* I2CSlave::getTransferAddress(void){
	return sBuf;
}
unsigned char* I2CSlave::getRecieveAddress(void){
	return rBuf;
}
int I2CSlave::check(void){
	int i = 0;
	TWCR |= (1<<TWINT) | (1<<TWEA) | (1<<TWEN);
	while(!((TWCR & (1<<TWINT)))){
		if(i>25) return 0;
		i++;
	}
	return communicate();
}
int  I2CSlave::communicate(){
	int r = 0;
	switch((TWSR & 0xF8)){
		case 0xA8:
		r = twiSend(0);
		break;
		case 0x60:
		r = twiRecieve(0);
		break;
		default:
		break;
	}
	return r;
}
ISR(TWI_vect){
	cli();
	(*TWIInterruptFuntction)();
	sei();
}
#endif
