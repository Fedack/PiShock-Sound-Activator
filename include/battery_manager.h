#ifndef BATTERY_MANAGER_H
#define BATTERY_MANAGER_H

#include <Arduino.h>

class BatteryManager
{
private:
    static const int BATTERY_PIN = 34;                   // ADC pin for battery voltage
    static const int CHARGING_PIN = 33;                  // Charging detection pin
    static constexpr float BATTERY_MAX_VOLTAGE = 4.2f;   // Max LiPo voltage
    static constexpr float BATTERY_MIN_VOLTAGE = 3.2f;   // Min safe voltage
    static constexpr float VOLTAGE_DIVIDER_RATIO = 3.0f; // T-Display voltage divider

    float readVoltage();

public:
    BatteryManager();
    void init();
    int getBatteryPercentage();
    float getBatteryVoltage();
    bool isCharging(); // New method to check charging status
};

#endif
