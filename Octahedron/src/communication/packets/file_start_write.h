#pragma once
#include "../packet_base.h"
#include "../../file_controller.h"
#pragma pack(push, 1)
struct FileWritePacket : PacketBase {
	uint32_t file_hash;
	uint16_t crc;
	uint16_t line_data[240]; // 240 pixels
};
struct FileWriteResponsePacket : PacketBase {
	bool success; // 0 = no, 1 = yes
};
#pragma pack(pop)

static_assert(sizeof(FileWritePacket) == sizeof(PacketBase) + sizeof(unsigned int) + sizeof(uint16_t) + sizeof(uint16_t) * 240, "FileWritePacket size is not 496 bytes");

class FileStartWritePacketHandler {
private:
	FileWritePacket* packet;
	bool is_success = false;
public:
	FileStartWritePacketHandler(FileWritePacket* pkt) : packet(pkt) {};
	FileStartWritePacketHandler* handle()
	{
		this->is_success = FileController::Singleton()->appendToFile(this->packet->file_hash, (uint8_t*)this->packet->line_data, sizeof(this->packet->line_data));
		return this;
	}

	std::vector<uint8_t> serialize()
	{
		FileWriteResponsePacket response;
		response.magic = 'LFR ';
		response.direction = DIRECTION_ESP_TO_DEVICE;
		response.type = PacketType::PACKET_FILE_WRITE_RESPONSE;
		response.success = this->is_success;

		std::vector<uint8_t> vector;
		vector.resize(sizeof(FileWriteResponsePacket));
		memcpy(vector.data(), &response, sizeof(FileWriteResponsePacket));
		return vector;
	}
};