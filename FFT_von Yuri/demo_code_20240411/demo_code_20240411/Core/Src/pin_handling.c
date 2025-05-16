/*
 * pin_handling.c
 *
 *  Created on: May 8, 2025
 *      Author: lucas
 */


#include "stm32f429i_discovery.h"
#include "stm32f429i_discovery_lcd.h"
#include "stm32f429i_discovery_ts.h"
#include <string.h>
#include "pin_handling.h"
#define PIN_MAX_LEN 6
/*int xSize = 240; // Breite des Displays in Pixeln
int ySize = 320; // Höhe des Displays in Pixeln
uint32_t countdown_start = 0;
uint32_t countdown_last_update = 0;
int32_t countdown_remaining = 30;  // Sekunden*/
/*char* PIN_input_screen(void)
{
	start_countdown();
    static char pin_buffer[PIN_MAX_LEN + 1] = {0}; // Rückgabe-Buffer
    uint8_t pin_index = 0;
    TS_StateTypeDef TS_State;
    const uint16_t btn_w = 60, btn_h = 60;
    const uint16_t start_x = 20, start_y = 50;
    const uint16_t gap = 10;

    // Bildschirm löschen & Layout anzeigen
    BSP_LCD_Clear(LCD_COLOR_WHITE);
    BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
    BSP_LCD_SetBackColor(LCD_COLOR_WHITE);
    BSP_LCD_SetFont(&Font24);
    HAL_Delay(1000);
    // Zeichne Buttons 0–9
    char label[2] = {'0', '\0'};
    for (uint8_t i = 1; i <= 9; i++) {
        uint16_t row = (i - 1) / 3;
        uint16_t col = (i - 1) % 3;
        uint16_t x = start_x + col * (btn_w + gap);
        uint16_t y = start_y + row * (btn_h + gap);
        BSP_LCD_DrawRect(x, y, btn_w, btn_h);
        label[0] = '0' + i;
        BSP_LCD_DisplayStringAt(x + 20, y + 20, (uint8_t *)label, LEFT_MODE);
    }
    // Button 0
    BSP_LCD_DrawRect(start_x + btn_w + gap, start_y + 3 * (btn_h + gap), btn_w, btn_h);
    BSP_LCD_DisplayStringAt(start_x + btn_w + gap + 20, start_y + 3 * (btn_h + gap) + 20, (uint8_t *)"0", LEFT_MODE);

    // OK-Button
    BSP_LCD_SetTextColor(LCD_COLOR_GREEN);
    BSP_LCD_DrawRect(start_x, start_y + 3 * (btn_h + gap), btn_w, btn_h);
    BSP_LCD_DisplayStringAt(start_x + 10, start_y + 3 * (btn_h + gap) + 20, (uint8_t *)"OK", LEFT_MODE);

    // Clear-Button
    BSP_LCD_SetTextColor(LCD_COLOR_RED);
    BSP_LCD_DrawRect(start_x + 2 * (btn_w + gap), start_y + 3 * (btn_h + gap), btn_w, btn_h);
    BSP_LCD_DisplayStringAt(start_x + 10 + 2 * (btn_w + gap), start_y + 3 * (btn_h + gap) + 20, (uint8_t *)"CLR", LEFT_MODE);

    BSP_LCD_SetTextColor(LCD_COLOR_BLACK); // Reset Farbe

    // Eingabelogik
    while (1) {
    	update_countdown();
    	if(countdown_remaining <= 0)
    		return NULL;
        BSP_TS_GetState(&TS_State);
        if (TS_State.TouchDetected) {


        	uint16_t tx = 240 - TS_State.X;  // X spiegeln
        	uint16_t ty = TS_State.Y;        // Y bleibt gleich

            // Touchscreen-Ausrichtung korrigieren (STM32F429I-Discovery)
            HAL_Delay(150); // Debounce

            // Ziffern-Buttons
            for (uint8_t i = 1; i <= 9; i++) {
                uint16_t row = (i - 1) / 3;
                uint16_t col = (i - 1) % 3;
                uint16_t x = start_x + col * (btn_w + gap);
                uint16_t y = start_y + row * (btn_h + gap);
                if (tx >= x && tx <= x + btn_w && ty >= y && ty <= y + btn_h) {
                    if (pin_index < PIN_MAX_LEN) {
                        pin_buffer[pin_index++] = '0' + i;
                        pin_buffer[pin_index] = '\0';
                    }
                }
            }
            // Button 0
            uint16_t x0 = start_x + btn_w + gap;
            uint16_t y0 = start_y + 3 * (btn_h + gap);
            if (tx >= x0 && tx <= x0 + btn_w && ty >= y0 && ty <= y0 + btn_h) {
                if (pin_index < PIN_MAX_LEN) {
                    pin_buffer[pin_index++] = '0';
                    pin_buffer[pin_index] = '\0';
                }
            }
            // OK Button
            uint16_t ok_x = start_x;
            uint16_t ok_y = start_y + 3 * (btn_h + gap);
            if (tx >= ok_x && tx <= ok_x + btn_w && ty >= ok_y && ty <= ok_y + btn_h) {
                return pin_buffer;
            }
            // CLR Button
            uint16_t clr_x = start_x + 2 * (btn_w + gap);
            uint16_t clr_y = start_y + 3 * (btn_h + gap);
            if (tx >= clr_x && tx <= clr_x + btn_w && ty >= clr_y && ty <= clr_y + btn_h) {
                memset(pin_buffer, 0, sizeof(pin_buffer));
                BSP_LCD_DisplayStringAt(0, 3,(uint8_t *)"       ", LEFT_MODE);
                pin_index = 0;
            }

            // Anzeige aktualisieren
            BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
            BSP_LCD_FillRect(0, ySize - 40, xSize, 30);
            BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
            BSP_LCD_DisplayStringAt(0, 3, (uint8_t *)pin_buffer, LEFT_MODE);
            // Button 0
            BSP_LCD_DrawRect(start_x + btn_w + gap, start_y + 3 * (btn_h + gap), btn_w, btn_h);
            BSP_LCD_DisplayStringAt(start_x + btn_w + gap + 20, start_y + 3 * (btn_h + gap) + 20, (uint8_t *)"0", LEFT_MODE);

            // OK-Button
            BSP_LCD_SetTextColor(LCD_COLOR_GREEN);
            BSP_LCD_DrawRect(start_x, start_y + 3 * (btn_h + gap), btn_w, btn_h);
            BSP_LCD_DisplayStringAt(start_x + 10, start_y + 3 * (btn_h + gap) + 15, (uint8_t *)"OK", LEFT_MODE);

            // Clear-Button
            BSP_LCD_SetTextColor(LCD_COLOR_RED);
            BSP_LCD_DrawRect(start_x + 2 * (btn_w + gap), start_y + 3 * (btn_h + gap), btn_w, btn_h);
            BSP_LCD_DisplayStringAt(start_x + 10 + 2 * (btn_w + gap), start_y + 3 * (btn_h + gap) + 20, (uint8_t *)"CLR", LEFT_MODE);

            BSP_LCD_SetTextColor(LCD_COLOR_BLACK); // Reset Farbe
        }
    }
    return NULL;
}*/


/*void start_countdown() {
    countdown_start = HAL_GetTick();
    countdown_last_update = countdown_start;
    countdown_remaining = 30;
}*/


/*void update_countdown() {
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
}*/
