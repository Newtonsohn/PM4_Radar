/** ***************************************************************************
 * @file
 * @brief See measuring.c
 *
 * Prefixes MEAS, ADC, DAC
 *
 *****************************************************************************/

#ifndef MEAS_H_
#define MEAS_H_


/******************************************************************************
 * Includes
 *****************************************************************************/
#include <stdbool.h>
#include "menu.h"  // Header wo MENU_get_transition deklariert ist
#include "arm_math.h"
#include "arm_const_structs.h"


/******************************************************************************
 * Defines
 *****************************************************************************/
extern bool MEAS_data_ready;
extern uint32_t MEAS_input_count;
extern bool DAC_active;


//extern float32_t fft_input[FFT_SIZE*2];
//extern float32_t fft_output[FFT_SIZE];
//extern uint32_t fft_mag[FFT_SIZE/2];

/******************************************************************************
 * Functions
 *****************************************************************************/
void MEAS_GPIO_analog_init(void);
void MEAS_timer_init(void);
void DAC_reset(void);
void DAC_init(void);
void DAC_increment(void);
void ADC_reset(void);
void ADC3_IN4_single_init(void);
void ADC3_IN4_single_read(void);
void ADC3_IN4_timer_init(void);
void ADC3_IN4_timer_start(void);
void ADC3_IN4_DMA_init(void);
void ADC3_IN4_DMA_start(void);
void ADC1_IN13_ADC2_IN5_dual_init(void);
void ADC1_IN13_ADC2_IN5_dual_start(void);
void ADC2_IN13_IN5_scan_init(void);
void ADC2_IN13_IN5_scan_start(void);
void ADC3_IN13_IN4_scan_init(void);
void ADC3_IN13_IN4_scan_start(void);

float32_t FMCW_getPeakMangitudeFrequency(void);
float32_t FMCW_getRadialVelocity(void);

bool FMCW_calibrateFft(void);
void FMCW_applyCalibrationFft(void);

void MEAS_show_data(void);
void FFT_init(void);


#endif
