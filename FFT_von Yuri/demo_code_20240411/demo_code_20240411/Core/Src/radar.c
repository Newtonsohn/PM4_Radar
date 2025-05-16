/*
 * radar.c
 *
 *  Created on: Mar 30, 2025
 *      Author: Jurek_Volpez
 */
#include "radar.h"
#include "stm32f429i_discovery.h"


void init_radar(void)
{
	init_gpio();
}

void init_gpio(void)
{
	GPIO_InitTypeDef  GPIO_InitStruct;

	/* Enable the GPIO Clocks */
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOD_CLK_ENABLE();
	__HAL_RCC_GPIOE_CLK_ENABLE();
	__HAL_RCC_GPIOF_CLK_ENABLE();

	uint16_t pin;
	GPIO_TypeDef *port;

/*configure outputs********************************/

	//default settings
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_LOW;


	/* Configure the PC11 pin (LED3)*/
	port = GPIOC;
	pin = GPIO_PIN_11;
	GPIO_InitStruct.Pin = pin;
	HAL_GPIO_Init(port, &GPIO_InitStruct);
	HAL_GPIO_WritePin(port, pin, GPIO_PIN_SET);

	/* Configure the PD4 pin (LED2)*/
	port = GPIOD;
	pin = GPIO_PIN_4;
	GPIO_InitStruct.Pin = pin;
	HAL_GPIO_Init(port, &GPIO_InitStruct);
	HAL_GPIO_WritePin(port, pin, GPIO_PIN_SET);

	/* Configure the PD2 pin (power LED)*/
	port = GPIOD;
	pin = GPIO_PIN_2;
	GPIO_InitStruct.Pin = pin;
	HAL_GPIO_Init(port, &GPIO_InitStruct);
	HAL_GPIO_WritePin(port, pin, GPIO_PIN_RESET);

	/* Configure the PF6 pin (power enable)*/
	port = GPIOF;
	pin = GPIO_PIN_6;
	GPIO_InitStruct.Pin = pin;
	HAL_GPIO_Init(port, &GPIO_InitStruct);
	HAL_GPIO_WritePin(port, pin, GPIO_PIN_SET);

/*configure inputs********************************/

	//default settings
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;

	/* Configure the PB7 pin (Button:SW4)*/
	port = GPIOB;
	GPIO_InitStruct.Pin = GPIO_PIN_7;
	GPIO_InitStruct.Pull = GPIO_PULLDOWN;
	HAL_GPIO_Init(port, &GPIO_InitStruct);

	/* Configure the PE3 pin (Button:SW5)*/
	port = GPIOE;
	GPIO_InitStruct.Pin = GPIO_PIN_3;
	GPIO_InitStruct.Pull = GPIO_PULLDOWN;
	HAL_GPIO_Init(port, &GPIO_InitStruct);

	/* Configure the PE5 pin (Switch:SW3)*/
	port = GPIOE;
	GPIO_InitStruct.Pin = GPIO_PIN_5;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(port, &GPIO_InitStruct);

	/* Configure the PC13 pin (Switch:SW2)*/
	port = GPIOC;
	GPIO_InitStruct.Pin = GPIO_PIN_13;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(port, &GPIO_InitStruct);




//	__HAL_RCC_GPIOC_CLK_ENABLE();		// Enable Clock for GPIO port C
//	GPIOF->MODER |= (3 << GPIO_MODER_MODER1_Pos);	// Analog PC1 = ADC1_IN11



}

void subsystem_test(void)
{
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_11, HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_5));
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_4, HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_13));
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, /*HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_7)|*/HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_3));
}


