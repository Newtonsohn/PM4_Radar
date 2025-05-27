/** ***************************************************************************
 * @file
 * @brief See main.c
 *
 *****************************************************************************/

#ifndef MAIN_H_
#define MAIN_H_


/******************************************************************************
 * Includes
 *****************************************************************************/


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
void MX_GPIO_Init(void);
void MX_TIM3_Init(void);
void PWM_Start(void);
void PWM_Stop(void);
void PWM_SetFrequency(uint32_t frequency_hz);
char* change_pin(void);
void GPIO_PG13_Output_Init(void);
/** ***************************************************************************
 * Flip LCD = rotate LCD orientation by 180°
 * @attention
 * Comment this \#define if you do NOT want to flip the LCD.
 *****************************************************************************/
#define FLIPPED_LCD


/******************************************************************************
 * Functions
 *****************************************************************************/


#endif
