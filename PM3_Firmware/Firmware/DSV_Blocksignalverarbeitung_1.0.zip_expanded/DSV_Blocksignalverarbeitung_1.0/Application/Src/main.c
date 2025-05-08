/************************************************************************************/
/*
                             M A I N. C

 ------------------------------------------------------------------------------------

  (c) Copyright       ZHAW / ZSN
                      Technikumstr. 9
                      CH-8401 Winterthur

                      Author: 31.7.2017 / zols
                      Version: 17.8.2017 / wyrs

 ------------------------------------------------------------------------------------

  Subsystem         : APP (Application)
  Module            : main
  Type of SW-Unit   : C File

 ------------------------------------------------------------------------------------

  Description:
  ------------
  DSV Praktikum framework. The project is based on the "Audio_playback_and_record"
  example, see original header below.
 ------------------------------------------------------------------------------------

*/
/************************************************************************************/


/************************************************************************************/
/**
  ******************************************************************************
  * @file    Audio/Audio_playback_and_record/Src/main.c
  * @author  MCD Application Team
  * @version V1.2.1
  * @date    14-April-2017
  * @brief   Audio playback and record main file.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2017 STMicroelectronics International N.V.
  * All rights reserved.</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without 
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice, 
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other 
  *    contributors to this software may be used to endorse or promote products 
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this 
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under 
  *    this license is void and will automatically terminate your rights under 
  *    this license. 
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS" 
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT 
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT 
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/************************************************************************************/

/*** Imported external Objects ******************************************************/
/************************************************************************************/
#include <stdio.h>
#include "stm32f769i_discovery_lcd.h"
#include "stm32f769i_discovery.h"
#include "main.h"
#include "Codec.h"
#include "LAB_CONFIG.h"
#include "Pictures.h"
#include "Processing.h"
#include "stm32f769i_discovery_qspi.h"
#include "GUI.h"

#include "usbh_core.h"
#include "stm32f7xx_hal.h"
#include "stm32f769i_discovery_ts.h"

#include "usbh_diskio.h"

/*** Module Objects *****************************************************************/
/************************************************************************************/

/*** Constants **********************************************************************/

/*** Types **************************************************************************/

/*** Public Variables ***************************************************************/
char user_text_line_mode_1[USER_TEXT_LENGTH + 1];
char user_text_line_mode_2[USER_TEXT_LENGTH + 1];
char user_text_line_mode_3[USER_TEXT_LENGTH + 1];
char user_text_line_mode_4[USER_TEXT_LENGTH + 1];
char user_text_line_mode_5[USER_TEXT_LENGTH + 1];

/*** Private Function Prototypes ****************************************************/
static void SystemClock_Config(void);
// The USB host is not started as it is not needed for the Praktikum. Still the functions
// are kept here for easy integration if needed. To suppress undesired warning by the
// compiler the function is marked as unused.
static void USBH_UserProcess(USBH_HandleTypeDef *phost, uint8_t id) __attribute__ ((unused));
static void CPU_CACHE_Enable(void);
static void MPU_Config(void);


/*** Private Variables **************************************************************/
USBH_HandleTypeDef hUSBHost;



/*** Public Functions ***************************************************************/
/************************************************************************************/


