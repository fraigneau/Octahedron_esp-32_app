#pragma once
#include "../packet_base.h"
#include "../../file_controller.h"
#pragma pack(push, 1)
#include <vector>
#include <CRC32.h>
#include <unordered_map>
#pragma pack(push, 1)
struct FileWritePacket : PacketBase {
	uint32_t file_hash;
	uint16_t line_data[240]; // 240 pixels
};
#pragma pack(pop)

static_assert(sizeof(FileWritePacket) == sizeof(PacketBase) + sizeof(unsigned int) + sizeof(uint16_t) * 240, "FileWritePacket size is not 490 bytes");
static constexpr size_t kLineBytes = sizeof(uint16_t) * 240;           // 480
static constexpr size_t kImageBytes = kLineBytes * 240;                  // 115200

inline std::unordered_map<uint32_t, std::vector<uint8_t>> file_write_buffers;
int last_image_id = 0;

class FileStartWritePacketHandler {
private:
	FileWritePacket* packet;
	bool is_success = false;
public:
	FileStartWritePacketHandler(FileWritePacket* pkt) : packet(pkt) {};
	FileStartWritePacketHandler* handle()
	{
		if (last_image_id != packet->file_hash)
		{
			file_write_buffers.clear();
		}

		auto& buf = file_write_buffers[packet->file_hash];
		if (buf.empty()) {
			buf.reserve(kImageBytes);
			last_image_id = packet->file_hash;
		}

		const uint8_t* lineBytes = reinterpret_cast<const uint8_t*>(packet->line_data);
		buf.insert(buf.end(), lineBytes, lineBytes + kLineBytes);

		if (buf.size() >= kImageBytes) {
			CRC32 crc;
			crc.update<uint8_t>(buf.data(), buf.size());
			auto crc_value = crc.finalize();

			if (crc_value == this->packet->file_hash)
				is_success = FileController::Singleton()->overwriteToFile(packet->file_hash,
					buf.data(),
					buf.size());

			file_write_buffers.erase(packet->file_hash);
		}

		return this;
	}

	std::vector<uint8_t> serialize()
	{
		
		return {};
	}
};
