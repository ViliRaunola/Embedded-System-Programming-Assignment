/*
 * modulationCalculationTask.h
 *
 *  Created on: 18 Dec 2023
 *      Author: vraun
 */

#ifndef SRC_MODULATIONCALCULATIONTASK_H_
#define SRC_MODULATIONCALCULATIONTASK_H_

#include "utilities.h"

extern float gKi;
extern float gKp;
extern float gU3;
extern float gURef;
extern SemaphoreHandle_t u3Semaphore;
extern SemaphoreHandle_t uRefSemaphore;

void calculateModulation();


#endif /* SRC_MODULATIONCALCULATIONTASK_H_ */
