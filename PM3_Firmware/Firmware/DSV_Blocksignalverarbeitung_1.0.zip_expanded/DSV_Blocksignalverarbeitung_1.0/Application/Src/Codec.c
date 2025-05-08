/************************************************************************************/
/*
                                C O D E C . C

 ------------------------------------------------------------------------------------

  (c) Copyright       ZHAW
                      Technikumstr. 9
                      CH-8200 Winterthur

 ------------------------------------------------------------------------------------

  Subsystem         : APP (Application)
  Module            : Codec
  Type of SW-Unit   : C File

 ------------------------------------------------------------------------------------

  Description:
  ------------
  Initialization of the audio codec and handling of the DMA callbacks for the audio
  streaming.
 ------------------------------------------------------------------------------------

*/
/************************************************************************************/

/*** Imported external Objects ******************************************************/
/************************************************************************************/
#include "Codec.h"
#include "GUI.h"
//#include "main.h"
#include "math.h"
#include "LAB_CONFIG.h"
#include "Processing.h"
#include "wm8994.h"
#include "stm32f769i_discovery_audio.h"
#include "stm32f7xx_hal.h"
#include "stm32f769i_discovery.h"

/*** Module Objects *****************************************************************/
/************************************************************************************/

/*** Constants **********************************************************************/

/*** Types **************************************************************************/

/*** Public Variables ***************************************************************/
volatile uint32_t watchdog_counter MAP_TO_DTCM = BSP_AUDIO_FREQUENCY_48K / BUFFER_SIZE_SAMPLES;
volatile uint32_t audio_buffer_a_timeout MAP_TO_DTCM = 0;
volatile uint32_t audio_buffer_b_timeout MAP_TO_DTCM = 0;

/*** Private Function Prototypes ****************************************************/

/*** Private Variables **************************************************************/
extern SAI_HandleTypeDef haudio_in_sai;

// buffer between CPU, DMA and interface
static volatile int32_t audio_input_scratch[AUDIO_INPUT_SCRATCH_SIZE] MAP_TO_DTCM;
static volatile int32_t audio_input_buffer[AUDIO_INPUT_BUFFER_SIZE * NUMBER_OF_INPUT_CHANNELS * 2] MAP_TO_DTCM;
static volatile int32_t audio_output_buffer[BUFFER_SIZE_SAMPLES * NUMBER_OF_OUTPUT_CHANNELS * 2] MAP_TO_DTCM;
static volatile uint32_t plotInputBufferIndex MAP_TO_DTCM = 0;
static volatile uint32_t plotOutputBufferIndex MAP_TO_DTCM = 0;

// buffer for user signal processing
static volatile int32_t audio_processing_buffer_A_CH1[AUDIO_INPUT_BUFFER_SIZE] MAP_TO_DTCM;
static volatile int32_t audio_processing_buffer_A_CH2[AUDIO_INPUT_BUFFER_SIZE] MAP_TO_DTCM;
static volatile int32_t audio_processing_buffer_B_CH1[AUDIO_INPUT_BUFFER_SIZE] MAP_TO_DTCM;
static volatile int32_t audio_processing_buffer_B_CH2[AUDIO_INPUT_BUFFER_SIZE] MAP_TO_DTCM;
static volatile int32_t audio_output_buffer_A_CH1[BUFFER_SIZE_SAMPLES] MAP_TO_DTCM;
static volatile int32_t audio_output_buffer_A_CH2[BUFFER_SIZE_SAMPLES] MAP_TO_DTCM;
static volatile int32_t audio_output_buffer_B_CH1[BUFFER_SIZE_SAMPLES] MAP_TO_DTCM;
static volatile int32_t audio_output_buffer_B_CH2[BUFFER_SIZE_SAMPLES] MAP_TO_DTCM;


/*** Public Functions ***************************************************************/
/************************************************************************************/

