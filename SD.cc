#include "SD.h"


//0 - Init OK
//1 - Unable to reset SD card
//2 - Unable to initialize SD card
uint8_t SD::init()
{
	uint8_t temp;
	for(uint8_t i = 0; i < 10; i++) {_SPI_sendByte(0xFF);} //wait 80 SCK pulses for SD init (do NOT low CS)
	SD_PORT &= ~(1<<CS_PIN);
	temp = _run_SD_cmd(0x40, 0x00, 0x00, 0x00, 0x00, 0x95); //reset the SD card (CMD0)
	if(temp != 0x01) {return 3;}
	_SPI_sendByte(0xFF);

	uint16_t cnt = 0;
	do
	{
		temp = _run_SD_cmd(0x41, 0x00, 0x00, 0x00, 0x00, 0x95); //send CMD1
		_SPI_sendByte(0xFF);
    	cnt++;
	} while ((temp != 0) && cnt < 0xFFFF); //wait for R1 answer
	if(cnt >= 0xFFFF) {return 2;}
	return temp;
}

uint8_t SD::write_block(uint8_t* buf, uint8_t addr0, uint8_t addr1, uint8_t addr2, uint8_t addr3)
{
	uint8_t result;
	uint16_t cnt = 0;

	result = _run_SD_cmd(0x58, addr0, addr1, addr2, addr3, 0x95); //CMD24 write block command
	if(result != 0x00) {return 6;}

	_SPI_sendByte(0xFF);
	_SPI_sendByte(0xFE); //data begin
	for(cnt = 0; cnt < 512; cnt++) {_SPI_sendByte(buf[cnt]);}
	_SPI_sendByte(0xFF); //checksum, doesn't matter
	_SPI_sendByte(0xFF);

	result = _SPI_recvByte();
	if((result&0x05) != 0x05) {return 6;}

	cnt = 0;
	do
	{
		result = _SPI_recvByte();
		cnt++;
	} while(result != 0xFF && cnt < 0xFFFF);
	if(cnt >= 0xFFFF) {return 6;}
	return 0;
}

uint8_t SD::read_block(uint8_t* buf, uint8_t addr0, uint8_t addr1, uint8_t addr2, uint8_t addr3)
{
	uint8_t result;
	uint16_t cnt = 0;

	result = _run_SD_cmd(0x51, addr0, addr1, addr2, addr3, 0x95); //CMD17 read block command
	if(result != 0x00) {return 5;}
	_SPI_sendByte(0xFF);

	do
	{
		result = _SPI_recvByte();
		cnt++;
	} while ((result!=0xFE) && (cnt<0xFFFF)); //wait for block to start
	if(cnt >= 0xFFFF) {return 5;}

	for(cnt = 0; cnt < 512; cnt++) {buf[cnt] = _SPI_recvByte();}
	_SPI_recvByte(); //checksum skip
	_SPI_recvByte();
	return 0;
}

void SD::_SPI_sendByte(uint8_t data)
{
	for(uint8_t i = 0; i < 8; i++) //walk through bits
	{
		if((data&0x80) == 0) {SD_PORT &= ~(1<<MOSI_PIN);} //if MSB is zero set MOSI to zero
		else {SD_PORT |= (1<<MOSI_PIN);}
		data <<= 1;                                       //left shift byte to check next bit
		SD_PORT |= (1<<SCK_PIN);                          //generate signal on SCK
		asm("nop");
		SD_PORT &= ~(1<<SCK_PIN);
	}
}

uint8_t SD::_SPI_recvByte()
{
	uint8_t result = 0;
	for(uint8_t i = 0; i < 8; i++)
	{
		SD_PORT |= (1<<SCK_PIN); //generate sync front
		result <<= 1;            //left shift to record next bit
		if((SD_PIN&(1<<MISO_PIN)) != 0) {result |= 0x01;} //record 1, else zero
		SD_PORT &= ~(1<<SCK_PIN);
		asm("nop");
	}
	return result;
}

uint8_t SD::_run_SD_cmd(uint8_t dt0, uint8_t dt1, uint8_t dt2, uint8_t dt3, uint8_t dt4, uint8_t dt5)
{
	uint8_t result = 0xFF;
	uint16_t cnt = 0;

	_SPI_sendByte(dt0);
	_SPI_sendByte(dt1);
	_SPI_sendByte(dt2);
	_SPI_sendByte(dt3);
	_SPI_sendByte(dt4);
	_SPI_sendByte(dt5);

	do
	{
		result = _SPI_recvByte();
		cnt++;
	} while (((result&0x80) != 0) && cnt < 0xFFFF);
	return result;
}