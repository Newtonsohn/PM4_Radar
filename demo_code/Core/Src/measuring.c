/** ***************************************************************************
 * @file
 * @brief Measuring voltages with the ADC(s) in different configurations
 *
 *
 * Demonstrates different ADC (Analog to Digital Converter) modes
 * ==============================================================
 *
 * - ADC in single conversion mode
 * - ADC triggered by a timer and with interrupt after end of conversion
 * - ADC combined with DMA (Direct Memory Access) to fill a buffer
 * - Dual mode = simultaneous sampling of two inputs by two ADCs
 * - Scan mode = sequential sampling of two inputs by one ADC
 * - Simple DAC output is demonstrated as well
 * - Analog mode configuration for GPIOs
 * - Display recorded data on the graphics display
 *
 * Peripherals @ref HowTo
 *
 * @image html demo_screenshot_board.jpg
 *
 *
 * HW used for the demonstrations
 * ==============================
 * A simple HW was used for testing the code.
 * It is connected to the pins marked in red in the above image.
 *
 * @image html demo_board_schematic.png
 *
 * Of course the code runs also without this HW.
 * Simply connect a signal or waveform generator to the input(s).
 *
 *
 * @anchor HowTo
 * How to Configure the Peripherals: ADC, TIMER and DMA
 * ====================================================
 *
 * All the peripherals are accessed by writing to or reading from registers.
 * From the programmer’s point of view this is done exactly as
 * writing or reading the value of a variable.
 * @n Writing to a register configures the HW of the associated peripheral
 * to do what is required.
 * @n Reading from a registers gets status and data from the HW peripheral.
 *
 * The information on which bits have to be set to get a specific behavior
 * is documented in the <b>reference manual</b> of the mikrocontroller.
 *
 *
 * ----------------------------------------------------------------------------
 * @author Hanspeter Hochreutener, hhrt@zhaw.ch
 * @date 17.06.2021
 *****************************************************************************/


/******************************************************************************
 * Includes
 *****************************************************************************/
#include <stdio.h>
#include "stm32f4xx.h"
#include "stm32f429i_discovery.h"
#include "stm32f429i_discovery_lcd.h"
#include "stm32f429i_discovery_ts.h"

#include "measuring.h"
#include "arm_math.h"
#include "arm_const_structs.h"

/******************************************************************************
 * Defines
 *****************************************************************************/
#define ADC_DAC_RES		12			///< Resolution
#define ADC_NUMS		256			///< Number of samples
#define ADC_FS			5000	///< Sampling freq. => 12 samples for a 50Hz period
#define ADC_CLOCK		84000000	///< APB2 peripheral clock frequency
#define ADC_CLOCKS_PS	15			///< Clocks/sample: 3 hold + 12 conversion
#define TIM_CLOCK		84000000	///< APB1 timer clock frequency
#define TIM_TOP			9			///< Timer top value
#define TIM_PRESCALE	(TIM_CLOCK/ADC_FS/(TIM_TOP+1)-1) ///< Clock prescaler
#define SPEED_OF_LIGHT    3e8f            // Lichtgeschwindigkeit in m/s
#define FMCW_CARRIER_HZ   24000000000.0f  // FMCW Trägerfrequenz z.B. 24 GHz
#define FFT_SIZE   		ADC_NUMS		// No zero padding
#define IFFT_FLAG 		0 	// Normal FFT
#define DO_BIT_REVERSE 	1 // Unscramble fft bins

/******************************************************************************
 * Variables
 *****************************************************************************/
bool MEAS_data_ready = false;			///< New data is ready
uint32_t MEAS_input_count = 2;			///< 1 or 2 input channels?
bool DAC_active = false;				///< DAC output active?
bool FFT_data_ready = false;			///< New data is ready

static uint32_t ADC_sample_count = 0;	///< Index for buffer
static uint32_t ADC_samples[2*ADC_NUMS];///< ADC values of max. 2 input channels
static uint32_t DAC_sample = 0;			///< DAC output value

static float32_t Q_samples[FFT_SIZE];
static float32_t I_samples[FFT_SIZE];
static float32_t FFT_INOUT[2 * FFT_SIZE];
static float32_t FFT_MAG[FFT_SIZE];

#define VELOCITY_SMOOTH_WINDOW 5
float32_t velocity_history[VELOCITY_SMOOTH_WINDOW] = {0};
uint32_t velocity_index = 0;


/******************************************************************************
 * Functions
 *****************************************************************************/

// FFT Variablen
static float32_t FFT_INOUT[2*FFT_SIZE];
static float32_t FFT_MAG[FFT_SIZE];
static float32_t FFT_MAG_calibrate[FFT_SIZE];
uint32_t calibrate_status;					// 0: Start calibration, 1: calibration in progress, 2: calibration complete

//void FFT_init(void) {
//    arm_cfft_radix4_init_f32(&fft_inst, FFT_SIZE, 0, 1);
//}



void ADC1_IN14_ADC2_IN15_dual_init(void)
{
    MEAS_input_count = 2;                // 2 inputs are converted
    __HAL_RCC_ADC1_CLK_ENABLE();        // Enable Clock for ADC1
    __HAL_RCC_ADC2_CLK_ENABLE();        // Enable Clock for ADC2

    // Common ADC configuration
    ADC->CCR |= ADC_CCR_DMA_1;            // Enable DMA mode 2 = dual DMA
    ADC->CCR |= ADC_CCR_MULTI_1 | ADC_CCR_MULTI_2; // ADC1 and ADC2 simultaneous

    // ADC1 configuration
    ADC1->CR2 |= (1UL << ADC_CR2_EXTEN_Pos);    // Enable external trigger on rising edge
    ADC1->CR2 |= (6UL << ADC_CR2_EXTSEL_Pos);   // Timer 2 TRGO event
    ADC1->SQR3 |= (14UL << ADC_SQR3_SQ1_Pos);   // Input 14 = first conversion (changed from 13)

    // ADC2 configuration
    ADC2->SQR3 |= (15UL << ADC_SQR3_SQ1_Pos);   // Input 15 = first conversion (changed from 5)

    // DMA configuration
    __HAL_RCC_DMA2_CLK_ENABLE();        // Enable Clock for DMA2
    DMA2_Stream4->CR &= ~DMA_SxCR_EN;    // Disable the DMA stream 4
    while (DMA2_Stream4->CR & DMA_SxCR_EN) { ; }    // Wait for DMA to finish

    DMA2->HIFCR |= DMA_HIFCR_CTCIF4;    // Clear transfer complete interrupt flag
    DMA2_Stream4->CR |= (0UL << DMA_SxCR_CHSEL_Pos);    // Select channel 0
    DMA2_Stream4->CR |= DMA_SxCR_PL_1;        // Priority high
    DMA2_Stream4->CR |= DMA_SxCR_MSIZE_1;    // Memory data size = 32 bit
    DMA2_Stream4->CR |= DMA_SxCR_PSIZE_1;    // Peripheral data size = 32 bit
    DMA2_Stream4->CR |= DMA_SxCR_MINC;        // Increment memory address pointer
    DMA2_Stream4->CR |= DMA_SxCR_TCIE;        // Transfer complete interrupt enable

    DMA2_Stream4->NDTR = ADC_NUMS;        // Number of data items to transfer
    DMA2_Stream4->PAR = (uint32_t)&ADC->CDR;    // Peripheral register address
    DMA2_Stream4->M0AR = (uint32_t)ADC_samples;    // Buffer memory location address
}


