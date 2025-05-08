/*
                                    G U I . C

 ------------------------------------------------------------------------------------

  (c) Copyright       ZSN / ZHAW
                      Technikumstr. 9
                      CH-8401 Winterthur

					  Author; 31.7.2017 / zols
					  Version: 25.8.2017 / wyrs

 ------------------------------------------------------------------------------------

  Subsystem         : APP (Application)
  Module            : GUI
  Type of SW-Unit   : C File

 ------------------------------------------------------------------------------------

  Description:
  ------------
  Graphical user interface functions.
 ------------------------------------------------------------------------------------

*/
/************************************************************************************/


/*** Imported external Objects ******************************************************/
/************************************************************************************/
#include "GUI.h"
#include "stm32f7xx_hal.h"
#include "stm32f769i_discovery.h"
#include "stm32f769i_discovery_audio.h"
#include "stm32f769i_discovery_ts.h"
#include "math.h"
#include "Window.h"
#include "core_cm7.h"
#include "arm_math.h"
#include "main.h"


/*** Module Objects *****************************************************************/
/************************************************************************************/

/*** Constants **********************************************************************/

/*** Types **************************************************************************/

/*** Public Variables ***************************************************************/

// page handling
menue_page_t menue_page MAP_TO_DTCM = PAGE1;
volatile uint32_t user_button MAP_TO_DTCM = 1;
volatile uint32_t user_mode MAP_TO_DTCM = 0;

// wav gen
float wav_gen_output_frequency MAP_TO_DTCM = 2000;
wav_gen_t wav_gen MAP_TO_DTCM = WAV_SINUS;

// volume handling
float audio_input_volume_dB = AUDIO_INPUT_STARTUP_VOLUME;
float audio_output_volume_dB = AUDIO_OUTPUT_STARTUP_VOLUME;

// buffers to read data from the codec
volatile int32_t plotBufferCH1_in[PLOT_IO_DATA_BUFFER + 10];
volatile int32_t plotBufferCH2_in[PLOT_IO_DATA_BUFFER + 10];
volatile int32_t plotBufferCH1_out[PLOT_IO_DATA_BUFFER + 10];
volatile int32_t plotBufferCH2_out[PLOT_IO_DATA_BUFFER + 10];
volatile uint32_t fetchNewPlotDataNow MAP_TO_DTCM = 1 ;
volatile uint32_t newPlotDataReady MAP_TO_DTCM = 0;


/*** Private Function Prototypes ****************************************************/
// Frequently used GUI elements.
static void drawButtonRow(void);
static void drawWaveGenWaveforms(void);
static void drawWaveGenInfo(void);


/*** Private Variables **************************************************************/
static float wav_gen_output_volume_dB = WAVE_GEN_STARTUP_VOLUME;

static uint32_t measure_CH1_in = MEASURE_CH1_IN_BY_DEFAULT;
static uint32_t measure_CH2_in = MEASURE_CH2_IN_BY_DEFAULT;
static uint32_t measure_CH1_out = MEASURE_CH1_OUT_BY_DEFAULT;
static uint32_t measure_CH2_out = MEASURE_CH2_OUT_BY_DEFAULT;

static arm_rfft_fast_instance_f32 rFFT_plot_inst; // instance needed to compute the FFT for plotting the spectrogram


/*** Public Functions ***************************************************************/
/************************************************************************************/


/***********************************************************************************/
/*
	initGUI

 	Initialize the GUI module.
*/
/***********************************************************************************/
void initGUI(void){
	// initialize the FFT module
	arm_rfft_fast_init_f32(&rFFT_plot_inst, FREQ_PLOT_FFT_SIZE);

	// initialize the LCD
	BSP_LCD_Init();

	// LCD Layer Initialization
	BSP_LCD_LayerDefaultInit(1, LCD_FB_START_ADDRESS);

	// Select the LCD Layer
	BSP_LCD_SelectLayer(1);

	// Enable the display
	BSP_LCD_DisplayOn();

	// clear the screen
	BSP_LCD_Clear(LCD_COLOR_BLACK);

	// draw the title
	drawTitle();

	// draw the first page
	initPage1();


}


/***********************************************************************************/
/*
	initPageX

 	Draw the entire GUI of the according page onto the screen.
*/
/***********************************************************************************/
void initPage1(void){
	char string_buffer[100];

	wm8994_SetOutputVolume_dB(AUDIO_I2C_ADDRESS, audio_output_volume_dB);

	// init the screen
	BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
	BSP_LCD_SetBackColor(LCD_COLOR_BLACK);
	BSP_LCD_FillRect(0, 72, 800, 408);

	// draw the lower button row
	drawButtonRow();

	// draw the volume buttons
	BSP_LCD_SetFont(&Font24);

	BSP_LCD_SetTextColor(LCD_COLOR_GREEN);
	BSP_LCD_FillRect(690, 104, 100, 100);
	BSP_LCD_FillRect(690, 237, 100, 100);

	BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
	BSP_LCD_FillRect(700, 114, 80, 80);
	BSP_LCD_FillRect(700, 247, 80, 80);

	BSP_LCD_SetTextColor(LCD_COLOR_GREEN);
	BSP_LCD_SetBackColor(LCD_COLOR_BLACK);
	snprintf(string_buffer,2, "+");
	BSP_LCD_DisplayStringAt(732, 141, (uint8_t *)&string_buffer[0], LEFT_MODE);
	snprintf(string_buffer,2, "-");
	BSP_LCD_DisplayStringAt(732, 274, (uint8_t *)&string_buffer[0], LEFT_MODE);

	updatePage1();
}
/************************************************************************************/

/************************************************************************************/
void initPage2(void){
	wm8994_SetOutputVolume_dB(AUDIO_I2C_ADDRESS, audio_output_volume_dB);

	updatePage2();
}
/************************************************************************************/

/************************************************************************************/
void initPage3(void){
	wm8994_SetOutputVolume_dB(AUDIO_I2C_ADDRESS, audio_output_volume_dB);

	updatePage3();
}
/************************************************************************************/

/************************************************************************************/
void initPage4(void){
	wm8994_SetOutputVolume_dB(AUDIO_I2C_ADDRESS, wav_gen_output_volume_dB);
	updatePage4();
}
/************************************************************************************/

