#pragma once
#include <vector>
#include <cstdint>

class PacketHandler {
public:
	std::vector<uint8_t> onPacketReceived(const uint8_t* data, uint16_t length);
};