/** ***************************************************************************
 * @brief Configure GPIOs in analog mode.
 *
 * @note The input number for the ADCs is not equal to the GPIO pin number!
 * - ADC3_IN4 = GPIO PF6
 * - ADC123_IN13 = GPIO PC3
 * - ADC12_IN5 = GPIO PA5
 * - DAC_OUT2 = GPIO PA5 (= same GPIO as ADC12_IN5)
 *****************************************************************************/
void MEAS_GPIO_analog_init(void)
{
	__HAL_RCC_GPIOF_CLK_ENABLE();		// Enable Clock for GPIO port F
	GPIOF->MODER |= (3UL << GPIO_MODER_MODER6_Pos);	// Analog PF6 = ADC3_IN4
	__HAL_RCC_GPIOC_CLK_ENABLE();		// Enable Clock for GPIO port C
	GPIOC->MODER |= (3UL << GPIO_MODER_MODER3_Pos);	// Analog PC3 = ADC123_IN13
	__HAL_RCC_GPIOA_CLK_ENABLE();		// Enable Clock for GPIO port A
	GPIOA->MODER |= (3UL << GPIO_MODER_MODER5_Pos);	// Analog PA5 ADC12_IN5
    __HAL_RCC_GPIOC_CLK_ENABLE();
    // Für ADC1_IN14 (PC4) und ADC2_IN15 (PC5)
    GPIOC->MODER |= (3UL << GPIO_MODER_MODER4_Pos) | (3UL << GPIO_MODER_MODER5_Pos);
}


/** ***************************************************************************
 * @brief Resets the DAC
 *
 * when it is no longer used.
 *****************************************************************************/
void DAC_reset(void) {
	RCC->APB1RSTR |= RCC_APB1RSTR_DACRST;	// Reset the DAC
	RCC->APB1RSTR &= ~RCC_APB1RSTR_DACRST;	// Release reset of the DAC
}


/** ***************************************************************************
 * @brief Initialize the DAC
 *
 * The output used is DAC_OUT2 = GPIO PA5
 * @n As DAC_OUT2 = GPIO PA5 (= same GPIO as ADC12_IN5)
 * it is possible to monitor the output voltage DAC_OUT2 by the input ADC12_IN5.
 *****************************************************************************/
void DAC_init(void)
{
	__HAL_RCC_DAC_CLK_ENABLE();			// Enable Clock for DAC
	DAC->CR |= DAC_CR_EN2;				// Enable DAC output 2
}


/** ***************************************************************************
 * @brief Increment the DAC value and write it to the output
 *
 *****************************************************************************/
void DAC_increment(void) {
	DAC_sample += 20;				// Increment DAC output
	if (DAC_sample >= (1UL << ADC_DAC_RES)) { DAC_sample = 0; }	// Go to 0
	DAC->DHR12R2 = DAC_sample;		// Write new DAC output value
}


/** ***************************************************************************
 * @brief Resets the ADCs and the timer
 *
 * to make sure the different demos do not interfere.
 *****************************************************************************/
void ADC_reset(void) {
	RCC->APB2RSTR |= RCC_APB2RSTR_ADCRST;	// Reset ADCs
	RCC->APB2RSTR &= ~RCC_APB2RSTR_ADCRST;	// Release reset of ADCs
	TIM2->CR1 &= ~TIM_CR1_CEN;				// Disable timer
}


/** ***************************************************************************
 * @brief Initialize the ADC in single conversion mode
 *
 * The input is ADC3_IN4 = GPIO PF6
 *****************************************************************************/
void ADC3_IN4_single_init(void)
{
	MEAS_input_count = 1;				// Only 1 input is converted
	__HAL_RCC_ADC3_CLK_ENABLE();		// Enable Clock for ADC3
	ADC3->SQR3 |= (4UL << ADC_SQR3_SQ1_Pos);	// Input 4 = first conversion
}


/** ***************************************************************************
 * @brief Read one single value of the ADC in single conversion mode
 *
 * Start the conversion, wait in a while loop for end of conversion, read data.
 *****************************************************************************/
void ADC3_IN4_single_read(void)
{
	ADC3->CR2 |= ADC_CR2_ADON;			// Enable ADC3
	HAL_Delay(1);						// ADC needs some time to stabilize
	ADC3->CR2 |= ADC_CR2_SWSTART;
	while (!(ADC3->SR & ADC_SR_EOC)) { ; }	// Wait for end of conversion
	ADC_samples[0] = ADC3->DR;			// Read the converted value
	ADC3->CR2 &= ~ADC_CR2_ADON;			// Disable ADC3
	if (DAC_active) {
		DAC_increment();
	}
	ADC_reset();
	MEAS_data_ready = true;
}


/** ***************************************************************************
 * @brief Configure the timer to trigger the ADC(s)
 *
 * @note For debugging purposes the timer interrupt might be useful.
 *****************************************************************************/
void MEAS_timer_init(void)
{
	__HAL_RCC_TIM2_CLK_ENABLE();		// Enable Clock for TIM2
	TIM2->PSC = TIM_PRESCALE;			// Prescaler for clock freq. = 1MHz
	TIM2->ARR = TIM_TOP;				// Auto reload = counter top value
	TIM2->CR2 |= TIM_CR2_MMS_1; 		// TRGO on update
	/* If timer interrupt is not needed, comment the following lines */
	TIM2->DIER |= TIM_DIER_UIE;			// Enable update interrupt
	NVIC_ClearPendingIRQ(TIM2_IRQn);	// Clear pending interrupt on line 0
	NVIC_EnableIRQ(TIM2_IRQn);			// Enable interrupt line 0 in the NVIC
}


