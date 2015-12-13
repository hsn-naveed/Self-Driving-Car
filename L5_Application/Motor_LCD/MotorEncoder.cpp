/*
 * MotorEncoder.cpp
 *
 * Definition class for MotorEncoder.hpp
 */

#include "MotorEncoder.hpp"
#include "file_logger.h"
#include "io.hpp"



const int circumferenceOfTire = 31; // cm

/// Done through excel sheet after logging speed
const double slowSpeedAverageTime = 190;
const double slowSpeedAverageRateInCMPerMilliSecond = circumferenceOfTire/slowSpeedAverageTime;
double tempSlowSpeedAverage = slowSpeedAverageRateInCMPerMilliSecond;
/*
 * @about Each tick represents a distance traveled of about 31cm
 * Diameter = 9.9cm, radius = 4.95 cm
 * Circumference = ~31cm which is the distance traveled
 *
 * Will calculate rate based of distance traveled and amount of time it took.
 */
double *currentSpeed = &tempSlowSpeedAverage;

double percentDifferenceOfSpeed = .05;
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
    //LOG_INFO("Current time = %f, begin time = %.0f\n", (double)sys_get_uptime_ms(), (double)beginTimeOfEncoder);
    //printf("Current time = %f, begin time = %f\n", (double)sys_get_uptime_ms(), (double)beginTimeOfEncoder);

    *currentSpeed = circumferenceOfTire / timeDiffOfTickMarksInSeconds;
    //printf("Current speed = %.5f, average measured speed = %.5f\n\n", *currentSpeed, slowSpeedAverageRateInMetersPerMilliSecond);
}

int HasSpeedChanged(){
    upperLimitThresholdDiffOfSpeed = (slowSpeedAverageRateInCMPerMilliSecond + (slowSpeedAverageRateInCMPerMilliSecond * percentDifferenceOfSpeed));
    lowerLimitThresholdDiffOfSpeed = (slowSpeedAverageRateInCMPerMilliSecond - (slowSpeedAverageRateInCMPerMilliSecond * percentDifferenceOfSpeed));

    if (*currentSpeed < lowerLimitThresholdDiffOfSpeed){
        printf("CUR < LOWER\nCurrent speed = %.5f, lowerLimit = %.10f\n\n", *currentSpeed, lowerLimitThresholdDiffOfSpeed);
        return 1;
    }
    else if (*currentSpeed > upperLimitThresholdDiffOfSpeed){
        printf("CUR > UPPER\nCurrent speed = %.5f, upperLimit = %.10f\n\n", *currentSpeed, upperLimitThresholdDiffOfSpeed);
        return 2;
    }
    return 0;
}
