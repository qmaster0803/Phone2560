#include <avr/io.h>
#include "PFFS/pff.h"

#define SD_PORT  PORTK
#define SD_PIN   PINK
#define SD_DDR   DDRK
#define CS_PIN   0
#define MOSI_PIN 1
#define MISO_PIN 7
#define SCK_PIN  6

class SD
{
public:
	inline SD()
	{
		SD_PORT |= (1<<CS_PIN)|(1<<MOSI_PIN)|(1<<MISO_PIN); //CS, MOSI, MISO set high level
		SD_DDR |= (1<<CS_PIN)|(1<<SCK_PIN)|(1<<MOSI_PIN); //CS, SCK, MOSI as output
	}

	uint8_t init();
	uint8_t write_block(uint8_t* buf, uint8_t addr0, uint8_t addr1, uint8_t addr2, uint8_t addr3);
	uint8_t read_block(uint8_t* buf, uint8_t addr0, uint8_t addr1, uint8_t addr2, uint8_t addr3);

	FATFS fs;

private:
	uint8_t _run_SD_cmd(uint8_t dt0, uint8_t dt1, uint8_t dt2, uint8_t dt3, uint8_t dt4, uint8_t dt5);
	void _SPI_sendByte(uint8_t data);
	uint8_t _SPI_recvByte();
};