#include "Display.h"

#define LCD_C	 0
#define LCD_D	 1

#define LCD_X	 84
#define LCD_Y	 48


void Display::setPin(volatile uint8_t *port, uint8_t pin, bool value)
{
	if(value == 1) *port |= (1<<pin);
	else *port &= ~(1<<pin);
}

void Display::shiftOut(volatile uint8_t *dataPort, uint8_t dataPin, volatile uint8_t *clockPort, uint8_t clockPin, bitOrder bit_ord, uint8_t val)
{
	uint8_t i;

	for(i = 0; i < 8; i++)
	{
		if(bit_ord == LSBFIRST)
		{
			setPin(dataPort, dataPin, !!(val & (1 << i)));
		}
		else
		{
			setPin(dataPort, dataPin, !!(val & (1 << (7 - i))));
		}
		
		setPin(clockPort, clockPin, 1);
		setPin(clockPort, clockPin, 0);
	}
}

void Display::write(bool dc, uint8_t data)
{
	//set D/C pin
	if(dc == 0) {PORTE &= ~(1<<3);}
	else {PORTE |= (1<<3);}

	PORTH &= ~(1<<4); //PIN_SCE to low
	shiftOut(&PORTG, 5, &PORTE, 5, MSBFIRST, data);
	PORTH |= (1<<4); //PIN_SCE to high
}

void Display::clear()
{
	write(LCD_C, 0x40); //set cursor to Y0
	write(LCD_C, 0x80); //set cursor to X0
	for (int index = 0; index < LCD_X * LCD_Y / 8; index++)
	{
		write(LCD_D, 0x00);
	}
}

void Display::printCharacter(char character)
{
	write(LCD_D, 0x00);
	for (int index = 0; index < 5; index++)
	{
		write(LCD_D, ASCII[character - 0x20][index]);
	}
	write(LCD_D, 0x00);
}

void Display::init()
{
	/*
	PIN_SCE   7 PH4
	PIN_RESET 6 PH3
	PIN_DC	5 PE3
	PIN_DIN   4 PG5
	PIN_CLK   3 PE5
	*/

	DDRH |= (1<<4); //PIN_SCE
	DDRH |= (1<<3); //PIN_RESET
	DDRE |= (1<<3); //PIN_DC
	DDRG |= (1<<5); //PIN_DIN
	DDRE |= (1<<5); //PIN_CLK

	//set display pins to zero
	setPin(&PORTH, 4, 0);
	setPin(&PORTH, 3, 0);
	setPin(&PORTE, 3, 0);
	setPin(&PORTG, 5, 0);
	setPin(&PORTE, 5, 0);

	PORTH &= ~(1<<3); //PIN_RESET LOW
	PORTH |= (1<<3);  //PIN_RESET HIGH

	write(LCD_C, 0x21);  // LCD Extended Commands
	write(LCD_C, 0xBA);  // Set LCD Vop (Contrast) 
	write(LCD_C, 0x04);  // Set Temp coefficent.
	write(LCD_C, 0x14);  // LCD bias mode 1:48.
	write(LCD_C, 0x20);  // LCD Basic Commands
	write(LCD_C, 0x0C);  // LCD in normal mode.
}

void Display::print(char *characters)
{
  while (*characters)
  {
	printCharacter(*characters++);
  }
}