/************************************************************************************/
void initPage5(void){
	char string_buffer[100];
	uint16_t line = 0;
	float vol_vpp;
	float vol_rms;

	wm8994_SetOutputVolume_dB(AUDIO_I2C_ADDRESS, audio_output_volume_dB);

	BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
	BSP_LCD_FillRect(0, 72, 800, 408);

	BSP_LCD_SetBackColor(LCD_COLOR_BLACK);
	BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
	BSP_LCD_SetFont(&INFO_TEXT_FONT_PAGE_5);

	snprintf(string_buffer,100, "Evalboard: 32F769I Discovery");
	BSP_LCD_DisplayStringAt(10, LCD_TEXT_Y_OFFSET +  INFO_TEXT_FONT_PAGE_5.Height * line, (uint8_t *)&string_buffer[0], LEFT_MODE);
	line = 1;

	snprintf(string_buffer,100, "MCU: STM32F769NIH6");
	BSP_LCD_DisplayStringAt(10, LCD_TEXT_Y_OFFSET +  INFO_TEXT_FONT_PAGE_5.Height * line, (uint8_t *)&string_buffer[0], LEFT_MODE);
	line++;

	snprintf(string_buffer,100, "CPU: Cortex-M7 @ 200 MHz");
	BSP_LCD_DisplayStringAt(10, LCD_TEXT_Y_OFFSET +  INFO_TEXT_FONT_PAGE_5.Height * line, (uint8_t *)&string_buffer[0], LEFT_MODE);
	line++;

	snprintf(string_buffer,100, "Flash: 2 MByte");
	BSP_LCD_DisplayStringAt(10, LCD_TEXT_Y_OFFSET +  INFO_TEXT_FONT_PAGE_5.Height * line, (uint8_t *)&string_buffer[0], LEFT_MODE);
	line++;

	snprintf(string_buffer,100, "RAM: 368 kByte SRAM + 128 kByte TCM");
	BSP_LCD_DisplayStringAt(10, LCD_TEXT_Y_OFFSET +  INFO_TEXT_FONT_PAGE_5.Height * line, (uint8_t *)&string_buffer[0], LEFT_MODE);
	line++;

	snprintf(string_buffer,100, "Display: 800x480 Pixel");
	BSP_LCD_DisplayStringAt(10, LCD_TEXT_Y_OFFSET +  INFO_TEXT_FONT_PAGE_5.Height * line, (uint8_t *)&string_buffer[0], LEFT_MODE);
	line++;

	snprintf(string_buffer,100, "Codec: Cirrus Logic WM8994, 24 Bit, 2x ADC + 4x DAC");
	BSP_LCD_DisplayStringAt(10, LCD_TEXT_Y_OFFSET +  INFO_TEXT_FONT_PAGE_5.Height * line, (uint8_t *)&string_buffer[0], LEFT_MODE);
	line = line +2;


	snprintf(string_buffer,100, "Abtastrate: %lu Hz", CODEC_SAMPLING_FREQUENCY);
	BSP_LCD_DisplayStringAt(10, LCD_TEXT_Y_OFFSET +  INFO_TEXT_FONT_PAGE_5.Height * line, (uint8_t *)&string_buffer[0], LEFT_MODE);
	line ++;

	snprintf(string_buffer,100, "Blockgroesse: %i Abtastwerte pro Kanal", BUFFER_SIZE_SAMPLES);
	BSP_LCD_DisplayStringAt(10, LCD_TEXT_Y_OFFSET +  INFO_TEXT_FONT_PAGE_5.Height * line, (uint8_t *)&string_buffer[0], LEFT_MODE);
	line++;

	snprintf(string_buffer,100, "Eingang: Stereo Line In (analog)");
	BSP_LCD_DisplayStringAt(10, LCD_TEXT_Y_OFFSET +  INFO_TEXT_FONT_PAGE_5.Height * line, (uint8_t *)&string_buffer[0], LEFT_MODE);
	line++;

	snprintf(string_buffer,100, "Ausgang: Stereo Line Out (analog)");
	BSP_LCD_DisplayStringAt(10, LCD_TEXT_Y_OFFSET +  INFO_TEXT_FONT_PAGE_5.Height * line, (uint8_t *)&string_buffer[0], LEFT_MODE);
	line++;

	#if USE_FP_PROCESSING_ON_CH1 == 1
		snprintf(string_buffer,100, "Kanal 1: Floating point Modus");
		BSP_LCD_DisplayStringAt(10, LCD_TEXT_Y_OFFSET +  INFO_TEXT_FONT_PAGE_5.Height * line, (uint8_t *)&string_buffer[0], LEFT_MODE);
	#else
		snprintf(string_buffer,100, "Kanal 1: Fixed point Modus");
		BSP_LCD_DisplayStringAt(10, LCD_TEXT_Y_OFFSET +  INFO_TEXT_FONT_PAGE_5.Height * line, (uint8_t *)&string_buffer[0], LEFT_MODE);
	#endif
	line++;

	#if USE_FP_PROCESSING_ON_CH2 == 1
		snprintf(string_buffer,100, "Kanal 2: Floating point Modus");
		BSP_LCD_DisplayStringAt(10, LCD_TEXT_Y_OFFSET +  INFO_TEXT_FONT_PAGE_5.Height * line, (uint8_t *)&string_buffer[0], LEFT_MODE);
	#else
		snprintf(string_buffer,100, "Kanal 2: Fixed point Modus");
		BSP_LCD_DisplayStringAt(10, LCD_TEXT_Y_OFFSET +  INFO_TEXT_FONT_PAGE_5.Height * line, (uint8_t *)&string_buffer[0], LEFT_MODE);
	#endif
	line = line + 2;

	snprintf(string_buffer,100, "Eingangspegel: full scale = 1.000 Vrms = 2.800 Vpp (fix)");
	BSP_LCD_DisplayStringAt(10, LCD_TEXT_Y_OFFSET +  INFO_TEXT_FONT_PAGE_5.Height * line, (uint8_t *)&string_buffer[0], LEFT_MODE);
	line++;

	vol_vpp = pow((double)10, (double)audio_output_volume_dB/(double)20) * 2.8;
	vol_rms = pow((double)10, (double)audio_output_volume_dB/(double)20);

	snprintf(string_buffer,100, "Ausgangspegel: full scale = %li.%.03li Vrms = %li.%.03li Vpp (var)  ", (int32_t)(vol_rms), (int32_t)fmod(vol_rms * 1000, (double)1000), (int32_t)(vol_vpp), (int32_t)fmod(vol_vpp * 1000, (double)1000));
	BSP_LCD_DisplayStringAt(10, LCD_TEXT_Y_OFFSET +  INFO_TEXT_FONT_PAGE_5.Height * line, (uint8_t *)&string_buffer[0], LEFT_MODE);
	line++;

	if(audio_output_volume_dB > 0){
		BSP_LCD_SetTextColor(LCD_COLOR_LIGHTRED);
		snprintf(string_buffer,100, "Achtung: Die Ausgangsstufe satturiert ab 2.8 Vpp,");
		BSP_LCD_DisplayStringAt(10, LCD_TEXT_Y_OFFSET +  INFO_TEXT_FONT_PAGE_5.Height * line, (uint8_t *)&string_buffer[0], LEFT_MODE);
		line++;

		snprintf(string_buffer,100, "         es kann zu Clipping kommen!");
		BSP_LCD_DisplayStringAt(10, LCD_TEXT_Y_OFFSET +  INFO_TEXT_FONT_PAGE_5.Height * line, (uint8_t *)&string_buffer[0], LEFT_MODE);
		line++;
	}

	else{
		snprintf(string_buffer,100, "                                                              ");
		BSP_LCD_DisplayStringAt(10, LCD_TEXT_Y_OFFSET +  INFO_TEXT_FONT_PAGE_5.Height * line, (uint8_t *)&string_buffer[0], LEFT_MODE);
		line++;
		snprintf(string_buffer,100, "                                                              ");
		BSP_LCD_DisplayStringAt(10, LCD_TEXT_Y_OFFSET +  INFO_TEXT_FONT_PAGE_5.Height * line, (uint8_t *)&string_buffer[0], LEFT_MODE);
		line++;
	}
	BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
}
/************************************************************************************/



