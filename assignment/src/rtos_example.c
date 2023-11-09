/*
    FreeRTOS V8.2.1 - Copyright (C) 2015 Real Time Engineers Ltd.
    All rights reserved

    VISIT http://www.FreeRTOS.org TO ENSURE YOU ARE USING THE LATEST VERSION.

    This file is part of the FreeRTOS distribution.

    FreeRTOS is free software; you can redistribute it and/or modify it under
    the terms of the GNU General Public License (version 2) as published by the
    Free Software Foundation >>!AND MODIFIED BY!<< the FreeRTOS exception.

    >>!   NOTE: The modification to the GPL is included to allow you to     !<<
    >>!   distribute a combined work that includes FreeRTOS without being   !<<
    >>!   obliged to provide the source code for proprietary components     !<<
    >>!   outside of the FreeRTOS kernel.                                   !<<

    FreeRTOS is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  Full license text is available on the following
    link: http://www.freertos.org/a00114.html

    1 tab == 4 spaces!

    ***************************************************************************
     *                                                                       *
     *    Having a problem?  Start by reading the FAQ "My application does   *
     *    not run, what could be wrong?".  Have you defined configASSERT()?  *
     *                                                                       *
     *    http://www.FreeRTOS.org/FAQHelp.html                               *
     *                                                                       *
    ***************************************************************************

    ***************************************************************************
     *                                                                       *
     *    FreeRTOS provides completely free yet professionally developed,    *
     *    robust, strictly quality controlled, supported, and cross          *
     *    platform software that is more than just the market leader, it     *
     *    is the industry's de facto standard.                               *
     *                                                                       *
     *    Help yourself get started quickly while simultaneously helping     *
     *    to support the FreeRTOS project by purchasing a FreeRTOS           *
     *    tutorial book, reference manual, or both:                          *
     *    http://www.FreeRTOS.org/Documentation                              *
     *                                                                       *
    ***************************************************************************

    ***************************************************************************
     *                                                                       *
     *   Investing in training allows your team to be as productive as       *
     *   possible as early as possible, lowering your overall development    *
     *   cost, and enabling you to bring a more robust product to market     *
     *   earlier than would otherwise be possible.  Richard Barry is both    *
     *   the architect and key author of FreeRTOS, and so also the world's   *
     *   leading authority on what is the world's most popular real time     *
     *   kernel for deeply embedded MCU designs.  Obtaining your training    *
     *   from Richard ensures your team will gain directly from his in-depth *
     *   product knowledge and years of usage experience.  Contact Real Time *
     *   Engineers Ltd to enquire about the FreeRTOS Masterclass, presented  *
     *   by Richard Barry:  http://www.FreeRTOS.org/contact
     *                                                                       *
    ***************************************************************************

    ***************************************************************************
     *                                                                       *
     *    You are receiving this top quality software for free.  Please play *
     *    fair and reciprocate by reporting any suspected issues and         *
     *    participating in the community forum:                              *
     *    http://www.FreeRTOS.org/support                                    *
     *                                                                       *
     *    Thank you!                                                         *
     *                                                                       *
    ***************************************************************************

    http://www.FreeRTOS.org - Documentation, books, training, latest versions,
    license and Real Time Engineers Ltd. contact details.

    http://www.FreeRTOS.org/plus - A selection of FreeRTOS ecosystem products,
    including FreeRTOS+Trace - an indispensable productivity tool, a DOS
    compatible FAT file system, and our tiny thread aware UDP/IP stack.

    http://www.FreeRTOS.org/labs - Where new FreeRTOS products go to incubate.
    Come and try FreeRTOS+TCP, our new open source TCP/IP stack for FreeRTOS.

    http://www.OpenRTOS.com - Real Time Engineers ltd license FreeRTOS to High
    Integrity Systems ltd. to sell under the OpenRTOS brand.  Low cost OpenRTOS
    licenses offer ticketed support, indemnification and commercial middleware.

    http://www.SafeRTOS.com - High Integrity Systems also provide a safety
    engineered and independently SIL3 certified version for use in safety and
    mission critical applications that require provable dependability.

    1 tab == 4 spaces!

    Modified by lindh LUT
*/

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

/* Xilinx includes. */
#include "xil_printf.h"
#include "xparameters.h"
#include "xil_types.h"

/* LUT includes. */
#include "zynq_registers.h"

static void ledita_1();
static void ledita_2();
static void ledita_3();
static void ledita_4();
static void printtaa_5();

static TaskHandle_t xTask4;
xSemaphoreHandle LEDsem;

