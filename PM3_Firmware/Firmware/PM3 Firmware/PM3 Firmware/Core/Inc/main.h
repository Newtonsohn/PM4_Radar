/** ***************************************************************************
 * @file
 * @brief See main.c
 * main.h
 *****************************************************************************/

#ifndef MAIN_H_
#define MAIN_H_


/******************************************************************************
 * Includes
 *****************************************************************************/
#include "stm32f4xx_hal.h"
#include <stdbool.h>
#include "arm_math.h"


void Error_Handler(void);
extern UART_HandleTypeDef huart1;
extern uint32_t ADC_samples[]; // Declare the shared variable
extern float32_t channel1[];
extern float32_t channel2[];
extern uint32_t current_time;
/******************************************************************************
 * Defines
 *****************************************************************************/

/** ***************************************************************************
 * Evalboard revision E (blue PCB)
 * has an inverted y-axis in the touch controller compared to the display.
 * @attention
 * Comment this \#define if you are NOT using evalboard revision E (blue PCB).
 *****************************************************************************/
#define EVAL_REV_E

/** ***************************************************************************
 * Flip LCD = rotate LCD orientation by 180°
 * @attention
 * Comment this \#define if you do NOT want to flip the LCD.
 *****************************************************************************/
//#define FLIPPED_LCD


/******************************************************************************
 * Functions
 *****************************************************************************/


#endif
