#ifndef BUZZER_H
#define BUZZER_H

#include "stm32f4xx_hal.h"

void Buzzer_Init(void);
void Buzzer_SetFrequency(uint32_t freq_hz);
void Buzzer_Start(void);
void Buzzer_Stop(void);

#endif // BUZZER_H
