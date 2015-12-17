/*
 * MotorEncoder.cpp
 *
 * Definition class for MotorEncoder.hpp
 */

#include "MotorEncoder.hpp"
#include "file_logger.h"
#include "io.hpp"

extern MotorControl motorObj;
MotorEncoder *ptrToMotorEncoder;

const int numberOfTickMarks = 5;
const int circumferenceOfTire = 31; // cm

/// Done through excel sheet after logging speed
const double slowSpeedAverageTime = 190/numberOfTickMarks;
const double mediumSpeedAverageTime = 1;

const double slowSpeedAverageRateInCMPerMilliSecond = circumferenceOfTire/slowSpeedAverageTime;
const double mediumSpeedAverageRateInCMPerMillisecond = circumferenceOfTire/mediumSpeedAverageTime;

double tempSlowSpeedAverage = slowSpeedAverageRateInCMPerMilliSecond;
//double tempMediumSpeedAverage = mediumSpeedAverageRateInCMPerMillisecond;

/*
 * @about Used for keeping track of how many times
 * the encoder hits a tick mark
 *
 * @note Will be used in conjunction with some timer
 * to make sure car is moving at a constant speed
 */
int tickCount = 0;

/*
 * @about Each tick represents a distance traveled of about 31cm
 * Diameter = 9.9cm, radius = 4.95 cm
 * Circumference = ~31cm which is the distance traveled
 *
 * Will calculate rate based of distance traveled and amount of time it took.
 */
double *currentSpeed = &tempSlowSpeedAverage;

double totalTimeOfAllTicks = 0;

double percentDifferenceOfSpeed = .05;
double upperLimitThresholdDiffOfSpeed;
double lowerLimitThresholdDiffOfSpeed;

uint64_t beginTimeOfEncoder;
bool startOfNewTime = true;


void CalculateSpeed(){
    double currentTime = (double)sys_get_uptime_ms();
    double timeDiffOfTickMarksInSeconds = currentTime - (double)beginTimeOfEncoder;

    beginTimeOfEncoder = (uint64_t)currentTime;
    totalTimeOfAllTicks += timeDiffOfTickMarksInSeconds;

   // if (tickCount == 5){
        *currentSpeed = (circumferenceOfTire/numberOfTickMarks) / timeDiffOfTickMarksInSeconds;
    //}
}

encoderSpeedResult_t HasSpeedChanged(){
    float currentMotorValue = motorObj.CurrentMotorValue;
    bool isSlowSpeed = (currentMotorValue == motorObj.SLOW_SPEED);
    bool isMediumSpeed = (currentMotorValue == motorObj.MEDIUM_SPEED);

    if ((isSlowSpeed) || (isMediumSpeed)){
        if (isSlowSpeed){
            upperLimitThresholdDiffOfSpeed = (slowSpeedAverageRateInCMPerMilliSecond + (slowSpeedAverageRateInCMPerMilliSecond * percentDifferenceOfSpeed));
            lowerLimitThresholdDiffOfSpeed = (slowSpeedAverageRateInCMPerMilliSecond - (slowSpeedAverageRateInCMPerMilliSecond * percentDifferenceOfSpeed));
        }
        else if (isMediumSpeed){
            upperLimitThresholdDiffOfSpeed = (mediumSpeedAverageRateInCMPerMillisecond + (mediumSpeedAverageRateInCMPerMillisecond * percentDifferenceOfSpeed));
            lowerLimitThresholdDiffOfSpeed = (mediumSpeedAverageRateInCMPerMillisecond  - (mediumSpeedAverageRateInCMPerMillisecond * percentDifferenceOfSpeed));
        }

        if (*currentSpeed < lowerLimitThresholdDiffOfSpeed){
            printf("CUR < LOWER\nCurrent speed = %.5f, lowerLimit = %.10f\n\n", *currentSpeed, lowerLimitThresholdDiffOfSpeed);

            if (isSlowSpeed)
                return SPEED_BELOW_THRESHOLD_SLOW;
            if (isMediumSpeed)
                return SPEED_BELOW_THRESHOLD_MEDIUM;
        }
        else if (*currentSpeed > upperLimitThresholdDiffOfSpeed){
            printf("CUR > UPPER\nCurrent speed = %.5f, upperLimit = %.10f\n\n", *currentSpeed, upperLimitThresholdDiffOfSpeed);

            if (isSlowSpeed)
                return SPEED_ABOVE_THRESHOLD_SLOW;
            if (isMediumSpeed)
                return SPEED_ABOVE_THRESHOLD_MEDIUM;
        }
    }

    return SPEED_NO_CHANGE;
}


void StartTickTimer_ISR(){
    if (startOfNewTime){
        beginTimeOfEncoder = sys_get_uptime_ms();
        startOfNewTime = false;
    }
    else {
//        CalculateSpeed();
        puts("inside tick timer\n");
//        /// Next time around, when the tick count = 5,
//        // timer should begin once again
//        if (tickCount == 4)
//            startOfNewTime = true;
    }
}

static BaseType_t xHigherPriorityTaskWoken = true;

void IncrementTickCounter_ISR(){
    /// Increment counter
    tickCount++;

    /// Give signal to function waiting for semaphore to be free, to perform
            // the necessary function
            xSemaphoreGiveFromISR(ptrToMotorEncoder->motorEncoderSemaphore, &xHigherPriorityTaskWoken);

            /// Task1 ---->
            //          IncrementTickCounter_ISR---->
            //                          MotorEncoder---->
            if (xHigherPriorityTaskWoken){
                portYIELD_FROM_ISR(true);

                xHigherPriorityTaskWoken = pdFALSE;
            }
}

MotorEncoder::MotorEncoder(uint8_t priorityToUse) :
        scheduler_task("motorEncoder", 1024, priorityToUse){
    ptrToMotorEncoder = this;
}

bool MotorEncoder::init(){
    motorEncoderSemaphore = xSemaphoreCreateBinary();
}

bool MotorEncoder::run(void *p){
    if(xSemaphoreTake(motorEncoderSemaphore, portMAX_DELAY)){
        //printf("Inside motorEncoder tasks after 5 ticks\nTick count = %i\n\n", tickCount);

        CalculateSpeed();

        //if (tickCount == 5)
            LOG_DEBUG("Current speed = %.5f\n", *currentSpeed);

        tickCount = 0;
        xHigherPriorityTaskWoken = pdTRUE;
    }
    return true;
}

