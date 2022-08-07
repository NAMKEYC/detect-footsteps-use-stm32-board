/*

 * L3GD20.c
 *
 *  Created on: August 01, 2022
 *      Author: Nam Nguyen

*/
/* Includes ----------------------------------------------------------------- */
#include <l3gd20_process.h>
#include "stdint.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_spi.h"

/* Private variables ---------------------------------------------------------*/
/**
 * @brief   L3GD20 data ready flag
 */
static L3GD20_datareadyflagtype_t datareadyflag = L3GD20_DATA_READY;
/**
 * @brief   L3GD20 calibration state definition
 */
static L3GD20_calistatetype_t currentcalistate = L3GD20_COLLECT_CALIBRATION_SAMPLES;
/**
 * @brief   L3GD20 current state
 */
static L3GD20_statetype_t currentstate = L3GD20_FIRST;

/**
 * @brief   Calculated angle rate values in degrees/s
 */
/**
 * @brief   Offset values
 */
static int32_t offset_x = 0;
static int32_t offset_y = 0;
static int32_t offset_z = 0;
/**
 * @brief   Noise values
 */
static float noise_x = 0;
static float noise_y = 0;
static float noise_z = 0;
/**
 * @brief   Calculated angle values in degrees
 */
/**
 * @brief   Last calculated angle rate values in degrees/s
 */
// static float lastanglerate_x = 0;
// static float lastanglerate_y = 0;
// static float lastanglerate_z = 0;
/**
 * @brief   Temp variables for collecting noise values
 */
static int32_t tempnoise_x = 0;
static int32_t tempnoise_y = 0;
static int32_t tempnoise_z = 0;
/**
 * @brief   Calibration Counter
 */
volatile static uint32_t calicounter = 0;

/**
 * @brief   Calibration buffers
 */
static int16_t calibrationbuffer_x[CALIBRATION_BUFFER_LENGTH];
static int16_t calibrationbuffer_y[CALIBRATION_BUFFER_LENGTH];
static int16_t calibrationbuffer_z[CALIBRATION_BUFFER_LENGTH];

/* APIs implementations ----------------------------------------------------- */

