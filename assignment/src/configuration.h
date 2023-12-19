/*
 * configuration.h
 *
 *  Created on: 12 Dec 2023
 *      Author: vraun
 */

#ifndef SRC_CONFIGURATION_H_
#define SRC_CONFIGURATION_H_

#include "utilities.h"

extern SemaphoreHandle_t buttonSemaphore;
extern SemaphoreHandle_t modeSemaphore;
extern float gKi;
extern float gKp;

void configuration();
void selectParameter(uint8_t selectedKParameter);

#endif /* SRC_CONFIGURATION_H_ */
