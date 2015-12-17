/*
 * MotorEncoder.hpp
 *
 * Class used for tracking white markings
 */

#ifndef MOTORENCODER_HPP_
#define MOTORENCODER_HPP_

#include "io.hpp"
#include "FreeRTOS.h"
#include <stdio.h>
#include "utilities.h"
#include "gpio.hpp"
#include "scheduler_task.hpp"
#include "lpc_sys.h"
#include "periodic_scheduler/periodic_callback.h"
#include "MotorControl.hpp"
#include "task.h"
#include "semphr.h"


static GPIO encoderInput(P2_6);

/// Results from encoder calculation
typedef enum{
    SPEED_BELOW_THRESHOLD_SLOW,
    SPEED_BELOW_THRESHOLD_MEDIUM,
    SPEED_ABOVE_THRESHOLD_SLOW,
    SPEED_ABOVE_THRESHOLD_MEDIUM,
    SPEED_NO_CHANGE
}encoderSpeedResult_t;

///ISR function calls for timer, and tick counts based off
// encoder input from GPIO
void StartTickTimer_ISR();
void IncrementTickCounter_ISR();


/* @about Stores current speed to use
 * for comparing against previous speed in
 * HasSpeedChanged()
 */
void CalculateSpeed();

/*
 * @about Returns encoderSpeedResult_t type
 */
encoderSpeedResult_t HasSpeedChanged();



class MotorEncoder : public scheduler_task{
    public:
        MotorEncoder(uint8_t priorityToUse);

        bool init();
        bool run(void *p);

        SemaphoreHandle_t motorEncoderSemaphore;
};

#endif