void l3gd20_main(ang_device_t *dev, spi_cs_t *cs)
{
	/* Raw angular rate data */
    volatile int16_t raw_x = 0;
	volatile int16_t raw_y = 0;
	volatile int16_t raw_z = 0;
	/* Variable for time difference calculations */
	/* Variables for moving average calculation */
	int16_t averageWindow_X[AVERAGE_WINDOW_SIZE] = {0};
	int16_t averageWindow_Y[AVERAGE_WINDOW_SIZE] = {0};
	int16_t averageWindow_Z[AVERAGE_WINDOW_SIZE] = {0};

	uint32_t windowPosition = 0;
	int32_t tempSum_X = 0;
	int32_t tempSum_Y = 0;
	int32_t tempSum_Z = 0;
	/* Check current state */
	switch (currentstate)
	{
	//---------------------------------------------------------------------------// data
	case (L3GD20_FIRST):
	{
		/* Wait for data ready flag to be set before */
		if (datareadyflag == L3GD20_DATA_READY)
		{
			/* Read output registers */
			l3gd20_read(cs, L3GD20_OUT_X_L_ADDR, 1);
			l3gd20_read(cs, L3GD20_OUT_X_H_ADDR, 2);
			l3gd20_read(cs, L3GD20_OUT_Y_L_ADDR, 3);
			l3gd20_read(cs, L3GD20_OUT_Y_H_ADDR, 4);
			l3gd20_read(cs, L3GD20_OUT_Z_L_ADDR, 5);
			l3gd20_read(cs, L3GD20_OUT_Z_H_ADDR, 6);
			/* Change current state */
			currentstate = L3GD20_SECOND;
			/* Clear data ready flag */
			datareadyflag = L3GD20_DATA_NOT_READY;
		}
		break;
	}
		//-----------------------------------------------------------------------------------// varibla
	case (L3GD20_SECOND):
	{
		/* Read buffer data */
		raw_x = (cs->spirxbuf[2] << 8) | cs->spirxbuf[1];
		raw_y = (cs->spirxbuf[4] << 8) | cs->spirxbuf[3];
		raw_z = (cs->spirxbuf[6] << 8) | cs->spirxbuf[5];
		switch (currentcalistate)
		{
		//---------------------------------------------------------------------------------------------------------
		case (L3GD20_COLLECT_CALIBRATION_SAMPLES):
		{
			/* Fill calibration buffer */
			calibrationbuffer_x[calicounter] = raw_x;
			calibrationbuffer_y[calicounter] = raw_y;
			calibrationbuffer_z[calicounter] = raw_z;
			/* Increment calibration counter */
			calicounter++;
			/* Check calibration counter */
			if (calicounter >= CALIBRATION_BUFFER_LENGTH)
			{
				/* Finish sampling */
				calicounter = 0;
				/* Change current calibration state */
				currentcalistate = L3GD20_PROCESS_CALIBRATION_SAMPLES;
			}
			break;
		}
		case (L3GD20_PROCESS_CALIBRATION_SAMPLES):
		{
			/* Calculate offset using moving average */
			for (uint32_t idx = 0; idx < CALIBRATION_BUFFER_LENGTH; idx++)
			{
				/* Subtract the oldest sample from the old sum, add the new number */
				tempSum_X = tempSum_X - averageWindow_X[windowPosition] + calibrationbuffer_x[idx];
				tempSum_Y = tempSum_Y - averageWindow_Y[windowPosition] + calibrationbuffer_y[idx];
				tempSum_Z = tempSum_Z - averageWindow_Z[windowPosition] + calibrationbuffer_z[idx];
				/* Store current sample in the average window */
				averageWindow_X[windowPosition] = calibrationbuffer_x[idx];
				averageWindow_Y[windowPosition] = calibrationbuffer_y[idx];
				averageWindow_Z[windowPosition] = calibrationbuffer_z[idx];
				/* Calculate the offset */
				offset_x = tempSum_X / (int32_t)AVERAGE_WINDOW_SIZE;
				offset_y = tempSum_Y / (int32_t)AVERAGE_WINDOW_SIZE;
				offset_z = tempSum_Z / (int32_t)AVERAGE_WINDOW_SIZE;
				/* Increment window position */
				windowPosition++;
				/* Check last window position */
				if (windowPosition >= AVERAGE_WINDOW_SIZE)
				{
					/* Reset window position */
					windowPosition = 0;
				}
			}
			/* Calculate noise levels */
			for (uint32_t idx = 0; idx < CALIBRATION_BUFFER_LENGTH; idx++)
			{
				/* Calculate X noise values */
				if (((int32_t)calibrationbuffer_x[idx] - offset_x) > tempnoise_x)
				{
					tempnoise_x = (int32_t)calibrationbuffer_x[idx] - offset_x;
				}
				else if (((int32_t)calibrationbuffer_x[idx] - offset_x) < -tempnoise_x)
				{
					tempnoise_x = -((int32_t)calibrationbuffer_x[idx] - offset_x);
				}
				/* Calculate Y noise values */
				if (((int32_t)calibrationbuffer_y[idx] - offset_y) > tempnoise_y)
				{
					tempnoise_y = (int32_t)calibrationbuffer_y[idx] - offset_y;
				}
				else if (((int32_t)calibrationbuffer_y[idx] - offset_y) < -tempnoise_y)
				{
					tempnoise_y = -((int32_t)calibrationbuffer_y[idx] - offset_y);
				}
				/* Calculate Z noise values */
				if (((int32_t)calibrationbuffer_z[idx] - offset_z) > tempnoise_z)
				{
					tempnoise_z = (int32_t)calibrationbuffer_z[idx] - offset_z;
				}
				else if (((int32_t)calibrationbuffer_z[idx] - offset_z) < -tempnoise_z)
				{
					tempnoise_z = -((int32_t)calibrationbuffer_z[idx] - offset_z);
				}
			}
			/* Get noise values in degree/s */
			noise_x = (float)tempnoise_x * L3GD20_SENSITIVITY;
			noise_y = (float)tempnoise_y * L3GD20_SENSITIVITY;
			noise_z = (float)tempnoise_z * L3GD20_SENSITIVITY;
			/* Change current calibration state */
			currentcalistate = L3GD20_CALIBRATED;
			break;
		}
		case (L3GD20_CALIBRATED):
		{
			/* Get angle rate values in degrees/s */
			dev->anglerate_x = (float)(raw_x - (offset_x)) * L3GD20_SENSITIVITY;
			dev->anglerate_y = (float)(raw_y - (offset_y)) * L3GD20_SENSITIVITY;
			dev->anglerate_z = (float)(raw_z - (offset_z)) * L3GD20_SENSITIVITY;
		}
		default:
		{
			break;
		}
		}
		/* Change current state */
		currentstate = L3GD20_FIRST;
		/*turn on data ready flag*/
		datareadyflag = L3GD20_DATA_READY;
		break;
	}
	default:
		break;
	}
}
/* End files ----------------------------------------------------------------- */
