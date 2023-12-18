/*
 * modulating.c
 *
 *  Created on: 12 Dec 2023
 *      Author: vraun
 */

#include "modulating.h"


void modulating()
{
	AXI_LED_DATA = 0b0011;
	xil_printf("Modulating mode selected. Exit by typing 'e' or pressing the 1. button.\n Change the reference voltage from 3rd & 4th button or typing it.\n");

	TickType_t xLastWakeTime;
	const TickType_t xPeriod = pdMS_TO_TICKS( 10 );
	const TickType_t yPeriod = pdMS_TO_TICKS( BUTTON_PRESS_DELAY );
	xLastWakeTime = xTaskGetTickCount();

	if(xSemaphoreTake(uRefSemaphore, portMAX_DELAY))
	{
		uartSendString("uRef: ");
		floatToIntPrint(gURef);
		xSemaphoreGive(uRefSemaphore);
	}

	uartSendString("\n");
	uartSendString("Ki: ");
	floatToIntPrint(gKi);
	uartSendString("\n");
	uartSendString("Kp: ");
	floatToIntPrint(gKp);
	uartSendString("\n");
	vTaskDelayUntil( &xLastWakeTime, yPeriod );

	// Resuming the printing task
	vTaskResume(calculateModulationHandle);
	vTaskResume(printModulationHandle);

	for(;;)
	{
		char* input;
		/* Checking if user wants to adjust reference voltage from console or exit the mode*/
		input = uartReceiveString(); // polling UART receive buffer
		if(input != 0)
		{
			if((strlen(input) > 0) && isNumber(input))
			{
				if(xSemaphoreTake(uRefSemaphore, portMAX_DELAY))
				{
					gURef = atof(input);
					uartSendString("New uRef: ");
					floatToIntPrint(gURef);
					xSemaphoreGive(uRefSemaphore);
				}
				uartSendString(" (Exit by typing 'e' or pressing the 1. button.)\n");
			}
			else if ((input[0] == 'e') && (strlen(input) < 2))
			{
				vTaskSuspend( calculateModulationHandle );
				vTaskSuspend( printModulationHandle );
				uartSendString("Exiting modulation\n\n");
				TTC0_MATCH_0 = TTC0_MATCH_1_COUNTER_2 = TTC0_MATCH_1_COUNTER_3 = 0;
				handleTaskExit();
				return;
			}
			else
			{
				uartSendString("Invalid input! Only numbers allowed. Exit by typing 'e' or pressing the 1. button.\n\n");
			}
		}

		/* Checking if user wants to adjust reference voltage from buttons */
		if(AXI_BTN_DATA & 0b1000)
		{
			if(xSemaphoreTake(uRefSemaphore, portMAX_DELAY))
			{
				gURef += REF_VOLT_INCREMENT;
				uartSendString("Increased uRef: ");
				floatToIntPrint(gURef);
				xSemaphoreGive(uRefSemaphore);
			}
			uartSendString("\n");
			vTaskDelayUntil( &xLastWakeTime, yPeriod );
		}
		if(AXI_BTN_DATA & 0b0100)
		{
			if(xSemaphoreTake(uRefSemaphore, portMAX_DELAY))
			{
				gURef -= REF_VOLT_INCREMENT;
				uartSendString("Decreased uRef: ");
				floatToIntPrint(gURef);
				xSemaphoreGive(uRefSemaphore);
			}
			uartSendString("\n");
			vTaskDelayUntil( &xLastWakeTime, yPeriod );
		}

		/* Check for user to close the modulation */
		if(AXI_BTN_DATA & 0b0001)
		{
			vTaskSuspend( calculateModulationHandle );
			vTaskSuspend( printModulationHandle );
			uartSendString("Exiting modulation\n\n");
			TTC0_MATCH_0 = TTC0_MATCH_1_COUNTER_2 = TTC0_MATCH_1_COUNTER_3 = 0;
			vTaskDelayUntil( &xLastWakeTime, yPeriod );
			handleTaskExit();
			return;
		}





		/* Controlling the Red and Green led brightness.
		 * If the voltage is positive the green led's brightness is adjusted.
		 * For negative values the red led is used.
		 */
		// TODO: make own task
		if(gU3 >= 0)
		{
			TTC0_MATCH_0 = 0;
			TTC0_MATCH_1_COUNTER_2 = gU3;
		}
		else
		{
			TTC0_MATCH_0 = abs(gU3);
			TTC0_MATCH_1_COUNTER_2 = 0;
		}

		vTaskDelayUntil( &xLastWakeTime, xPeriod );
	}
	handleTaskExit();
	return;
}