/** ***************************************************************************
 * @brief Initialize the ADC to be triggered by a timer
 *
 * The ADC3 trigger is set to TIM2 TRGO event
 * and the timer starts the ADC directly without CPU intervention.
 * @n The ADC is configured for end of conversion interrupt.
 * @n The input is ADC3_IN4 = GPIO PF6
 *****************************************************************************/
void ADC3_IN4_timer_init(void)
{
	MEAS_input_count = 1;				// Only 1 input is converted
	__HAL_RCC_ADC3_CLK_ENABLE();		// Enable Clock for ADC3
	ADC3->SQR3 |= (4UL << ADC_SQR3_SQ1_Pos);	// Input 4 = first conversion
	ADC3->CR1 |= ADC_CR1_EOCIE;			// Enable end of conversion interrupt
	ADC3->CR2 |= (1UL << ADC_CR2_EXTEN_Pos);	// En. ext. trigger on rising e.
	ADC3->CR2 |= (6UL << ADC_CR2_EXTSEL_Pos);	// Timer 2 TRGO event

}


/** ***************************************************************************
 * @brief Start the ADC and the timer
 *
 * The ADC isues an end of conversion interrupt.
 * The converted value can be read in the associated interrupt handler.
 *****************************************************************************/
void ADC3_IN4_timer_start(void)
{
	NVIC_ClearPendingIRQ(ADC_IRQn);		// Clear pending interrupt on line 0
	NVIC_EnableIRQ(ADC_IRQn);			// Enable interrupt line 0 in the NVIC
	ADC3->CR2 |= ADC_CR2_ADON;			// Enable ADC3
	TIM2->CR1 |= TIM_CR1_CEN;			// Enable timer
}


/** ***************************************************************************
 * @brief Initialize ADC, timer and DMA for data acquisition in the background
 *
 * Uses ADC3 and DMA2_Stream1 Channel2
 * @n The ADC3 trigger is set to TIM2 TRGO event
 * and the timer starts the ADC directly without CPU intervention.
 * @ The ADC3 triggers the DMA2_Stream1 to transfer the new data directly
 * to memory without CPU intervention.
 * @n The DMA triggers the transfer complete interrupt when all data is ready.
 * @n The input is ADC3_IN4 = GPIO PF6
 *****************************************************************************/
void ADC3_IN4_DMA_init(void)
{
	MEAS_input_count = 1;				// Only 1 input is converted
	__HAL_RCC_ADC3_CLK_ENABLE();		// Enable Clock for ADC3
	ADC3->SQR3 |= (4UL << ADC_SQR3_SQ1_Pos);	// Input 4 = first conversion
	ADC3->CR2 |= (1UL << ADC_CR2_EXTEN_Pos);	// En. ext. trigger on rising e.
	ADC3->CR2 |= (6UL << ADC_CR2_EXTSEL_Pos);	// Timer 2 TRGO event
	ADC3->CR2 |= ADC_CR2_DMA;			// Enable DMA mode
	__HAL_RCC_DMA2_CLK_ENABLE();		// Enable Clock for DMA2
	DMA2_Stream1->CR &= ~DMA_SxCR_EN;	// Disable the DMA stream 1
	while (DMA2_Stream1->CR & DMA_SxCR_EN) { ; }	// Wait for DMA to finish
	DMA2->LIFCR |= DMA_LIFCR_CTCIF1;	// Clear transfer complete interrupt fl.
	DMA2_Stream1->CR |= (2UL << DMA_SxCR_CHSEL_Pos);	// Select channel 2
	DMA2_Stream1->CR |= DMA_SxCR_PL_1;		// Priority high
	DMA2_Stream1->CR |= DMA_SxCR_MSIZE_1;	// Memory data size = 32 bit
	DMA2_Stream1->CR |= DMA_SxCR_PSIZE_1;	// Peripheral data size = 32 bit
	DMA2_Stream1->CR |= DMA_SxCR_MINC;	// Increment memory address pointer
	DMA2_Stream1->CR |= DMA_SxCR_TCIE;	// Transfer complete interrupt enable
	DMA2_Stream1->NDTR = ADC_NUMS;		// Number of data items to transfer
	DMA2_Stream1->PAR = (uint32_t)&ADC3->DR;	// Peripheral register address
	DMA2_Stream1->M0AR = (uint32_t)ADC_samples;	// Buffer memory loc. address
}


/** ***************************************************************************
 * @brief Start DMA, ADC and timer
 *
 *****************************************************************************/
void ADC3_IN4_DMA_start(void)
{
	DMA2_Stream1->CR |= DMA_SxCR_EN;	// Enable DMA
	NVIC_ClearPendingIRQ(DMA2_Stream1_IRQn);	// Clear pending DMA interrupt
	NVIC_EnableIRQ(DMA2_Stream1_IRQn);	// Enable DMA interrupt in the NVIC
	ADC3->CR2 |= ADC_CR2_ADON;			// Enable ADC3
	TIM2->CR1 |= TIM_CR1_CEN;			// Enable timer
}


/** ***************************************************************************
 * @brief Initialize ADCs, timer and DMA for simultaneous dual ADC acquisition
 *
 * Uses ADC1 and ADC2 in dual mode and DMA2_Stream4 Channel0
 * @n The ADC1 trigger is set to TIM2 TRGO event
 * @n ADC1 is the master and simultaneously triggers ADC2.
 * @n Both converted data from ADC1 and ADC2 are packed into a 32-bit register
 * in this way: <b> ADC_CDR[31:0] = ADC2_DR[15:0] | ADC1_DR[15:0] </b>
 * and are transfered with the DMA in one single step.
 * @n The ADC1 triggers the DMA2_Stream4 to transfer the new data directly
 * to memory without CPU intervention.
 * @n The DMA triggers the transfer complete interrupt when all data is ready.
 * @n The input used with ADC1 is ADC123_IN13 = GPIO PC3
 * @n The input used with ADC2 is ADC12_IN5 = GPIO PA5
 *****************************************************************************/
