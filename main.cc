#define F_CPU 16000000

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>

#include "Display.h"
#include "SIM_A6.h"
#include "TTP229.h"

#include "PFFS/pff.h"
#include "PFFS/diskio.h"
#include "PFFS/integer.h"

char buffer[] = "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Maecenas fermentum sem quis est ultricies ultrices. Integer in cursus quam. Aliquam erat volutpat. Morbi sollicitudin varius ornare. Suspendisse ante nisi, placerat eget malesuada at, eleifend auctor lectus. Maecenas rutrum tortor id consectetur faucibus. Duis aliquam ultricies condimentum. Vestibulum cursus augue et arcu finibus hendrerit. Nam sit amet varius libero. Vivamus lorem purus, semper at nunc eu, porttitor ultricies nunc. Mauris eget nulla.";
uint8_t buffer2[513] = {0};

FATFS fs;
Display display;
SIM_A6 sim;
TTP229 keyboard;

int main()
{
	display.init();
	sim.init();
	display.clear();
	//display.print("Wait SIM");
	//_delay_ms(15000);
	sei();
	sim.send_AT();

	//uint8_t temp = sd_card.init();
	//display.printCharacter(0x30+temp);
	//display.print(" ");
	//temp = sd_card.write_block((uint8_t*)buffer,0x00,0x00,0x04,0x00);
	//display.printCharacter(0x30+temp);

	/*char str[15] = {0};
	uint8_t temp = pf_mount(&fs);
	if(temp != 0) {sprintf(str, "SD init error: %d", temp); display.print(str); return 1;}
	uint8_t temp2 = pf_open("/sd_test.txt");
	uint8_t temp3 = pf_lseek(0);
	WORD bytes_read = 0;
	uint8_t temp4 = pf_read(buffer2, 128, &bytes_read);
	sprintf(str, "%d-%d-%d-%d-%d", temp, temp2, temp3, temp4, &bytes_read);
	display.print(str);
	display.print((char*)buffer2);
	return 0;*/

	uint16_t temp;
	while(true)
	{
		temp = keyboard.get();
		display.clear();
		for(uint8_t i = 0; i < 16; i++)
		{
			if(temp & 0x01) {display.print("1");}
			else {display.print("0");}
			temp >>= 1;
		}
		_delay_ms(100);
	}

	while(true)
	{
		display.clear();
		display.print("AT:");
		uint8_t i = 0;
		char str[50] = {0};
		char buf = 0;
		sim.getBufStr(&buf, &i);
		sprintf(str, "%d text: ", i);
		display.print(str);
		display.printCharacter(buf);
		_delay_ms(500);
	}
}

ISR(USART3_RX_vect)
{
	sim.newDataGot();
}