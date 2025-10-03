#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include "SensorQMI8658.hpp"
#include <ArduinoBLE.h>  // Include BLE library

#define USE_WIRE
#define SENSOR_SDA 6
#define SENSOR_SCL 7
#define SENSOR_IRQ -1
#define IMU_CS 5

#define MOTION_THRESHOLD_ACC 1.5f
#define MOTION_THRESHOLD_GYRO 1.0f
#define MOVING_AVG_WINDOW 10
#define DEBOUNCE_TIME 1000

SensorQMI8658 qmi;
IMUdata acc;
IMUdata gyr;

float accMagFiltered = 0;
float gyroMagFiltered = 0;

float accMagBuffer[MOVING_AVG_WINDOW] = {0};
float gyroMagBuffer[MOVING_AVG_WINDOW] = {0};
int accMagBufferIndex = 0;
int gyroMagBufferIndex = 0;

unsigned long lastMotionTime = 0;
bool lastMotionState = false;

// BLE Service and Characteristics
BLEService motionService("180A");  // Custom BLE Service
BLECharacteristic motionCharacteristic("2A57", BLERead | BLENotify, 50);  // Motion State
BLECharacteristic sensorDataCharacteristic("2A58", BLERead | BLENotify, 100);  // Sensor Data

float calculateMagnitude(float x, float y, float z) {
    return sqrt(x * x + y * y + z * z);
}

float movingAverageFilter(float *buffer, int &bufferIndex, float newValue) {
    buffer[bufferIndex] = newValue;
    bufferIndex = (bufferIndex + 1) % MOVING_AVG_WINDOW;

    float sum = 0;
    for (int i = 0; i < MOVING_AVG_WINDOW; i++) {
        sum += buffer[i];
    }

    return sum / MOVING_AVG_WINDOW;
}

bool isInMotion(float accMag, float gyroMag) {
    return (accMag > MOTION_THRESHOLD_ACC || gyroMag > MOTION_THRESHOLD_GYRO);
}

void setup() {
    Serial.begin(115200);
    while (!Serial);

    // Initialize BLE
    if (!BLE.begin()) {
        Serial.println("Failed to initialize BLE!");
        while (1);
    }

    BLE.setLocalName("MotionSensor");
    BLE.setAdvertisedService(motionService);
    motionService.addCharacteristic(motionCharacteristic);
    motionService.addCharacteristic(sensorDataCharacteristic);
    BLE.addService(motionService);

    motionCharacteristic.writeValue("Initializing...");
    sensorDataCharacteristic.writeValue("Waiting for data...");

    BLE.advertise();
    Serial.println("BLE Initialized. Advertising...");

#ifdef USE_WIRE
    if (!qmi.begin(Wire, QMI8658_L_SLAVE_ADDRESS, SENSOR_SDA, SENSOR_SCL)) {
        Serial.println("Failed to find QMI8658!");
        while (1) delay(1000);
    }
#else
    if (!qmi.begin(IMU_CS)) {
        Serial.println("Failed to find QMI8658!");
        while (1) delay(1000);
    }
#endif

    Serial.print("Device ID:");
    Serial.println(qmi.getChipID(), HEX);

    qmi.configAccelerometer(
        SensorQMI8658::ACC_RANGE_8G,
        SensorQMI8658::ACC_ODR_1000Hz,
        SensorQMI8658::LPF_MODE_0
    );

    qmi.configGyroscope(
        SensorQMI8658::GYR_RANGE_64DPS,
        SensorQMI8658::GYR_ODR_896_8Hz,
        SensorQMI8658::LPF_MODE_3
    );

    qmi.enableGyroscope();
    qmi.enableAccelerometer();
    qmi.dumpCtrlRegister();

    Serial.println("Sensor Initialized. Starting motion detection...");
}

void loop() {
    BLE.poll();  // Handle BLE events

    if (qmi.getDataReady()) {
        float accMag = 0;
        float gyroMag = 0;

        if (qmi.getAccelerometer(acc.x, acc.y, acc.z) && qmi.getGyroscope(gyr.x, gyr.y, gyr.z)) {
            accMag = calculateMagnitude(acc.x, acc.y, acc.z);
            gyroMag = calculateMagnitude(gyr.x, gyr.y, gyr.z);

            accMagFiltered = movingAverageFilter(accMagBuffer, accMagBufferIndex, accMag);
            gyroMagFiltered = movingAverageFilter(gyroMagBuffer, gyroMagBufferIndex, gyroMag);

            bool motionState = isInMotion(accMagFiltered, gyroMagFiltered);
            unsigned long currentMillis = millis();

            char sensorData[100];  // Buffer for sensor data transmission

            if (motionState != lastMotionState) {
                if (motionState) {
                    if (currentMillis - lastMotionTime > DEBOUNCE_TIME) {
                        Serial.println("STATE: Motion Detected");
                        Serial.printf("ACC: %.2f, %.2f, %.2f (mag: %.2f)\n", acc.x, acc.y, acc.z, accMag);
                        Serial.printf("GYRO: %.2f, %.2f, %.2f (mag: %.2f)\n", gyr.x, gyr.y, gyr.z, gyroMag);

                        motionCharacteristic.writeValue("STATE: Motion Detected");

                        snprintf(sensorData, sizeof(sensorData), 
                                 "ACC: %.2f, %.2f, %.2f (mag: %.2f)\nGYRO: %.2f, %.2f, %.2f (mag: %.2f)", 
                                 acc.x, acc.y, acc.z, accMag, gyr.x, gyr.y, gyr.z, gyroMag);
                        sensorDataCharacteristic.writeValue(sensorData);
                        lastMotionTime = currentMillis;
                    }
                } else {
                    Serial.println("STATE: Resting");
                    motionCharacteristic.writeValue("STATE: Resting");

                    snprintf(sensorData, sizeof(sensorData), 
                             "ACC: %.2f, %.2f, %.2f (mag: %.2f)\nGYRO: %.2f, %.2f, %.2f (mag: %.2f)", 
                             acc.x, acc.y, acc.z, accMag, gyr.x, gyr.y, gyr.z, gyroMag);
                    sensorDataCharacteristic.writeValue(sensorData);
                }

                lastMotionState = motionState;
            }
        }
    }
    delay(100);
}
