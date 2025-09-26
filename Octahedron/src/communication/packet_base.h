#pragma once
#include <cstdint>
#include "packet_defs.h"
// forcing 1-byte alignment to save space in the packet structures
#pragma pack(push, 1)
struct PacketBase {
	int magic; // 'LFR '
	PacketDirection direction;
	PacketType type;
};
#pragma pack(pop)