void ADC1_IN13_ADC2_IN5_dual_init(void)
{
	MEAS_input_count = 2;				// 2 inputs are converted
	__HAL_RCC_ADC1_CLK_ENABLE();		// Enable Clock for ADC1
	__HAL_RCC_ADC2_CLK_ENABLE();		// Enable Clock for ADC2
	ADC->CCR |= ADC_CCR_DMA_1;			// Enable DMA mode 2 = dual DMA
	ADC->CCR |= ADC_CCR_MULTI_1 | ADC_CCR_MULTI_2; // ADC1 and ADC2 simultan.
	ADC1->CR2 |= (1UL << ADC_CR2_EXTEN_Pos);	// En. ext. trigger on rising e.
	ADC1->CR2 |= (6UL << ADC_CR2_EXTSEL_Pos);	// Timer 2 TRGO event
	ADC1->SQR3 |= (13UL << ADC_SQR3_SQ1_Pos);	// Input 13 = first conversion
	ADC2->SQR3 |= (5UL << ADC_SQR3_SQ1_Pos);	// Input 5 = first conversion
	__HAL_RCC_DMA2_CLK_ENABLE();		// Enable Clock for DMA2
	DMA2_Stream4->CR &= ~DMA_SxCR_EN;	// Disable the DMA stream 4
	while (DMA2_Stream4->CR & DMA_SxCR_EN) { ; }	// Wait for DMA to finish
	DMA2->HIFCR |= DMA_HIFCR_CTCIF4;	// Clear transfer complete interrupt fl.
	DMA2_Stream4->CR |= (0UL << DMA_SxCR_CHSEL_Pos);	// Select channel 0
	DMA2_Stream4->CR |= DMA_SxCR_PL_1;		// Priority high
	DMA2_Stream4->CR |= DMA_SxCR_MSIZE_1;	// Memory data size = 32 bit
	DMA2_Stream4->CR |= DMA_SxCR_PSIZE_1;	// Peripheral data size = 32 bit
	DMA2_Stream4->CR |= DMA_SxCR_MINC;	// Increment memory address pointer
	DMA2_Stream4->CR |= DMA_SxCR_TCIE;	// Transfer complete interrupt enable
	DMA2_Stream4->NDTR = ADC_NUMS;		// Number of data items to transfer
	DMA2_Stream4->PAR = (uint32_t)&ADC->CDR;	// Peripheral register address
	DMA2_Stream4->M0AR = (uint32_t)ADC_samples;	// Buffer memory loc. address
}


/** ***************************************************************************
 * @brief Start DMA, ADC and timer
 *
 *****************************************************************************/
void ADC1_IN13_ADC2_IN5_dual_start(void)
{
	DMA2_Stream4->CR |= DMA_SxCR_EN;	// Enable DMA
	NVIC_ClearPendingIRQ(DMA2_Stream4_IRQn);	// Clear pending DMA interrupt
	NVIC_EnableIRQ(DMA2_Stream4_IRQn);	// Enable DMA interrupt in the NVIC
	ADC1->CR2 |= ADC_CR2_ADON;			// Enable ADC1
	ADC2->CR2 |= ADC_CR2_ADON;			// Enable ADC2
	TIM2->CR1 |= TIM_CR1_CEN;			// Enable timer
}

void ADC1_IN14_ADC2_IN15_dual_start(void)
{
    DMA2_Stream4->CR |= DMA_SxCR_EN;          // DMA-Stream aktivieren
    NVIC_ClearPendingIRQ(DMA2_Stream4_IRQn);  // Ausstehende DMA-Interrupts löschen
    NVIC_EnableIRQ(DMA2_Stream4_IRQn);        // DMA-Interrupt im NVIC aktivieren

    ADC1->CR2 |= ADC_CR2_ADON;                // ADC1 einschalten
    ADC2->CR2 |= ADC_CR2_ADON;                // ADC2 einschalten

    TIM2->CR1 |= TIM_CR1_CEN;                 // Timer starten
}

/** ***************************************************************************
 * @brief Initialize ADC, timer and DMA for sequential acquisition = scan mode
 *
 * Uses ADC2 and DMA2_Stream3 channel1
 * @n The ADC2 trigger is set to TIM2 TRGO event
 * @n At each trigger both inputs are converted sequentially
 * and transfered to memory by the DMA.
 * @n As each conversion triggers the DMA, the number of transfers is doubled.
 * @n The DMA triggers the transfer complete interrupt when all data is ready.
 * @n The inputs used are ADC123_IN13 = GPIO PC3 and ADC12_IN5 = GPIO PA5
 *****************************************************************************/
void ADC2_IN13_IN5_scan_init(void)
{
	MEAS_input_count = 2;				// 2 inputs are converted
	__HAL_RCC_ADC2_CLK_ENABLE();		// Enable Clock for ADC2
	ADC2->SQR1 |= (1UL << ADC_SQR1_L_Pos);			// Convert 2 inputs
	ADC2->SQR3 |= (13UL << ADC_SQR3_SQ1_Pos);	// Input 13 = first conversion
	ADC2->SQR3 |= (5UL << ADC_SQR3_SQ2_Pos);	// Input 5 = second conversion
	ADC2->CR1 |= ADC_CR1_SCAN;			// Enable scan mode
	ADC2->CR2 |= (1UL << ADC_CR2_EXTEN_Pos);	// En. ext. trigger on rising e.
	ADC2->CR2 |= (6UL << ADC_CR2_EXTSEL_Pos);	// Timer 2 TRGO event
	ADC2->CR2 |= ADC_CR2_DMA;			// Enable DMA mode
	__HAL_RCC_DMA2_CLK_ENABLE();		// Enable Clock for DMA2
	DMA2_Stream3->CR &= ~DMA_SxCR_EN;	// Disable the DMA stream 3
	while (DMA2_Stream3->CR & DMA_SxCR_EN) { ; }	// Wait for DMA to finish
	DMA2->LIFCR |= DMA_LIFCR_CTCIF3;	// Clear transfer complete interrupt fl.
	DMA2_Stream3->CR |= (1UL << DMA_SxCR_CHSEL_Pos);	// Select channel 1
	DMA2_Stream3->CR |= DMA_SxCR_PL_1;		// Priority high
	DMA2_Stream3->CR |= DMA_SxCR_MSIZE_1;	// Memory data size = 32 bit
	DMA2_Stream3->CR |= DMA_SxCR_PSIZE_1;	// Peripheral data size = 32 bit
	DMA2_Stream3->CR |= DMA_SxCR_MINC;	// Increment memory address pointer
	DMA2_Stream3->CR |= DMA_SxCR_TCIE;	// Transfer complete interrupt enable
	DMA2_Stream3->NDTR = 2*ADC_NUMS;	// Number of data items to transfer
	DMA2_Stream3->PAR = (uint32_t)&ADC2->DR;	// Peripheral register address
	DMA2_Stream3->M0AR = (uint32_t)ADC_samples;	// Buffer memory loc. address
}


