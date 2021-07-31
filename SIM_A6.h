#include <avr/io.h>
#include <avr/interrupt.h>

class SIM_A6
{
public:
	inline SIM_A6() {};

	void init();
	void newDataGot();
	void send_AT();
	void getBufStr(char *data, uint8_t *bufLen);

private:
	uint8_t _rxBuffer[512] = {0};
	uint8_t _rxBufferLength = 0;
	void _writeUART(uint8_t data);
};