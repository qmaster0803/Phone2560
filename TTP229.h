#define F_CPU 16000000

#include <avr/io.h>
#include <util/delay.h>

#define KEYBOARD_PORT PORTH
#define KEYBOARD_PIN  PINH
#define KEYBOARD_DDR  DDRH
#define SCL_PIN       6
#define SDO_PIN       5

class TTP229
{
public:
	TTP229();
	uint16_t get();
};