/***********************************************************************************/
/*
	checkButtonsPageX

 	Test for all the possible inputs on the page X.
*/
/***********************************************************************************/
void checkButtonsPage1(void){
	static TS_StateTypeDef  TS_State={0};
	uint16_t i;

	static char last_user_text_line_mode_1[USER_TEXT_LENGTH + 1];
	static char last_user_text_line_mode_2[USER_TEXT_LENGTH + 1];
	static char last_user_text_line_mode_3[USER_TEXT_LENGTH + 1];
	static char last_user_text_line_mode_4[USER_TEXT_LENGTH + 1];
	static char last_user_text_line_mode_5[USER_TEXT_LENGTH + 1];

	volatile unsigned char buffer;

	if(TS_State.touchDetected == 1)    // test for new button press
	{
		BSP_TS_GetState(&TS_State);
	}
	else
	{
		BSP_TS_GetState(&TS_State);

		if(TS_State.touchDetected == 1)
		{
			for(i = 1; i<6; i++){	// test lower button row
				if((TS_State.touchX[0] > (10 + (i-1)*170)) && (TS_State.touchX[0] < (110 + (i-1)*170)) &&
				  (TS_State.touchY[0] > 370) && (TS_State.touchY[0] < 470) && (i != user_mode)) {
					user_mode = i;
					drawButtonRow();
					updatePage1();
					drawTitle();
				 }
			}

			// test volume buttons
			if((TS_State.touchX[0] > 690) && (TS_State.touchX[0] < 790) &&
			  (TS_State.touchY[0] > 104) && (TS_State.touchY[0] < 204)) {
				if((audio_output_volume_dB + CODEC_MAX_OUTPUT_VOLUE_STEP)  <= CODEC_MAX_OUTPUT_VOLUE_dB){
					audio_output_volume_dB = audio_output_volume_dB + CODEC_MAX_OUTPUT_VOLUE_STEP;
					wm8994_SetOutputVolume_dB(AUDIO_I2C_ADDRESS, audio_output_volume_dB);
					updatePage1();
				}
			}

			if((TS_State.touchX[0] > 690) && (TS_State.touchX[0] < 790) &&
			  (TS_State.touchY[0] > 237) && (TS_State.touchY[0] < 337)) {
				if((audio_output_volume_dB - CODEC_MAX_OUTPUT_VOLUE_STEP)  >=  CODEC_MIN_OUTPUT_VOLUE_dB){
					audio_output_volume_dB = audio_output_volume_dB - CODEC_MAX_OUTPUT_VOLUE_STEP;
					wm8994_SetOutputVolume_dB(AUDIO_I2C_ADDRESS, audio_output_volume_dB);
					updatePage1();
				}
			}
		}
	}



	// update the user text in tihs mode too
	switch(user_mode){
		case 0:
			BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
			BSP_LCD_FillRect(0, 336, 689, 20);
		break;

		case 1:
			if(strcmp(last_user_text_line_mode_1, user_text_line_mode_1) != 0){
				BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
				BSP_LCD_FillRect(0, 336, 689, 20);
				BSP_LCD_SetBackColor(LCD_COLOR_BLACK);
				BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
				BSP_LCD_SetFont(&INFO_TEXT_FONT_PAGE_1);
				BSP_LCD_DisplayStringAt(10, LCD_TEXT_Y_OFFSET +  INFO_TEXT_FONT_PAGE_1.Height * 12, (uint8_t *)&user_text_line_mode_1[0], LEFT_MODE);
				strcpy(last_user_text_line_mode_1, user_text_line_mode_1);
			}
		break;

		case 2:
			if(strcmp(last_user_text_line_mode_2, user_text_line_mode_2) != 0){
				BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
				BSP_LCD_FillRect(0, 336, 689, 20);
				BSP_LCD_SetBackColor(LCD_COLOR_BLACK);
				BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
				BSP_LCD_SetFont(&INFO_TEXT_FONT_PAGE_1);
				BSP_LCD_DisplayStringAt(10, LCD_TEXT_Y_OFFSET +  INFO_TEXT_FONT_PAGE_1.Height * 12, (uint8_t *)&user_text_line_mode_2[0], LEFT_MODE);
				strcpy(last_user_text_line_mode_2, user_text_line_mode_2);
			}
		break;

		case 3:
			if(strcmp(last_user_text_line_mode_3, user_text_line_mode_3) != 0){
				BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
				BSP_LCD_FillRect(0, 336, 689, 20);
				BSP_LCD_SetBackColor(LCD_COLOR_BLACK);
				BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
				BSP_LCD_SetFont(&INFO_TEXT_FONT_PAGE_1);
				BSP_LCD_DisplayStringAt(10, LCD_TEXT_Y_OFFSET +  INFO_TEXT_FONT_PAGE_1.Height * 12, (uint8_t *)&user_text_line_mode_3[0], LEFT_MODE);
				strcpy(last_user_text_line_mode_3, user_text_line_mode_3);
			}
		break;

		case 4:
			if(strcmp(last_user_text_line_mode_4, user_text_line_mode_4) != 0){
				BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
				BSP_LCD_FillRect(0, 336, 689, 20);
				BSP_LCD_SetBackColor(LCD_COLOR_BLACK);
				BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
				BSP_LCD_SetFont(&INFO_TEXT_FONT_PAGE_1);
				BSP_LCD_DisplayStringAt(10, LCD_TEXT_Y_OFFSET +  INFO_TEXT_FONT_PAGE_1.Height * 12, (uint8_t *)&user_text_line_mode_4[0], LEFT_MODE);
				strcpy(last_user_text_line_mode_4, user_text_line_mode_4);
			}
		break;

		case 5:
			if(buffer = strcmp(last_user_text_line_mode_5, user_text_line_mode_5) != 0){
				BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
				BSP_LCD_FillRect(0, 336, 689, 20);
				BSP_LCD_SetBackColor(LCD_COLOR_BLACK);
				BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
				BSP_LCD_SetFont(&INFO_TEXT_FONT_PAGE_1);
				BSP_LCD_DisplayStringAt(10, LCD_TEXT_Y_OFFSET +  INFO_TEXT_FONT_PAGE_1.Height * 12, (uint8_t *)&user_text_line_mode_5[0], LEFT_MODE);
				strcpy(last_user_text_line_mode_5, user_text_line_mode_5);
			}
		break;
	}


}
/************************************************************************************/

/************************************************************************************/
void checkButtonsPage2(void){
	static TS_StateTypeDef  TS_State={0};
	uint32_t i;

	if(TS_State.touchDetected == 1)    // test for new button press
	{
		BSP_TS_GetState(&TS_State);
	}
	else
	{
		BSP_TS_GetState(&TS_State);

		if(TS_State.touchDetected == 1)
		{
			for(i = 0; i < 4; i++){
				if((TS_State.touchX[0] > (125+150*i)) && (TS_State.touchX[0] < (250+150*i)) &&
				  (TS_State.touchY[0] > 65) && (TS_State.touchY[0] < 115)) {

					if(i == 0){
						measure_CH1_in = measure_CH1_in ^ 1;
					}

					if(i == 1){
						measure_CH2_in = measure_CH2_in ^ 1;
					}

					if(i == 2){
						measure_CH1_out = measure_CH1_out ^ 1;
					}

					if(i == 3){
						measure_CH2_out = measure_CH2_out ^ 1;
					}

					updatePage2();
					return;
				}
			}

			if((TS_State.touchX[0] > 0) && (TS_State.touchX[0] < 800) &&
			  (TS_State.touchY[0] > 72) && (TS_State.touchY[0] < 480)) {
				updatePage2();
			}
		}
	}
}
/************************************************************************************/

/************************************************************************************/
void checkButtonsPage3(void){
	static TS_StateTypeDef  TS_State={0};
	uint32_t i;

	if(TS_State.touchDetected == 1)    // test for new button press
	{
		BSP_TS_GetState(&TS_State);
	}
	else
	{
		BSP_TS_GetState(&TS_State);

		if(TS_State.touchDetected == 1)
		{
			for(i = 0; i < 4; i++){
				if((TS_State.touchX[0] > (125+150*i)) && (TS_State.touchX[0] < (250+150*i)) &&
				  (TS_State.touchY[0] > 65) && (TS_State.touchY[0] < 115)) {

					if(i == 0){
						measure_CH1_in = measure_CH1_in ^ 1;
					}

					if(i == 1){
						measure_CH2_in = measure_CH2_in ^ 1;
					}

					if(i == 2){
						measure_CH1_out = measure_CH1_out ^ 1;
					}

					if(i == 3){
						measure_CH2_out = measure_CH2_out ^ 1;
					}

					updatePage3();
					return;
				}
			}

			if((TS_State.touchX[0] > 0) && (TS_State.touchX[0] < 800) &&
			  (TS_State.touchY[0] > 72) && (TS_State.touchY[0] < 480)) {
				updatePage3();
			}
		}
	}
}
/************************************************************************************/

