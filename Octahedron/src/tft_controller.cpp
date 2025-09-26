#include <Arduino.h>
#include <cmath>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include "tft_controller.h"
#include "file_controller.h"
#include "../config.h"

TFTController* tft_singleton = nullptr;
SPIClass spiTFT = SPIClass(HSPI);

Adafruit_ST7789 tft = Adafruit_ST7789(&spiTFT, TFT_CS, TFT_DC, TFT_RST);

TFTController* TFTController::Singleton()
{
	return tft_singleton;
}
bool TFTController::Initialize()
{
	assert(tft_singleton == nullptr); // should only be called once
	tft_singleton = this;
	this->setActive();
	spiTFT.begin(TFT_SCLK, -1, TFT_MOSI, TFT_CS);
	tft.init(240, 240);
	tft.setRotation(0);

	return true;
}

void TFTController::setActive()
{
	digitalWrite(TFT_CS, true);
	digitalWrite(SD_CS, false);
}

void TFTController::drawLine(int line_number, uint16_t* line_data)
{
	this->setActive();
	Serial.println("Drawing line " + String(line_number));
	tft.startWrite();
	tft.setAddrWindow(0, line_number, 240, 1);
	tft.writePixels(line_data, 240);
	tft.endWrite();
}
