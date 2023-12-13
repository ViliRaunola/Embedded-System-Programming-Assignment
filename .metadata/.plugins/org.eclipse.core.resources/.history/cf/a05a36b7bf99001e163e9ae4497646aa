/*
 * idling.c
 *
 *  Created on: 12 Dec 2023
 *      Author: vraun
 */

#include "idling.h"

void idling()
{
	AXI_LED_DATA = 0b0011;
	xil_printf("In idling task\n");
	vTaskDelay( pdMS_TO_TICKS( 1000 ) );

	handleTaskExit();
}
