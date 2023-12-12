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


/* Other imports */
#include "utilities.h"
#include "configuration.h"
#include "modulating.h"
#include "idling.h"

/* Global variables */
volatile float gKi = 0;
volatile float gKp = 0;

/* Function declarations */
static void modeSelection();
static void selectModeBasedOnInput(uint8_t modeNumber, uint8_t uartCheck);



/* Semaphores */
SemaphoreHandle_t modeSemaphore = 0;
SemaphoreHandle_t buttonSemaphore = 0;


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


	/*
	 Create the needed tasks to start the program
	*/
	xTaskCreate( modeSelection, "Mode Selection Task", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY+1, NULL );



	/* Attempt to create a semaphore. */
	modeSemaphore = xSemaphoreCreateBinary();
	if( modeSemaphore == NULL )
	{
		xil_printf("Insufficient FreeRTOS heap available for the semaphore to be created successfully.");
	}
	xSemaphoreGive(modeSemaphore);

	buttonSemaphore = xSemaphoreCreateBinary();
	if( buttonSemaphore == NULL )
	{
		xil_printf("Insufficient FreeRTOS heap available for the semaphore to be created successfully.");
	}
	xSemaphoreGive(buttonSemaphore);

	// Start the tasks and timer running.
	vTaskStartScheduler();


	for( ;; );
}



static void selectModeBasedOnInput(uint8_t modeNumber, uint8_t uartCheck)
{
	switch(modeNumber)
		{
			case 1:
				if(xSemaphoreTake(modeSemaphore, portMAX_DELAY))
				{
					if(uartCheck)
					{
						if(xSemaphoreTake(buttonSemaphore, portMAX_DELAY))
						{
							xTaskCreate( configuration, "configuration task", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY+2, NULL );
						}
					}else
					{
						xTaskCreate( configuration, "configuration task", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY+2, NULL );
					}
				}
				break;
			case 2:
				if(xSemaphoreTake(modeSemaphore, portMAX_DELAY))
				{
					xTaskCreate( idling, "configuration task", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY+2, NULL );
				}
				break;
			case 3:
				if(xSemaphoreTake(modeSemaphore, portMAX_DELAY))
				{
					xTaskCreate( modulating, "configuration task", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY+2, NULL );
				}
				break;
			default:
				break;
		}
		vTaskDelay( pdMS_TO_TICKS( 2000 ) );

}

static void modeSelection()
{

	printMenu();
	uint8_t modeNumber = 0;

	for( ;; )
	{
		if(xSemaphoreTake(modeSemaphore, portMAX_DELAY))
		{
			xSemaphoreGive(modeSemaphore);
			char* input;
			input = uartReceiveString(); // polling UART receive buffer

			if(input != 0)
			{
				if ((input[0] >= '1' || '3' <= input[0]) && (strlen(input) < 2))
				{
					int tempInput = input[0] - '0';
					modeNumber = tempInput;
					selectModeBasedOnInput(modeNumber, 1);
				}else
				{
					uartSendString("Invalid input! :/ \n");
				}
			}


			if(AXI_BTN_DATA & 0x01){
				modeNumber++;
				if(modeNumber > 3)
				{
					modeNumber = 1;
				}
				selectModeBasedOnInput(modeNumber, 0);
			}
		}
	}
}

