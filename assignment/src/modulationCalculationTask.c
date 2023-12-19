/*
 * modulationTask.c
 *
 *  Created on: 18 Dec 2023
 *      Author: vraun
 */

#include "modulationCalculationTask.h"

/* Functions */
static float converterModel(float uIn);
static float controllerPi(float uRef, float uAct, float* pU1);

void calculateModulation()
{
	float uIn;
	float pU1 = 0;
	TickType_t xLastWakeTime;
	const TickType_t xPeriod = pdMS_TO_TICKS( CALCULATION_INTERVAL_MS );
	xLastWakeTime = xTaskGetTickCount();

	for(;;)
	{
		if(xSemaphoreTake(u3Semaphore, portMAX_DELAY) && xSemaphoreTake(uRefSemaphore, portMAX_DELAY))
		{
			/* The controller part PI and the model */
			uIn = controllerPi(gURef, gU3, &pU1);
			gU3 = converterModel(uIn);

			xSemaphoreGive(u3Semaphore);
			xSemaphoreGive(uRefSemaphore);
			vTaskDelayUntil( &xLastWakeTime, xPeriod );
		}
	}
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

	/* Integral */
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

	/* Error */
	uNew = gKp*errorNew + u1New;

	u1Old = u1New;
	*pU1 = u1Old;
	return uNew;
}
