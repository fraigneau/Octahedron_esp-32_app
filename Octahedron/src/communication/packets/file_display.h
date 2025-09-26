#pragma once
#include "../packet_base.h"
#include "../../file_controller.h"
#include "../../tft_controller.h"
#pragma pack(push, 1)
struct FileDisplayPacket : PacketBase {
	uint32_t file_hash;
	bool success; 
};
#pragma pack(pop)

static_assert(sizeof(FileDisplayPacket) == sizeof(PacketBase) + sizeof(unsigned int) + sizeof(unsigned char), "FileDisplayPacket size is not 6+4+1 bytes");

class FileDisplayPacketHandler {
private:
	FileDisplayPacket* packet;
public:
	FileDisplayPacketHandler(FileDisplayPacket* pkt) : packet(pkt) {};
	FileDisplayPacketHandler* handle()
	{
		this->packet->direction = DIRECTION_ESP_TO_DEVICE;
		auto start = esp_timer_get_time();
		FileController::Singleton()->readLines(this->packet->file_hash, 0, 2);
		//TFTController::Singleton()->turnScreen(false);
		auto numbers_of_lines = 240;
		for (int i = 0; i < 240; i)
		{
			// first: read it
			auto drawData = FileController::Singleton()->readLines(this->packet->file_hash, i, numbers_of_lines);

			// second: draw (them?) 
			for(int y = 0; y < numbers_of_lines; y++)
				TFTController::Singleton()->drawLine(i + y, (uint16_t*)drawData.data() + (y * 240));

			i += numbers_of_lines;
		}
		//TFTController::Singleton()->turnScreen(true);

		auto end = esp_timer_get_time();
		Serial.printf("Displayed image %u in %.2f seconds\n", this->packet->file_hash, (end - start) / 1000000.0);
		this->packet->success = true; // TO:DO 
		return this;
	}

	std::vector<uint8_t> serialize()
	{
		std::vector<uint8_t> vector;
		vector.resize(sizeof(FileDisplayPacket));
		memcpy(vector.data(), this->packet, sizeof(FileDisplayPacket));
		return vector;
	}
};
