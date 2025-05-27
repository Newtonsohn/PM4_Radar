/** ***************************************************************************
 * @file
 * @brief Sets up the microcontroller, the clock system and the peripherals.
 *
 * Initialization is done for the system, the blue user button, the user LEDs,
 * and the LCD display with the touchscreen.
 * @n Then the code enters an infinite while-loop, where it checks for
 * user input and starts the requested demonstration.
 *
 * @author  Hanspeter Hochreutener, hhrt@zhaw.ch
 * @date	17.06.2021
 *****************************************************************************/


/******************************************************************************
 * Includes
 *****************************************************************************/
#include "stm32f4xx.h"
#include "stm32f429i_discovery.h"
#include "stm32f429i_discovery_lcd.h"
#include "stm32f429i_discovery_ts.h"
#include "stm32f4xx_hal_tim.h"
#include "stm32f4xx_hal_tim_ex.h"

#include "main.h"
#include "pushbutton.h"
#include "menu.h"
#include "measuring.h"
#include "misc.h"

#include "radar.h"
#include <string.h>
#include <stdio.h>
/******************************************************************************
 * Defines
 *****************************************************************************/
#define TIME_AWAY 10


/******************************************************************************
 * Variables
 *****************************************************************************/

TIM_HandleTypeDef htim3;

/******************************************************************************
 * Functions
 *****************************************************************************/
static void SystemClock_Config(void);	///< System Clock Configuration
static void gyro_disable(void);			///< Disable the onboard gyroscope
uint32_t motion= 0;


/** ***************************************************************************
 * @brief  Main function
 * @return not used because main ends in an infinite loop
 *
 * Initialization and infinite while loop
 *****************************************************************************/