/** ***************************************************************************
 * @brief Start DMA, ADC and timer
 *
 *****************************************************************************/
void ADC2_IN13_IN5_scan_start(void)
{
	DMA2_Stream3->CR |= DMA_SxCR_EN;	// Enable DMA
	NVIC_ClearPendingIRQ(DMA2_Stream3_IRQn);	// Clear pending DMA interrupt
	NVIC_EnableIRQ(DMA2_Stream3_IRQn);	// Enable DMA interrupt in the NVIC
	ADC2->CR2 |= ADC_CR2_ADON;			// Enable ADC2
	TIM2->CR1 |= TIM_CR1_CEN;			// Enable timer
}


/** ***************************************************************************
 * @brief Initialize ADC, timer and DMA for sequential acquisition = scan mode
 *
 * Uses ADC3 and DMA2_Stream1 channel2
 * @n The ADC3 trigger is set to TIM2 TRGO event
 * @n At each trigger both inputs are converted sequentially
 * and transfered to memory by the DMA.
 * @n As each conversion triggers the DMA, the number of transfers is doubled.
 * @n The DMA triggers the transfer complete interrupt when all data is ready.
 * @n The inputs used are ADC123_IN13 = GPIO PC3 and ADC3_IN4 = GPIO PF6
 *****************************************************************************/
void ADC3_IN13_IN4_scan_init(void)
{
	MEAS_input_count = 2;				// 2 inputs are converted
	__HAL_RCC_ADC3_CLK_ENABLE();		// Enable Clock for ADC3
	ADC3->SQR1 |= (1UL << ADC_SQR1_L_Pos);		// Convert 2 inputs
	ADC3->SQR3 |= (13UL << ADC_SQR3_SQ1_Pos);	// Input 13 = first conversion
	ADC3->SQR3 |= (4UL << ADC_SQR3_SQ2_Pos);	// Input 4 = second conversion
	ADC3->CR1 |= ADC_CR1_SCAN;			// Enable scan mode
	ADC3->CR2 |= (1UL << ADC_CR2_EXTEN_Pos);	// En. ext. trigger on rising e.
	ADC3->CR2 |= (6UL << ADC_CR2_EXTSEL_Pos);	// Timer 2 TRGO event
	ADC3->CR2 |= ADC_CR2_DMA;			// Enable DMA mode
	__HAL_RCC_DMA2_CLK_ENABLE();		// Enable Clock for DMA2
	DMA2_Stream1->CR &= ~DMA_SxCR_EN;	// Disable the DMA stream 1
	while (DMA2_Stream1->CR & DMA_SxCR_EN) { ; }	// Wait for DMA to finish
	DMA2->LIFCR |= DMA_LIFCR_CTCIF1;	// Clear transfer complete interrupt fl.
	DMA2_Stream1->CR |= (2UL << DMA_SxCR_CHSEL_Pos);	// Select channel 2
	DMA2_Stream1->CR |= DMA_SxCR_PL_1;		// Priority high
	DMA2_Stream1->CR |= DMA_SxCR_MSIZE_1;	// Memory data size = 32 bit
	DMA2_Stream1->CR |= DMA_SxCR_PSIZE_1;	// Peripheral data size = 32 bit
	DMA2_Stream1->CR |= DMA_SxCR_MINC;	// Increment memory address pointer
	DMA2_Stream1->CR |= DMA_SxCR_TCIE;	// Transfer complete interrupt enable
	DMA2_Stream1->NDTR = 2*ADC_NUMS;	// Number of data items to transfer
	DMA2_Stream1->PAR = (uint32_t)&ADC3->DR;	// Peripheral register address
	DMA2_Stream1->M0AR = (uint32_t)ADC_samples;	// Buffer memory loc. address

}


/** ***************************************************************************
 * @brief Start DMA, ADC and timer
 *
 *****************************************************************************/
void ADC3_IN13_IN4_scan_start(void)
{
	DMA2_Stream1->CR |= DMA_SxCR_EN;	// Enable DMA
	NVIC_ClearPendingIRQ(DMA2_Stream1_IRQn);	// Clear pending DMA interrupt
	NVIC_EnableIRQ(DMA2_Stream1_IRQn);	// Enable DMA interrupt in the NVIC
	ADC3->CR2 |= ADC_CR2_ADON;			// Enable ADC3
	TIM2->CR1 |= TIM_CR1_CEN;			// Enable timer
}


/** ***************************************************************************
 * @brief Interrupt handler for the timer 2
 *
 * @note This interrupt handler was only used for debugging purposes
 * and to increment the DAC value.
 *****************************************************************************/
void TIM2_IRQHandler(void)
{
	TIM2->SR &= ~TIM_SR_UIF;			// Clear pending interrupt flag
	if (DAC_active) {
		DAC_increment();
	}
}


/** ***************************************************************************
 * @brief Interrupt handler for the ADCs
 *
 * Reads one sample from the ADC3 DataRegister and transfers it to a buffer.
 * @n Stops when ADC_NUMS samples have been read.
 *****************************************************************************/
void ADC_IRQHandler(void)
{
	if (ADC3->SR & ADC_SR_EOC) {		// Check if ADC3 end of conversion
		ADC_samples[ADC_sample_count++] = ADC3->DR;	// Read input channel 1 only
		if (ADC_sample_count >= ADC_NUMS) {		// Buffer full
			TIM2->CR1 &= ~TIM_CR1_CEN;	// Disable timer
			ADC3->CR2 &= ~ADC_CR2_ADON;	// Disable ADC3
			ADC_reset();
			MEAS_data_ready = true;
		}

	}
}


/** ***************************************************************************
 * @brief Interrupt handler for DMA2 Stream1
 *
 * The samples from the ADC3 have been transfered to memory by the DMA2 Stream1
 * and are ready for processing.
 *****************************************************************************/
