#pragma once
#include "communication/packet_handler.h"
#include <NimBLEDevice.h>

/*
* This class manages the Bluetooth Low Energy (BLE) functionality of the device.
*
*/
class BluetoothController {
public:
	bool Initialize();
};

/*
* Callback class for BLE server events such as connections and disconnections.
*/
class ServerCallbacks : public NimBLEServerCallbacks {
public:
    void onConnect(NimBLEServer* s, NimBLEConnInfo& ci);
    void onDisconnect(NimBLEServer* s, NimBLEConnInfo& ci, int reason);
    void onMTUChange(uint16_t mtu, NimBLEConnInfo& ci);
};

/*
* Callback class for BLE characteristic write events. (Client -> Server)
*/
class RxCallbacks : public NimBLECharacteristicCallbacks {
    void onWrite(NimBLECharacteristic* chr, NimBLEConnInfo& conn);
};