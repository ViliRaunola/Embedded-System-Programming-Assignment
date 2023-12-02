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
#include <xuartps_hw.h>

/* Other imports */
#include <stdlib.h>

static void modeSelection();
static void testTask();
static void printMenu();
static char uart_receive();

xSemaphoreHandle LEDsem;


int main( void ) {
	AXI_BTN_TRI |= 0xF; // Set direction for buttons 0..3 ,  0 means output, 1 means input
    AXI_LED_TRI = ~0xF; // Set direction for bits 0-3 to output for the LEDs


    // Ex4 from the course on how to initialize the uart.
    uint32_t r = 0; // Temporary value variable
    r = UART_CTRL;
	r &= ~(XUARTPS_CR_TX_EN | XUARTPS_CR_RX_EN); // Clear Tx & Rx Enable
	r |= XUARTPS_CR_RX_DIS | XUARTPS_CR_TX_DIS; // Tx & Rx Disable
	UART_CTRL = r;
	UART_MODE = 0;
	UART_MODE &= ~XUARTPS_MR_CLKSEL; // Clear "Input clock selection" - 0: clock source is uart_ref_clk
	UART_MODE |= XUARTPS_MR_CHARLEN_8_BIT; // Set "8 bits data"
	UART_MODE |= XUARTPS_MR_PARITY_NONE; // Set "No parity mode"
	UART_MODE |= XUARTPS_MR_STOPMODE_1_BIT; // Set "1 stop bit"
	UART_MODE |= XUARTPS_MR_CHMODE_NORM; // Set "Normal mode"
	// baud_rate = sel_clk / (CD * (BDIV + 1) (ref: UG585 - TRM - UART)
	UART_BAUD_DIV = 6; // ("BDIV")
	UART_BAUD_GEN = 124; // ("CD")
	// Baud Rate = 100Mhz / (124 * (6 + 1)) = 115200 bps
	UART_CTRL |= (XUARTPS_CR_TXRST | XUARTPS_CR_RXRST); // TX & RX logic reset
	r = UART_CTRL;
	r |= XUARTPS_CR_RX_EN | XUARTPS_CR_TX_EN; // Set TX & RX enabled
	r &= ~(XUARTPS_CR_RX_DIS | XUARTPS_CR_TX_DIS); // Clear TX & RX disabled
	UART_CTRL = r;


	xil_printf( "Hello from Freertos example main\r\n" );

	/**
	 * Create four tasks t
	 * Each function behaves as if it had full control of the controller.
	 * https://www.freertos.org/a00125.html
	 */
	xTaskCreate( 	modeSelection, 					// The function that implements the task.
					"Mode Selection Task", 					// Text name for the task, provided to assist debugging only.
					configMINIMAL_STACK_SIZE, 	// The stack allocated to the task.
					NULL, 						// The task parameter is not used, so set to NULL.
					tskIDLE_PRIORITY+1,			// The task runs at the idle priority. Higher number means higher priority.
					NULL );

	xTaskCreate( 	testTask, "test task", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY+1, NULL );


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

static void printMenu() {
	xil_printf("Select a mode by pressing the 1. button\n1st mode is the configuration\n2. mode is the idling\n3. mode is the modulating\n\n");
}

static void modeSelection() {

	printMenu();
	uint8_t modeNumber = 0;

	for( ;; )
	{
		// delay loop
		for(uint32_t i = 1; i < (1 << 20); i++){
			 char input = uart_receive(); // polling UART receive buffer
			 if(input)
			 {
				int tempInput = input - '0';
				if (input >= '1' || '3' <= input)
				{
					modeNumber = tempInput;
					switch(modeNumber)
					{
						case 1:
							AXI_LED_DATA = 0b0001;
							xil_printf("Mode '%d. configuration' selected\n", modeNumber);
							break;
						case 2:
							AXI_LED_DATA = 0b0011;
							xil_printf("Mode '%d. idle' selected\n", modeNumber);
							break;
						case 3:
							AXI_LED_DATA = 0b0111;
							xil_printf("Mode '%d. modulating' selected\n", modeNumber);
							break;
						default:
							break;
					}
					vTaskDelay( pdMS_TO_TICKS( 2000 ) );
					AXI_LED_DATA = 0b0000;
				}
			 }
		 }

		if(AXI_BTN_DATA & 0x01){
			modeNumber++;
			if(modeNumber > 3)
			{
				modeNumber = 1;
			}


			switch(modeNumber)
			{
				case 1:
					AXI_LED_DATA = 0b0001;
					xil_printf("Mode '%d. configuration' selected\n", modeNumber);
					break;
				case 2:
					AXI_LED_DATA = 0b0011;
					xil_printf("Mode '%d. idle' selected\n", modeNumber);
					break;
				case 3:
					AXI_LED_DATA = 0b0111;
					xil_printf("Mode '%d. modulating' selected\n", modeNumber);
					break;
				default:
					break;
			}
			vTaskDelay( pdMS_TO_TICKS( 2000 ) );
			AXI_LED_DATA = 0b0000;
		}

	}
}

// Check if UART receive FIFO is not empty and return the new data
char uart_receive() {
 if ((UART_STATUS & XUARTPS_SR_RXEMPTY) == XUARTPS_SR_RXEMPTY) return 0;
 return UART_FIFO;
}



static void testTask() {
	const TickType_t freq = pdMS_TO_TICKS( 500 );
	TickType_t wakeTime;



	wakeTime = xTaskGetTickCount();
	for( ;; ) {
		//xil_printf("test task\n");
		vTaskDelayUntil( &wakeTime, freq );
	}
}

