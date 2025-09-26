#include <NimBLEDevice.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include <SPI.h>


#define TFT_CS   10
#define TFT_DC   8
#define TFT_RST  9
#define TFT_BL   1

#define TFT_SCLK 14
#define TFT_MOSI 13

SPIClass spiTFT = SPIClass(HSPI);

Adafruit_ST7789 tft = Adafruit_ST7789(&spiTFT, TFT_CS, TFT_DC, TFT_RST);


void setup() {
	Serial.begin(115200);
	sleep(2);
}

void loop() {
	Serial.println("Hello World");
}
