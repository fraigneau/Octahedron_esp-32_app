#pragma once
#include "../packet_base.h"
#pragma pack(push, 1)
struct HealthStatusPacket : PacketBase {
	HealthStatus status;
};
#pragma pack(pop)

static_assert(sizeof(HealthStatusPacket) == sizeof(PacketBase) + sizeof(unsigned char), "HealthStatusPacket size is not 7 bytes");

class HealthStatusPacketHandler {
private:
	HealthStatusPacket* packet;
public:
	HealthStatusPacketHandler(HealthStatusPacket* pkt) : packet(pkt) {};
	HealthStatusPacketHandler* handle(HealthStatus status)
	{
		if (this->packet->status != HEALTH_ASKED)
			return this;

		this->packet->direction = DIRECTION_ESP_TO_DEVICE;
		this->packet->status = status;
		return this;
	}

	std::vector<uint8_t> serialize()
	{
		std::vector<uint8_t> vector;
		vector.resize(sizeof(HealthStatusPacket));
		memcpy(vector.data(), this->packet, sizeof(HealthStatusPacket));
		return vector;
	}
};