void DMA2_Stream1_IRQHandler(void)
{
	if (DMA2->LISR & DMA_LISR_TCIF1) {	// Stream1 transfer compl. interrupt f.
		NVIC_DisableIRQ(DMA2_Stream1_IRQn);	// Disable DMA interrupt in the NVIC
		NVIC_ClearPendingIRQ(DMA2_Stream1_IRQn);// Clear pending DMA interrupt
		DMA2_Stream1->CR &= ~DMA_SxCR_EN;	// Disable the DMA
		while (DMA2_Stream1->CR & DMA_SxCR_EN) { ; }	// Wait for DMA to finish
		DMA2->LIFCR |= DMA_LIFCR_CTCIF1;// Clear transfer complete interrupt fl.
		TIM2->CR1 &= ~TIM_CR1_CEN;		// Disable timer
		ADC3->CR2 &= ~ADC_CR2_ADON;		// Disable ADC3
		ADC3->CR2 &= ~ADC_CR2_DMA;		// Disable DMA mode
		ADC_reset();
		MEAS_data_ready = true;
	}
}


/** ***************************************************************************
 * @brief Interrupt handler for DMA2 Stream3
 *
 * The samples from the ADC3 have been transfered to memory by the DMA2 Stream1
 * and are ready for processing.
 *****************************************************************************/
void DMA2_Stream3_IRQHandler(void)
{
	if (DMA2->LISR & DMA_LISR_TCIF3) {	// Stream3 transfer compl. interrupt f.
		NVIC_DisableIRQ(DMA2_Stream3_IRQn);	// Disable DMA interrupt in the NVIC
		NVIC_ClearPendingIRQ(DMA2_Stream3_IRQn);// Clear pending DMA interrupt
		DMA2_Stream3->CR &= ~DMA_SxCR_EN;	// Disable the DMA
		while (DMA2_Stream3->CR & DMA_SxCR_EN) { ; }	// Wait for DMA to finish
		DMA2->LIFCR |= DMA_LIFCR_CTCIF3;// Clear transfer complete interrupt fl.
		TIM2->CR1 &= ~TIM_CR1_CEN;		// Disable timer
		ADC2->CR2 &= ~ADC_CR2_ADON;		// Disable ADC2
		ADC2->CR2 &= ~ADC_CR2_DMA;		// Disable DMA mode
		ADC_reset();
		MEAS_data_ready = true;
	}
}


/** ***************************************************************************
 * @brief Interrupt handler for DMA2 Stream4
 *
 * Here the interrupt handler is used together with ADC1 and ADC2
 * in dual mode where they sample simultaneously.
 * @n The samples from both ADCs packed in a 32 bit word have been transfered
 * to memory by the DMA2 and are ready for unpacking.
 * @note In dual ADC mode two values are combined (packed) in a single uint32_t
 * ADC_CDR[31:0] = ADC2_DR[15:0] | ADC1_DR[15:0]
 * and are therefore extracted before further processing.
 *****************************************************************************/
void DMA2_Stream4_IRQHandler(void)
{
	if (DMA2->HISR & DMA_HISR_TCIF4) {	// Stream4 transfer compl. interrupt f.
		NVIC_DisableIRQ(DMA2_Stream4_IRQn);	// Disable DMA interrupt in the NVIC
		NVIC_ClearPendingIRQ(DMA2_Stream4_IRQn);// Clear pending DMA interrupt
		DMA2_Stream4->CR &= ~DMA_SxCR_EN;	// Disable the DMA
		while (DMA2_Stream4->CR & DMA_SxCR_EN) { ; }	// Wait for DMA to finish
		DMA2->HIFCR |= DMA_HIFCR_CTCIF4;// Clear transfer complete interrupt fl.
		TIM2->CR1 &= ~TIM_CR1_CEN;		// Disable timer
		ADC1->CR2 &= ~ADC_CR2_ADON;		// Disable ADC1
		ADC2->CR2 &= ~ADC_CR2_ADON;		// Disable ADC2
		ADC->CCR &= ~ADC_CCR_DMA_1;		// Disable DMA mode
		/* Extract combined samples */
		for (int32_t i = ADC_NUMS-1; i >= 0; i--){
			ADC_samples[2*i+1] = (ADC_samples[i] >> 16);
			ADC_samples[2*i]   = (ADC_samples[i] & 0xffff);
		}
		ADC_reset();
		MEAS_data_ready = true;
	}
}




void FMCW_prepareFft(void){
	// Get FFT array into proper form
	for (int n = 0; n < ADC_NUMS; n++) {
			FFT_INOUT[n*2] = Q_samples[n];
			FFT_INOUT[(n*2)+1] = I_samples[n];
	}
}

void FMCW_separateSamples(void){
	float32_t Q_sum = 0;
	float32_t I_sum = 0;

	// Separate Q and I Components of ADC Array
	for (int n = 0; n < ADC_NUMS; n++) {

		Q_samples[n] = (float32_t)(ADC_samples[n] & 0x0000FFFF);


		I_samples[n] = (float32_t)((ADC_samples[n] >> 16) & 0x0000FFFF);

		//// Simulation
		//float32_t t = (float32_t)n / ADC_FS;
		//Q_samples[n] = (float32_t)100.0*(float32_t)sinf(2 * PI * 1000 * t);  // Sine on Q
		//I_samples[n] = (float32_t)100.0*(float32_t)cosf(2 * PI * 1000 * t);  // Cosine on I

		// Sum of all Q and I values
		Q_sum = Q_sum + Q_samples[n];
		I_sum = I_sum + I_samples[n];
	}

	// Remove DC offset
	Q_sum = Q_sum / ADC_NUMS;
	I_sum = I_sum / ADC_NUMS;

	for (int n = 0; n < ADC_NUMS; n++) {
		Q_samples[n] = Q_samples[n] - Q_sum;
		I_samples[n] = I_samples[n] - I_sum;
	}
}

void FMCW_applyHanningWindow(void) {
    for (int n = 0; n < ADC_NUMS; n++) {
        float32_t window = 0.5f * (1.0f - arm_cos_f32((2.0f * PI * n) / (ADC_NUMS - 1)));
        I_samples[n] *= window;
        Q_samples[n] *= window;
    }
}

void FMCW_doFft(void){
	arm_cfft_instance_f32 cfft_instance;
	arm_cfft_init_f32(&cfft_instance, FFT_SIZE);
	arm_cfft_f32(&cfft_instance, FFT_INOUT, IFFT_FLAG, DO_BIT_REVERSE);
}

void FMCW_convertFftToMagnitude(void){
	 // Convert complex FFT to magnitude
	for (int i = 0; i < FFT_SIZE; i++) {
		float real = FFT_INOUT[2 * i];
		float imag = FFT_INOUT[2 * i + 1];
		FFT_MAG[i] = sqrtf(real * real + imag * imag);  // or use hypotf(real, imag)
	}
}

