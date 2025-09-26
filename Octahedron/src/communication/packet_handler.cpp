#include "packet_handler.h"
#include "packet_base.h"
#include <Arduino.h>

#include "packets/health_status.h"
#include "packets/file_start_write.h"
#include "packets/file_display.h"
#include "../file_controller.h"

std::vector<uint8_t> PacketHandler::onPacketReceived(const uint8_t* data, uint16_t length)
{
	if (length < sizeof(PacketBase))
	{
		Serial.println("Packet too small");
		return {};
	}

	auto packet = reinterpret_cast<const PacketBase*>(data);
	if (packet->magic != 'LFR ')
	{
		Serial.println("Invalid packet magic");
		return {};
	}
	std::vector<uint8_t> answer = {};
	switch (packet->type)
	{
	case PacketType::PACKET_HEALTH:
	{
		if (length != sizeof(HealthStatusPacket))
		{
			Serial.println("Invalid HealthStatusPacket size");
			return {};
		}
		auto status = HealthStatus::HEALTH_OK;
		if (FileController::Singleton()->IsFatalErrored())
			status = HealthStatus::HEALTH_SD_ERROR;

		auto casted_packet = (HealthStatusPacket*)data;
		answer = (HealthStatusPacketHandler(casted_packet)).handle(HealthStatus::HEALTH_OK)->serialize();
		Serial.printf("Received HealthStatusPacket: status=%d\n", ((HealthStatusPacket*)answer.data())->status);
		break;
	}
	case PacketType::PACKET_FILE_WRITE:
	{
		if (length != sizeof(FileWritePacket))
		{
			Serial.println("Invalid FileWritePacket size");
			return {};
		}
		auto casted_packet = (FileWritePacket*)data;
		answer = (FileStartWritePacketHandler(casted_packet)).handle()->serialize();
		Serial.printf("Received FileWritePacket for file %u\n", casted_packet->file_hash);

		break;
	}
	case PacketType::PACKET_FILE_DISPLAY:
	{
		if (length != sizeof(FileDisplayPacket))
		{
			Serial.println("Invalid FileDisplayPacket size");
			return {};
		}
		auto casted_packet = (FileDisplayPacket*)data;
		answer = (FileDisplayPacketHandler(casted_packet)).handle()->serialize();
		Serial.printf("Received FileDisplayPacket for file %u\n", casted_packet->file_hash);
		break;
	}
	}

	return answer;
}