#include "sleep_manager.h"

SleepManager::SleepManager() : lastActivityTime(0) {}

void SleepManager::init()
{
    esp_sleep_enable_ext0_wakeup(BUTTON_PIN, 0);                              // Wake on button press
    esp_sleep_enable_ext1_wakeup(1ULL << WAKE_PIN, ESP_EXT1_WAKEUP_ANY_HIGH); // Wake on sound
    lastActivityTime = micros();
}

void SleepManager::resetTimer()
{
    lastActivityTime = micros();
}

void SleepManager::checkSleep()
{
    if (micros() - lastActivityTime > SLEEP_TIMEOUT)
    {
        Serial.println("Going to sleep...");
        delay(100); // Allow serial to complete
        esp_deep_sleep_start();
    }
}