/************************************************************************************/
void checkButtonsPage4(void){
	static TS_StateTypeDef  TS_State={0};

	if(TS_State.touchDetected == 1)    // test for new button press
	{
		BSP_TS_GetState(&TS_State);
	}
	else
	{
		BSP_TS_GetState(&TS_State);

		if(TS_State.touchDetected == 1)
		{
			if((TS_State.touchX[0] > 0) && (TS_State.touchX[0] < 230) && 	// wav sinus
			  (TS_State.touchY[0] > 104) && (TS_State.touchY[0] < 204)) {
				wav_gen = WAV_SINUS;
				updatePage4();
			}

			if((TS_State.touchX[0] > 0) && (TS_State.touchX[0] < 230) && 	// wav rectange
			  (TS_State.touchY[0] > 237) && (TS_State.touchY[0] < 337)) {
				wav_gen = WAV_RECTANGLE;
				updatePage4();
			}

			if((TS_State.touchX[0] > 0) && (TS_State.touchX[0] < 230) && 	// wav sawtooth
			  (TS_State.touchY[0] > 370) && (TS_State.touchY[0] < 480)) {
				wav_gen = WAV_SAWTOOTH;
				updatePage4();
			}

			if((TS_State.touchX[0] > 290) && (TS_State.touchX[0] < 510) && 	// frequency + 100 Hz
			  (TS_State.touchY[0] > 137) && (TS_State.touchY[0] < 237)) {
				if((wav_gen_output_frequency + 100) <= (CODEC_SAMPLING_FREQUENCY / 2)){
					wav_gen_output_frequency = wav_gen_output_frequency + 100;
					drawWaveGenInfo();
				}
			}

			if((TS_State.touchX[0] > 290) && (TS_State.touchX[0] < 510) && 	// frequency - 100 Hz
			  (TS_State.touchY[0] > 337) && (TS_State.touchY[0] < 437)) {
				if(wav_gen_output_frequency > 100){
					wav_gen_output_frequency = wav_gen_output_frequency - 100;
					drawWaveGenInfo();
				}
			}

			if((TS_State.touchX[0] > 570) && (TS_State.touchX[0] < 800) && 	// volume + 1 dB
			  (TS_State.touchY[0] > 137) && (TS_State.touchY[0] < 237)) {
				if((wav_gen_output_volume_dB + CODEC_MAX_OUTPUT_VOLUE_STEP) <= CODEC_MAX_OUTPUT_VOLUE_dB){
					wav_gen_output_volume_dB = wav_gen_output_volume_dB + CODEC_MAX_OUTPUT_VOLUE_STEP;
					drawWaveGenInfo();
					wm8994_SetOutputVolume_dB(AUDIO_I2C_ADDRESS, wav_gen_output_volume_dB);
				}
			}

			if((TS_State.touchX[0] > 570) && (TS_State.touchX[0] < 800) && 	// volume - 1 dB
			  (TS_State.touchY[0] > 337) && (TS_State.touchY[0] < 437)) {
				if((wav_gen_output_volume_dB - CODEC_MAX_OUTPUT_VOLUE_STEP) >= CODEC_MIN_OUTPUT_VOLUE_dB){
					wav_gen_output_volume_dB = wav_gen_output_volume_dB - CODEC_MAX_OUTPUT_VOLUE_STEP;
					drawWaveGenInfo();
					wm8994_SetOutputVolume_dB(AUDIO_I2C_ADDRESS, wav_gen_output_volume_dB);
				}
			}
		}
	}
}
/************************************************************************************/

/************************************************************************************/
void checkButtonsPage5(void){} // do nothing here
/************************************************************************************/


