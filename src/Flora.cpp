#include <Flora.h>

Flora::Flora(const char deviceAddress[BLEDEVICE_ADDRESS_SIZE]) {
    strncpy(address, deviceAddress, sizeof(address));
    address[BLEDEVICE_ADDRESS_SIZE - 1] = 0;
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

    uint8_t data[2] = {0xA0, 0x1F};
    dataModeCharacteristic->writeValue(data, 2, true);

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

    uint16_t* temp = (uint16_t*)raw;
    uint32_t light = raw[6] << 24 | raw[5] << 16 | raw[4] << 8 | raw[3];
    uint8_t moisture = raw[7];
    uint16_t conductivity = raw[9] << 8 | raw[8];

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
    uint8_t battery = raw[0];
    data->battery = battery;
    return true;
}