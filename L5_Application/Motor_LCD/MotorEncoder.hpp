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
void storeBeginTime();

void CalculateSpeed();
bool HasSpeedChanged();

#endif
