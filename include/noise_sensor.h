#ifndef NOISE_SENSOR_H
#define NOISE_SENSOR_H

#include "display_manager.h"

class NoiseSensor
{
private:
    static const int SAMPLES_PER_READ = 32;
    static const int CALIBRATION_SAMPLES = 100; // Reduced from 1000
    static const int DISCARD_SAMPLES = 20;      // Reduced from 100
    static const int CALIBRATION_SETS = 3;      // Reduced from 5

    const int micPin;
    int ambientPeakToPeak;
    int ambientMin;
    int ambientMax;
    int ambientLevel;
    DisplayManager &display;
    int lastSoundLevel = 0;
    int noiseThreshold;
    int spikeThreshold;

    int getRawSoundLevel();

public:
    NoiseSensor(int pin, DisplayManager &display_manager);
    void calibrate();
    int getSoundLevel();
    int getLastSoundLevel() { return lastSoundLevel; }
};

#endif
