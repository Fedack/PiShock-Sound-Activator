#include "battery_manager.h"

BatteryManager::BatteryManager() {}

void BatteryManager::init()
{
    analogSetPinAttenuation(BATTERY_PIN, ADC_11db);
    pinMode(CHARGING_PIN, INPUT_PULLUP);
}

float BatteryManager::readVoltage()
{
    const int READINGS = 10;
    float sum = 0;

    for (int i = 0; i < READINGS; i++)
    {
        sum += analogRead(BATTERY_PIN);
        delay(1);
    }

    float averageReading = sum / READINGS;
    float voltage = (averageReading * 3.3 / 4095) * VOLTAGE_DIVIDER_RATIO;
    return voltage;
}

int BatteryManager::getBatteryPercentage()
{
    float voltage = readVoltage();
    int percentage = map(voltage * 100,
                         BATTERY_MIN_VOLTAGE * 100,
                         BATTERY_MAX_VOLTAGE * 100,
                         0,
                         100);
    return constrain(percentage, 0, 100);
}

float BatteryManager::getBatteryVoltage()
{
    return readVoltage();
}

bool BatteryManager::isCharging()
{
    return digitalRead(CHARGING_PIN) == LOW;
}
