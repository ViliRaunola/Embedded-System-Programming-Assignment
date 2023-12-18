/*
 * modulationPrint.c
 *
 *  Created on: 18 Dec 2023
 *      Author: vraun
 */

#include "modulationPrintTask.h"


void printModulationValues()
{
	float tempU3 = 0;
	for(;;)
	{
		if(xSemaphoreTake(u3Semaphore, portMAX_DELAY))
		{
			tempU3 = gU3;
			xSemaphoreGive(u3Semaphore);
		}
		uartSendString("Output voltage (u3): ");
		floatToIntPrint(tempU3);
		uartSendString("\n");
		vTaskDelay( pdMS_TO_TICKS( 500 ) );
	}

}
