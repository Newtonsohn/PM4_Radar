/************************************************************************************/
/*
                            P R O C E S S I N G . H

 ------------------------------------------------------------------------------------

  (c) Copyright       ZHAW
                      Technikumstr. 9
                      CH-8200 Winterthur

 ------------------------------------------------------------------------------------

  Subsystem         : APP (Application)
  Module            : Signal Processing
  Type of SW-Unit   : H File

 ------------------------------------------------------------------------------------

  Description:
  ------------
  DSP operation on the audio stream.

  IT is recommended to use the CMSI DSP library developed by ARM for standard
  operations like FIR / IIR filter, FFT, matrix operation or math functions like sin.

  Most routines can be used with fp32, q31 or q15. The performance is improved if
  the block size is a multiple of 8.

  For documentation of the CMSIS lib look here:
  http://www.keil.com/pack/doc/CMSIS/DSP/html/modules.html

 ------------------------------------------------------------------------------------

*/
/************************************************************************************/

#ifndef PROCESSING_H_
#define PROCESSING_H_

/*** Imported external Objects ******************************************************/
/************************************************************************************/

#include "stdio.h"
#include "LAB_CONFIG.h"
#include "main.h"
#include "GUI.h"


/*** Exported Objects ***************************************************************/
/************************************************************************************/

/*** Constants **********************************************************************/

/*** Types **************************************************************************/

/*** Events *************************************************************************/

/*** Global variables ***************************************************************/

/*** Functions **********************************************************************/

/***********************************************************************************/
/*
	processing_init

 	Initialization of the processing. This function is called once after start-up.
 	Init the CMSIS lib functions here if needed.
*/
/***********************************************************************************/
void processing_init(void);


/***********************************************************************************/
/*
	audio_signal_processing

 	Main signal processing routine:
 	 - input_data_CH1:  input data channel 1, array length = BUFFER_SIZE_SAMPLES
 	 - input_data_CH2:  input data channel 2, array length = BUFFER_SIZE_SAMPLES
 	 - output_data_CH1: output data channel 1, array length = BUFFER_SIZE_SAMPLES
 	 - output_data_CH2: output data channel 2, array length = BUFFER_SIZE_SAMPLES

 	The samples are directly taken from the ADC without conversation. As the ADC has
 	a resolution of 24 bit, the input data goes from -2^23 to -2^23 - 1.

 	If floating point processing is used, the values are automatically converted to
 	the floating point version of the buffers:

 	 - input_data_CH1_f
 	 - input_data_CH2_f
 	 - output_data_CH1_f
 	 - output_data_CH2_f

*/
/***********************************************************************************/
void audio_signal_processing(int32_t * input_data_CH1, int32_t * input_data_CH2, int32_t * output_data_CH1, int32_t * output_data_CH2);


/***********************************************************************************/
/*
	waveform_generator

	Waveform generator to test the output path of the codec.

*/
/***********************************************************************************/
void waveform_generator(int32_t * output_data_CH1, int32_t * output_data_CH2);

#endif /* PROCESSING_H_ */
