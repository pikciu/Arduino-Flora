#ifndef FLORA_h
#define FLORA_h

#include <Arduino.h>
#include <BLEDevice.h>

#define SERVICE_UUID                        BLEUUID("00001204-0000-1000-8000-00805f9b34fb")

#define BATTERY_CHARACTERISTIC_UUID         BLEUUID("00001a02-0000-1000-8000-00805f9b34fb")
#define SENSOR_DATA_CHARACTERISTIC_UUID     BLEUUID("00001a01-0000-1000-8000-00805f9b34fb")
#define WRITE_MODE_CHARACTERISTIC_UUID      BLEUUID("00001a00-0000-1000-8000-00805f9b34fb")

#define BLEDEVICE_ADDRESS_SIZE 18

#ifndef FLORA_DATA_H
#define FLORA_DATA_H
struct FloraData {
    float temperature;
    uint8_t moisture;
    uint32_t light;
    uint16_t conductivity;
    uint8_t battery;
};
#endif

class Flora {
    public:
    Flora(const char deviceAddress[BLEDEVICE_ADDRESS_SIZE]);

    bool connect();
    void disconnect();
    bool read(FloraData* data);

    private:
    char address[BLEDEVICE_ADDRESS_SIZE];
    BLEClient* client;
    BLERemoteService* getService();
    BLERemoteCharacteristic* getCharacteristic(BLEUUID uuid, BLERemoteService* service);
    bool enableDataMode(BLERemoteService* service);
    bool read(BLERemoteService* service, FloraData* data);
    bool readBattery(BLERemoteService* service, FloraData* data);
};

#endif
