#define F_CPU 1000000

#include "avr/io.h"
#include "avr/interrupt.h"
#include "I2CSlave.h"

#define ADDR 0x4E
#define USE_INTERRUPT 1

I2CSlave i2c(ADDR);

#if (USE_INTERRUPT==1)

void interruptFunc(void){
	unsigned char *data;
	
	data = i2c.getTransferAddress();
	data[0] = PINB;
	data[1] = PIND;
	
	i2c.communicate();

	data = i2c.getRecieveAddress();
	PORTC = *data;
}

int main(){
	unsigned char transData[3];
	unsigned char recieveData;
	DDRB  = 0x00;
	DDRC  = 0xFF;
	DDRD  = 0x00;
	i2c.setRecieveData (&recieveData);
	i2c.setTransferData(transData);
	while(1);
}

#else

int main(){
	unsigned char transData[3];
	unsigned char recieveData;
	DDRB  = 0x00;
	DDRC  = 0xFF;
	DDRD  = 0x00;
	i2c.setRecieveData (&recieveData);
	i2c.setTransferData(transData);
	while(1) {
		i2c.check();
		//check メソッドはやり取りしたバイト数を返す
		//  n -> n byte受信
		// -n -> n byte送信
		//  0 -> 指令なし
		transData[0] = PINB;
		transData[1] = PIND;
		PORTC = recieveData;
	}
}

#endif