/***********************************************************************************/
/*
	initCodec

 	Initialization of the audio codec with a given frequency.
*/
/***********************************************************************************/
uint8_t initCodec(uint32_t samplingFreqency){

  uint8_t ret = AUDIO_ERROR;

  /* Initialize the Audio codec and all related peripherals (I2S, I2C, IOExpander, IOs...) */
  ret = BSP_AUDIO_OUT_Init(OUTPUT_DEVICE_HEADPHONE, 0, CODEC_SAMPLING_FREQUENCY);
  if(ret != AUDIO_OK)
  {
    return(ret);
  }
  else
  {
	 BSP_AUDIO_OUT_SetAudioFrameSlot(CODEC_AUDIOFRAME_SLOT_02);
  }

  // input init
  BSP_AUDIO_IN_Init(CODEC_SAMPLING_FREQUENCY, 24, DEFAULT_AUDIO_IN_CHANNEL_NBR);
  BSP_AUDIO_IN_AllocScratch ((int32_t *)&audio_input_scratch[0], AUDIO_INPUT_SCRATCH_SIZE);

  // start the ping/pong buffer
  BSP_AUDIO_OUT_Play((uint16_t*)&audio_output_buffer[0], BUFFER_SIZE_SAMPLES * NUMBER_OF_OUTPUT_CHANNELS*BYTES_PER_SAMPLE);
  ret = BSP_AUDIO_IN_Record((uint16_t*)&audio_input_buffer[0], AUDIO_INPUT_BUFFER_SIZE * NUMBER_OF_INPUT_CHANNELS*2);

  return(ret);
}
/************************************************************************************/


/***********************************************************************************/
/*
	DMA2_Stream4_IRQHandler

 	DMA stream interrupt: process the event
*/
/***********************************************************************************/
void DMA2_Stream4_IRQHandler(void){
	HAL_DMA_IRQHandler(haudio_in_sai.hdmarx);
}
/************************************************************************************/


/***********************************************************************************/
/*
	BSP_AUDIO_OUT_TransferComplete_CallBack

 	The audio output stream DMA runs out of data and needs to be reloaded. Switch
	the buffer order in case only one sample per step is used.
*/
/***********************************************************************************/
#if BUFFER_SIZE_SAMPLES == 1
void BSP_AUDIO_OUT_HalfTransfer_CallBack(void)
#else
void BSP_AUDIO_OUT_TransferComplete_CallBack(void)
#endif
{
	uint32_t i;

	if(audio_buffer_a_timeout == 0){	// check if the new output data block for the DMA is ready
		for(i = 0; i < BUFFER_SIZE_SAMPLES; i++){
			audio_output_buffer[2*i+ (BUFFER_SIZE_SAMPLES * NUMBER_OF_OUTPUT_CHANNELS)] = audio_output_buffer_A_CH1[i];
			audio_output_buffer[2*i + 1+ (BUFFER_SIZE_SAMPLES * NUMBER_OF_OUTPUT_CHANNELS)] = audio_output_buffer_A_CH2[i];
		}
	}

	else{
		for(i = 0; i < BUFFER_SIZE_SAMPLES; i++){	// set the output to 0 in CPU overload condition
			audio_output_buffer[2*i+ (BUFFER_SIZE_SAMPLES * NUMBER_OF_OUTPUT_CHANNELS)] = 0;
			audio_output_buffer[2*i + 1+ (BUFFER_SIZE_SAMPLES * NUMBER_OF_OUTPUT_CHANNELS)] = 0;
		}
		audio_buffer_a_timeout = 1;
	}

	if(watchdog_counter > 1){
		watchdog_counter--;
	}

	else{
		audio_buffer_a_timeout = 1;
		audio_buffer_b_timeout = 1;
	}
}
/************************************************************************************/


/***********************************************************************************/
/*
	BSP_AUDIO_OUT_HalfTransfer_CallBack

 	The audio output stream DMA runs out of data and needs to be reloaded. Switch
	the buffer order in case only one sample per step is used.
*/
/***********************************************************************************/
#if BUFFER_SIZE_SAMPLES == 1
void BSP_AUDIO_OUT_TransferComplete_CallBack(void)
#else
void BSP_AUDIO_OUT_HalfTransfer_CallBack(void)
#endif
{
	uint16_t i;

	if(audio_buffer_b_timeout == 0){	// check if the new output data block for the DMA is ready
		for(i = 0; i < BUFFER_SIZE_SAMPLES; i++){
			audio_output_buffer[2*i] = audio_output_buffer_B_CH1[i];
			audio_output_buffer[2*i + 1] = audio_output_buffer_B_CH2[i];
		}
	}

	else{
		for(i = 0; i < BUFFER_SIZE_SAMPLES; i++){ 	// set the output to 0 if the CPU was to slow to compute new data
			audio_output_buffer[2*i] = 0;
			audio_output_buffer[2*i + 1] = 0;
		}
		audio_buffer_b_timeout = 1;
	}
}
/************************************************************************************/


