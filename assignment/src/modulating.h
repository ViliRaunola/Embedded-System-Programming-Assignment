/*
 * modulating.h
 *
 *  Created on: 12 Dec 2023
 *      Author: vraun
 */

#ifndef SRC_MODULATING_H_
#define SRC_MODULATING_H_

/* Others */
#include "utilities.h"

extern float gKi;
extern float gKp;
extern float gU3;
extern float gURef;
extern SemaphoreHandle_t uRefSemaphore;
extern TaskHandle_t printModulationHandle;
extern TaskHandle_t calculateModulationHandle;
extern TaskHandle_t modulationLedHandle;

/* Functions */
void modulating();

#endif /* SRC_MODULATING_H_ */
