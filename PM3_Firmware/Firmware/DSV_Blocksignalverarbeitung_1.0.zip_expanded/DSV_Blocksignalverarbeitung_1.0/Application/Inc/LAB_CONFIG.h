/************************************************************************************/
/*
                        L A B _ C O N F I G . H

 ------------------------------------------------------------------------------------

  (c) Copyright       ZHAW / ZSN
                      Technikumstr. 9
                      CH-8401 Winterthur

                      Author: 31.7.2017 / zols
                      Version: 9.9.2017  / wyrs / 8kHz, Print

 ------------------------------------------------------------------------------------

  Subsystem         : APP (Application)
  Module            : Configuration
  Type of SW-Unit   : H File

 ------------------------------------------------------------------------------------

  Description:
  ------------
  Global settings for the audio codec and signal processing.

 ------------------------------------------------------------------------------------

 */
/************************************************************************************/

#ifndef LAB_CONFIG_H_
#define LAB_CONFIG_H_

/*** User settings ******************************************************************/
/************************************************************************************/

// Information on startup screen
#define PRAKTIKUM_NAME						"Blocksignalverarbeitung 1.0"


// Set the precision for the two data streams
#define USE_FP_PROCESSING_ON_CH1			1	// 0: integer processing
												// 1: floating point processing (single precision)

#define USE_FP_PROCESSING_ON_CH2			0	// 0: integer processing
												// 1: floating point processing (single precision)


// Set the block size of the signal processing, a size of 2^n is recommended but not required
#define BUFFER_SIZE_SAMPLES					1024	// 1 to 1024


//Select the sampling frequency, choose one from the list below:
/* - BSP_AUDIO_FREQUENCY_48K
   - BSP_AUDIO_FREQUENCY_44K
   - BSP_AUDIO_FREQUENCY_32K
   - BSP_AUDIO_FREQUENCY_22K
   - BSP_AUDIO_FREQUENCY_16K
   - BSP_AUDIO_FREQUENCY_11K				// no oversampling
   - BSP_AUDIO_FREQUENCY_8K					// no oversampling */
#define CODEC_SAMPLING_FREQUENCY			BSP_AUDIO_FREQUENCY_48K


// The pictures used for plotting need to be programmed to the QSPI flash on the evalboard once.
// To do so set the PROGRAMM_QSPI flag to "1", build project, compile & download the software and run it.
// Then wait until the green LED lights up, this can take up to 3 minutes. Stop the programm,
// set the PROGRAMM_QSPI to 0 again, compile & download & run it.
#define	PROGRAMM_QSPI						0


// default startup volume
#define WAVE_GEN_STARTUP_VOLUME				-25	// -57 dB to +6 dB

#define AUDIO_INPUT_STARTUP_VOLUME			0	// -16.5 dB to +30 dB
#define AUDIO_OUTPUT_STARTUP_VOLUME			0	// -57 dB to +6 dB




/************************************************************************************/
/*** System settings: don't change ANYTHING below this line *************************/
/*** You may vary the texts to clarify what you think is executed in each Mode ******/
/************************************************************************************/

// button
#define MAX_BUTTON_STATES					5

// set the text show on the screen
#define NO_MODE_LINE_1						"Willkommen zum DSV Praktikum "
#define NO_MODE_LINE_2						"Blocksignalverarbeitung"
#define NO_MODE_LINE_3						"Verkabeln Sie das Board wie in der Anleitung"
#define NO_MODE_LINE_4						"beschrieben und waehlen Sie dann Mode 1 aus."
#define NO_MODE_LINE_5						" "

#define MODE_1_LINE_1						"Aufgabe "
#define MODE_1_LINE_2						" "
#define MODE_1_LINE_3						"FIR Filter / FFT"
#define MODE_1_LINE_4						"Output1 = Input1 float"
#define MODE_1_LINE_5						"Output2 = Input2 q15"

#define MODE_2_LINE_1						"Vorgabe 2"
#define MODE_2_LINE_2						" "
#define MODE_2_LINE_3						"Kurschluss"
#define MODE_2_LINE_4						"Output1 = Input1 float"
#define MODE_2_LINE_5						"Output2 = Input2 q31"

#define MODE_3_LINE_1						"Vorgabe 3"
#define MODE_3_LINE_2						" "
#define MODE_3_LINE_3						"Kurschluss"
#define MODE_3_LINE_4						"Output1 = Input1 float"
#define MODE_3_LINE_5						"Output2 = Input2 q31"

