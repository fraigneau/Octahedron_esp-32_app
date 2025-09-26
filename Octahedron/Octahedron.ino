#include <NimBLEDevice.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include <SPI.h>
#include "src/bluetooth_controller.h"
#include "src/file_controller.h"
#include "src/tft_controller.h"
#include "config.h"

BluetoothController ble_ctrl = BluetoothController();
FileController file_ctrl = FileController();
TFTController tft_ctrl = TFTController();

void error()
{
	Serial.println("womp womp; restarting");
	delay(1000);
	ESP.restart();
}

void setup() {
	Serial.begin(115200);
	sleep(2);

	pinMode(TFT_BL, OUTPUT);
	digitalWrite(TFT_BL, HIGH);
	pinMode(TFT_CS, OUTPUT);
	pinMode(SD_CS, OUTPUT);

	if (!file_ctrl.Initialize())
		return error();

	if (!ble_ctrl.Initialize())
		return error();

	if (!tft_ctrl.Initialize())
		return error();
}

void loop() {
	Serial.println("Hello World");
}
