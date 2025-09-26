#pragma once
#include "communication/packet_handler.h"
#include <NimBLEDevice.h>

class BluetoothController {
public:
    bool Initialize();
};

class ServerCallbacks : public NimBLEServerCallbacks {
public:
    void onConnect(NimBLEServer* s, NimBLEConnInfo& ci);
    void onDisconnect(NimBLEServer* s, NimBLEConnInfo& ci, int reason);
    void onMTUChange(uint16_t mtu, NimBLEConnInfo& ci);
};

class RxCallbacks : public NimBLECharacteristicCallbacks {
    void onWrite(NimBLECharacteristic* chr, NimBLEConnInfo& conn);
};