/***********************************************************************************/
/*
	updatePageX

 	Update only the changed information on the page X.
*/
/***********************************************************************************/
void updatePage1(void){
	char string_buffer[100];
	uint16_t line;
	float vol_vpp;
	float vol_rms;

	BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
	BSP_LCD_FillRect(0, 72, 689, 296);

	BSP_LCD_SetBackColor(LCD_COLOR_BLACK);
	BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
	BSP_LCD_SetFont(&INFO_TEXT_FONT_PAGE_1);

	switch(user_mode){
		case 0:
			snprintf(string_buffer,100, NO_MODE_LINE_1);
			BSP_LCD_DisplayStringAt(10, LCD_TEXT_Y_OFFSET, (uint8_t *)&string_buffer[0], LEFT_MODE);
			line = 1;

			snprintf(string_buffer,100, NO_MODE_LINE_2);
			BSP_LCD_DisplayStringAt(10, LCD_TEXT_Y_OFFSET +  INFO_TEXT_FONT_PAGE_1.Height * line, (uint8_t *)&string_buffer[0], LEFT_MODE);
			line++;

			snprintf(string_buffer,100, NO_MODE_LINE_3);
			BSP_LCD_DisplayStringAt(10, LCD_TEXT_Y_OFFSET +  INFO_TEXT_FONT_PAGE_1.Height * line, (uint8_t *)&string_buffer[0], LEFT_MODE);
			line++;

			snprintf(string_buffer,100, NO_MODE_LINE_4);
			BSP_LCD_DisplayStringAt(10, LCD_TEXT_Y_OFFSET +  INFO_TEXT_FONT_PAGE_1.Height * line, (uint8_t *)&string_buffer[0], LEFT_MODE);
			line++;

			snprintf(string_buffer,100, NO_MODE_LINE_5);
			BSP_LCD_DisplayStringAt(10, LCD_TEXT_Y_OFFSET +  INFO_TEXT_FONT_PAGE_1.Height * line, (uint8_t *)&string_buffer[0], LEFT_MODE);
			line++;
		break;

		case 1:
			snprintf(string_buffer,100, MODE_1_LINE_1);
			BSP_LCD_DisplayStringAt(10, LCD_TEXT_Y_OFFSET, (uint8_t *)&string_buffer[0], LEFT_MODE);
			line = 1;

			snprintf(string_buffer,100, MODE_1_LINE_2);
			BSP_LCD_DisplayStringAt(10, LCD_TEXT_Y_OFFSET +  INFO_TEXT_FONT_PAGE_1.Height * line, (uint8_t *)&string_buffer[0], LEFT_MODE);
			line++;

			snprintf(string_buffer,100, MODE_1_LINE_3);
			BSP_LCD_DisplayStringAt(10, LCD_TEXT_Y_OFFSET +  INFO_TEXT_FONT_PAGE_1.Height * line, (uint8_t *)&string_buffer[0], LEFT_MODE);
			line++;

			snprintf(string_buffer,100, MODE_1_LINE_4);
			BSP_LCD_DisplayStringAt(10, LCD_TEXT_Y_OFFSET +  INFO_TEXT_FONT_PAGE_1.Height * line, (uint8_t *)&string_buffer[0], LEFT_MODE);
			line++;

			snprintf(string_buffer,100, MODE_1_LINE_5);
			BSP_LCD_DisplayStringAt(10, LCD_TEXT_Y_OFFSET +  INFO_TEXT_FONT_PAGE_1.Height * line, (uint8_t *)&string_buffer[0], LEFT_MODE);
			line++;

			BSP_LCD_DisplayStringAt(10, LCD_TEXT_Y_OFFSET +  INFO_TEXT_FONT_PAGE_1.Height * 12, (uint8_t *)&user_text_line_mode_1[0], LEFT_MODE);
		break;

		case 2:
			snprintf(string_buffer,100, MODE_2_LINE_1);
			BSP_LCD_DisplayStringAt(10, LCD_TEXT_Y_OFFSET, (uint8_t *)&string_buffer[0], LEFT_MODE);
			line = 1;

			snprintf(string_buffer,100, MODE_2_LINE_2);
			BSP_LCD_DisplayStringAt(10, LCD_TEXT_Y_OFFSET +  INFO_TEXT_FONT_PAGE_1.Height * line, (uint8_t *)&string_buffer[0], LEFT_MODE);
			line++;

			snprintf(string_buffer,100, MODE_2_LINE_3);
			BSP_LCD_DisplayStringAt(10, LCD_TEXT_Y_OFFSET +  INFO_TEXT_FONT_PAGE_1.Height * line, (uint8_t *)&string_buffer[0], LEFT_MODE);
			line++;

			snprintf(string_buffer,100, MODE_2_LINE_4);
			BSP_LCD_DisplayStringAt(10, LCD_TEXT_Y_OFFSET +  INFO_TEXT_FONT_PAGE_1.Height * line, (uint8_t *)&string_buffer[0], LEFT_MODE);
			line++;

			snprintf(string_buffer,100, MODE_2_LINE_5);
			BSP_LCD_DisplayStringAt(10, LCD_TEXT_Y_OFFSET +  INFO_TEXT_FONT_PAGE_1.Height * line, (uint8_t *)&string_buffer[0], LEFT_MODE);
			line++;

			BSP_LCD_DisplayStringAt(10, LCD_TEXT_Y_OFFSET +  INFO_TEXT_FONT_PAGE_1.Height * 12, (uint8_t *)&user_text_line_mode_2[0], LEFT_MODE);
		break;

		case 3:
			snprintf(string_buffer,100, MODE_3_LINE_1);
			BSP_LCD_DisplayStringAt(10, LCD_TEXT_Y_OFFSET, (uint8_t *)&string_buffer[0], LEFT_MODE);
			line = 1;

			snprintf(string_buffer,100, MODE_3_LINE_2);
			BSP_LCD_DisplayStringAt(10, LCD_TEXT_Y_OFFSET +  INFO_TEXT_FONT_PAGE_1.Height * line, (uint8_t *)&string_buffer[0], LEFT_MODE);
			line++;

			snprintf(string_buffer,100, MODE_3_LINE_3);
			BSP_LCD_DisplayStringAt(10, LCD_TEXT_Y_OFFSET +  INFO_TEXT_FONT_PAGE_1.Height * line, (uint8_t *)&string_buffer[0], LEFT_MODE);
			line++;

			snprintf(string_buffer,100, MODE_3_LINE_4);
			BSP_LCD_DisplayStringAt(10, LCD_TEXT_Y_OFFSET +  INFO_TEXT_FONT_PAGE_1.Height * line, (uint8_t *)&string_buffer[0], LEFT_MODE);
			line++;

			snprintf(string_buffer,100, MODE_3_LINE_5);
			BSP_LCD_DisplayStringAt(10, LCD_TEXT_Y_OFFSET +  INFO_TEXT_FONT_PAGE_1.Height * line, (uint8_t *)&string_buffer[0], LEFT_MODE);
			line++;

			BSP_LCD_DisplayStringAt(10, LCD_TEXT_Y_OFFSET +  INFO_TEXT_FONT_PAGE_1.Height * 12, (uint8_t *)&user_text_line_mode_3[0], LEFT_MODE);
		break;

		case 4:
			snprintf(string_buffer,100, MODE_4_LINE_1);
			BSP_LCD_DisplayStringAt(10, LCD_TEXT_Y_OFFSET, (uint8_t *)&string_buffer[0], LEFT_MODE);
			line = 1;

			snprintf(string_buffer,100, MODE_4_LINE_2);
			BSP_LCD_DisplayStringAt(10, LCD_TEXT_Y_OFFSET +  INFO_TEXT_FONT_PAGE_1.Height * line, (uint8_t *)&string_buffer[0], LEFT_MODE);
			line++;

			snprintf(string_buffer,100, MODE_4_LINE_3);
			BSP_LCD_DisplayStringAt(10, LCD_TEXT_Y_OFFSET +  INFO_TEXT_FONT_PAGE_1.Height * line, (uint8_t *)&string_buffer[0], LEFT_MODE);
			line++;

			snprintf(string_buffer,100, MODE_4_LINE_4);
			BSP_LCD_DisplayStringAt(10, LCD_TEXT_Y_OFFSET +  INFO_TEXT_FONT_PAGE_1.Height * line, (uint8_t *)&string_buffer[0], LEFT_MODE);
			line++;

			snprintf(string_buffer,100, MODE_4_LINE_5);
			BSP_LCD_DisplayStringAt(10, LCD_TEXT_Y_OFFSET +  INFO_TEXT_FONT_PAGE_1.Height * line, (uint8_t *)&string_buffer[0], LEFT_MODE);
			line++;

			BSP_LCD_DisplayStringAt(10, LCD_TEXT_Y_OFFSET +  INFO_TEXT_FONT_PAGE_1.Height * 12, (uint8_t *)&user_text_line_mode_4[0], LEFT_MODE);
		break;

		case 5:
			snprintf(string_buffer,100, MODE_5_LINE_1);
			BSP_LCD_DisplayStringAt(10, LCD_TEXT_Y_OFFSET, (uint8_t *)&string_buffer[0], LEFT_MODE);
			line = 1;

			snprintf(string_buffer,100, MODE_5_LINE_2);
			BSP_LCD_DisplayStringAt(10, LCD_TEXT_Y_OFFSET +  INFO_TEXT_FONT_PAGE_1.Height * line, (uint8_t *)&string_buffer[0], LEFT_MODE);
			line++;

			snprintf(string_buffer,100, MODE_5_LINE_3);
			BSP_LCD_DisplayStringAt(10, LCD_TEXT_Y_OFFSET +  INFO_TEXT_FONT_PAGE_1.Height * line, (uint8_t *)&string_buffer[0], LEFT_MODE);
			line++;

			snprintf(string_buffer,100, MODE_5_LINE_4);
			BSP_LCD_DisplayStringAt(10, LCD_TEXT_Y_OFFSET +  INFO_TEXT_FONT_PAGE_1.Height * line, (uint8_t *)&string_buffer[0], LEFT_MODE);
			line++;

			snprintf(string_buffer,100, MODE_5_LINE_5);
			BSP_LCD_DisplayStringAt(10, LCD_TEXT_Y_OFFSET +  INFO_TEXT_FONT_PAGE_1.Height * line, (uint8_t *)&string_buffer[0], LEFT_MODE);
			line++;

			BSP_LCD_DisplayStringAt(10, LCD_TEXT_Y_OFFSET +  INFO_TEXT_FONT_PAGE_1.Height * 12, (uint8_t *)&user_text_line_mode_5[0], LEFT_MODE);
		break;
	}

	line++;

	snprintf(string_buffer,100, "Eingangspegel: full scale = 2.800 Vpp");
	BSP_LCD_DisplayStringAt(10, LCD_TEXT_Y_OFFSET +  INFO_TEXT_FONT_PAGE_5.Height * line, (uint8_t *)&string_buffer[0], LEFT_MODE);
	line++;
	line++;

	vol_vpp = pow((double)10, (double)audio_output_volume_dB/(double)20) * 2.8;
	vol_rms = pow((double)10, (double)audio_output_volume_dB/(double)20);

	snprintf(string_buffer,100, "Spannungsverstaerkung: %li dB = %li.%.03li", (int32_t)audio_output_volume_dB, (int32_t)(vol_rms), (int32_t)fmod(vol_rms * 1000, (double)1000));
	BSP_LCD_DisplayStringAt(10, LCD_TEXT_Y_OFFSET +  INFO_TEXT_FONT_PAGE_5.Height * line, (uint8_t *)&string_buffer[0], LEFT_MODE);
	line++;

	snprintf(string_buffer,100, "Ausgangspegel: full scale = %li.%.03li Vpp",(int32_t)(vol_vpp), (int32_t)fmod(vol_vpp * 1000, (double)1000));
	BSP_LCD_DisplayStringAt(10, LCD_TEXT_Y_OFFSET +  INFO_TEXT_FONT_PAGE_5.Height * line, (uint8_t *)&string_buffer[0], LEFT_MODE);
	line++;

	if(audio_output_volume_dB > 0){
		BSP_LCD_SetTextColor(LCD_COLOR_LIGHTRED);
		snprintf(string_buffer,100, "Achtung: Die Ausgangsstufe satturiert ab");
		BSP_LCD_DisplayStringAt(10, LCD_TEXT_Y_OFFSET +  INFO_TEXT_FONT_PAGE_1.Height * line, (uint8_t *)&string_buffer[0], LEFT_MODE);
		line++;

		snprintf(string_buffer,100, "2.8 Vpp, es kann zu Clipping kommen!");
		BSP_LCD_DisplayStringAt(10, LCD_TEXT_Y_OFFSET +  INFO_TEXT_FONT_PAGE_1.Height * line, (uint8_t *)&string_buffer[0], LEFT_MODE);
		line++;
	}
	BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
}
/************************************************************************************/

