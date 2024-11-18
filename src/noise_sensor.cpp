#include "noise_sensor.h"

NoiseSensor::NoiseSensor(int pin, DisplayManager &display_manager)
    : micPin(pin), display(display_manager)
{
    ambientPeakToPeak = 0;
    ambientMin = 4095;
    ambientMax = 0;
    ambientLevel = 0;
    noiseThreshold = 0;

    // Configure ADC
    analogSetWidth(12);
    analogSetAttenuation(ADC_11db);
    analogSetClockDiv(2);
}

void NoiseSensor::calibrate()
{
    display.showCalibrationStart();
    Serial.println("Starting calibration...");

    // First phase: Basic ambient level calibration
    long sumMin = 0;
    long sumMax = 0;

    for (int i = 0; i < DISCARD_SAMPLES; i++)
    {
        analogRead(micPin);
        delay(1);
    }

    // Calculate initial ambient levels
    for (int set = 0; set < CALIBRATION_SETS; set++)
    {
        int setMin = 4095;
        int setMax = 0;

        for (int i = 0; i < CALIBRATION_SAMPLES; i++)
        {
            long sum = 0;
            for (int j = 0; j < 10; j++)
            {
                sum += analogRead(micPin);
                delay(1);
            }
            int value = sum / 10;

            setMin = min(setMin, value);
            setMax = max(setMax, value);

            int progress = (set * CALIBRATION_SAMPLES + i) * 50 / (CALIBRATION_SETS * CALIBRATION_SAMPLES);
            display.updateCalibrationProgress(progress);
        }

        sumMin += setMin;
        sumMax += setMax;
    }

    ambientMin = sumMin / CALIBRATION_SETS;
    ambientMax = sumMax / CALIBRATION_SETS;
    ambientPeakToPeak = ambientMax - ambientMin;
    ambientLevel = (ambientMax + ambientMin) / 2;

    // Second phase: Noise pattern analysis (10 seconds)
    Serial.println("Analyzing noise patterns...");
    const int ANALYSIS_TIME = 10000; // 10 seconds
    long startTime = millis();
    int maxNoiseLevel = 0;

    while (millis() - startTime < ANALYSIS_TIME)
    {
        int level = getRawSoundLevel();
        maxNoiseLevel = max(maxNoiseLevel, level);

        int progress = 50 + ((millis() - startTime) * 50 / ANALYSIS_TIME);
        display.updateCalibrationProgress(progress);
        delay(10);
    }

    // Set noise threshold just above the maximum noise seen
    noiseThreshold = maxNoiseLevel + 5; // 5% margin

    Serial.printf("Calibration complete:\n");
    Serial.printf("Ambient - Min: %d, Max: %d, Peak-to-Peak: %d, Level: %d\n",
                  ambientMin, ambientMax, ambientPeakToPeak, ambientLevel);
    Serial.printf("Noise Threshold: %d\n", noiseThreshold);
}

// New helper function to get raw sound level
int NoiseSensor::getRawSoundLevel()
{
    const int NUM_SAMPLES = 64;
    long total = 0;
    int maxLevel = 0;

    for (int i = 0; i < NUM_SAMPLES; i++)
    {
        int val = abs(analogRead(micPin) - ambientLevel);
        total += val;
        maxLevel = max(maxLevel, val);
        delayMicroseconds(100);
    }

    int avgLevel = total / NUM_SAMPLES;
    int level = (maxLevel * 4 + avgLevel) / 5;

    return map(level, 100, 1000, 0, 100);
}

int NoiseSensor::getSoundLevel()
{
    int mappedValue = getRawSoundLevel();
    mappedValue = constrain(mappedValue, 0, 100);
    static int lastValue = 0;

    // Apply calibrated noise threshold
    if (mappedValue <= noiseThreshold)
    {
        mappedValue = 0;
    }

    // Smooth transitions for real sounds
    if (abs(mappedValue - lastValue) < 30)
    {
        mappedValue = (mappedValue + lastValue * 2) / 3;
    }

    lastValue = mappedValue;

    // Remap values above 12% to 0-100 range, anything below becomes 0
    return (mappedValue <= 12) ? 0 : map(mappedValue, 15, 100, 0, 100);
}