/***********************************************************************************/
/*
	main
*/
/***********************************************************************************/
int main(void)
{
  uint32_t i = 0;
  menue_page_t last_menue_page = PAGE1;
  char string_buffer[100];
  uint32_t last_button_state = 0;
  uint32_t button_state_now = 0;

  // enable strict error checking
  SCB->SHCSR |= SCB_SHCSR_USGFAULTENA_Msk;	// usage fault
  SCB->SHCSR |= SCB_SHCSR_BUSFAULTENA_Msk;	// bus fault

  SCB->CCR |= SCB_CCR_DIV_0_TRP_Msk;		// divison by 0
  SCB->CCR |= SCB_CCR_UNALIGN_TRP_Msk;		// unaligned access fault

  /* Configure the MPU attributes as Write Through */
  MPU_Config();


  /* STM32F7xx HAL library initialization:
       - Configure the Flash ART accelerator on ITCM interface
       - Configure the Systick to generate an interrupt each 1 msec
       - Set NVIC Group Priority to 4
       - Global MSP (MCU Support Package) initialization
     */   
  HAL_Init();
  
  /* Configure the system clock to 200 MHz */
  SystemClock_Config(); 

  // reset the user text
  snprintf(user_text_line_mode_1, USER_TEXT_LENGTH, "User text line 1");
  snprintf(user_text_line_mode_2, USER_TEXT_LENGTH, "User text line 2");
  snprintf(user_text_line_mode_3, USER_TEXT_LENGTH, "User text line 3");
  snprintf(user_text_line_mode_4, USER_TEXT_LENGTH, "User text line 4");
  snprintf(user_text_line_mode_5, USER_TEXT_LENGTH, "User text line 5");


  /* init the QSPI */
  BSP_QSPI_Init();
#if PROGRAMM_QSPI == 1	// program the QSPI with the pictures if needed

  volatile QSPI_Info pQSPI_Info;
  volatile uint8_t qspires = BSP_QSPI_GetStatus();

  BSP_LED_Init(LED_RED);
  BSP_LED_Init(LED_GREEN);

  pQSPI_Info.FlashSize          = (uint32_t)0x00;
  pQSPI_Info.EraseSectorSize    = (uint32_t)0x00;
  pQSPI_Info.EraseSectorsNumber = (uint32_t)0x00;
  pQSPI_Info.ProgPageSize       = (uint32_t)0x00;
  pQSPI_Info.ProgPagesNumber    = (uint32_t)0x00;
  BSP_QSPI_GetInfo(&pQSPI_Info);

  BSP_QSPI_Erase_Chip();	// erase the memory, takes up to 200 seconds

  // program the pictures
  qspires = BSP_QSPI_Write((uint8_t *)&Pfeil_rechts[0],PFEIL_RECHTS_ADR, sizeof(Pfeil_rechts));
  qspires = BSP_QSPI_Write((uint8_t *)&Pfeil_links[0],PFEIL_LINKS_ADR, sizeof(Pfeil_links));
  qspires = BSP_QSPI_Write((uint8_t *)&Time_graph[0],TIME_GRAPH_ADR, sizeof(Time_graph));
  qspires = BSP_QSPI_Write((uint8_t *)&Freq_graph[0],FREQ_GRAPH_ADR, sizeof(Freq_graph));
  BSP_LED_On(LED_GREEN);

  while(1){};	// wait for ever
#else

  // use external memory as if it was an internal one
  BSP_QSPI_EnableMemoryMappedMode();

  /* Init Audio Application */
 // AUDIO_InitApplication();
  
  /* Init TS module */
  BSP_TS_Init(800, 480);

  /* Init Host Library */
//  USBH_Init(&hUSBHost, USBH_UserProcess, 0);

  /* Add Supported Class */
//  USBH_RegisterClass(&hUSBHost, USBH_MSC_CLASS);
  
  /* Start Host Process */
//  USBH_Start(&hUSBHost);
  
  // init the user LEDs
  BSP_LED_Init(LED_RED);
  BSP_LED_Init(LED_GREEN);

  // init the button
  BSP_PB_Init(BUTTON_USER, BUTTON_USER);

  // init processing
  processing_init();

  // init the GUI
  initGUI();

  // init the audio codec
  initCodec(CODEC_SAMPLING_FREQUENCY);

  /* Enable the CPU Cache */
  CPU_CACHE_Enable();

  HAL_Delay(10);

  //reset the time out flags after the initialization
  __disable_irq();
  audio_buffer_a_timeout = 0;
  audio_buffer_b_timeout = 0;
  __enable_irq();

  // set the default output volume
  wm8994_SetOutputVolume_dB(AUDIO_I2C_ADDRESS, audio_output_volume_dB);
  wm8994_SetInputVolume_dB(AUDIO_I2C_ADDRESS, audio_input_volume_dB);

  /* Run Application (Blocking mode) */
  while (1)
  {
    /* USB Host Background task */
//    USBH_Process(&hUSBHost);

	// reset the watchdog
	watchdog_counter = BSP_AUDIO_FREQUENCY_48K / BUFFER_SIZE_SAMPLES;
    
	checkPageSwitchButtons();	// look for a page switch

	if(last_menue_page != menue_page){
		switch(menue_page){
			case PAGE1:
				initPage1();
			break;

			case PAGE2:
				initPage2();
			break;

			case PAGE3:
				initPage3();
			break;

			case PAGE4:
				initPage4();
			break;

			case PAGE5:
				initPage5();
			break;
		}

		last_menue_page = menue_page;
	}

	// look for button press on the current page
	switch(menue_page){
		case PAGE1:
			checkButtonsPage1();
		break;

		case PAGE2:
			checkButtonsPage2();
		break;

		case PAGE3:
			checkButtonsPage3();
		break;

		case PAGE4:
			checkButtonsPage4();
		break;

		case PAGE5:
			checkButtonsPage5();
		break;
	}

	// read the push button
	button_state_now = BSP_PB_GetState(BUTTON_USER);

	if((button_state_now == 1) && (last_button_state == 0)){
		if(user_button < MAX_BUTTON_STATES){
			user_button++;
		}
		else{
			user_button = 1;
		}

		drawTitle();

		switch(menue_page){
			case PAGE1:
		//		initPage1();
			break;

			case PAGE2:
				initPage2();
			break;

			case PAGE3:
				initPage3();
			break;

			case PAGE4:
	//			initPage4();
			break;

			case PAGE5:
	//			initPage5();
			break;
		}
	}


	last_button_state = button_state_now;

	// flash the red LED to show the system is alive
    i++;

    if(i < 30){
    	BSP_LED_On(LED_RED);
    }

    else{
    	BSP_LED_Off(LED_RED);
    	if(i>2*30){
    		i = 0;

    	}
    }

    // test if the processing overloads the CPU
	if((audio_buffer_a_timeout == 1) || (audio_buffer_b_timeout == 1)){

		// clear the screen
		BSP_LCD_Clear(LCD_COLOR_WHITE);

		BSP_LCD_SetBackColor(LCD_COLOR_WHITE);
		BSP_LCD_SetTextColor(LCD_COLOR_RED);
		BSP_LCD_SetFont(&Font24);

		snprintf(string_buffer,100, "Achtung: CPU ueberlastet!!!");
		BSP_LCD_DisplayStringAt(10, LCD_TEXT_Y_OFFSET, (uint8_t *)&string_buffer[0], LEFT_MODE);

		snprintf(string_buffer,100, "Moegliche Loesungen:");
		BSP_LCD_DisplayStringAt(10, LCD_TEXT_Y_OFFSET + 35, (uint8_t *)&string_buffer[0], LEFT_MODE);

		snprintf(string_buffer,100, " - Blockgroesse erhoehen");
		BSP_LCD_DisplayStringAt(10, LCD_TEXT_Y_OFFSET + 70, (uint8_t *)&string_buffer[0], LEFT_MODE);

		snprintf(string_buffer,100, " - Abtastrate reduzieren");
		BSP_LCD_DisplayStringAt(10, LCD_TEXT_Y_OFFSET+ 105, (uint8_t *)&string_buffer[0], LEFT_MODE);

		snprintf(string_buffer,100, " - Angorithmus optimieren / verkleinern");
		BSP_LCD_DisplayStringAt(10, LCD_TEXT_Y_OFFSET + 140, (uint8_t *)&string_buffer[0], LEFT_MODE);


		while(1){}	// lock the system
	}

    HAL_Delay(10);	// slow down the touch screen polling to give the driver time to detect the touch
  }
#endif
}
/************************************************************************************/



