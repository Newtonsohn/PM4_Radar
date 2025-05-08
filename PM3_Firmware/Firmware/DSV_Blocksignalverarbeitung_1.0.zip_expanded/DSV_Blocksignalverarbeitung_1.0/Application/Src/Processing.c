/************************************************************************************/
/*
                            P R O C E S S I N G . C

 ------------------------------------------------------------------------------------

  (c) Copyright       ZSN/ZHAW
                      Technikumstr. 9
                      CH-8401 Winterthur

					  Author: 31.7.2017 / zols
                      Version: 5.3.2018 / kaeb/wyrs  (Print 8kHz)

 ------------------------------------------------------------------------------------

  Subsystem         : APP (Application)
  Module            : Signal Processing
  Type of SW-Unit   : C File

 ------------------------------------------------------------------------------------

  Description:
  ------------
  DSP operation on the audio stream.

  It is recommended to use the CMSI DSP library developed by ARM for standard
  operations like FIR / IIR filter, FFT, matrix operation or math functions like sin.

  Most routines can be used with fp32, q31 or q15. The performance is improved if
  the block size is a multiple of 8.

  For documentation of the CMSIS lib look here:
  http://arm-software.github.io/CMSIS_5/DSP/html/index.html

 ------------------------------------------------------------------------------------

 */
/************************************************************************************/


/*** Imported external Objects ******************************************************/
/************************************************************************************/
#include "Processing.h"
#include "main.h"
#include "math.h"
#include "LAB_CONFIG.h"
#include "core_cm7.h"
#include "arm_math.h"

#include "arm_const_structs.h"

/*** Module Objects *****************************************************************/
/************************************************************************************/

/*** Constants **********************************************************************/
#define FILTER_ORDER 127
#define IFFT_FLAG 0
#define FFT_BIT_REVERSE 1
#define FFT_SIZE BUFFER_SIZE_SAMPLES
#if FFT_SIZE > BUFFER_SIZE_SAMPLES
#error FFT_SIZE has to be smaller than the buffer size
#endif

/*** Types **************************************************************************/

/*** Variables **********************************************************************/
static arm_fir_instance_q15 fir_bp_filter_q15 MAP_TO_DTCM;
static arm_fir_instance_f32 fir_bp_filter_float MAP_TO_DTCM;
static arm_fir_instance_q15 fir_bs_filter_q15 MAP_TO_DTCM;
static arm_fir_instance_f32 fir_bs_filter_float MAP_TO_DTCM;

static arm_rfft_fast_instance_f32 rfft_fast_float;
static arm_rfft_instance_q15 rfft_q15;

static float_t bp_coeffs_float[] = {
#include "bp_float.h"
};
static q15_t bp_coeffs_q15[] = {
#include "bp_q15.h"
};
static float_t bs_coeffs_float[] = {
#include "bs_float.h"
};
static q15_t bs_coeffs_q15[] = {
#include "bs_q15.h"
};

static float filter_state_bp_float[FILTER_ORDER + 1 + BUFFER_SIZE_SAMPLES] MAP_TO_DTCM; // FIR-Filter States
static float filter_state_bs_float[FILTER_ORDER + 1 + BUFFER_SIZE_SAMPLES] MAP_TO_DTCM; // FIR-Filter States
static q15_t filter_state_bp_q15[FILTER_ORDER + 1 + BUFFER_SIZE_SAMPLES] MAP_TO_DTCM; // FIR-Filter States
static q15_t filter_state_bs_q15[FILTER_ORDER + 1 + BUFFER_SIZE_SAMPLES] MAP_TO_DTCM; // FIR-Filter States

/*** Functions **********************************************************************/

/***********************************************************************************/
/*
	processing_init

 	Initialization of the processing. This function is called once after start-up.
 	Init the CMSIS lib functions here if needed.
 */