/************************************************************************************/
void updatePage2(void){	// draw new measurement onto the screen
	uint32_t n, i;
	float length;
	int32_t last_point, actual_point;
	volatile int32_t * data = plotBufferCH1_in;
	char string_buffer[100];
	uint32_t skip_channel = 0;

	__disable_irq();
	fetchNewPlotDataNow = 1;	// request new data
	__enable_irq();

	SCB_CleanInvalidateDCache();			// make sure to read the new data delivered by the DMA

	HAL_Delay(100);

	// init the screen
	BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
	BSP_LCD_SetBackColor(LCD_COLOR_WHITE);
	BSP_LCD_FillRect(0, 72, 800, 408);
	BSP_LCD_DrawBitmap(0, 80, (uint8_t *)(QSPI_BASE_ADR + TIME_GRAPH_ADR));


	BSP_LCD_SetFont(&Font16);

	while(newPlotDataReady == 0){} // wait for new data

	for(n = 0; n < 4; n++){
		if(n == 0){
			BSP_LCD_SetTextColor(LCD_COLOR_RED);
			snprintf(string_buffer,100, "Refresh");
			BSP_LCD_DisplayStringAt(700, 460, (uint8_t *)&string_buffer[0], LEFT_MODE);

			if(measure_CH1_in == 1){
				data = plotBufferCH1_in;
				BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
				BSP_LCD_SetBackColor(LCD_COLOR_RED);
				skip_channel = 0;
			}

			else{
				skip_channel = 1;
				BSP_LCD_SetTextColor(LCD_COLOR_RED);
				BSP_LCD_SetBackColor(LCD_COLOR_WHITE);
			}

			snprintf(string_buffer,100, "CH 1 in");
			BSP_LCD_DisplayStringAt(130, 85, (uint8_t *)&string_buffer[0], LEFT_MODE);
			BSP_LCD_SetTextColor(LCD_COLOR_RED);
		}

		if(n == 1){
			if(measure_CH2_in == 1){
				data = plotBufferCH2_in;
				BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
				BSP_LCD_SetBackColor(LCD_COLOR_DARKGREEN);
				skip_channel = 0;
			}

			else{
				skip_channel = 1;
				BSP_LCD_SetTextColor(LCD_COLOR_DARKGREEN);
				BSP_LCD_SetBackColor(LCD_COLOR_WHITE);
			}

			snprintf(string_buffer,100, "CH 2 in");
			BSP_LCD_DisplayStringAt(130 + 150, 85, (uint8_t *)&string_buffer[0], LEFT_MODE);
			BSP_LCD_SetTextColor(LCD_COLOR_DARKGREEN);
		}

		if(n == 2){
			if(measure_CH1_out == 1){
				data = plotBufferCH1_out;
				BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
				BSP_LCD_SetBackColor(LCD_COLOR_BLUE);
				skip_channel = 0;
			}

			else{
				skip_channel = 1;
				BSP_LCD_SetTextColor(LCD_COLOR_BLUE);
				BSP_LCD_SetBackColor(LCD_COLOR_WHITE);
			}

			snprintf(string_buffer,100, "CH 1 out");
			BSP_LCD_DisplayStringAt(130 + 150*2, 85, (uint8_t *)&string_buffer[0], LEFT_MODE);
			BSP_LCD_SetTextColor(LCD_COLOR_BLUE);
		}

		if(n == 3){
			if(measure_CH2_out == 1){
				data = plotBufferCH2_out;
				BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
				BSP_LCD_SetBackColor(LCD_COLOR_BLACK);
				skip_channel = 0;
			}

			else{
				skip_channel = 1;
				BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
				BSP_LCD_SetBackColor(LCD_COLOR_WHITE);
			}

			snprintf(string_buffer,100, "CH 2 out");
			BSP_LCD_DisplayStringAt(130 + 150*3, 85, (uint8_t *)&string_buffer[0], LEFT_MODE);
			BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
		}

		if(skip_channel == 0){
			last_point = roundf((data[0] /TIME_PLOT_Y_SCALING));

			for(i = 0; i < (TIME_PLOT_DATA_SIZE - 1); i++){

				actual_point = roundf((data[i] /TIME_PLOT_Y_SCALING));
				length = abs(actual_point - last_point);

				if(length < 1){
					length = 1;
				}

				if(actual_point > last_point){
					BSP_LCD_DrawVLine(50 +i ,480 - (actual_point + 43 + TIME_PLOT_Y_SIZE/2),length);
				}

				else{
					BSP_LCD_DrawVLine(50 +i ,480 - (last_point + 43 + TIME_PLOT_Y_SIZE/2),length);
				}

				last_point = actual_point;
			}
		}
	}
}
/************************************************************************************/