#define MODE_4_LINE_1						"Vorgabe 4"
#define MODE_4_LINE_2						" "
#define MODE_4_LINE_3						"Kurschluss"
#define MODE_4_LINE_4						"Output1 = Input1 float"
#define MODE_4_LINE_5						"Output2 = Input2 q31"

#define MODE_5_LINE_1						"Vorgabe 5"
#define MODE_5_LINE_2						" "
#define MODE_5_LINE_3						"Kurschluss"
#define MODE_5_LINE_4						"Output1 = Input1 float"
#define MODE_5_LINE_5						"Output2 = Input2 q31"

#define USER_TEXT_LENGTH					49


// measurement
#define MEASURE_CH1_IN_BY_DEFAULT			1		// 0: measurement disabled; 1 = measurement enabled
#define MEASURE_CH2_IN_BY_DEFAULT			0
#define MEASURE_CH1_OUT_BY_DEFAULT			0
#define MEASURE_CH2_OUT_BY_DEFAULT			0


// set the ARM core version for the CMSIS lib
#define ARM_MATH_CM7

// global FP usage
#define USE_FP_PROCESSING					(USE_FP_PROCESSING_ON_CH1 | USE_FP_PROCESSING_ON_CH2)

// codec analog amplifier stage
#define CODEC_MIN_OUTPUT_VOLUE_dB			-57
#define CODEC_MAX_OUTPUT_VOLUE_dB			6
#define CODEC_MAX_OUTPUT_VOLUE_STEP			1

#define CODEC_MIN_INPUT_VOLUE_dB			-16.5
#define CODEC_MAX_INPUT_VOLUE_dB			30
#define CODEC_MAX_INPUT_VOLUE_STEP			1.5


// Set the input mode, only analog input is supported right now
#define ADC_INPUT_ANALOG_DIGITAL_MIX		0	// 2x analog form 3.5 mm jack & 2x digital from onboard MP34DT01 microphone
#define ADC_INPUT_DIGITAL_ONLY				1	// 4x digital onboard MP34DT01 microphone
#define ADC_INPUT_SELECTION					ADC_INPUT_ANALOG_DIGITAL_MIX


// define the input data size
#define BYTES_PER_SAMPLE					4
#define NUMBER_OF_OUTPUT_CHANNELS			2

#define AUDIO_INPUT_BUFFER_SIZE				BUFFER_SIZE_SAMPLES
#define AUDIO_INPUT_SCRATCH_SIZE			1024
#define NUMBER_OF_INPUT_CHANNELS			2
#define PROCESSING_BUFFER_SIZE				(BUFFER_SIZE_SAMPLES * NUMBER_OF_OUTPUT_CHANNELS)


// constants for the LCD and plotting
#define LCD_TEXT_Y_OFFSET					(4*24)
#define INFO_TEXT_FONT						Font16

#define INFO_TEXT_FONT_PAGE_1				Font20
#define INFO_TEXT_FONT_PAGE_5				Font20

#define FREQ_PLOT_FFT_SIZE					4096
#define FREQ_PLOT_DATA_SIZE					700
#define FREQ_PLOT_Y_SIZE					330

#define TIME_PLOT_DATA_SIZE					720
#define TIME_PLOT_Y_SIZE					330
#define TIME_PLOT_Y_SCALING					50840.04848f	// 2^22 / TIME_PLOT_Y_SIZE = 50840.04848

#define PLOT_IO_DATA_BUFFER					FREQ_PLOT_FFT_SIZE


// constants for the QSPI flash with the pictures used for the GUI
#define QSPI_BASE_ADR						0x90000000
#define PFEIL_RECHTS_ADR					0x00000000
#define PFEIL_LINKS_ADR						0x00008000
#define TIME_GRAPH_ADR						0x00010000
#define FREQ_GRAPH_ADR						0x00100000
#define FIRST_FREE_ADR						0x00200000

#define QSPI_FLASH_BLOCK_SIZE				0x00008000


// The STM32F769NIH6 features a 128 kByte tightly coupled memory (TCM) for data and 16 kByte for instructions.
// It can be used to to accelerate the signal processing.
#define ENABLE_TCM_MAPPING					1

#if ENABLE_TCM_MAPPING == 1
	#define MAP_TO_DTCM						__attribute__((section (".dtcm")))
	#define MAP_TO_ITCM					//	__attribute__((section (".itcm")))
#else
	#define MAP_TO_DTCM
	#define MAP_TO_ITCM
#endif


#endif /* LAB_CONFIG_H_ */
