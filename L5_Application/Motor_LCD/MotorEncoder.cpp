/*
 * MotorEncoder.cpp
 *
 * Definition class for MotorEncoder.hpp
 */

#include "MotorEncoder.hpp"
#include "file_logger.h"
#include "io.hpp"

MotorEncoder *ptrToMotorEncoder;

const int numberOfTickMarks = 5;
const int circumferenceOfTire = 31; // cm

/// Done through excel sheet after logging speed
const double slowSpeedAverageTime = 190;
const double mediumSpeedAverageTime = 1;

const double slowSpeedAverageRateInCMPerMilliSecond = circumferenceOfTire/slowSpeedAverageTime;
const double mediumSpeedAverageRateInCMPerMillisecond = circumferenceOfTire/mediumSpeedAverageTime;

double tempSlowSpeedAverage = slowSpeedAverageRateInCMPerMilliSecond;
double tempMediumSpeedAverage = mediumSpeedAverageRateInCMPerMillisecond;

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
//        xSemaphoreTakeFromISR(motorEncoderSemaphore, 0);

        beginTimeOfEncoder = sys_get_uptime_ms();
        startOfNewTime = false;
    }
    else{
//        xSemaphoreGiveFromISR(motorEncoderSemaphore, NULL);

        CalculateSpeed();
        startOfNewTime = true;
    }
}


void CalculateSpeed(){
    double currentTime = (double)sys_get_uptime_ms();
    double timeDiffOfTickMarksInSeconds = currentTime - (double)beginTimeOfEncoder;
//    LOG_INFO("Current time = %f, begin time = %f\n", (double)sys_get_uptime_ms(), (double)beginTimeOfEncoder);


    //beginTimeOfEncoder = currentTime;

    /// Log time info for each tick mark

    //printf("Current time = %f, begin time = %f\n", (double)sys_get_uptime_ms(), (double)beginTimeOfEncoder);

    //*currentSpeed = (circumferenceOfTire/numberOfTickMarks) / timeDiffOfTickMarksInSeconds;
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





/*
 * @about Used for keeping track of how many times
 * the encoder hits a tick mark
 *
 * @note Will be used in conjunction with some timer
 * to make sure car is moving at a constant speed
 */
int tickCount = 0;


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

    xSemaphoreGiveFromISR(ptrToMotorEncoder->motorEncoderSemaphore, &xHigherPriorityTaskWoken);
    //
    if (xHigherPriorityTaskWoken){
        portYIELD_FROM_ISR(true);

        xHigherPriorityTaskWoken = pdFALSE;
    }


//    if (tickCount == 5){
//        /// Give signal to function waiting for semaphore to be free, to perform
//        // the necessary function
//        xSemaphoreGiveFromISR(ptrToMotorEncoder->motorEncoderSemaphore, &xHigherPriorityTaskWoken);
//
//        if (xHigherPriorityTaskWoken){
//            portYIELD_FROM_ISR(true);
//
//            xHigherPriorityTaskWoken = pdFALSE;
//        }
//    }
}

MotorEncoder::MotorEncoder(uint8_t priorityToUse) :
        scheduler_task("motorEncoder", 1024, priorityToUse){
    ptrToMotorEncoder = this;
}

bool MotorEncoder::init(){
    motorEncoderSemaphore = xSemaphoreCreateBinary();
}

bool MotorEncoder::run(void *p){
    if(xSemaphoreTake(motorEncoderSemaphore, 0)){
        double currentTime = (double)sys_get_uptime_ms();
        LOG_INFO("RUN func: Current time = %f, begin time = %f\n", (double)sys_get_uptime_ms(), (double)beginTimeOfEncoder);

        printf("Inside motorEncoder tasks after 5 ticks\nTick count = %i\n\n", tickCount);

        beginTimeOfEncoder = (uint64_t)currentTime;

        tickCount = 0;
        xHigherPriorityTaskWoken = pdTRUE;
    }
    //    if (previousTickCount == tickCount){

    //    }

    return true;
}

