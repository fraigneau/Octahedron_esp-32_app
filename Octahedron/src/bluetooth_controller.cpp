#include "bluetooth_controller.h"
#include <Arduino.h>

#define SVC_UUID "12345678-1234-1234-1234-1234567890AB"
#define CHAR_UUID "12345678-1234-1234-1234-1234567890AC"
#define DEVICE_NAME "ESP32-LFR"
NimBLEServer* server;
NimBLECharacteristic* chrHello;
ServerCallbacks serverCallbacks;
RxCallbacks rxCallbacks;
PacketHandler packetHandler;


bool BluetoothController::Initialize() {
	if (!NimBLEDevice::init(DEVICE_NAME))
	{
		Serial.printf("[!] FATAL [!] Failed to initialize BLE device\n");
		return false;
	}
	NimBLEDevice::setDeviceName(DEVICE_NAME);
	NimBLEDevice::setPower(ESP_PWR_LVL_P6);
	NimBLEDevice::setMTU(512);
	packetHandler = PacketHandler();
	server = NimBLEDevice::createServer();
	if (!server) {
		Serial.printf("[!] FATAL [!] Failed to create BLE server\n");
		return false;
	}
	server->setCallbacks(&serverCallbacks);
	server->advertiseOnDisconnect(true); // relance auto l advertising apres deconnexion

	NimBLEService* svc = server->createService(SVC_UUID);
	if (!svc) {
		Serial.printf("[!] FATAL [!] Failed to create BLE service\n");
		return false;
	}
	chrHello = svc->createCharacteristic(
		CHAR_UUID,
		NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::WRITE | NIMBLE_PROPERTY::NOTIFY
	);
	if (!chrHello) {
		Serial.printf("[!] FATAL [!] Failed to create BLE characteristic\n");
		return false;
	}
	chrHello->setValue("HELLO");
	chrHello->setCallbacks(&rxCallbacks);
	if (!svc->start()) {
		Serial.printf("[!] FATAL [!] Failed to start BLE service\n");
		return false;
	}
	NimBLEAdvertisementData advData;
	advData.setFlags(0x06);
	advData.setName(DEVICE_NAME);
	auto* adv = NimBLEDevice::getAdvertising();
	adv->addServiceUUID(SVC_UUID);
	adv->setName(DEVICE_NAME);
	adv->setAdvertisementData(advData);
	adv->enableScanResponse(true);
	if (!adv->start()) {
		Serial.printf("[!] FATAL [!] Failed to start advertising ahah prout\n");
		return false;
	}
	Serial.printf("[*] BLE initialized. Advertising as 'FLR'\n");
	return true;
}

void ServerCallbacks::onConnect(NimBLEServer* s, NimBLEConnInfo& ci)
{
	Serial.printf("Connected: %s (mtu=%u)\n", ci.getAddress().toString().c_str(), ci.getMTU());
	s->updateConnParams(ci.getConnHandle(), 24, 48, 0, 400);
}

void ServerCallbacks::onDisconnect(NimBLEServer* s, NimBLEConnInfo& ci, int reason)
{
	Serial.printf("Disconnected (%d). Advertising�\n", reason);
	NimBLEDevice::startAdvertising();
}

void ServerCallbacks::onMTUChange(uint16_t mtu, NimBLEConnInfo& ci)
{
	Serial.printf("MTU updated: %u\n", mtu);
}

void RxCallbacks::onWrite(NimBLECharacteristic* chr, NimBLEConnInfo& conn)
{
	NimBLEAttValue v = chr->getValue();
	//for(int i = 0; i < v.length(); i++)
	//{
	//	Serial.printf("%02X ", v[i]);
	//}
	auto response = packetHandler.onPacketReceived(v.data(), v.length());
	if (response.size())
	{
		Serial.printf("Sending response (%d bytes): \n", response.size());
		for (int i = 0; i < response.size(); i++)
		{
			Serial.printf("%02X ", response[i]);
		}
		chr->notify(response.data(), response.size(), conn.getConnHandle());
	}
}