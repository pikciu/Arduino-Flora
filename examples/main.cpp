#include <Arduino.h>
#include <BLEDevice.h>
#include <Flora.h>

Flora flora("C4:7C:8D:6B:14:D6");
FloraData data;

void setup() {
    Serial.begin(9600);
    Serial.println();

    BLEDevice::init("Flora");
    BLEDevice::setPower(ESP_PWR_LVL_P9);

    if (flora.connect()) {
        Serial.println("connection success");

        if (flora.read(&data)) {
            Serial.print("temp: ");
            Serial.println(data.temperature);
            Serial.print("conductivity: ");
            Serial.println(data.conductivity);
            Serial.print("moisture: ");
            Serial.println(data.moisture);
            Serial.print("light: ");
            Serial.println(data.light);
            Serial.print("battery: ");
            Serial.println(data.battery);

        } else {
            Serial.println("reading data failed")
        }

        flora.disconnect();

    } else {
        Serial.println("connection to flora failed");
    }
}

void loop() {
}