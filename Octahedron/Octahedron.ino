#include <CRC32.h>
#include <FS.h>
#include <FSImpl.h>
#include <vfs_api.h>
#include "config.h"
#include "src/bluetooth_controller.h"
#include "src/file_controller.h"
#include "src/tft_controller.h"


BluetoothController ble_ctrl = BluetoothController();
FileController file_ctrl = FileController();
TFTController tft_ctrl = TFTController();

void error()
{
	Serial.println("womp womp; restarting");
	delay(1000);
	ESP.restart();
}

void printLogo(const uint16_t name) {
	auto numbers_of_lines = 240;
	for (int i = 0; i < 240; i)
	{
		auto drawData = FileController::Singleton()->readLines(name, i, numbers_of_lines);

		for (int y = 0; y < numbers_of_lines; y++)
			TFTController::Singleton()->drawLine(i + y, (uint16_t*)drawData.data() + (y * 240));

		i += numbers_of_lines;
	}
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

	printLogo(0x1);

}

void loop() {

}