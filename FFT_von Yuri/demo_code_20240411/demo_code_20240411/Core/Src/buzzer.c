#include "buzzer.h"

static TIM_HandleTypeDef htim3;

void Buzzer_Init(void)
{
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_TIM3_CLK_ENABLE();

    // PB6 als Ausgang, Ruhepegel LOW
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF2_TIM3;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    //HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_RESET);

    // Timer-Konfiguration (TIM4 CH1)
    htim3.Instance = TIM3;
    htim3.Init.Prescaler = 84 - 1;          // 1 MHz
    htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim3.Init.Period = 500 - 1;            // default: 2 kHz
    htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    HAL_TIM_PWM_Init(&htim3);
}

void Buzzer_SetFrequency(uint32_t freq_hz)
{
    uint32_t timer_clk = 1000000;
    uint32_t period = timer_clk / freq_hz;

    __HAL_TIM_DISABLE(&htim3);
    htim3.Init.Period = period - 1;
    HAL_TIM_PWM_Init(&htim3);

    TIM_OC_InitTypeDef sConfigOC = {0};
    sConfigOC.OCMode = TIM_OCMODE_PWM1;
    sConfigOC.Pulse = (period / 2);
    sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
    sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;

    HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_2);
    //HAL_TIM_PWM_Stop(&htim3, TIM_CHANNEL_2);
    //__HAL_TIM_DISABLE(&htim3);
}

void Buzzer_Start(void)
{
  /*  GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = GPIO_PIN_6;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF2_TIM4;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);*/

    HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_2);
}

void Buzzer_Stop(void)
{
	__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2,0);
    /*HAL_TIM_PWM_Stop(&htim3, TIM_CHANNEL_2);
    __HAL_TIM_DISABLE(&htim4);
    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_6);

    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = GPIO_PIN_6;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_RESET);*/
}
