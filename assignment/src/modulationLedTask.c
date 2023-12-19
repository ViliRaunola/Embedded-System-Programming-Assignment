/*
 * modulationLedTask.c
 *
 *  Created on: 18 Dec 2023
 *      Author: jesse
 */

#include "modulationLedTask.h"

void modulationLed()
{
    /* Controlling the Red and Green led brightness.
    * If the voltage is positive the green led's brightness is adjusted.
    * For negative values the red led is used.
    */
    TickType_t xLastWakeTime;
    const TickType_t xPeriod = pdMS_TO_TICKS( CALCULATION_INTERVAL_MS );
	xLastWakeTime = xTaskGetTickCount();

	for(;;)
	{
		if(xSemaphoreTake(u3Semaphore, portMAX_DELAY))
		{
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
			xSemaphoreGive(u3Semaphore);
			vTaskDelayUntil( &xLastWakeTime, xPeriod );
		}
	}


}