float32_t FMCW_getPeakMangitudeFrequency(void) {
    float32_t max_magnitude = 0.0f;
    int max_index = 0;
    int filter = 2;

    // Find peak magnitude and its index (ignoring edges if needed)
    for (int i = 0; i < FFT_SIZE; i++) {
        if ((FFT_MAG[i] > max_magnitude)&&((i < FFT_SIZE/2-filter)||(i > FFT_SIZE/2+filter))) {
            max_magnitude = FFT_MAG[i];
            max_index = i;
        }
    }

    // Convert bin index to signed frequency
    int shifted_index = (max_index < FFT_SIZE / 2) ? max_index : (max_index - FFT_SIZE);
    float32_t bin_resolution = (float32_t)ADC_FS / (float32_t)FFT_SIZE;
    float32_t max_frequency = shifted_index * bin_resolution;

    // Optional: Display on LCD
    char text_[32];
    snprintf(text_, sizeof(text_), "Freq: %+6.1f Hz", max_frequency);
    BSP_LCD_DisplayStringAt(0, 210, (uint8_t*)text_, CENTER_MODE);

    return max_frequency;
}

float32_t FMCW_getRadialVelocity(void) {
    float32_t max_magnitude = 0.0f;
    int max_index = 0;
    const float32_t MIN_PEAK_THRESHOLD = 2000.0f; // Adjust based on your signal level
    int filter = 2;
    char text_[32];

        // Find peak magnitude and its index (ignoring edges if needed)
    for (int i = 0; i < FFT_SIZE; i++) {
    	if ((FFT_MAG[i] > max_magnitude)&&((i < FFT_SIZE/2-filter)||(i > FFT_SIZE/2+filter))&&(i != 0)) {
            max_magnitude = FFT_MAG[i];
            max_index = i;
        }
    }

    // If the peak is too small, consider it noise
    if (max_magnitude < MIN_PEAK_THRESHOLD) {
        BSP_LCD_DisplayStringAt(0, 210, (uint8_t*)"No motion", CENTER_MODE);
        return 0.0f;
    }

    // Shift index to center FFT around 0 Hz
    int bin_centered = (max_index >= FFT_SIZE / 2) ? (max_index - FFT_SIZE) : max_index;
    float32_t bin_resolution = ADC_FS / (float32_t)FFT_SIZE;
    float32_t doppler_frequency = bin_centered * bin_resolution;

    // Calculate velocity (λ = c / f_c)
    float32_t lambda = SPEED_OF_LIGHT / FMCW_CARRIER_HZ;
    float32_t velocity = (doppler_frequency * lambda) / 2.0f;

    // Update history buffer
    velocity_history[velocity_index++] = velocity;
    if (velocity_index >= VELOCITY_SMOOTH_WINDOW) velocity_index = 0;

    // Compute average
    float32_t velocity_avg = 0;
    for (int i = 0; i < VELOCITY_SMOOTH_WINDOW; i++) {
        velocity_avg += velocity_history[i];
    }
    velocity_avg /= VELOCITY_SMOOTH_WINDOW;

    // Display smoothed velocity
    snprintf(text_, sizeof(text_), "Vel: %6.1f m/s", velocity_avg);
    BSP_LCD_DisplayStringAt(0, 240, (uint8_t*)text_, CENTER_MODE);

    return velocity_avg;
/*
    // Show result

    snprintf(text_, sizeof(text_), "Vel: %6.1f m/s", velocity);
    BSP_LCD_DisplayStringAt(0, 240, (uint8_t*)text_, CENTER_MODE);

    return velocity;*/
}

void FMCW_show_fft_magnitude_scaled(float32_t scale)
{
	const uint32_t Y_OFFSET = 260;
	const uint32_t X_SIZE = 240;
	//const uint32_t bins_to_draw = (FFT_SIZE < X_SIZE * 2) ? FFT_SIZE : X_SIZE * 2;
	uint32_t bins_to_draw = FFT_SIZE ;
	const uint32_t IGNORE = 0;
	const uint32_t start_bin = IGNORE;
	const uint32_t end_bin = bins_to_draw - IGNORE;

	// Clear display
	BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
	BSP_LCD_FillRect(0, 0, X_SIZE, Y_OFFSET + 1);

	// Initial point
	uint32_t y0 = (uint32_t)(FFT_MAG[start_bin] * scale);
	uint32_t x0 = 0;

	for (uint32_t i = start_bin, x = 1; i < end_bin; i += 1, x++) {
		uint32_t y1 = (uint32_t)(FFT_MAG[i] * scale);

		// Clip to LCD height
		if (y0 > Y_OFFSET - 1) y0 = Y_OFFSET - 1;
		if (y1 > Y_OFFSET - 1) y1 = Y_OFFSET - 1;

		// Draw magnitude in green
		BSP_LCD_SetTextColor(LCD_COLOR_BLUE);
		BSP_LCD_DrawLine(x0, Y_OFFSET - y0, x, Y_OFFSET - y1);

		y0 = y1;
		x0 = x;
	}

	// Optional label
	BSP_LCD_SetFont(&Font20);
	BSP_LCD_SetBackColor(LCD_COLOR_WHITE);
	BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
	BSP_LCD_DisplayStringAt(0, Y_OFFSET + 10, (uint8_t *)"Green: Magnitude (scaled)", CENTER_MODE);

	FFT_data_ready = false;
}

void FMCW_applyCalibrationFft(void){
	for(int i = 0; i < ADC_NUMS; i++){
		FFT_MAG[i] = FFT_MAG[i] - FFT_MAG_calibrate[i];
	}
}

bool FMCW_calibrateFft(void){
	static int counter = 0;
	static int number_of_counts = 5;

	switch(calibrate_status){

	case 0:		// Start calibration
		counter = 0;
		for(int i = 0; i < FFT_SIZE; i++){
			FFT_MAG[i] = 0.0f;
			calibrate_status = 1;
		}
		break;

	case 1: 	// Calibration in progress
		if(counter < number_of_counts){
			for(int i = 0; i < FFT_SIZE; i++){
				FFT_MAG_calibrate[i] = FFT_MAG_calibrate[i] + FFT_MAG[i];
			}
			counter++;
		} else{
			for(int i = 0; i < ADC_NUMS; i++){
				FFT_MAG_calibrate[i] = FFT_MAG_calibrate[i] / number_of_counts;

			}
			calibrate_status = 2;
		}
		break;

	case 2:

		break;

	default:
		calibrate_status = 0;
		break;

	}


	// return 1
	if(calibrate_status == 2){
		return 1;
	} else{
		return 0;
	}

}

