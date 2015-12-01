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

/* @about Stores current speed to use
 * for comparing against previous speed in
 * HasSpeedChanged()
 */
void CalculateSpeed();
/*
 * @about Returns 1 if speed has decreased
 * returns 2 if speed has increased *Should not happen*
 * default return 0 if speed is within percent threshold
 */
int HasSpeedChanged();

#endif