/***********************************************************************************/
/*
	BSP_AUDIO_IN_TransferComplete_CallBack

 	New data from the audio coded has been received -> process them
*/
/***********************************************************************************/
void BSP_AUDIO_IN_TransferComplete_CallBack(void)
{
	uint16_t i;

	if(audio_buffer_b_timeout == 1){	// stop the processing if the CPU is overloaded
		return;
	}

	if(newPlotDataReady == 0){
		for(i = 0; i < (AUDIO_INPUT_BUFFER_SIZE); i++){	// copy the input data to the processing buffer

			audio_processing_buffer_B_CH1[i] = audio_input_buffer[2*i + (AUDIO_INPUT_BUFFER_SIZE * NUMBER_OF_INPUT_CHANNELS)];
			if(audio_processing_buffer_B_CH1[i] & 0x00800000){	// sign extension from 24 bit to 32 bit
				audio_processing_buffer_B_CH1[i] = (audio_processing_buffer_B_CH1[i] | 0xff000000);
			}

			audio_processing_buffer_B_CH2[i] = audio_input_buffer[2*i + 1 + (AUDIO_INPUT_BUFFER_SIZE * NUMBER_OF_INPUT_CHANNELS)];
			if(audio_processing_buffer_B_CH2[i] & 0x00800000){	// sign extension from 24 bit to 32 bit
				audio_processing_buffer_B_CH2[i] = (audio_processing_buffer_B_CH2[i] | 0xff000000);
			}

			if(plotInputBufferIndex < PLOT_IO_DATA_BUFFER){	// copy data to plot on screen
				plotInputBufferIndex++;
			}
		}

	}
	else{
		for(i = 0; i < (AUDIO_INPUT_BUFFER_SIZE); i++){	// copy the input data to the processing buffer

			audio_processing_buffer_B_CH1[i] = audio_input_buffer[2*i + (AUDIO_INPUT_BUFFER_SIZE * NUMBER_OF_INPUT_CHANNELS)];
			if(audio_processing_buffer_B_CH1[i] & 0x00800000){	// sign extension from 24 bit to 32 bit
				audio_processing_buffer_B_CH1[i] = (audio_processing_buffer_B_CH1[i] | 0xff000000);
			}

			audio_processing_buffer_B_CH2[i] = audio_input_buffer[2*i + 1 + (AUDIO_INPUT_BUFFER_SIZE * NUMBER_OF_INPUT_CHANNELS)];
			if(audio_processing_buffer_B_CH2[i] & 0x00800000){	// sign extension from 24 bit to 32 bit
				audio_processing_buffer_B_CH2[i] = (audio_processing_buffer_B_CH2[i] | 0xff000000);
			}
		}
	}

	if(menue_page == PAGE4){	// waveform generator
		waveform_generator((int32_t *)&audio_output_buffer_B_CH1[0], (int32_t *)&audio_output_buffer_B_CH2[0]);
	}

	else{
		// let the student process the audio data
		audio_signal_processing((int32_t *)&audio_processing_buffer_B_CH1[0], (int32_t *)&audio_processing_buffer_B_CH2[0], (int32_t *)&audio_output_buffer_B_CH1[0], (int32_t *)&audio_output_buffer_B_CH2[0]);
	}

	// save the data to plot it later if needed
	i = 0;
	while(plotOutputBufferIndex < plotInputBufferIndex){ // copy data to plot on screen
		plotBufferCH1_in[plotOutputBufferIndex] = audio_processing_buffer_B_CH1[i];
		plotBufferCH2_in[plotOutputBufferIndex] = audio_processing_buffer_B_CH2[i];
		plotBufferCH1_out[plotOutputBufferIndex] = audio_output_buffer_B_CH1[i];
		plotBufferCH2_out[plotOutputBufferIndex] = audio_output_buffer_B_CH2[i];

		plotOutputBufferIndex++;
		i++;
	}

	if(plotInputBufferIndex >= PLOT_IO_DATA_BUFFER){
		newPlotDataReady = 1;
	}
}
/************************************************************************************/


