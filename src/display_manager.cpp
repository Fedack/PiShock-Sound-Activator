#include "display_manager.h"

DisplayManager::DisplayManager(TFT_eSPI &tft_instance)
    : tft(tft_instance), displayOn(true), lastDisplayedValue(0), currentBatteryLevel(0) {}

void DisplayManager::init()
{
    tft.init();
    tft.setRotation(1);
    tft.setTextFont(2); // Set default font
    tft.setTextColor(TFT_WHITE);
    tft.setTextSize(1);
    pinMode(TFT_BL, OUTPUT);
    digitalWrite(TFT_BL, HIGH);
    displayOn = true;
    showNormalDisplay();
}

void DisplayManager::drawBarFrame()
{
    tft.drawRect(5, BAR_Y, SCREEN_WIDTH - 10, BAR_HEIGHT, TFT_WHITE);
}

void DisplayManager::clearBar()
{
    tft.fillRect(6, BAR_Y + 1, SCREEN_WIDTH - 12, BAR_HEIGHT - 2, TFT_BLACK);
}

void DisplayManager::drawBar(int width, uint16_t color)
{
    if (width > 0)
    {
        tft.fillRect(6, BAR_Y + 1, width, BAR_HEIGHT - 2, color);
    }
}

void DisplayManager::showCalibrationStart()
{
    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.setTextSize(2);

    // Draw calibration text
    tft.drawString("Calibrating", 10, 10);
    tft.drawString("Ambient Noise", 10, 30);

    // Draw the bar frame in the same position as normal mode
    drawBarFrame();
}

void DisplayManager::updateCalibrationProgress(int progress)
{
    // Clear the previous progress text
    tft.fillRect(180, 30, 60, 20, TFT_BLACK);
    tft.drawString(String(progress) + "%", 180, 30);

    // Update the bar using the same bar as noise display
    clearBar();
    int barWidth = map(progress, 0, 100, 0, SCREEN_WIDTH - 12);
    drawBar(barWidth, TFT_BLUE); // Use blue for calibration
}

void DisplayManager::showNormalDisplay()
{
    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.setTextSize(2);
    tft.drawString("Noise:", 10, 10);
    drawBarFrame();
    updateBatteryLevel(currentBatteryLevel, currentVoltage, false); // Default to not charging
}

void DisplayManager::updateDisplay(int level)
{
    lastDisplayedValue = level;

    if (!displayOn)
        return;

    // Update numerical value
    tft.fillRect(100, 10, 60, 20, TFT_BLACK);
    tft.drawString(String(level), 100, 10);

    // Update bar
    int barWidth = map(level, 0, 100, 0, SCREEN_WIDTH - 12);

    uint16_t barColor;
    if (level < 40)
        barColor = TFT_GREEN;
    else if (level < 70)
        barColor = TFT_YELLOW;
    else
        barColor = TFT_RED;

    clearBar();
    drawBar(barWidth, barColor);
}

void DisplayManager::turnOffDisplay()
{
    if (displayOn)
    {
        digitalWrite(TFT_BL, LOW);
        displayOn = false;
    }
}

void DisplayManager::turnOnDisplay()
{
    if (!displayOn)
    {
        digitalWrite(TFT_BL, HIGH);
        displayOn = true;
        showNormalDisplay();
        updateDisplay(lastDisplayedValue);
        updateBatteryLevel(currentBatteryLevel, currentVoltage, false); // Default to not charging
    }
}

void DisplayManager::showBatteryIcon(int x, int y, int percentage, bool isCharging)
{
    // Clear the area first
    tft.fillRect(x - 40, y, 70, 20, TFT_BLACK);

    // Draw battery outline
    tft.drawRect(x, y, 25, 12, TFT_WHITE);
    tft.fillRect(x + 25, y + 3, 3, 6, TFT_WHITE);

    // Draw battery level
    int width = map(percentage, 0, 100, 0, 23);
    uint16_t color = TFT_GREEN;
    if (percentage <= 20)
        color = TFT_RED;
    else if (percentage <= 50)
        color = TFT_YELLOW;

    tft.fillRect(x + 1, y + 1, width, 10, color);

    // Show percentage - simplified text drawing
    tft.setTextSize(1);
    tft.setTextColor(TFT_WHITE);
    tft.setCursor(x - 30, y + 2);
    tft.print(percentage);
    tft.print("%");

    // Show charging indicator when charging
    if (isCharging)
    {
        tft.fillTriangle(x - 12, y + 6, x - 8, y + 2, x - 8, y + 5, TFT_YELLOW);
        tft.fillTriangle(x - 8, y + 5, x - 8, y + 2, x - 4, y + 6, TFT_YELLOW);
        tft.fillTriangle(x - 8, y + 7, x - 8, y + 10, x - 4, y + 6, TFT_YELLOW);
        tft.fillTriangle(x - 12, y + 6, x - 8, y + 7, x - 8, y + 10, TFT_YELLOW);
    }
}

void DisplayManager::updateBatteryLevel(int percentage, float voltage, bool isCharging)
{
    currentBatteryLevel = percentage;
    currentVoltage = voltage;
    if (displayOn)
    {
        showBatteryIcon(SCREEN_WIDTH - 35, 5, percentage, isCharging);
    }
}
