#include "../config.h"
#include <Arduino.h>
#include <SPI.h>
#include <SD.h>
#include "file_controller.h"
SPIClass spiSD(FSPI);
bool fatalError = false;
FileController* singleton = nullptr;
#define FILEHASH_TO_NAME(x) ("/imgs/" + std::to_string(x) + ".bin").c_str()
FileController* FileController::Singleton()
{
	return singleton;
}
bool FileController::Initialize() {
	assert(singleton == nullptr); // should only be called once
	singleton = this;
	this->setActive();
	spiSD.begin(SD_CLK, SD_MISO, SD_MOSI, SD_CS);
	if (!SD.begin(SD_CS, spiSD)) {
		Serial.println("[!] FATAL [!] SD initialization failed!");
		fatalError = true;
		return false;
	}

	Serial.println("[*] SD initialized.");
	if (SD.exists("/imgs") == false) {
		Serial.println("[*] /imgs folder doesn't exist, creating it...");
		if (!SD.mkdir("/imgs")) {
			Serial.println("[!] FATAL [!] Failed to create /imgs folder!");
			fatalError = true;
			return false;
		}
		Serial.println("[*] /imgs folder created.");
	}
	return true;
}

bool FileController::IsFatalErrored()
{
	return fatalError;
}

void FileController::setActive()
{
	digitalWrite(SD_CS, true);
	digitalWrite(TFT_CS, false);
}

bool FileController::exists(uint32_t filename)
{
	this->setActive();

	return SD.exists(FILEHASH_TO_NAME(filename));
}

bool FileController::createFile(uint32_t filename)
{
	this->setActive();

	auto file = SD.open(FILEHASH_TO_NAME(filename), FILE_WRITE);
	if (!file)
		return false;
	file.close();
	return true;
}

bool FileController::appendToFile(uint32_t filename, const uint8_t* data, size_t length)
{
	this->setActive();
	if (!this->exists(filename))
	{
		if (!this->createFile(filename))
		{
			Serial.printf("Failed to create file %s\n", FILEHASH_TO_NAME(filename));
			return false;
		}
	}

	auto file = SD.open(FILEHASH_TO_NAME(filename), FILE_APPEND);
	if (!file)
	{
		Serial.printf("Failed to open file %s for appending\n", FILEHASH_TO_NAME(filename));
		return false;
	}

	auto size_wrote = file.write((uint8_t*)data, length);
	file.close();
	Serial.printf("Wrote %u/%u bytes to file %s\n", size_wrote, length, FILEHASH_TO_NAME(filename));
	return size_wrote == length;

}

bool FileController::removeFile(uint32_t filename)
{
	this->setActive();

	return SD.remove(FILEHASH_TO_NAME(filename));
}

std::vector<uint16_t> FileController::readLines(uint32_t filename, uint16_t line_number, size_t line_count)
{
	this->setActive();

	auto file = SD.open(FILEHASH_TO_NAME(filename), FILE_READ);
	if (!file)
		return {};
	std::vector<uint16_t> line_data;
	line_data.resize(line_count * 240);
	file.seek(line_number * 240 * sizeof(uint16_t));
	auto size_read = file.read((uint8_t*)line_data.data(), line_count * 240 * sizeof(uint16_t));
	file.close();
	if (size_read != line_count * 240 * sizeof(uint16_t))
		return {};
	return line_data;
}