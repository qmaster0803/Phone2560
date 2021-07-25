#include <avr/io.h>
#include <avr/interrupt.h>

class SIM_A6
{
public:
	SIM_A6();
	~SIM_A6();

	void init();
	void newDataGot(uint8_t data);

private:
	uint8_t _rxBuffer[256] = {0};
	uint8_t _rxBufferLength = 0;
	void _writeUART(uint8_t data);
};