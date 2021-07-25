#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>

Adafruit_PCD8544 display = Adafruit_PCD8544(3, 4, 5, 7, 6);

void setup()
{
  display.begin();
  display.setContrast(55);
  display.clearDisplay();
  Serial.begin(115200);
  Serial1.begin(115200);
}

void loop()
{
  if (Serial1.available())
  {
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(BLACK);
    display.setCursor(0,0);
    delay(100);
    while(Serial1.available() > 0)
    {
      display.print(Serial1.read());
    }
    display.display();
  }
}
