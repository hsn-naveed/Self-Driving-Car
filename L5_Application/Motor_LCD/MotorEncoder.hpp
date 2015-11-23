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

static GPIO encoderInput(P2_6);

extern uint64_t beginTimeOfEncoder;
extern uint64_t endTimeOfEncoder;

extern uint64_t diffOfEncoderTimePrev;
extern uint64_t diffOfEncoderTime;





void storeBeginTime();


void storeEndTime();

void CalculateEncoderTimeDifference();



void CalculateSpeed();
bool HasSpeedChanged();

#endif