int main(void) {
	HAL_Init();							// Initialize the system
	SystemClock_Config();				// Configure system clocks

#ifdef FLIPPED_LCD
	BSP_LCD_Init_Flipped();				// Initialize the LCD for flipped orientation
#else
	BSP_LCD_Init();						// Initialize the LCD display
#endif
	BSP_LCD_LayerDefaultInit(LCD_FOREGROUND_LAYER, LCD_FRAME_BUFFER);
	BSP_LCD_SelectLayer(LCD_FOREGROUND_LAYER);
	BSP_LCD_DisplayOn();
	BSP_LCD_Clear(LCD_COLOR_WHITE);
	BSP_TS_Init(BSP_LCD_GetXSize(), BSP_LCD_GetYSize());	// Touchscreen
	/* Uncomment next line to enable touchscreen interrupt */
	// BSP_TS_ITConfig();					// Enable Touchscreen interrupt

	PB_init();							// Initialize the user pushbutton
	PB_enableIRQ();						// Enable interrupt on user pushbutton

	BSP_LED_Init(LED3);					// Toggles in while loop
	BSP_LED_Init(LED4);					// Is toggled by user button

	//MENU_draw();						// Draw the menu
	//MENU_hint();						// Show hint at startup

	gyro_disable();						// Disable gyro, use those analog inputs

	MEAS_GPIO_analog_init();			// Configure GPIOs in analog mode
	MEAS_timer_init();					// Configure the timer

	//init_radar();
	//MX_GPIO_Init();
	//MX_TIM3_Init();
	ADC1_IN11_ADC2_IN13_dual_init();
	ADC1_IN11_ADC2_IN13_dual_start();
	//GPIO_PG13_Output_Init();
	int select_state = 0;
	int8_t t = 1;

	static char soll_pin[7] = "1234";

	typedef enum{
		STARTMENU = 0, ARMED, DEFUSE, SUBMENU, ALARM
	}SYSTEMSTATE;
	SYSTEMSTATE STATE = STARTMENU;

    Buzzer_Init();
	/* Infinite while loop */
	while (1) {							// Infinitely loop in main function
		switch (STATE){

		case STARTMENU:
			Buzzer_Stop();
			//HAL_GPIO_WritePin(GPIOG, GPIO_PIN_13, GPIO_PIN_RESET);  // Zum Test HIGH
			BSP_LED_On(LED3);
			select_state = SHOW_STARTMENU();
			if(select_state == 1)
			{
				STATE = ARMED;
				BSP_LCD_Clear(LCD_COLOR_WHITE);
				start_countdown(30);
			}
			else if(select_state == 2)
			{
				STATE = SUBMENU;
			}
			break;

		case ARMED:
			if(t > 0)
				t = update_countdown();
			if ((motion != 0)&&(t <= 0))
			{
				STATE = DEFUSE;
				t = 1;
			}
			break;
		case DEFUSE:
		{
			char* entered_pin = SHOW_DEFUSEMENU();
			if(entered_pin != NULL)
			{
				if(strcmp(entered_pin, soll_pin) == 0)
				{
					STATE = STARTMENU;
					BSP_LCD_Clear(LCD_COLOR_WHITE);
				} else{
					BSP_LCD_Clear(LCD_COLOR_WHITE);
					STATE = ALARM;
					Buzzer_SetFrequency(2500);
					Buzzer_Start();
					BSP_LED_Off(LED3);
				}
			} else {
				BSP_LCD_Clear(LCD_COLOR_WHITE);
				STATE = ALARM;
				Buzzer_SetFrequency(2000);
				Buzzer_Start();
				//HAL_GPIO_WritePin(GPIOG, GPIO_PIN_13, GPIO_PIN_SET);  // Zum Test HIGH
				BSP_LED_Off(LED3);

			}
			break;
		}
		case SUBMENU:
			char* new_pin = change_pin();
			if (new_pin != NULL) {
			    strcpy(soll_pin, new_pin);  // ✅ Inhalt in das statische Array kopieren
			    STATE = STARTMENU;
				BSP_LCD_Clear(LCD_COLOR_WHITE);
			}
			break;
		case ALARM:

			char* entered_pin = SHOW_ALARMMENU();
			if(entered_pin != NULL)
			{
				if(strcmp(entered_pin, soll_pin) == 0)
				{
					Buzzer_Stop();
					STATE = STARTMENU;
					BSP_LCD_Clear(LCD_COLOR_WHITE);
				}
			}
			break;

		default:
			break;
		}

		//subsystem_test();

		if (MEAS_data_ready) {			// Show data if new data available
			MEAS_data_ready = false;


			//MEAS_show_data();
			motion = fft();

			//ADC1_IN11_timer_init();
			//ADC1_IN11_timer_start();
			ADC1_IN11_ADC2_IN13_dual_init();
			ADC1_IN11_ADC2_IN13_dual_start();
		}

		if (PB_pressed()) {				// Check if user pushbutton was pressed
			DAC_active = !DAC_active;	// Toggle DAC on/off
			if (DAC_active) {
				DAC_init();
				BSP_LED_On(LED4);
			} else {
				DAC_reset();
				BSP_LED_Off(LED4);
			}
		}

		/* Comment next line if touchscreen interrupt is enabled */
		MENU_check_transition();

		switch (MENU_get_transition()) {	// Handle user menu choice
		case MENU_NONE:					// No transition => do nothing
			break;
		case MENU_ZERO:
			ADC1_IN11_ADC2_IN13_dual_init();
			ADC1_IN11_ADC2_IN13_dual_start();
			break;
		case MENU_ONE:
			break;
		case MENU_TWO:
			ADC3_IN4_DMA_init();
			ADC3_IN4_DMA_start();
			break;
		case MENU_THREE:
			ADC1_IN13_ADC2_IN5_dual_init();
			ADC1_IN13_ADC2_IN5_dual_start();
			break;
		case MENU_FOUR:
			ADC2_IN13_IN5_scan_init();
			ADC2_IN13_IN5_scan_start();
			break;
		case MENU_FIVE:
			ADC3_IN13_IN4_scan_init();
			ADC3_IN13_IN4_scan_start();
			break;
		default:						// Should never occur
			break;
		}

		HAL_Delay(200);					// Wait or sleep
	}
}


/** ***************************************************************************
 * @brief System Clock Configuration
 *
 *****************************************************************************/
