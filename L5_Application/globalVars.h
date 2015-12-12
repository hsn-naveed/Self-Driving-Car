/*
 * globalVars.h
 *
 *  Created on: Nov 29, 2015
 *      Author: Calvin
 */
#include <stdint.h>

#ifndef L5_APPLICATION_GLOBALVARS_H_
#define L5_APPLICATION_GLOBALVARS_H_

static ANDROID_TX_ANDROID_INFO_COORDINATES_t dest[128] = {0};
static uint8_t currentDest = 0;
static uint8_t num_rx_checkpoints = 0;
static ANDROID_TX_ANDROID_INFO_CHECKPOINTS_t lastDest = {0};


#endif /* L5_APPLICATION_GLOBALVARS_H_ */