/*** Private Functions **************************************************************/
/************************************************************************************/

/**
  * @brief  User Process
  * @param  phost: Host Handle
  * @param  id: Host Library user message ID
  * @retval None
  */
static void USBH_UserProcess(USBH_HandleTypeDef *phost, uint8_t id)
{
  switch(id)
  { 
  case HOST_USER_SELECT_CONFIGURATION:
    break;
    
  case HOST_USER_DISCONNECTION:
    appli_state = APPLICATION_DISCONNECT;
    break;

  case HOST_USER_CLASS_ACTIVE:
    appli_state = APPLICATION_READY;
    break;
 
  case HOST_USER_CONNECTION:
    appli_state = APPLICATION_START;
    break;
   
  default:
    break; 
  }
}
/************************************************************************************/


/**
  * @brief  System Clock Configuration
  *         The system Clock is configured as follow : 
  *            System Clock source            = PLL (HSE)
  *            SYSCLK(Hz)                     = 200000000
  *            HCLK(Hz)                       = 200000000
  *            AHB Prescaler                  = 1
  *            APB1 Prescaler                 = 4
  *            APB2 Prescaler                 = 2
  *            HSE Frequency(Hz)              = 25000000
  *            PLL_M                          = 25
  *            PLL_N                          = 432
  *            PLL_P                          = 2
  *            PLL_Q                          = 9
  *            VDD(V)                         = 3.3
  *            Main regulator output voltage  = Scale1 mode
  *            Flash Latency(WS)              = 7
  * @param  None
  * @retval None
  */
