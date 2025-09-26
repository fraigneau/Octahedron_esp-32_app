#pragma once
#include "../packet_base.h"
#include "../../file_controller.h"
#pragma pack(push, 1)
struct FileExistsPacket : PacketBase {
	uint32_t file_hash;
	bool exists; // 0 = no, 1 = yes
};
#pragma pack(pop)

static_assert(sizeof(FileExistsPacket) == sizeof(PacketBase) + sizeof(unsigned int) + sizeof(unsigned char), "FileExistsPacket size is not 6+4+1 bytes");

class FileExistsPacketHandler {
private:
	FileExistsPacket* packet;
public:
	FileExistsPacketHandler(FileExistsPacket* pkt) : packet(pkt) {};
	FileExistsPacketHandler* handle()
	{
		this->packet->direction = DIRECTION_ESP_TO_DEVICE;
		this->packet->exists = FileController::Singleton()->exists(this->packet->file_hash);

		return this;
	}

	std::vector<uint8_t> serialize()
	{
		std::vector<uint8_t> vector;
		vector.resize(sizeof(FileExistsPacket));
		memcpy(vector.data(), this->packet, sizeof(FileExistsPacket));
		return vector;
	}
};