/***********************************************************************************/
/*
	BSP_AUDIO_IN_HalfTransfer_CallBack

 	New data from the audio coded has been received -> process them
*/
/***********************************************************************************/
void BSP_AUDIO_IN_HalfTransfer_CallBack(void)
{
	uint16_t i;

	if(audio_buffer_a_timeout == 1){	// stop the processing if the CPU is overloaded
		return;
	}

	if(fetchNewPlotDataNow == 1){
		fetchNewPlotDataNow = 0;
		plotInputBufferIndex = 0;
		plotOutputBufferIndex = 0;
		newPlotDataReady = 0;
	}

	if(newPlotDataReady == 0){
		for(i = 0; i < (AUDIO_INPUT_BUFFER_SIZE); i++){	// copy the input data to the processing buffer

			audio_processing_buffer_A_CH1[i] = audio_input_buffer[2*i];
			if(audio_processing_buffer_A_CH1[i] & 0x00800000){	// sign extension from 24 bit to 32 bit
				audio_processing_buffer_A_CH1[i] = (audio_processing_buffer_A_CH1[i] | 0xff000000);
			}

			audio_processing_buffer_A_CH2[i] = audio_input_buffer[2*i + 1];
			if(audio_processing_buffer_A_CH2[i] & 0x00800000){	// sign extension from 24 bit to 32 bit
				audio_processing_buffer_A_CH2[i] = (audio_processing_buffer_A_CH2[i] | 0xff000000);
			}

			if(plotInputBufferIndex < PLOT_IO_DATA_BUFFER){	// copy data to plot on screen
				plotInputBufferIndex++;
			}
		}
	}

	else{
		for(i = 0; i < (AUDIO_INPUT_BUFFER_SIZE); i++){	// copy the input data to the processing buffer


			audio_processing_buffer_A_CH1[i] = audio_input_buffer[2*i];
			if(audio_processing_buffer_A_CH1[i] & 0x00800000){	// sign extension from 24 bit to 32 bit
				audio_processing_buffer_A_CH1[i] = (audio_processing_buffer_A_CH1[i] | 0xff000000);
			}

			audio_processing_buffer_A_CH2[i] = audio_input_buffer[2*i + 1];
			if(audio_processing_buffer_A_CH2[i] & 0x00800000){	// sign extension from 24 bit to 32 bit
				audio_processing_buffer_A_CH2[i] = (audio_processing_buffer_A_CH2[i] | 0xff000000);
			}
		}
	}

	if(menue_page == PAGE4){	// waveform generator
		waveform_generator((int32_t *)&audio_output_buffer_A_CH1[0], (int32_t *)&audio_output_buffer_A_CH2[0]);
	}

	else{
		// let the student process the audio data
		audio_signal_processing((int32_t *)&audio_processing_buffer_A_CH1[0], (int32_t *)&audio_processing_buffer_A_CH2[0], (int32_t *)&audio_output_buffer_A_CH1[0], (int32_t *)&audio_output_buffer_A_CH2[0]);
	}

	// save the data to plot it later if needed
	i = 0;
	while(plotOutputBufferIndex < plotInputBufferIndex){ // copy data to plot on screen

		plotBufferCH1_in[plotOutputBufferIndex] = audio_processing_buffer_A_CH1[i];
		plotBufferCH2_in[plotOutputBufferIndex] = audio_processing_buffer_A_CH2[i];
		plotBufferCH1_out[plotOutputBufferIndex] = audio_output_buffer_A_CH1[i];
		plotBufferCH2_out[plotOutputBufferIndex] = audio_output_buffer_A_CH2[i];

		plotOutputBufferIndex++;
		i++;
	}

	if(plotInputBufferIndex >= PLOT_IO_DATA_BUFFER){
		newPlotDataReady = 1;
	}
}
/************************************************************************************/

