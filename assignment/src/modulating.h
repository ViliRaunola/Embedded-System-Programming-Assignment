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

void modulating();
static float converterModel(float uIn);
static float controllerPi(float uRef, float uAct, float* pU1);

#endif /* SRC_MODULATING_H_ */
