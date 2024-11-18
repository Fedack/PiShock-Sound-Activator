#include <Arduino.h>
#include "display_manager.h"
#include "noise_sensor.h"
#include "battery_manager.h"
#include "sleep_manager.h"
#include "ble_manager.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

TFT_eSPI tft;
DisplayManager display(tft);
NoiseSensor sensor(36, display);
BatteryManager battery;
SleepManager sleepManager;
BLEManager bleManager;

// Constants
const int THRESHOLD = 1;
const int DISPLAY_TIMEOUT = 10000;        // 10 seconds
const int BATTERY_UPDATE_INTERVAL = 5000; // Update battery every 5 seconds

// Global variables
int lastPrintedValue = 0;
unsigned long lastNoiseChange = 0;
unsigned long lastBatteryUpdate = 0;

void checkDisplayTimeout()
{
  if (millis() - lastNoiseChange > DISPLAY_TIMEOUT)
  {
    display.turnOffDisplay();
  }
  else if (!display.isDisplayOn())
  {
    display.turnOnDisplay();
    lastBatteryUpdate = millis();
    display.updateBatteryLevel(
        battery.getBatteryPercentage(),
        battery.getBatteryVoltage(),
        battery.isCharging());
  }
}

void setup()
{
  btStart();
  esp_bt_controller_enable(ESP_BT_MODE_BLE);
  esp_bt_controller_mem_release(ESP_BT_MODE_CLASSIC_BT);

  Serial.begin(115200);
  Serial.println("Starting...");

  // Check wake up reason
  esp_sleep_wakeup_cause_t wakeup_reason = esp_sleep_get_wakeup_cause();
  Serial.printf("Wakeup reason: %d\n", wakeup_reason);

  // Initialize all components
  battery.init();
  display.init();
  sleepManager.init();

  // Initial calibration and display setup
  sensor.calibrate();
  display.showNormalDisplay();
  display.updateBatteryLevel(
      battery.getBatteryPercentage(),
      battery.getBatteryVoltage(),
      battery.isCharging());

  // Initialize timers
  lastNoiseChange = millis();
  lastBatteryUpdate = millis();

  bleManager.init(); // Initialize BLE

  Serial.println("Setup complete");
}

void loop()
{
  // Read sound level
  int soundLevel = sensor.getSoundLevel();

  // Get current battery status
  bool currentlyCharging = battery.isCharging();

  // Check if charging status changed
  static bool previousChargingState = currentlyCharging;
  if (previousChargingState != currentlyCharging)
  {
    display.updateBatteryLevel(
        battery.getBatteryPercentage(),
        battery.getBatteryVoltage(),
        currentlyCharging);
    previousChargingState = currentlyCharging;
  }

  // Check if sound level has changed significantly
  if (abs(soundLevel - lastPrintedValue) >= THRESHOLD)
  {
    Serial.printf("Level: %d\n", soundLevel);
    lastPrintedValue = soundLevel;
    lastNoiseChange = millis();
    sleepManager.resetTimer();

    // Update BLE immediately when sound level changes significantly
    if (bleManager.isConnected())
    {
      bleManager.updateNoiseLevel(soundLevel);
    }
  }

  // Update battery display periodically
  if (millis() - lastBatteryUpdate >= BATTERY_UPDATE_INTERVAL)
  {
    display.updateBatteryLevel(
        battery.getBatteryPercentage(),
        battery.getBatteryVoltage(),
        currentlyCharging);
    Serial.printf("Battery: %.2fV (%d%%) %s\n",
                  battery.getBatteryVoltage(),
                  battery.getBatteryPercentage(),
                  currentlyCharging ? "CHARGING" : "ON BATTERY");
    lastBatteryUpdate = millis();
  }

  // Update display and check various timeouts
  display.updateDisplay(soundLevel);
  checkDisplayTimeout();
  sleepManager.checkSleep();

  // Small delay to prevent too frequent updates
  delay(5);
}
