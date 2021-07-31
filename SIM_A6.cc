#include "SIM_A6.h"

void SIM_A6::_writeUART(uint8_t data)
{
	while(!(UCSR3A & (1<<5))); //wait until UDR3 empty
	UDR3 = data;
}

void SIM_A6::init()
{
	const uint16_t speed = 16;     //115200 baud; U2X = 1
	UBRR3H = (uint8_t)(speed>>8);
	UBRR3L = (uint8_t)(speed);

	UCSR3A = (1<<1);               //U2X double speed enable
	UCSR3B = (1<<3)|(1<<4)|(1<<7); //RX enable, TX enable, RX complete interrupt enable
	UCSR3C = (1<<1)|(1<<2);        //8-bit character size
}

void SIM_A6::newDataGot()
{
	uint8_t got_data = UDR3;
	if(got_data == 0x0D || got_data == 0x0A) {return;}
	_rxBuffer[_rxBufferLength] = got_data;
	_rxBufferLength++;
}

void SIM_A6::getBufStr(char *data, uint8_t *bufLen)
{
	*bufLen = _rxBufferLength;
	*data = (char)_rxBuffer[_rxBufferLength-1];
}

void SIM_A6::send_AT()
{
	_writeUART((uint8_t)'A');
	_writeUART((uint8_t)'T');
	_writeUART(0x0D);
	_writeUART(0x0A);
}