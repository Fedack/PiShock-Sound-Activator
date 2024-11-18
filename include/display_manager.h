#ifndef DISPLAY_MANAGER_H
#define DISPLAY_MANAGER_H

#include <TFT_eSPI.h>

class DisplayManager
{
private:
    TFT_eSPI &tft;
    static const int SCREEN_WIDTH = 240;
    static const int SCREEN_HEIGHT = 135;
    static const int BAR_HEIGHT = 30;
    static const int BAR_Y = SCREEN_HEIGHT - BAR_HEIGHT - 5;
    bool displayOn;
    int lastDisplayedValue;
    int currentBatteryLevel;
    float currentVoltage; // Add this

    void drawBar(int width, uint16_t color);
    void clearBar();
    void drawBarFrame();
    void showBatteryIcon(int x, int y, int percentage, bool isCharging);

public:
    DisplayManager(TFT_eSPI &tft_instance);
    void init();
    void showCalibrationStart();
    void updateCalibrationProgress(int progress);
    void showNormalDisplay();
    void updateDisplay(int level);
    void turnOffDisplay();
    void turnOnDisplay();
    void updateBatteryLevel(int percentage, float voltage, bool isCharging); // Modified
    bool isDisplayOn() const { return displayOn; }
};

#endif