static void SystemClock_Config(void)
{
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_OscInitTypeDef RCC_OscInitStruct;
  HAL_StatusTypeDef ret = HAL_OK;

  /* Enable HSE Oscillator and activate PLL with HSE as source */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 25;
  RCC_OscInitStruct.PLL.PLLN = 400;  
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 8;
  RCC_OscInitStruct.PLL.PLLR = 7;

  ret = HAL_RCC_OscConfig(&RCC_OscInitStruct);
  if(ret != HAL_OK)
  {
    while(1) { ; }
  }

  /* Activate the OverDrive to reach the 200 MHz Frequency */
  ret = HAL_PWREx_EnableOverDrive();
  if(ret != HAL_OK)
  {
    while(1) { ; }
  }
  
  /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2 clocks dividers */
  RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;  
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  ret = HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_6);
  if(ret != HAL_OK)
  {
    while(1) { ; }
  }
}
/************************************************************************************/


/**
  * @brief  Configure the MPU attributes as Write Through for Internal SRAM1/2.
  * @note   The Base Address is 0x20020000 since this memory interface is the AXI.
  *         The Configured Region Size is 512KB because the internal SRAM1/2 
  *         memory size is 384KB.
  * @param  None
  * @retval None
  */
static void MPU_Config(void)
{
  MPU_Region_InitTypeDef MPU_InitStruct;

  
  /* Disable the MPU */
  HAL_MPU_Disable();

  /* Configure the MPU attributes as WT for SRAM */
  // SRAM1 / 2 WT
  MPU_InitStruct.Enable = MPU_REGION_ENABLE;
  MPU_InitStruct.BaseAddress = 0x20020000;
  MPU_InitStruct.Size = MPU_REGION_SIZE_512KB;
  MPU_InitStruct.AccessPermission = MPU_REGION_FULL_ACCESS;
  MPU_InitStruct.IsBufferable = MPU_ACCESS_NOT_BUFFERABLE;
  MPU_InitStruct.IsCacheable = MPU_ACCESS_CACHEABLE;
  MPU_InitStruct.IsShareable = MPU_ACCESS_NOT_SHAREABLE;
  MPU_InitStruct.Number = MPU_REGION_NUMBER7;
  MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL0;
  MPU_InitStruct.SubRegionDisable = 0x00;
  MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_ENABLE;

  HAL_MPU_ConfigRegion(&MPU_InitStruct);


  // Do not cash the ITCM
  MPU_InitStruct.Enable = MPU_REGION_ENABLE;
  MPU_InitStruct.BaseAddress = 0x00000000;
  MPU_InitStruct.Size = MPU_REGION_SIZE_16KB;
  MPU_InitStruct.AccessPermission = MPU_REGION_FULL_ACCESS;
  MPU_InitStruct.IsBufferable = MPU_ACCESS_NOT_BUFFERABLE;
  MPU_InitStruct.IsCacheable = MPU_ACCESS_NOT_CACHEABLE;
  MPU_InitStruct.IsShareable = MPU_ACCESS_NOT_SHAREABLE;
  MPU_InitStruct.Number = MPU_REGION_NUMBER6;
  MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL1;
  MPU_InitStruct.SubRegionDisable = 0x00;
  MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_ENABLE;

  HAL_MPU_ConfigRegion(&MPU_InitStruct);

  /* Enable the MPU */
  HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);

}
/************************************************************************************/


/**
  * @brief  CPU L1-Cache enable.
  * @param  None
  * @retval None
  */
static void CPU_CACHE_Enable(void)
{

  SCB_InvalidateICache();
  SCB_InvalidateDCache();

  /* Enable I-Cache */
  SCB_EnableICache();

  /* Enable D-Cache */
  SCB_EnableDCache();

  SCB_InvalidateICache();
  SCB_InvalidateDCache();
}
/************************************************************************************/



