/*
 * MotorEncoder.cpp
 *
 * Definition class for MotorEncoder.hpp
 */

#include "MotorEncoder.hpp"
#include "file_logger.h"

/*
 * @about Each tick represents a distance traveled of about 31cm
 * Diameter = 9.9cm, radius = 4.95 cm
 * Circumference = ~31cm which is the distance traveled
 *
 * Will calculate rate based of distance traveled and amount of time it took.
 */
//double previousSpeed = 0;
double currentSpeed = 0;

const int circumferenceOfTire = 31;
const double distanceInMeters = circumferenceOfTire / 100;

/// Done through excel sheet after logging speed
double slowSpeedAverageTime = 62;
double slowSpeedAverageRateInMetersPerMilliSecond = distanceInMeters/slowSpeedAverageTime;

double percentDifferenceOfSpeed = .10;
double upperLimitThresholdDiffOfSpeed;
double lowerLimitThresholdDiffOfSpeed;

uint64_t beginTimeOfEncoder;
bool startOfNewTime = true;

void storeBeginTime(){
    if (startOfNewTime){
        beginTimeOfEncoder = sys_get_uptime_ms();
        startOfNewTime = false;
    }
    else{
        CalculateSpeed();
        startOfNewTime = true;
    }
}

void CalculateSpeed(){
    double timeDiffOfTickMarksInSeconds = (double)sys_get_uptime_ms() - (double)beginTimeOfEncoder;

    /// Log time info for each tick mark
    LOG_INFO("Current time = %f, begin time = %.0f\n", (double)sys_get_uptime_ms(), (double)beginTimeOfEncoder);
    //printf("Current time = %f, begin time = %f\n", (double)sys_get_uptime_ms(), (double)beginTimeOfEncoder);

    currentSpeed = distanceInMeters / timeDiffOfTickMarksInSeconds;
}

bool HasSpeedChanged(){
    upperLimitThresholdDiffOfSpeed = slowSpeedAverageRateInMetersPerMilliSecond + (slowSpeedAverageRateInMetersPerMilliSecond * percentDifferenceOfSpeed);
    lowerLimitThresholdDiffOfSpeed = slowSpeedAverageRateInMetersPerMilliSecond - (slowSpeedAverageRateInMetersPerMilliSecond * percentDifferenceOfSpeed);

    if (currentSpeed < lowerLimitThresholdDiffOfSpeed)
        return 1;
    else if (currentSpeed > upperLimitThresholdDiffOfSpeed)
        return 2;
    return 0;
}
