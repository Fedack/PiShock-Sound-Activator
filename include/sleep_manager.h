#ifndef SLEEP_MANAGER_H
#define SLEEP_MANAGER_H

#include <Arduino.h>
#include "driver/rtc_io.h"

class SleepManager
{
private:
    static const uint64_t SLEEP_TIMEOUT = 200000000;  // 5 minutes in microseconds
    static const gpio_num_t BUTTON_PIN = GPIO_NUM_35; // Button PIN
    static const gpio_num_t WAKE_PIN = GPIO_NUM_36;   // Sound sensor PIN
    unsigned long lastActivityTime;

public:
    SleepManager();
    void init();
    void checkSleep();
    void resetTimer();
};

#endif
