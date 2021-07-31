#include "TTP229.h"

TTP229::TTP229()
{
	KEYBOARD_DDR |= (1<<SCL_PIN);  //SCL as OUTPUT
	KEYBOARD_DDR &= ~(1<<SDO_PIN); //SDO as INPUT
	DDRB |= (1<<7);
}

uint16_t TTP229::get()
{
	uint16_t state = 0;
	for(uint8_t i = 0; i < 16; i++)
	{
		KEYBOARD_PORT |= (1<<SCL_PIN); //SCL to HIGH
		_delay_us(10);
		state <<= 1;
		if(KEYBOARD_PIN & (1<<SDO_PIN)) {state |= 1;}
		KEYBOARD_PORT &= ~(1<<SCL_PIN); //SCL back to LOW
		_delay_us(10);
	}
	return state;
}