int main( void ) {
	  AXI_BTN_TRI |= 0xF; // Set direction for buttons 0..3 ,  0 means output, 1 means input
      AXI_LED_TRI = ~0xF;			// Set direction for bits 0-3 to output for the LEDs


	xil_printf( "Hello from Freertos example main\r\n" );

	/**
	 * Create four tasks t
	 * Each function behaves as if it had full control of the controller.
	 * https://www.freertos.org/a00125.html
	 */
	xTaskCreate( 	ledita_1, 					// The function that implements the task.
					"Led1", 					// Text name for the task, provided to assist debugging only.
					configMINIMAL_STACK_SIZE, 	// The stack allocated to the task.
					NULL, 						// The task parameter is not used, so set to NULL.
					tskIDLE_PRIORITY+1,			// The task runs at the idle priority. Higher number means higher priority.
					NULL );

	xTaskCreate( ledita_2, "Led2", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY+2, NULL );
	xTaskCreate( ledita_3, "Led3", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY+3, NULL );
	xTaskCreate( ledita_4, "Led4", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY+4, NULL );			// Higher priority
	xTaskCreate(printtaa_5,"Printti",configMINIMAL_STACK_SIZE, NULL, configMAX_PRIORITIES-2, NULL );    // Highest of priority

	// LED Sema just as an example

	vSemaphoreCreateBinary(LEDsem);

	// Start the tasks and timer running.
	// https://www.freertos.org/a00132.html
	vTaskStartScheduler();



	/**
	 * If all is well, the scheduler will now be running, and the following line
	 * will never be reached.  If the following line does execute, then there was
	 * insufficient FreeRTOS heap memory available for the idle and/or timer tasks
	 * to be created.  See the memory management section on the FreeRTOS web site
	 * for more details.
	 */
	for( ;; );
}


/* Implement a function in a C file to generate a periodic interrupt at the
required frequency. */
//void vSetupTickInterrupt( void )
//{
/* FreeRTOS_Tick_Handler() is itself defined in the RTOS port layer.  An extern
declaration is required to allow the following code to compile. */
//extern void FreeRTOS_Tick_Handler( void );

    /* Assume TIMER1_configure() configures a hypothetical timer peripheral called
    TIMER1 to generate a periodic interrupt with a frequency set by its parameter. */
  //  TIMER1_configure( 10);

    /* Next assume Install_Interrupt() installs the function passed as its second
    parameter as the handler for the peripheral passed as its first parameter. */
    //Install_Interrupt( TIMER1, FreeRTOS_Tick_Handler );
//}



static void ledita_1() {

	// ms -> tics, only if tick interval is more than ms, give directly as ticks
	const TickType_t delay = pdMS_TO_TICKS( 1000 );




	// Necessary forever loop. A thread should never be able to exit!
	for( ;; ) { // Same as while(1) or while(true)

		AXI_LED_DATA =AXI_LED_DATA ^ 0x01;
		// RTOS specific delay. Normal delay would halt the system for the duration => Is not multitasking!
		// https://www.freertos.org/a00127.html

		vTaskDelay( delay );  // not synchronous , do not use in control or in real time models
	}
}

static void ledita_2() {
	const TickType_t freq = pdMS_TO_TICKS( 750 );
	TickType_t wakeTime;
	uint16_t looppi;

	/* If juu need floating point context, and in conf it
	 *  is not set for all #define configUSE_TASK_FPU_SUPPORT 2
	 *  use the following */

	// portTASK_USES_FLOATING_POINT();

	// https://www.freertos.org/a00021.html#xTaskGetTickCount
	wakeTime = xTaskGetTickCount();  // only once initialized

	for( ;; ) {
		AXI_LED_DATA =AXI_LED_DATA ^ 0x02;
		while (AXI_BTN_DATA & 0x01) {
						  //AXI_LED_DATA =AXI_LED_DATA & 0x01;
						looppi++;
						}


		// https://www.freertos.org/vtaskdelayuntil.html
		vTaskDelayUntil( &wakeTime, freq );

	}
}

static void ledita_3() {
	const TickType_t freq = pdMS_TO_TICKS( 500 );
	TickType_t wakeTime;



	wakeTime = xTaskGetTickCount();
	for( ;; ) {

		xSemaphoreTake(LEDsem,portMAX_DELAY);
		AXI_LED_DATA =AXI_LED_DATA ^ 0x04;
		xSemaphoreGive(LEDsem);

		vTaskDelayUntil( &wakeTime, freq );
	}
}

static void ledita_4() {
	TickType_t wakeTime;



	wakeTime = xTaskGetTickCount();
	for( ;; ) {

		// Semaphore is just as an example here

		xSemaphoreTake(LEDsem,portMAX_DELAY);
		AXI_LED_DATA =AXI_LED_DATA ^ 0x08;
		xSemaphoreGive(LEDsem);

		uint8_t input = AXI_LED_DATA & 0xF;  // Range: 0..15
		uint16_t delay = 1000 - 60*input; // Range: 100-1000ms

		TickType_t freq = pdMS_TO_TICKS( delay );

		vTaskDelayUntil( &wakeTime, freq );
		//vTaskDelayUntil( &wakeTime, 200 );
	}
}

static void printtaa_5() {
	static uint8_t s;
	const TickType_t delay = pdMS_TO_TICKS( 1000 );
	for (;;) {
		xil_printf("Elapsed: %ds\n", s++);
		vTaskDelay(delay);  // not strictly synchronous
	}
}
