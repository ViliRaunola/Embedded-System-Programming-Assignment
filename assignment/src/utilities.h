/*
 * utilities.h
 *
 *  Created on: 12 Dec 2023
 *      Author: vraun
 */

#ifndef SRC_UTILITIES_H_
#define SRC_UTILITIES_H_

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

/* Xilinx includes. */
#include "xil_printf.h"
#include "xparameters.h"
#include "xil_types.h"
#include <xuartps.h>

/* LUT includes. */
#include "zynq_registers.h"
#include <xuartps_hw.h>
#include <xttcps.h>
#include <zynq_registers.h>

/* Standard imports */
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

/* Definitions/ macros */
#define INCREMENT_AMOUNT 0.001
#define BUTTON_PRESS_DELAY 1000
#define BUFFER_SIZE 40
#define U1MAX 1000
#define MAX_INTEGRAL 1000
#define REF_VOLT_INCREMENT 0.1

extern SemaphoreHandle_t modeSemaphore;
extern float gKi;
extern float gKp;

/* Functions */
void printMenu();
void uart_send(char c);
void uartSendString(char str[BUFFER_SIZE]);
char uart_receive();
void handleTaskExit();
void floatToIntPrint(float fval);
char* uartReceiveString();
int isNumber(char* input);

#endif /* SRC_UTILITIES_H_ */
