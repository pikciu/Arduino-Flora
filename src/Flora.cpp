#include <Flora.h>

Flora::Flora(const char* address) {
    this->address = address;
}

bool Flora::connect() {
    disconnect();
    if (!client) {
        client = BLEDevice::createClient();
    }

    return client->connect(BLEAddress(address));
}

void Flora::disconnect() {
    if (client) {
        client->disconnect();
    }
}

bool Flora::read(FloraData* data) {
    BLERemoteService* service = getService();
    if (!service) {
        return false;
    }

    if (!enableDataMode(service)) {
        return false;
    }

    if (!read(service, data)) {
        return false;
    }

    readBattery(service, data);

    return true;
}

BLERemoteService* Flora::getService() {
    try {
        return client->getService(SERVICE_UUID);
    } catch (const std::exception& e) {
        Serial.println(e.what());
        return nullptr;
    }
}

BLERemoteCharacteristic* Flora::getCharacteristic(BLEUUID uuid, BLERemoteService* service) {
    try {
        return service->getCharacteristic(uuid);
    } catch (const std::exception& e) {
        Serial.println(e.what());
        return nullptr;
    }
}

bool Flora::enableDataMode(BLERemoteService* service) {
    BLERemoteCharacteristic* dataModeCharacteristic = getCharacteristic(WRITE_MODE_CHARACTERISTIC_UUID, service);
    if (!dataModeCharacteristic) {
        return false;
    }

    uint8_t buf[2] = {0xA0, 0x1F};
    dataModeCharacteristic->writeValue(buf, 2, true);

    delay(100);
    return true;
}

bool Flora::read(BLERemoteService* service, FloraData* data) {
    BLERemoteCharacteristic* characteristic = getCharacteristic(SENSOR_DATA_CHARACTERISTIC_UUID, service);
    if (!characteristic) {
        return false;
    }
    std::string value;
    try {
        value = characteristic->readValue();
    } catch (const std::exception& e) {
        Serial.println(e.what());
        return false;
    }
    const char* raw = value.c_str();

    int16_t* temp = (int16_t*)raw;
    int moisture = raw[7];
    int light = raw[4] << 8 | raw[3];
    int conductivity = raw[9] << 8 | raw[8];
    
    data->temperature = *temp / 10.0;
    data->moisture = moisture;
    data->light = light;
    data->conductivity = conductivity;
    

    return true;
}

bool Flora::readBattery(BLERemoteService* service, FloraData* data) {
    BLERemoteCharacteristic* characteristic = getCharacteristic(BATTERY_CHARACTERISTIC_UUID, service);
    if (!characteristic) {
        return false;
    }
    std::string value;
    try {
        value = characteristic->readValue();
    } catch (const std::exception& e) {
        Serial.println(e.what());
        return false;
    }
    const char* raw = value.c_str();
    int battery = raw[0];
    data->battery = battery;
    return true;
}