static void SystemClock_Config(void){
	RCC_OscInitTypeDef RCC_OscInitStruct = {0};
	RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
	RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};
	/* Configure the main internal regulator output voltage */
	__HAL_RCC_PWR_CLK_ENABLE();
	__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
	/* Initialize High Speed External Oscillator and PLL circuits */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
	RCC_OscInitStruct.HSEState = RCC_HSE_ON;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	RCC_OscInitStruct.PLL.PLLM = 8;
	RCC_OscInitStruct.PLL.PLLN = 336;
	RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
	RCC_OscInitStruct.PLL.PLLQ = 7;
	HAL_RCC_OscConfig(&RCC_OscInitStruct);
	/* Initialize gates and clock dividers for CPU, AHB and APB busses */
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
			| RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;
	HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5);
	/* Initialize PLL and clock divider for the LCD */
	PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_LTDC;
	PeriphClkInitStruct.PLLSAI.PLLSAIN = 192;
	PeriphClkInitStruct.PLLSAI.PLLSAIR = 4;
	PeriphClkInitStruct.PLLSAIDivR = RCC_PLLSAIDIVR_8;
	HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct);
	/* Set clock prescaler for ADCs */
	ADC->CCR |= ADC_CCR_ADCPRE_0;
}


/** ***************************************************************************
 * @brief Disable the GYRO on the microcontroller board.
 *
 * @note MISO of the GYRO is connected to PF8 and CS to PC1.
 * @n Some times the GYRO goes into an undefined mode at startup
 * and pulls the MISO low or high thus blocking the analog input on PF8.
 * @n The simplest solution is to pull the CS of the GYRO low for a short while
 * which is done with the code below.
 * @n PF8 is also reconfigured.
 * @n An other solution would be to remove the GYRO
 * from the microcontroller board by unsoldering it.
 *****************************************************************************/
static void gyro_disable(void)
{
	__HAL_RCC_GPIOC_CLK_ENABLE();		// Enable Clock for GPIO port C
	/* Disable PC1 and PF8 first */
	GPIOC->MODER &= ~GPIO_MODER_MODER1_Msk;	// Reset mode for PC1
	GPIOC->MODER |= 1UL << GPIO_MODER_MODER1_Pos;	// Set PC1 as output
	GPIOC->BSRR |= GPIO_BSRR_BR1;		// Set GYRO (CS) to 0 for a short time
	HAL_Delay(10);						// Wait some time
	GPIOC->MODER |= 3UL << GPIO_MODER_MODER1_Pos;	// Analog PC1 = ADC123_IN11
	__HAL_RCC_GPIOF_CLK_ENABLE();		// Enable Clock for GPIO port F
	GPIOF->OSPEEDR &= ~GPIO_OSPEEDR_OSPEED8_Msk;	// Reset speed of PF8
	GPIOF->AFR[1] &= ~GPIO_AFRH_AFSEL8_Msk;	// Reset alternate function of PF8
	GPIOF->PUPDR &= ~GPIO_PUPDR_PUPD8_Msk;	// Reset pulup/down of PF8
	HAL_Delay(10);						// Wait some time
	GPIOF->MODER |= 3UL << GPIO_MODER_MODER8_Pos; // Analog mode PF8 = ADC3_IN4
}

void MX_GPIO_Init(void)
{
  __HAL_RCC_GPIOA_CLK_ENABLE();

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  //GPIO_InitStruct.Pin = GPIO_PIN_7;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;  // Direkt als Ausgang
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  //HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, GPIO_PIN_RESET);  // Ruhepegel
}

void GPIO_PG13_Output_Init(void)
{
    // Clock für Port G aktivieren
    __HAL_RCC_GPIOG_CLK_ENABLE();

    GPIO_InitTypeDef GPIO_InitStruct = {0};

    GPIO_InitStruct.Pin = GPIO_PIN_13;               // PG13
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;      // Push-Pull Output
    GPIO_InitStruct.Pull = GPIO_NOPULL;              // Kein Pull-Up / Pull-Down
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;     // Geschwindigkeit (nicht kritisch bei GPIO)

    HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

    // Optional: Initialzustand LOW setzen
    HAL_GPIO_WritePin(GPIOG, GPIO_PIN_13, GPIO_PIN_RESET);
}




// Default function implementations required to prevent build errors.
__attribute__((weak)) void _close(void){}
__attribute__((weak)) void _lseek(void){}
__attribute__((weak)) void _read(void){}
__attribute__((weak)) void _write(void){}