/***********************************************************************************/
void processing_init(void){

	/*
	audio_signal_processing

 	Main signal processing routine:
 	 - input_data_CH1:  input data channel 1, array length = BUFFER_SIZE_SAMPLES
 	 - input_data_CH2:  input data channel 2, array length = BUFFER_SIZE_SAMPLES
 	 - output_data_CH1: output data channel 1, array length = BUFFER_SIZE_SAMPLES
 	 - output_data_CH2: output data channel 2, array length = BUFFER_SIZE_SAMPLES

 	The samples are directly taken from the ADC without conversion. As the ADC has
 	a resolution of 24 bit, the input data goes from -2^23 to -2^23 - 1.

 	If floating point processing is used, the values are automatically converted to
 	the floating point version of the buffers:

 	 - input_data_CH1_f
 	 - input_data_CH2_f
 	 - output_data_CH1_f
 	 - output_data_CH2_f

	 */
	arm_fir_init_f32(&fir_bp_filter_float, FILTER_ORDER+1, &bp_coeffs_float[0], &filter_state_bp_float[0], BUFFER_SIZE_SAMPLES);
	arm_fir_init_q15(&fir_bp_filter_q15, FILTER_ORDER+1, &bp_coeffs_q15[0], &filter_state_bp_q15[0], BUFFER_SIZE_SAMPLES);
	arm_fir_init_f32(&fir_bs_filter_float, FILTER_ORDER+1, &bs_coeffs_float[0], &filter_state_bs_float[0], BUFFER_SIZE_SAMPLES);
	arm_fir_init_q15(&fir_bs_filter_q15, FILTER_ORDER+1, &bs_coeffs_q15[0], &filter_state_bs_q15[0], BUFFER_SIZE_SAMPLES);

	if(arm_rfft_fast_init_f32(&rfft_fast_float,FFT_SIZE)!=ARM_MATH_SUCCESS){
		// "missing" error handling
	}
	if(arm_rfft_init_q15(&rfft_q15,FFT_SIZE, IFFT_FLAG, FFT_BIT_REVERSE)!= ARM_MATH_SUCCESS){
		// "missing" error handling
	}
}
/***********************************************************************************/
void audio_signal_processing(int32_t * input_data_CH1, int32_t * input_data_CH2, int32_t * output_data_CH1, int32_t * output_data_CH2){
	uint32_t i;

	q15_t input_data_CH2_q15[BUFFER_SIZE_SAMPLES];
	q15_t output_data_CH2_q15[BUFFER_SIZE_SAMPLES];
	float32_t fft_float[FFT_SIZE];  // real & imaginary parts, stores only up to fs/2
	q15_t fft_q15[2*FFT_SIZE]; // real & imaginary parts

	// ---------------------------------------------------------------------------------------
	// Initialization: generate floating point variants of the buffers if needed
	// ---------------------------------------------------------------------------------------
#if USE_FP_PROCESSING == 1
#if USE_FP_PROCESSING_ON_CH1 == 1
	float input_data_CH1_f[BUFFER_SIZE_SAMPLES];
	float output_data_CH1_f[BUFFER_SIZE_SAMPLES];
#endif

#if USE_FP_PROCESSING_ON_CH2 == 1
	float input_data_CH2_f[BUFFER_SIZE_SAMPLES];
	float output_data_CH2_f[BUFFER_SIZE_SAMPLES];
#endif
#endif

#if USE_FP_PROCESSING == 1
	for(i = 0; i < BUFFER_SIZE_SAMPLES; i++){
#if USE_FP_PROCESSING_ON_CH1 == 1
		input_data_CH1_f[i] = input_data_CH1[i];
#endif

#if USE_FP_PROCESSING_ON_CH2 == 1
		input_data_CH2_f[i] = input_data_CH2[i];
#endif
	}
#endif
	// ---------------------------------------------------------------------------------------


	//BSP_LED_On(LED_GREEN);

	switch(user_mode){

	case 0:
		// ---------------------------------------------------------------------------------------
		// no mode selected by the user -> set output to 0
		// ---------------------------------------------------------------------------------------
		for(i = 0; i < BUFFER_SIZE_SAMPLES; i++){
#if USE_FP_PROCESSING_ON_CH1 == 1
			output_data_CH1_f[i] = 0;
#else
			output_data_CH1[i] = 0;
#endif

#if USE_FP_PROCESSING_ON_CH2 == 1
			output_data_CH2_f[i] = 0;
#else
			output_data_CH2[i] = 0;
#endif
		}
		// ---------------------------------------------------------------------------------------
		break;

	case 1:

	for (i = 0; i < BUFFER_SIZE_SAMPLES; i++){
		input_data_CH2_q15[i] = (q15_t)(input_data_CH2[i]>>8);
	}

	BSP_LED_Off(LED_GREEN);
	// BITTE HIER FIR-FILTER CODIEREN, ZUERST FLOAT AUF CH1 UND DANN Q15 AUF CH2
//	for (i = 0; i < BUFFER_SIZE_SAMPLES; i++){
//			output_data_CH1_f[i] = input_data_CH1_f[i];
//			output_data_CH2_q15[i] = input_data_CH2_q15[i];
//	}
	arm_fir_f32(&fir_bs_filter_float, input_data_CH1_f, output_data_CH1_f, BUFFER_SIZE_SAMPLES);
	// BITTE HIER FIR-FILTER CODIEREN, ZUERST FLOAT AUF CH1 UND DANN Q15 AUF CH2
	BSP_LED_On(LED_GREEN);

	//scale to 24 bit and add 32 bit sign extension
	for(i = 0; i < BUFFER_SIZE_SAMPLES; i++){
		if(output_data_CH2_q15[i]<0){
			output_data_CH2[i] = (int32_t)((((uint32_t)output_data_CH2_q15[i])<<8)|0xFF000000);
		}
		else{
			output_data_CH2[i] = (int32_t)(((uint32_t)output_data_CH2_q15[i])<<8);
		}
	}

		break;


	case 2:

		BSP_LED_On(LED_GREEN);
		// BITTE HIER FFT CODIEREN, FLOAT AUF CH1
		arm_fir_q15(&fir_bs_filter_q15, input_data_CH2_q15, output_data_CH2_q15, BUFFER_SIZE_SAMPLES);

		BSP_LED_Off(LED_GREEN);

		//set output to zero
//		for(i = 0; i < BUFFER_SIZE_SAMPLES; i++){
//			output_data_CH2[i] = 0;
//			output_data_CH1_f[i] = 0;
//		}

		break;

	case 3:
		// ---------------------------------------------------------------------------------------
		// Mode 3 selected with GUI: pass through the signals without modification
		// ---------------------------------------------------------------------------------------
		for(i = 0; i < BUFFER_SIZE_SAMPLES; i++){
#if USE_FP_PROCESSING_ON_CH1 == 1
			output_data_CH1_f[i] = input_data_CH1_f[i];
#else
			output_data_CH1[i] = input_data_CH1[i];
#endif

#if USE_FP_PROCESSING_ON_CH2 == 1
			output_data_CH2_f[i] = input_data_CH1_f[i];
#else
			output_data_CH2[i] = input_data_CH2[i];
#endif
		}
		snprintf(user_text_line_mode_3, USER_TEXT_LENGTH, " ");
		// ---------------------------------------------------------------------------------------
		break;

	case 4:
		// ---------------------------------------------------------------------------------------
		// Mode 4 selected with GUI: pass through the signals without modification
		// ---------------------------------------------------------------------------------------
		for(i = 0; i < BUFFER_SIZE_SAMPLES; i++){
#if USE_FP_PROCESSING_ON_CH1 == 1
			output_data_CH1_f[i] = input_data_CH1_f[i];
#else
			output_data_CH1[i] = input_data_CH1[i];
#endif

#if USE_FP_PROCESSING_ON_CH2 == 1
			output_data_CH2_f[i] = input_data_CH1_f[i];
#else
			output_data_CH2[i] = input_data_CH2[i];
#endif
		}
		snprintf(user_text_line_mode_4, USER_TEXT_LENGTH, " ");
		// ---------------------------------------------------------------------------------------
		break;

	case 5:
		// ---------------------------------------------------------------------------------------
		// Mode 5 selected with GUI: pass through the signals without modification
		// ---------------------------------------------------------------------------------------
		for(i = 0; i < BUFFER_SIZE_SAMPLES; i++){
#if USE_FP_PROCESSING_ON_CH1 == 1
			output_data_CH1_f[i] = input_data_CH1_f[i];
#else
			output_data_CH1[i] = input_data_CH1[i];
#endif

#if USE_FP_PROCESSING_ON_CH2 == 1
			output_data_CH2_f[i] = input_data_CH1_f[i];
#else
			output_data_CH2[i] = input_data_CH2[i];
#endif
		}
		snprintf(user_text_line_mode_5, USER_TEXT_LENGTH, " ");
		// ---------------------------------------------------------------------------------------
		break;

	default:
		// ---------------------------------------------------------------------------------------
		// undefined mode -> pass through the signals without modification
		// ---------------------------------------------------------------------------------------
		user_mode = 1;

		for(i = 0; i < BUFFER_SIZE_SAMPLES; i++){
#if USE_FP_PROCESSING_ON_CH1 == 1
			output_data_CH1_f[i] = input_data_CH1_f[i];
#else
			output_data_CH1[i] = input_data_CH1[i];
#endif

#if USE_FP_PROCESSING_ON_CH2 == 1
			output_data_CH2_f[i] = input_data_CH1_f[i];
#else
			output_data_CH2[i] = input_data_CH2[i];
#endif
		}
		// ---------------------------------------------------------------------------------------

		break;
	}




	// ---------------------------------------------------------------------------------------
	// Postprocessing: copy the processed data to the output buffer if needed
	// ---------------------------------------------------------------------------------------
#if USE_FP_PROCESSING == 1
	for(i = 0; i < BUFFER_SIZE_SAMPLES; i++){
#if USE_FP_PROCESSING_ON_CH1 == 1
		output_data_CH1[i] = output_data_CH1_f[i];
#endif

#if USE_FP_PROCESSING_ON_CH2 == 1
		output_data_CH2[i] = output_data_CH2_f[i];
#endif
	}
#endif
	// ---------------------------------------------------------------------------------------
}