/************************************************************************************/
void updatePage3(void){
	uint32_t n, i, line;
	float length;
	volatile int32_t * data = plotBufferCH1_in;
	char string_buffer[100];
	float fft_data_buffer[FREQ_PLOT_FFT_SIZE];
	float calc[FREQ_PLOT_FFT_SIZE];
	uint32_t skip_channel = 0;

	__disable_irq();
	fetchNewPlotDataNow = 1;	// request new data
	__enable_irq();

	HAL_Delay(100);

	// init the screen
	BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
	BSP_LCD_SetBackColor(LCD_COLOR_WHITE);
	BSP_LCD_FillRect(0, 72, 800, 408);
	BSP_LCD_DrawBitmap(0, 80, (uint8_t *)(QSPI_BASE_ADR + FREQ_GRAPH_ADR));

	BSP_LCD_SetFont(&Font16);

	while(newPlotDataReady == 0){ HAL_Delay(3); } // wait for new data

	SCB_CleanInvalidateDCache();			// make sure to read the new data delivered by the DMA

	HAL_Delay(20);

	for(n = 0; n < 4; n++){
		if(n == 0){
			BSP_LCD_SetTextColor(LCD_COLOR_RED);
			snprintf(string_buffer,100, "Refresh");
			BSP_LCD_DisplayStringAt(700, 460, (uint8_t *)&string_buffer[0], LEFT_MODE);

			if(measure_CH1_in == 1){
				data = plotBufferCH1_in;
				BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
				BSP_LCD_SetBackColor(LCD_COLOR_RED);
				skip_channel = 0;
			}

			else{
				skip_channel = 1;
				BSP_LCD_SetTextColor(LCD_COLOR_RED);
				BSP_LCD_SetBackColor(LCD_COLOR_WHITE);
			}

			snprintf(string_buffer,100, "CH 1 in");
			BSP_LCD_DisplayStringAt(130, 85, (uint8_t *)&string_buffer[0], LEFT_MODE);
			BSP_LCD_SetTextColor(LCD_COLOR_RED);
		}

		if(n == 1){
			if(measure_CH2_in == 1){
				data = plotBufferCH2_in;
				BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
				BSP_LCD_SetBackColor(LCD_COLOR_DARKGREEN);
				skip_channel = 0;
			}

			else{
				skip_channel = 1;
				BSP_LCD_SetTextColor(LCD_COLOR_DARKGREEN);
				BSP_LCD_SetBackColor(LCD_COLOR_WHITE);
			}

			snprintf(string_buffer,100, "CH 2 in");
			BSP_LCD_DisplayStringAt(130 + 150, 85, (uint8_t *)&string_buffer[0], LEFT_MODE);
			BSP_LCD_SetTextColor(LCD_COLOR_DARKGREEN);
		}

		if(n == 2){
			if(measure_CH1_out == 1){
				data = plotBufferCH1_out;
				BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
				BSP_LCD_SetBackColor(LCD_COLOR_BLUE);
				skip_channel = 0;
			}

			else{
				skip_channel = 1;
				BSP_LCD_SetTextColor(LCD_COLOR_BLUE);
				BSP_LCD_SetBackColor(LCD_COLOR_WHITE);
			}

			snprintf(string_buffer,100, "CH 1 out");
			BSP_LCD_DisplayStringAt(130 + 150*2, 85, (uint8_t *)&string_buffer[0], LEFT_MODE);
			BSP_LCD_SetTextColor(LCD_COLOR_BLUE);
		}

		if(n == 3){
			if(measure_CH2_out == 1){
				data = plotBufferCH2_out;
				BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
				BSP_LCD_SetBackColor(LCD_COLOR_BLACK);
				skip_channel = 0;
			}

			else{
				skip_channel = 1;
				BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
				BSP_LCD_SetBackColor(LCD_COLOR_WHITE);
			}

			snprintf(string_buffer,100, "CH 2 out");
			BSP_LCD_DisplayStringAt(130 + 150*3, 85, (uint8_t *)&string_buffer[0], LEFT_MODE);
			BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
		}

		if(skip_channel == 0){
			// window
			for(i = 0; i < FREQ_PLOT_FFT_SIZE; i++){
				calc[i] = data[i] * cheb_win_4096_190dB[i];
			}

			// Compute the FFT of the high pass filtered version of the signal.
			arm_rfft_fast_f32(&rFFT_plot_inst, calc, fft_data_buffer,0);

			// compute the complex magnitude values for the FFT output
			arm_cmplx_mag_f32(&fft_data_buffer[0], &fft_data_buffer[0], FREQ_PLOT_FFT_SIZE/2);

			// downsample to display resolution
			calc[0] = fft_data_buffer[0]  / 8388608 / 0.26828 * 2; // compensate for ADC resolution and window dampening
			calc[0] = 20*log10(calc[0]/FREQ_PLOT_FFT_SIZE);

			length = roundf(((calc[0] + 200) * FREQ_PLOT_Y_SIZE / 200));
			if(length < 1){
				length = 1;
			}
			BSP_LCD_DrawVLine(60 ,437-length,length);

			for(i = 1; i < FREQ_PLOT_DATA_SIZE; i++){
				line = lroundf((float)i / 2 * FREQ_PLOT_FFT_SIZE / FREQ_PLOT_DATA_SIZE);	// get line index
				//arm_max_f32(&fft_data_buffer[line-1], 3, &calc[i], &line);	// get the maximum of the 3 values

				// compensate for ADC resolution and window dampening
				calc[i] = (fft_data_buffer[line-1] + fft_data_buffer[line] + fft_data_buffer[line+1]) /3  / 8388608 / 0.26828 * 2;	// get the average value

				if(calc[i]/FREQ_PLOT_FFT_SIZE <= 0){ // avoid log(x) for x <= 0 (not defined)
					calc[i] = - 500;
				}

				else{
					calc[i] = 20*log10(calc[i]/FREQ_PLOT_FFT_SIZE);
				}


				if(i < (FREQ_PLOT_DATA_SIZE - 1)){

					length = roundf(((calc[i] + 200) * FREQ_PLOT_Y_SIZE / 200));

					if(length < 1){
						length = 1;
					}

					BSP_LCD_DrawVLine(60+i ,437-length,length);
				}
			}
		}
	}
}
/************************************************************************************/

/************************************************************************************/
void updatePage4(void){
	BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
	BSP_LCD_FillRect(0, 72, 800, 408);
	drawWaveGenWaveforms();
}
/************************************************************************************/

/************************************************************************************/
void updatePage5(void){} // do nothing here
/************************************************************************************/




/***********************************************************************************/
/*
	diverse

 	Frequently used GUI elements.
*/
/***********************************************************************************/
void drawTitle(void){
	char string_buffer[100];

	// title bar
	BSP_LCD_SetTextColor(LCD_COLOR_BLUE);
	BSP_LCD_FillRect(0, 0, BSP_LCD_GetXSize(), Font24.Height * 3);

	// draw the arrows
	BSP_LCD_DrawBitmap(660, 0, (uint8_t *)(QSPI_BASE_ADR + PFEIL_RECHTS_ADR));
	BSP_LCD_DrawBitmap(0 ,0, (uint8_t *)(QSPI_BASE_ADR + PFEIL_LINKS_ADR));

	BSP_LCD_SetBackColor(LCD_COLOR_BLUE);
	BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
	BSP_LCD_SetFont(&Font24);

	// draw the title
	switch(menue_page){
		case PAGE1:
			snprintf(string_buffer,100, "DSV Praktikum");
			BSP_LCD_DisplayStringAt(0, 8, (uint8_t *)&string_buffer[0], CENTER_MODE);
			snprintf(string_buffer,100, PRAKTIKUM_NAME);
			BSP_LCD_DisplayStringAt(0, Font24.Height * 1+16, (uint8_t *)&string_buffer[0], CENTER_MODE);
		break;

		case PAGE2:
			snprintf(string_buffer,100, "Tool 1:");
			BSP_LCD_DisplayStringAt(0, 8, (uint8_t *)&string_buffer[0], CENTER_MODE);
			snprintf(string_buffer,100, "Oszilloskop");
			BSP_LCD_DisplayStringAt(0, Font24.Height * 1+16, (uint8_t *)&string_buffer[0], CENTER_MODE);
		break;

		case PAGE3:
			snprintf(string_buffer,100, "Tool 2:");
			BSP_LCD_DisplayStringAt(0, 8, (uint8_t *)&string_buffer[0], CENTER_MODE);
			snprintf(string_buffer,100, "Spektrumanalysator");
			BSP_LCD_DisplayStringAt(0, Font24.Height * 1+16, (uint8_t *)&string_buffer[0], CENTER_MODE);
		break;

		case PAGE4:
			snprintf(string_buffer,100, "Tool 3:");
			BSP_LCD_DisplayStringAt(0, 8, (uint8_t *)&string_buffer[0], CENTER_MODE);
			snprintf(string_buffer,100, "Funktionsgenerator");
			BSP_LCD_DisplayStringAt(0, Font24.Height * 1+16, (uint8_t *)&string_buffer[0], CENTER_MODE);
		break;

		case PAGE5:
			snprintf(string_buffer,100, "Informationen");
			BSP_LCD_DisplayStringAt(0, 8, (uint8_t *)&string_buffer[0], CENTER_MODE);
			snprintf(string_buffer,100, "HW und SW");
			BSP_LCD_DisplayStringAt(0, Font24.Height * 1+16, (uint8_t *)&string_buffer[0], CENTER_MODE);
		break;
	}

	BSP_LCD_SetBackColor(LCD_COLOR_WHITE);
	BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
	BSP_LCD_SetFont(&Font20);

	snprintf(string_buffer,100, "M = %lu", user_mode);
	BSP_LCD_DisplayStringAt(65, 52, (uint8_t *)&string_buffer[0], LEFT_MODE);

	snprintf(string_buffer,100, "B = %lu", user_button);
	BSP_LCD_DisplayStringAt(665, 52, (uint8_t *)&string_buffer[0], LEFT_MODE);
}
/************************************************************************************/