void show_data_menu_zero(void)
{
    const uint32_t Y_OFFSET = 260;
    const uint32_t X_SIZE = 240;
    const uint32_t f = (1 << ADC_DAC_RES) / Y_OFFSET + 1;
    uint32_t data;
    uint32_t data_last;

    /* Clear the display */
    BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
    BSP_LCD_FillRect(0, 0, X_SIZE, Y_OFFSET + 1);

    /* Write first 2 samples as numbers */
    BSP_LCD_SetFont(&Font24);
    BSP_LCD_SetBackColor(LCD_COLOR_WHITE);
    BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
    char text[16];
    snprintf(text, 15, "1. sample %4d", (int)(ADC_samples[0]));
    BSP_LCD_DisplayStringAt(0, 50, (uint8_t *)text, LEFT_MODE);
    snprintf(text, 15, "2. sample %4d", (int)(ADC_samples[1]));
    BSP_LCD_DisplayStringAt(0, 80, (uint8_t *)text, LEFT_MODE);

    /* Draw the values of input channel 1 as a curve */
    BSP_LCD_SetTextColor(LCD_COLOR_BLUE);
    data = ADC_samples[MEAS_input_count * 0] / f;
    for (uint32_t i = 1; i < ADC_NUMS; i++) {
        data_last = data;
        data = (ADC_samples[MEAS_input_count * i]) / f;
        if (data > Y_OFFSET) data = Y_OFFSET;
        BSP_LCD_DrawLine(4 * (i - 1), Y_OFFSET - data_last, 4 * i, Y_OFFSET - data);
    }

    /* Draw the values of input channel 2 (if present) as a curve */
    if (MEAS_input_count == 2) {
        BSP_LCD_SetTextColor(LCD_COLOR_RED);
        data = ADC_samples[MEAS_input_count * 0 + 1] / f;
        for (uint32_t i = 1; i < ADC_NUMS; i++) {
            data_last = data;
            data = (ADC_samples[MEAS_input_count * i + 1]) / f;
            if (data > Y_OFFSET) data = Y_OFFSET;
            BSP_LCD_DrawLine(4 * (i - 1), Y_OFFSET - data_last, 4 * i, Y_OFFSET - data);
        }
    }

/* Clear buffer and flag */
	for (uint32_t i = 0; i < ADC_NUMS; i++) {
		ADC_samples[2 * i] = 0;
		ADC_samples[2 * i + 1] = 0;
	}
	ADC_sample_count = 0;
}

void show_data_menu_one(void) {


	FMCW_separateSamples();       // Split and DC correct I/Q
    FMCW_applyHanningWindow();    // Optional: Hanning window
    FMCW_prepareFft();            // Format I/Q as complex array
    FMCW_doFft();                 // Run FFT
    FMCW_convertFftToMagnitude(); // Get magnitudes
    FMCW_calibrateFft();


    // LCD plotting (FFT magnitude like in menu_zero)
    const uint32_t Y_OFFSET = 260;
    const uint32_t X_SIZE = 240;
    const uint32_t f = 300; // scaling factor for amplitude, adjust as needed
    uint32_t data, data_last;


    // Clear LCD
    BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
    BSP_LCD_FillRect(0, 0, X_SIZE, Y_OFFSET + 1);

    // Draw FFT magnitude as blue line
    BSP_LCD_SetTextColor(LCD_COLOR_BLUE);
    data = (uint32_t)(FFT_MAG[0] / f);
    for (uint32_t i = 1; i < FFT_SIZE; i++) {
        data_last = data;
        data = (uint32_t)(FFT_MAG[i] / f);
        if (data > Y_OFFSET) data = Y_OFFSET;
        if (data_last > Y_OFFSET) data_last = Y_OFFSET;
        BSP_LCD_DrawLine(3 * (i - 1), Y_OFFSET - data_last, 3 * i, Y_OFFSET - data);
    }

    // Optional: Display peak frequency and velocity
    FMCW_getPeakMangitudeFrequency();  // Also shows frequency on LCD
    if(calibrate_status == 2){
    	FMCW_applyCalibrationFft();
    	FMCW_getRadialVelocity();
    	//FMCW_show_fft_magnitude_scaled(0.001);
    	//FMCW_getPeakMangitudeFrequency();
    	MEAS_data_ready = false;


    }
}

void MEAS_show_data(void)
{
	const uint32_t Y_OFFSET = 260;
	const uint32_t X_SIZE = 240;
	const uint32_t f = (1 << ADC_DAC_RES) / Y_OFFSET + 1;	// Scaling factor
	uint32_t data;
	uint32_t data_last;
	/* Clear the display */
	BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
	BSP_LCD_FillRect(0, 0, X_SIZE, Y_OFFSET+1);
	/* Write first 2 samples as numbers */
	BSP_LCD_SetFont(&Font24);
	BSP_LCD_SetBackColor(LCD_COLOR_WHITE);
	BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
	char text[16];
	snprintf(text, 15, "1. sample %4d", (int)(ADC_samples[0]));
	BSP_LCD_DisplayStringAt(0, 50, (uint8_t *)text, LEFT_MODE);
	snprintf(text, 15, "2. sample %4d", (int)(ADC_samples[1]));
	BSP_LCD_DisplayStringAt(0, 80, (uint8_t *)text, LEFT_MODE);
	/* Draw the  values of input channel 1 as a curve */
	BSP_LCD_SetTextColor(LCD_COLOR_BLUE);
	data = ADC_samples[MEAS_input_count*0] / f;
	for (uint32_t i = 1; i < ADC_NUMS; i++){
		data_last = data;
		data = (ADC_samples[MEAS_input_count*i]) / f;
		if (data > Y_OFFSET) { data = Y_OFFSET; }// Limit value, prevent crash
		BSP_LCD_DrawLine((i-1), Y_OFFSET-data_last, i, Y_OFFSET-data);
	}
	/* Draw the  values of input channel 2 (if present) as a curve */
	if (MEAS_input_count == 2) {
		BSP_LCD_SetTextColor(LCD_COLOR_RED);
		data = ADC_samples[MEAS_input_count*0+1] / f;
		for (uint32_t i = 1; i < ADC_NUMS; i++){
			data_last = data;
			data = (ADC_samples[MEAS_input_count*i+1]) / f;
			if (data > Y_OFFSET) { data = Y_OFFSET; }// Limit value, prevent crash
			BSP_LCD_DrawLine((i-1), Y_OFFSET-data_last, i, Y_OFFSET-data);
		}
	}
}

