#ifndef I2C_H
#define I2C_H
void (*TWIInterruptFuntction)(void);
class I2CSlave{
	public:
		I2CSlave(unsigned char ADDRESS);
		void check(void);
		void setTransferData(unsigned char *buf);
		void setSendData(unsigned char *buf);
		void enableInterrupt(void (*function)(void));
		void disableInterrupt();
	private:
		unsigned char cr_bit;
		unsigned char *s_buf;
		unsigned char *r_buf;
		void twi_send(int n);
		void twi_recieve(int n);
		
};
I2CSlave::I2CSlave(unsigned char ADDRESS){
	TWBR = 2;
	TWSR = 0x00;
	cr_bit = 1<<TWEN;
	TWCR = cr_bit;
	TWAR = ADDRESS;
}
void I2CSlave::twi_send(int n){
	TWDR = s_buf[n];
	TWCR = (1<<TWINT) | (1<<TWEA) | cr_bit;

	while(  ! ( TWCR & ( 1<<TWINT ) ) );
	if((TWSR & 0xF8 ) != 0xC0){
		twi_send(n+1);
	}
}
void I2CSlave::twi_recieve(int n){
	TWCR = ( 1<<TWINT ) | ( 1<<TWEA) | cr_bit;

	while(  ! ( TWCR & ( 1<<TWINT ) ) );
	if((TWSR & 0xF8 ) != 0x80)
	return;

	r_buf[n] = TWDR;
	TWCR = ( 1<<TWINT ) | ( 1<<TWEA) | cr_bit;
	while( !( TWCR & (1<<TWINT) ) );
	if( (TWSR & 0xF8 ) != 0xA0){
		twi_recieve(n+1);
	}
}
void I2CSlave::enableInterrupt(void (*function)(void)){
	cr_bit = (1<<TWEN)|(1<<TWIE);
	TWCR = cr_bit;
	TWIInterruptFuntction = function;
}
void I2CSlave::disableInterrupt(){
	cr_bit = 1<<TWEN;
	TWCR = cr_bit;
}

void I2CSlave::setTransferData(unsigned char *buf){
	s_buf = buf;
}
void I2CSlave::setSendData(unsigned char *buf){
	r_buf = buf;	
}
void I2CSlave::check(void){
	TWCR = (1<<TWINT) | (1<<TWEA) | (1<<TWEN);
	while( !( TWCR & (1<<TWINT)));
	switch((TWSR & 0xF8)){
		case 0xA8:
		twi_send(0);
		break;
		case 0x60:
		twi_recieve(0);
		break;
		default:
		break;
	}
}
ISR(TWI_vect){
	(*TWIInterruptFuntction)();
}
#endif