/************************************************************************************/
void checkPageSwitchButtons(void){
	static TS_StateTypeDef  TS_State={0};

	if(TS_State.touchDetected == 1)   // test for new button press
	{
		BSP_TS_GetState(&TS_State);
	}
	else
	{
		BSP_TS_GetState(&TS_State);

		if(TS_State.touchDetected == 1){
			// left arrow
			if((TS_State.touchX[0] > 0) && (TS_State.touchX[0] < 140) &&
			  (TS_State.touchY[0] > 0) && (TS_State.touchY[0] < 72)) {
				switch(menue_page){
					case PAGE1:
						menue_page = PAGE5;
					break;

					case PAGE2:
						menue_page = PAGE1;
					break;

					case PAGE3:
						menue_page = PAGE2;
					break;

					case PAGE4:
						menue_page = PAGE3;
					break;

					case PAGE5:
						menue_page = PAGE4;
					break;
				}

				drawTitle();
			}

			// right arrow
			if((TS_State.touchX[0] > 659) && (TS_State.touchX[0] < 800) &&
			  (TS_State.touchY[0] > 0) && (TS_State.touchY[0] < 72)) {
				switch(menue_page){
					case PAGE1:
						menue_page = PAGE2;
					break;

					case PAGE2:
						menue_page = PAGE3;
					break;

					case PAGE3:
						menue_page = PAGE4;
					break;

					case PAGE4:
						menue_page = PAGE5;
					break;

					case PAGE5:
						menue_page = PAGE1;
					break;
				}

				drawTitle();
			}
		}
	}
}
/************************************************************************************/


/*** Private Functions **************************************************************/
/************************************************************************************/

/************************************************************************************/
static void drawButtonRow(void){
	uint16_t i;
	char string_buffer[3];

	// draw the buttons
	for(i = 1; i < 6; i++){
	  BSP_LCD_SetFont(&Font24);

	  BSP_LCD_SetTextColor(LCD_COLOR_GREEN);
	  BSP_LCD_FillRect(10 + (i-1)*170, 370, 100, 100);

	  if(user_mode != i){
		  BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
		  BSP_LCD_FillRect(10 + (i-1)*170+10, 380, 80, 80);

		  BSP_LCD_SetTextColor(LCD_COLOR_GREEN);
		  BSP_LCD_SetBackColor(LCD_COLOR_BLACK);
		  snprintf(string_buffer,2, "%u", i);
		  BSP_LCD_DisplayStringAt(10 + (i-1)*170+42, 395 +  Font24.Height / 2, (uint8_t *)&string_buffer[0], LEFT_MODE);
	  }

	  else{
		  BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
		  BSP_LCD_SetBackColor(LCD_COLOR_GREEN);
		  snprintf(string_buffer,2, "%u", i);
		  BSP_LCD_DisplayStringAt(10 + (i-1)*170+42, 395 +  Font24.Height / 2, (uint8_t *)&string_buffer[0], LEFT_MODE);
	  }

	}
}
/************************************************************************************/

/************************************************************************************/
static void drawWaveGenWaveforms(void){
	char string_buffer[100];

	BSP_LCD_SetFont(&Font24);

	// sinus
	BSP_LCD_SetTextColor(LCD_COLOR_GREEN);
	BSP_LCD_FillRect(10, 104, 220, 100);

	if(wav_gen == WAV_SINUS){
		BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
		BSP_LCD_SetBackColor(LCD_COLOR_GREEN);
	}

	else{
		BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
		BSP_LCD_FillRect(20, 114, 200, 80);
		BSP_LCD_SetTextColor(LCD_COLOR_GREEN);
		BSP_LCD_SetBackColor(LCD_COLOR_BLACK);

	}
	snprintf(string_buffer,100, "Sinus");
	BSP_LCD_DisplayStringAt(30, 142, (uint8_t *)&string_buffer[0], LEFT_MODE);

	// rectangle
	BSP_LCD_SetTextColor(LCD_COLOR_GREEN);
	BSP_LCD_FillRect(10, 237, 220, 100);

	if(wav_gen == WAV_RECTANGLE){
	  BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
	  BSP_LCD_SetBackColor(LCD_COLOR_GREEN);

	}

	else{
	  BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
	  BSP_LCD_FillRect(20, 247, 200, 80);
	  BSP_LCD_SetTextColor(LCD_COLOR_GREEN);
	  BSP_LCD_SetBackColor(LCD_COLOR_BLACK);
	}

	snprintf(string_buffer,100, "Rechteck");
	BSP_LCD_DisplayStringAt(30, 275, (uint8_t *)&string_buffer[0], LEFT_MODE);


	// sawtooth
	BSP_LCD_SetTextColor(LCD_COLOR_GREEN);
	BSP_LCD_FillRect(10, 370, 220, 100);

	if(wav_gen == WAV_SAWTOOTH){
	  BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
	  BSP_LCD_SetBackColor(LCD_COLOR_GREEN);

	}

	else {
	  BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
	  BSP_LCD_FillRect(20, 380, 200, 80);
	  BSP_LCD_SetTextColor(LCD_COLOR_GREEN);
	  BSP_LCD_SetBackColor(LCD_COLOR_BLACK);
	}

	snprintf(string_buffer,100, "Saegezahn");
	BSP_LCD_DisplayStringAt(30, 408, (uint8_t *)&string_buffer[0], LEFT_MODE);

	// frequency
	BSP_LCD_SetTextColor(LCD_COLOR_GREEN);
	BSP_LCD_FillRect(290, 137, 220, 100);
	BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
	BSP_LCD_FillRect(300, 147, 200, 80);

	BSP_LCD_SetTextColor(LCD_COLOR_GREEN);
	BSP_LCD_FillRect(290, 337, 220, 100);
	BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
	BSP_LCD_FillRect(300, 347, 200, 80);


	BSP_LCD_SetTextColor(LCD_COLOR_GREEN);
	BSP_LCD_SetBackColor(LCD_COLOR_BLACK);

	snprintf(string_buffer,100, " + 100 Hz");
	BSP_LCD_DisplayStringAt(315, 175, (uint8_t *)&string_buffer[0], LEFT_MODE);

	snprintf(string_buffer,100, " - 100 Hz");
	BSP_LCD_DisplayStringAt(315, 375, (uint8_t *)&string_buffer[0], LEFT_MODE);

	// volume
	BSP_LCD_SetTextColor(LCD_COLOR_GREEN);
	BSP_LCD_FillRect(570, 137, 220, 100);
	BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
	BSP_LCD_FillRect(580, 147, 200, 80);

	BSP_LCD_SetTextColor(LCD_COLOR_GREEN);
	BSP_LCD_FillRect(570, 337, 220, 100);
	BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
	BSP_LCD_FillRect(580, 347, 200, 80);

	BSP_LCD_SetTextColor(LCD_COLOR_GREEN);
	BSP_LCD_SetBackColor(LCD_COLOR_BLACK);

	snprintf(string_buffer,100, " + 1 dB");
	BSP_LCD_DisplayStringAt(605, 175, (uint8_t *)&string_buffer[0], LEFT_MODE);

	snprintf(string_buffer,100, " - 1 dB");
	BSP_LCD_DisplayStringAt(605, 375, (uint8_t *)&string_buffer[0], LEFT_MODE);

	drawWaveGenInfo();
}
/************************************************************************************/

/************************************************************************************/
static void drawWaveGenInfo(void){
	char string_buffer[100];

	BSP_LCD_SetTextColor(LCD_COLOR_GREEN);
	BSP_LCD_SetBackColor(LCD_COLOR_BLACK);

	BSP_LCD_SetFont(&Font24);

	snprintf(string_buffer,100, "f = %i Hz   ", (int)wav_gen_output_frequency);
	BSP_LCD_DisplayStringAt(290, 275, (uint8_t *)&string_buffer[0], LEFT_MODE);

	snprintf(string_buffer,100, "vol. = %i dB ", (int)wav_gen_output_volume_dB);
	BSP_LCD_DisplayStringAt(570, 275, (uint8_t *)&string_buffer[0], LEFT_MODE);
}
/************************************************************************************/
