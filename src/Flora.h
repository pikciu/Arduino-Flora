#ifndef FLORA_h
#define FLORA_h

#include <Arduino.h>
#include <BLEDevice.h>

#define SERVICE_UUID                        BLEUUID("00001204-0000-1000-8000-00805f9b34fb")

#define BATTERY_CHARACTERISTIC_UUID         BLEUUID("00001a02-0000-1000-8000-00805f9b34fb")
#define SENSOR_DATA_CHARACTERISTIC_UUID     BLEUUID("00001a01-0000-1000-8000-00805f9b34fb")
#define WRITE_MODE_CHARACTERISTIC_UUID      BLEUUID("00001a00-0000-1000-8000-00805f9b34fb")

#ifndef FLORA_DATA_H
#define FLORA_DATA_H
struct FloraData
{
    float temperature;
    int moisture;
    int light;
    int conductivity;
    int battery;
};
#endif

class Flora
{
    public:

    Flora(const char* address);
    
    bool connect();
    void disconnect();
    bool read(FloraData* data);

	private:
    const char* address;
    BLEClient* client;
    BLERemoteService* getService();
    BLERemoteCharacteristic* getCharacteristic(BLEUUID uuid, BLERemoteService* service);
    bool enableDataMode(BLERemoteService* service);
    bool read(BLERemoteService* service, FloraData* data);
    bool readBattery(BLERemoteService* service, FloraData* data);
    
};

#endif
