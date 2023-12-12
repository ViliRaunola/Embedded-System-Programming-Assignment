/*
 * configuration.c
 *
 *  Created on: 12 Dec 2023
 *      Author: vraun
 */

#include "utilities.h"

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

/* LUT includes. */
#include "zynq_registers.h"
#include <xuartps_hw.h>

#include "configuration.h"

extern SemaphoreHandle_t buttonSemaphore;
extern SemaphoreHandle_t modeSemaphore;
extern float gKi;
extern float gKp;


void configuration()
{
	AXI_LED_DATA = 0b0001;
	if(uxSemaphoreGetCount(buttonSemaphore)) {
		xil_printf("Configuration Mode selected through CLI. Buttons are disabled.\n");
		xil_printf("Type 'i' to change the Ki parameter (Default)\nType 'p' to change the Kp parameter.\n Type 'e' to EXIT the mode\n\n");
		xil_printf("Ki selected\n");
		xil_printf("Type a value for the Ki parameter");
	} else {
		xil_printf("Configuration Mode selected\n");
		xil_printf("Press the 2. button to select between Ki, Kp and EXIT. Default parameter is Ki.\nPress the button once to change to Kp parameter and two times to EXIT the mode.\n");
		xil_printf("Type 'i' to change the Ki parameter.\nType 'p' to change the Kp parameter.\n Type 'e' to EXIT the mode\n\n");
		xil_printf("Ki selected\n");
		xil_printf("Press the 3. button to decrease or the 4. button to increase the Ki parameter value.\n");
		xil_printf("Type a value for the Ki parameter: \n");
	}

	uint8_t selectedKParameter = 1;
	for( ;; )
	{
		char* input;
		input = uartReceiveString(); // polling UART receive buffer
		if(input != 0)
		{
			if ((input[0] == 'i' || input[0] == 'p' || input[0] == 'e') && (strlen(input) < 2))
			{
				if(input[0] == 'i')
				{
					selectedKParameter = 1;
					xil_printf("Ki selected\n");
					if(uxSemaphoreGetCount(buttonSemaphore)) {
						xil_printf("Type a value for the parameter: ");
					} else {
						xil_printf("Press the 3. button to decrease or the 4. button to increase the Ki parameter value.\n");
						xil_printf("Type a value for the Ki parameter: \n");
					}
				}else if (input[0] == 'p')
				{
					selectedKParameter = 2;
					xil_printf("Kp selected\n");
						if(uxSemaphoreGetCount(buttonSemaphore)) {
						xil_printf("Type a value for the Kp parameter: ");
					} else {
						xil_printf("Press the 3. button to decrease or the 4. button to increase the Kp parameter value.\n");
						xil_printf("Type a value for the parameter: \n");
					}
				} else if (input[0] == 'e'){
					uartSendString("Exiting Configuration Mode.\nParameters set as:\n Ki: ");
					floatToIntPrint(gKi);
					uartSendString("\nKp: ");
					floatToIntPrint(gKp);
					uartSendString("\n\n");

					xSemaphoreGive(buttonSemaphore);
					handleTaskExit();
				} else {
					xil_printf("Invalid input! Type 'i' to change the Ki parameter.\nType 'p' to change the Kp parameter.\n Type 'e' to EXIT the mode\n\n")
				}
			}

			float fuserInput = atof(input);
			/* Set the user input as a float to the selected K-value */
			if(selectedKParameter == 1  && (strlen(input) > 0) && fuserInput != 0)
			{
				gKi = fuserInput;
				uartSendString("Set Ki as: ");
				floatToIntPrint(gKi);
				uartSendString("\n");
			}if(selectedKParameter == 2  && (strlen(input) > 0) && fuserInput != 0)
			{
				gKp = fuserInput;
				uartSendString("Set Kp as: ");
				floatToIntPrint(gKp);
				uartSendString("\n");
			}
			/* Special case if the user input for K-value is 0 */
			if(selectedKParameter == 1  && (strlen(input) > 0) && input[0] == '0')
			{
				gKi = 0;
				uartSendString("Set Ki as: ");
				floatToIntPrint(gKi);
				uartSendString("\n");
			}if(selectedKParameter == 2  && (strlen(input) > 0) && input[0] == '0')
			{
				gKp = 0;
				uartSendString("set Kp as: ");
				floatToIntPrint(gKp);
				uartSendString("\n");
			}
		}


		if((AXI_BTN_DATA & 0b0010) && uxSemaphoreGetCount(buttonSemaphore))
		{
			selectedKParameter++;
			if(selectedKParameter > 2)
			{
				uartSendString("Exiting Configuration Mode.\nParameters set as:\n Ki: ");
				floatToIntPrint(gKi);
				uartSendString("\nKp: ");
				floatToIntPrint(gKp);
				uartSendString("\n\n");
				handleTaskExit();
			}
			if(selectedKParameter == 1)
			{
				xil_printf("Ki selected\n");
				if(uxSemaphoreGetCount(buttonSemaphore)) {
					xil_printf("Type a value for the parameter: ");
				} else {
					xil_printf("Press the 3. button to decrease or the 4. button to increase the Ki parameter value.\n");
					xil_printf("Type a value for the Ki parameter: \n");
				}
			} else
			{
				xil_printf("Kp selected\n");
					if(uxSemaphoreGetCount(buttonSemaphore)) {
					xil_printf("Type a value for the Kp parameter: ");
				} else {
					xil_printf("Press the 3. button to decrease or the 4. button to increase the Kp parameter value.\n");
					xil_printf("Type a value for the parameter: \n");
				}
			}
			TickType_t xLastWakeTime;
			const TickType_t xPeriod = pdMS_TO_TICKS( BUTTON_PRESS_DELAY );
			xLastWakeTime = xTaskGetTickCount();
			vTaskDelayUntil( &xLastWakeTime, xPeriod );
		}

		if(uxSemaphoreGetCount(buttonSemaphore))
		{
			selectParameter(selectedKParameter);
		}
	}

}

void selectParameter(uint8_t selectedKParameter)
{
	TickType_t xLastWakeTime;
	const TickType_t xPeriod = pdMS_TO_TICKS( BUTTON_PRESS_DELAY );
	xLastWakeTime = xTaskGetTickCount();
	switch(selectedKParameter)
	{
		case 1:
			if(AXI_BTN_DATA & 0b1000)
			{
				gKi += INCREMENT_AMOUNT;
				uartSendString("increased Ki: ");
				floatToIntPrint(gKi);
				uartSendString("\n");
				vTaskDelayUntil( &xLastWakeTime, xPeriod );
			}
			if(AXI_BTN_DATA & 0b0100)
			{
				gKi -= INCREMENT_AMOUNT;
				uartSendString("Decreased Ki: ");
				floatToIntPrint(gKi);
				uartSendString("\n");
				vTaskDelayUntil( &xLastWakeTime, xPeriod );
			}
			break;
		case 2:
			if(AXI_BTN_DATA & 0b1000)
			{
				gKp += INCREMENT_AMOUNT;
				uartSendString("increased Kp: ");
				floatToIntPrint(gKp);
				uartSendString("\n");
				vTaskDelayUntil( &xLastWakeTime, xPeriod );
			}
			if(AXI_BTN_DATA & 0b0100)
			{
				gKp -= INCREMENT_AMOUNT;
				uartSendString("Decreased Kp: ");
				floatToIntPrint(gKp);
				uartSendString("\n");
				vTaskDelayUntil( &xLastWakeTime, xPeriod );
			}
			break;
		default:
			break;
	}
}
