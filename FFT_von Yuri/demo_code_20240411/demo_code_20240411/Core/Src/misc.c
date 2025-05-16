/*
 * misc.c
 *
 *  Created on: May 10, 2025
 *      Author: lucas
 */
#include "stm32f4xx.h"
#include "stm32f429i_discovery.h"
#include "stm32f429i_discovery_lcd.h"
#include "stm32f429i_discovery_ts.h"

#include "misc.h"
#include <string.h>
#include <stdio.h>

uint32_t countdown_start = 0;
uint32_t countdown_last_update = 0;
int32_t countdown_remaining = 30;  // Sekunden
void start_countdown(uint8_t timer) {
    countdown_start = HAL_GetTick();
    countdown_last_update = countdown_start;
    countdown_remaining = timer;
}
uint16_t update_countdown() {
    uint32_t now = HAL_GetTick();

    // Alle 1000 ms aktualisieren
    if (now - countdown_last_update >= 1000) {
        countdown_remaining--;
        countdown_last_update = now;

        // Anzeige aktualisieren
        char buffer[32];
        snprintf(buffer, sizeof(buffer), "%lds", countdown_remaining);
        BSP_LCD_SetBackColor(LCD_COLOR_WHITE);
        BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
        BSP_LCD_DisplayStringAt(0, 3, (uint8_t *)"   ", RIGHT_MODE);
        BSP_LCD_DisplayStringAt(0, 3, (uint8_t *)buffer, RIGHT_MODE);
    }
    return countdown_remaining;
}
