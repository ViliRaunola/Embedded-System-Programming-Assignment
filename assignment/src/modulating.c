/*
 * modulating.c
 *
 *  Created on: 12 Dec 2023
 *      Author: vraun
 */

#include "modulating.h"

extern float gKi;
extern float gKp;

/* Functions */
static float converterModel(float uIn);
static float controllerPi(float uRef, float uAct, float* pU1);

void modulating()
{
	AXI_LED_DATA = 0b0011;
	xil_printf("Modulating mode selected. Exit by typing 'e' or pressing the 1. button.\n Change the reference voltage from 3rd & 4th button or typing it.\n");

	TickType_t xLastWakeTime;
	const TickType_t xPeriod = pdMS_TO_TICKS( 10 );
	const TickType_t yPeriod = pdMS_TO_TICKS( BUTTON_PRESS_DELAY );
	xLastWakeTime = xTaskGetTickCount();

	float uRef = 2.0;
	float u3 = 0;
	float uIn;
	int i = 0;
	float pU1 = 0;

	uartSendString("uRef: ");
	floatToIntPrint(uRef);
	uartSendString("\n");
	uartSendString("Ki: ");
	floatToIntPrint(gKi);
	uartSendString("\n");
	uartSendString("Kp: ");
	floatToIntPrint(gKp);
	uartSendString("\n");
	vTaskDelayUntil( &xLastWakeTime, yPeriod );

	for(;;)
	{
		char* input;
		/* Checking if user wants to adjust reference voltage from console or exit the mode*/
		input = uartReceiveString(); // polling UART receive buffer
		if(input != 0)
		{
			if((strlen(input) > 0) && isNumber(input))
			{
				uRef = atof(input);
				uartSendString("New uRef: ");
				floatToIntPrint(uRef);
				uartSendString(" (Exit by typing 'e' or pressing the 1. button.)\n");
			}
			else if ((input[0] == 'e') && (strlen(input) < 2))
			{
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
			uRef += REF_VOLT_INCREMENT;
			uartSendString("Increased uRef: ");
			floatToIntPrint(uRef);
			uartSendString("\n");
			vTaskDelayUntil( &xLastWakeTime, yPeriod );
		}
		if(AXI_BTN_DATA & 0b0100)
		{
			uRef -= REF_VOLT_INCREMENT;
			uartSendString("Decreased uRef: ");
			floatToIntPrint(uRef);
			uartSendString("\n");
			vTaskDelayUntil( &xLastWakeTime, yPeriod );
		}

		/* Check for user to close the modulation */
		if(AXI_BTN_DATA & 0b0001)
		{
			uartSendString("Exiting modulation\n\n");
			TTC0_MATCH_0 = TTC0_MATCH_1_COUNTER_2 = TTC0_MATCH_1_COUNTER_3 = 0;
			vTaskDelayUntil( &xLastWakeTime, yPeriod );
			handleTaskExit();
			return;
		}

		/* The controller part PI and the model */
		uIn = controllerPi(uRef, u3, &pU1);
		u3 = converterModel(uIn);


		/* Controlling the Red and Green led brightness.
		 * If the voltage is positive the green led's brightness is adjusted.
		 * For negative values the red led is used.
		 */
		if(u3 >= 0)
		{
			TTC0_MATCH_0 = 0;
			TTC0_MATCH_1_COUNTER_2 = u3;
		}
		else
		{
			TTC0_MATCH_0 = abs(u3);
			TTC0_MATCH_1_COUNTER_2 = 0;
		}

		if(i > 50)
		{
			uartSendString("Output voltage (u3): ");
			floatToIntPrint(u3);
			uartSendString("\n");
			i = 0;
		}
		i++;
		vTaskDelayUntil( &xLastWakeTime, xPeriod );
	}
	handleTaskExit();
	return;
}

/* Controller model of the given circuit in the assignment.
 * Returns the voltage of u3.
 * Heavily inspired by the lecture 7.
 */
static float converterModel(float uIn)
{
	/* Old values */
	static float i1K = 0;
	static float u1K = 0;
	static float i2K = 0;
	static float u2K = 0;
	static float i3K = 0;
	static float u3K = 0;

	/* Measurement */
	static float i1KPlusH = 0;
	static float u1KPlusH = 0;
	static float i2KPlusH = 0;
	static float u2KPlusH = 0;
	static float i3KPlusH = 0;
	static float u3KPlusH = 0;

	i1KPlusH = 0.9652*i1K + (-0.0172)*u1K +  0.0057*i2K + (-0.0058)*u2K + 0.0052*i3K + (-0.0251)*u3K + 0.0471*uIn;
	u1KPlusH = 0.7732*i1K + 0.1252*u1K +  0.2315*i2K + 0.07*u2K + 0.1282*i3K + 0.7754*u3K + 0.0377*uIn;
	i2KPlusH = 0.8278*i1K + (-0.7522)*u1K +  (-0.0956)*i2K + 0.3299*u2K + (-0.4855)*i3K + 0.3915*u3K + 0.0404*uIn;
	u2KPlusH = 0.9948*i1K + 0.2655*u1K +  (-0.3848)*i2K + 0.4212*u2K + 0.3927*i3K + 0.2899*u3K + 0.0485*uIn;
	i3KPlusH = 0.7648*i1K + (-0.4165)*u1K +  (-0.4855)*i2K + (-0.3366)*u2K + (-0.0986)*i3K + 0.7281*u3K + 0.0373*uIn;
	u3KPlusH = 1.1056*i1K + 0.7587*u1K +  0.1179*i2K + 0.0748*u2K + (-0.2192)*i3K + 0.1491*u3K + 0.0539*uIn;

	/* Saving the new values to the previous values */
	i1K = i1KPlusH;
	u1K = u1KPlusH;
	i2K = i2KPlusH;
	u2K = u2KPlusH;
	i3K = i3KPlusH;
	u3K = u3KPlusH;

	return u3KPlusH;
}

/* Reentrant PI-controller with wind-up and saturation.
 *  Heavily inspired by the lecture 7.
 */
static float controllerPi(float uRef, float uAct, float* pU1)
{
	float u1Old;
	float errorNew, uNew, u1New;
	u1Old = *pU1;
	errorNew = uRef - uAct;
	u1New = u1Old + gKi*errorNew;

	/* Anti-windup */
	if (u1New > MAX_INTEGRAL)
	{
		u1New = MAX_INTEGRAL;
	} else if (u1New < -MAX_INTEGRAL) {
		u1New = -MAX_INTEGRAL;
	}

	/* Saturation */
	if(abs(u1New) >= U1MAX)
	{
		u1New = u1Old;
	}
	uNew = gKp*errorNew + u1New;
	u1Old = u1New;
	*pU1 = u1Old;
	return uNew;
}
