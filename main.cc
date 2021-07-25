#include <avr/io.h>
#include "Display.h"
#include "SD.h"

char buffer[] = "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Maecenas fermentum sem quis est ultricies ultrices. Integer in cursus quam. Aliquam erat volutpat. Morbi sollicitudin varius ornare. Suspendisse ante nisi, placerat eget malesuada at, eleifend auctor lectus. Maecenas rutrum tortor id consectetur faucibus. Duis aliquam ultricies condimentum. Vestibulum cursus augue et arcu finibus hendrerit. Nam sit amet varius libero. Vivamus lorem purus, semper at nunc eu, porttitor ultricies nunc. Mauris eget nulla.";
uint8_t buffer2[513];

int main()
{
	SD sd_card;
	Display display;
	display.init();
	display.clear();
	uint8_t temp = sd_card.init();
	display.printCharacter(0x30+temp);
	display.print(" ");
	temp = sd_card.write_block((uint8_t*)buffer,0x00,0x00,0x04,0x00);
	display.printCharacter(0x30+temp);
	return 0;
}