/************************************************************************************/


/***********************************************************************************/
/*
	waveform_generator

	Waveform generator to test the output path of the codec.

 */
/***********************************************************************************/
void waveform_generator(int32_t * output_data_CH1, int32_t * output_data_CH2){
	uint32_t i;

	static double phase_accumulator = 0;
	double phase_increment;

	phase_increment = 2 * M_PI * (double)(wav_gen_output_frequency)/ (double)CODEC_SAMPLING_FREQUENCY;

	for(i = 0; i < BUFFER_SIZE_SAMPLES; i ++){
		switch(wav_gen){
		case WAV_SINUS:
			output_data_CH1[i] = sin(phase_accumulator) * (double)8388607;
			output_data_CH2[i] = -output_data_CH1[i];
			break;

		case WAV_RECTANGLE:
			if(phase_accumulator >= M_PI){
				output_data_CH1[i] = 8388607;
				output_data_CH2[i] = -8388607;
			}

			else{
				output_data_CH1[i] = -8388607;
				output_data_CH2[i] = 8388607;
			}
			break;

		case WAV_SAWTOOTH:
			output_data_CH1[i] = phase_accumulator / (2 * M_PI) * 8388607 * 2 - 8388607;
			output_data_CH2[i] = -output_data_CH1[i];
			break;
		}

		phase_accumulator += phase_increment;

		if(phase_accumulator > (2 * M_PI)){
			phase_accumulator -= (2 * M_PI);
		}
	}
}

/***********************************************************************************/
