/** ***************************************************************************
 * @file
 * @brief The menu
 *
 * Initializes and displays the menu.
 * @n Provides the function MENU_check_transition() for polling user actions.
 * The variable MENU_transition is set to the touched menu item.
 * If no touch has occurred the variable MENU_transition is set to MENU_NONE
 * @n If the interrupt handler is enabled by calling BSP_TS_ITConfig();
 * the variable MENU_transition is set to the touched menu entry as above.
 * @n Either call once BSP_TS_ITConfig() to enable the interrupt
 * or MENU_check_transition() in the main while loop for polling.
 * @n The function MENU_get_transition() returns the new menu item.
 *
 * @author  Hanspeter Hochreutener, hhrt@zhaw.ch
 * @date	30.04.2020
 *****************************************************************************/


/******************************************************************************
 * Includes
 *****************************************************************************/
#include "stm32f4xx.h"
#include "stm32f429i_discovery.h"
#include "stm32f429i_discovery_lcd.h"
#include "stm32f429i_discovery_ts.h"



#include "menu.h"
#include <stdio.h>
#include <string.h>
#include "misc.h"
#include "main.h"


/******************************************************************************
 * Defines
 *****************************************************************************/
#define MENU_FONT				&Font12	///< Possible font sizes: 8 12 16 20 24
#define MENU_HEIGHT				40		///< Height of menu bar
#define MENU_MARGIN				2		///< Margin around a menu entry
/** Position of menu bar: 0 = top, (BSP_LCD_GetYSize()-MENU_HEIGHT) = bottom */
#define MENU_Y					(BSP_LCD_GetYSize()-MENU_HEIGHT)

#define BTN_SIZE 100
#define BTN_NUM 2
#define BTN_GAP 13
#define TIME_DEFUSE 10
/******************************************************************************
 * Variables
 *****************************************************************************/
static MENU_item_t MENU_transition = MENU_NONE;	///< Transition to this menu
static MENU_entry_t MENU_entry[MENU_ENTRY_COUNT] = {
		{"Test",	" ",		LCD_COLOR_BLACK,	LCD_COLOR_LIGHTBLUE},
		{"Timer",	"+IRQ",		LCD_COLOR_BLACK,	LCD_COLOR_LIGHTGREEN},
		{"DMA",	    "+IRQ",		LCD_COLOR_BLACK,	LCD_COLOR_LIGHTRED},
		{"DMA",	    "dual",		LCD_COLOR_BLACK,	LCD_COLOR_LIGHTCYAN},
		{"DMA",	    "scan",		LCD_COLOR_BLACK,	LCD_COLOR_LIGHTMAGENTA},
		{"DMA",	    "scan_",	LCD_COLOR_BLACK,	LCD_COLOR_LIGHTYELLOW}
};										///< All the menu entries

#define PIN_MAX_LEN 6
#define PIN_MIN_LEN 4
int xSize = 240; // Breite des Displays in Pixeln
int ySize = 320; // Höhe des Displays in Pixeln

/******************************************************************************
 * Functions
 *****************************************************************************/


/** ***************************************************************************
 * @brief Draw the menu onto the display.
 *
 * Each menu entry has two lines.
 * Text and background colors are applied.
 * @n These attributes are defined in the variable MENU_draw[].
 *****************************************************************************/
void MENU_draw(void)
{
	BSP_LCD_SetFont(MENU_FONT);
	uint32_t x, y, m, w, h;
	y = MENU_Y;
	m = MENU_MARGIN;
	w = BSP_LCD_GetXSize()/MENU_ENTRY_COUNT;
	h = MENU_HEIGHT;
	for (uint32_t i = 0; i < MENU_ENTRY_COUNT; i++) {
		x = i*w;
		BSP_LCD_SetTextColor(MENU_entry[i].back_color);
		BSP_LCD_FillRect(x+m, y+m, w-2*m, h-2*m);
		BSP_LCD_SetBackColor(MENU_entry[i].back_color);
		BSP_LCD_SetTextColor(MENU_entry[i].text_color);
		BSP_LCD_DisplayStringAt(x+3*m, y+3*m,
				(uint8_t *)MENU_entry[i].line1, LEFT_MODE);
		BSP_LCD_DisplayStringAt(x+3*m, y+h/2,
				(uint8_t *)MENU_entry[i].line2, LEFT_MODE);
	}
}


/** ***************************************************************************
 * @brief Shows a hint at startup.
 *
 *****************************************************************************/
void MENU_hint(void)
{
	BSP_LCD_SetBackColor(LCD_COLOR_WHITE);
	BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
	BSP_LCD_SetFont(&Font24);
	BSP_LCD_DisplayStringAt(5,10, (uint8_t *)"DEMO-CODE", LEFT_MODE);
	BSP_LCD_SetFont(&Font16);
	BSP_LCD_DisplayStringAt(5, 60, (uint8_t *)"Touch a menu item", LEFT_MODE);
	BSP_LCD_DisplayStringAt(5, 80, (uint8_t *)"to start an ADC demo", LEFT_MODE);
	BSP_LCD_DisplayStringAt(5, 110, (uint8_t *)"Switch DAC on/off", LEFT_MODE);
	BSP_LCD_DisplayStringAt(5, 130, (uint8_t *)"with blue pushbutton", LEFT_MODE);
	BSP_LCD_DisplayStringAt(5, 160, (uint8_t *)"(c) hhrt@zhaw.ch", LEFT_MODE);
	BSP_LCD_DisplayStringAt(5, 160, (uint8_t *)"Version 27.09.2022", LEFT_MODE);
}


/** ***************************************************************************
 * @brief Set a menu entry.
 * @param [in] item number of menu bar
 * @param [in] entry attributes for that item
 *
 * @note Call MENU_draw() to update the display.
 *****************************************************************************/
void MENU_set_entry(const MENU_item_t item, const MENU_entry_t entry)
{
	if ((0 <= item) && (MENU_ENTRY_COUNT > item)) {
		MENU_entry[item] = entry;
	}
}


/** ***************************************************************************
 * @brief Get a menu entry.
 * @param [in] item number of menu bar
 * @return Menu_entry[item] or Menu_entry[0] if item not in range
 *****************************************************************************/
MENU_entry_t MENU_get_entry(const MENU_item_t item)
{
	MENU_entry_t entry = MENU_entry[0];
	if ((0 <= item) && (MENU_ENTRY_COUNT > item)) {
		entry = MENU_entry[item];
	}
	return entry;
}


/** ***************************************************************************
 * @brief Get menu selection/transition
 *
 * @return the selected MENU_item or MENU_NONE if no MENU_item was selected
 *
 * MENU_transition is used as a flag.
 * When the value is read by calling MENU_get_transition()
 * this flag is cleared, respectively set to MENU_NONE.
 *****************************************************************************/
MENU_item_t MENU_get_transition(void)
{
	MENU_item_t item = MENU_transition;
	MENU_transition = MENU_NONE;
	return item;
}


/** ***************************************************************************
 * @brief Check for selection/transition
 *
 * If the last transition has been consumed (MENU_NONE == MENU_transition)
 * and the touchscreen has been touched for a defined period
 * the variable MENU_transition is set to the touched item.
 * @note  Evalboard revision E (blue PCB) has an inverted y-axis
 * in the touch controller compared to the display.
 * Uncomment or comment the <b>\#define EVAL_REV_E</b> in main.h accordingly.
 *****************************************************************************/
void MENU_check_transition(void)
{
	static MENU_item_t item_old = MENU_NONE;
	static MENU_item_t item_new = MENU_NONE;
	static TS_StateTypeDef  TS_State;	// State of the touch controller
	BSP_TS_GetState(&TS_State);			// Get the state


// Evalboard revision E (blue) has an inverted y-axis in the touch controller
#ifdef EVAL_REV_E
	TS_State.Y = BSP_LCD_GetYSize() - TS_State.Y;	// Invert the y-axis
#endif
	// Invert x- and y-axis if LCD is flipped
#ifdef FLIPPED_LCD
	TS_State.X = BSP_LCD_GetXSize() - TS_State.X;	// Invert the x-axis
	TS_State.Y = BSP_LCD_GetYSize() - TS_State.Y;	// Invert the y-axis
#endif


/*
	#if (defined(EVAL_REV_E) && !defined(FLIPPED_LCD)) || (!defined(EVAL_REV_E) && defined(FLIPPED_LCD))
	TS_State.Y = BSP_LCD_GetYSize() - TS_State.Y;	// Invert the y-axis
#endif
#ifdef EVAL_REV_E
#endif
*/
	if (TS_State.TouchDetected) {		// If a touch was detected
		/* Do only if last transition not pending anymore */
		if (MENU_NONE == MENU_transition) {
			item_old = item_new;		// Store old item
			/* If touched within the menu bar? */
			if ((MENU_Y < TS_State.Y) && (MENU_Y+MENU_HEIGHT > TS_State.Y)) {
				item_new = TS_State.X	// Calculate new item
						/ (BSP_LCD_GetXSize()/MENU_ENTRY_COUNT);
				if ((0 > item_new) || (MENU_ENTRY_COUNT <= item_new)) {
					item_new = MENU_NONE;	// Out of bounds
				}
				if (item_new == item_old) {	// 2 times the same menu item
					item_new = MENU_NONE;
					MENU_transition = item_old;
				}
			}
		}
	}
}



/** ***************************************************************************
 * @brief Interrupt handler for the touchscreen
 *
 * @note BSP_TS_ITConfig(); must be called in the main function
 * to enable touchscreen interrupt.
 * @note There are timing issues when interrupt is enabled.
 * It seems that polling is the better choice with this evaluation board.
 * @n Call MENU_check_transition() from the while loop in main for polling.
 *
 * The touchscreen interrupt is connected to PA15.
 * @n The interrupt handler for external line 15 to 10 is called.
 *****************************************************************************/
void EXTI15_10_IRQHandler(void)
{
	if (EXTI->PR & EXTI_PR_PR15) {		// Check if interrupt on touchscreen
		EXTI->PR |= EXTI_PR_PR15;		// Clear pending interrupt on line 15
		if (BSP_TS_ITGetStatus()) {		// Get interrupt status
			BSP_TS_ITClear();				// Clear touchscreen controller int.
			MENU_check_transition();
		}
		EXTI->PR |= EXTI_PR_PR15;		// Clear pending interrupt on line 15
	}
}



int SHOW_STARTMENU(void)
{

	int pressed = 0;


	int xSize = 240;

	BSP_LCD_SetFont(&Font20);
	BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
	BSP_LCD_SetBackColor(LCD_COLOR_WHITE);
	BSP_LCD_DisplayStringAt(0, 3, (uint8_t *)"Security System", CENTER_MODE);

	BSP_LCD_SetTextColor(LCD_COLOR_RED);
	BSP_LCD_FillRect(BTN_GAP, 200, 100, 100);
	BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
	BSP_LCD_SetBackColor(LCD_COLOR_RED);
	BSP_LCD_SetFont(&Font24);
	BSP_LCD_DisplayStringAt(BTN_GAP+26, 240, (uint8_t *)"ARM", LEFT_MODE);

	BSP_LCD_SetTextColor(LCD_COLOR_LIGHTBLUE);
	BSP_LCD_FillRect(BTN_GAP+BTN_SIZE+BTN_GAP, 200, 100, 100);
	BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
	BSP_LCD_SetBackColor(LCD_COLOR_LIGHTBLUE);
	BSP_LCD_SetFont(&Font24);
	BSP_LCD_DisplayStringAt(BTN_GAP+BTN_SIZE+BTN_GAP+26, 230, (uint8_t *)"SET", LEFT_MODE);
	BSP_LCD_DisplayStringAt(BTN_GAP+BTN_SIZE+BTN_GAP+26, 250, (uint8_t *)"PIN", LEFT_MODE);
	while(1)
	{
		uint16_t touched = 0;
		static TS_StateTypeDef  TS_State;	// State of the touch controller
		BSP_TS_GetState(&TS_State);
		uint16_t x = TS_State.X;
		uint16_t y = TS_State.Y;


		if (TS_State.TouchDetected)
		{
			#ifdef EVAL_REV_E
				y = BSP_LCD_GetYSize() - y;
			#endif
			#ifdef FLIPPED_LCD
				x = BSP_LCD_GetXSize() - x;
				y = BSP_LCD_GetYSize() - y;
			#endif
			if((x >= BTN_GAP)&&(x <= (BTN_GAP+BTN_SIZE))&& (y>=240)&&(y<=350))
			{
				pressed = 1;
				touched = 1;
			}
			else if((x >= (BTN_GAP+BTN_SIZE+BTN_GAP))&&(x <= (BTN_GAP+BTN_SIZE+BTN_GAP+BTN_SIZE))&& (y>=240)&&(y<=350))
			{
				pressed = 2;
				touched = 1;
			}
		}
		while(TS_State.TouchDetected)
		{
			BSP_TS_GetState(&TS_State);
		}
		if(touched ==1)
			break;
	}


	return pressed;

}
char* SHOW_TRIGGEREDMENU(uint8_t alarmset)
{

	start_countdown(TIME_DEFUSE);
    static char pin_buffer[PIN_MAX_LEN + 1] = {0}; // Rückgabe-Buffer
    uint8_t pin_index = 0;
    TS_StateTypeDef TS_State;
    const uint16_t btn_w = 60, btn_h = 60;
    const uint16_t start_x = 20, start_y = 50;
    const uint16_t gap = 10;


    BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
    BSP_LCD_SetBackColor(LCD_COLOR_WHITE);
    BSP_LCD_SetFont(&Font24);
    HAL_Delay(1000);

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
	while (1) {
		if(alarmset)
		{
			BSP_LED_Toggle(LED4);			// Visual feedback when running
		}
		if(update_countdown() <= 0)
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

}


char* change_pin(void)
{
    static char new_pin[PIN_MAX_LEN + 1] = {0};
    uint8_t pin_index = 0;
    TS_StateTypeDef TS_State;
    const uint16_t btn_w = 60, btn_h = 60;
    const uint16_t start_x = 20, start_y = 50;
    const uint16_t gap = 10;

    BSP_LCD_Clear(LCD_COLOR_WHITE);
    BSP_LCD_SetFont(&Font24);
    BSP_LCD_SetBackColor(LCD_COLOR_WHITE);
    BSP_LCD_SetTextColor(LCD_COLOR_BLACK);

    // Zeichne Tastenfeld
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

    // Taste 0
    BSP_LCD_DrawRect(start_x + btn_w + gap, start_y + 3 * (btn_h + gap), btn_w, btn_h);
    BSP_LCD_DisplayStringAt(start_x + btn_w + gap + 20, start_y + 3 * (btn_h + gap) + 20, (uint8_t *)"0", LEFT_MODE);

    // OK-Taste
    BSP_LCD_SetTextColor(LCD_COLOR_GREEN);
    BSP_LCD_DrawRect(start_x, start_y + 3 * (btn_h + gap), btn_w, btn_h);
    BSP_LCD_DisplayStringAt(start_x + 10, start_y + 3 * (btn_h + gap) + 20, (uint8_t *)"OK", LEFT_MODE);

    // CLR-Taste
    BSP_LCD_SetTextColor(LCD_COLOR_RED);
    BSP_LCD_DrawRect(start_x + 2 * (btn_w + gap), start_y + 3 * (btn_h + gap), btn_w, btn_h);
    BSP_LCD_DisplayStringAt(start_x + 10 + 2 * (btn_w + gap), start_y + 3 * (btn_h + gap) + 20, (uint8_t *)"CLR", LEFT_MODE);

    BSP_LCD_SetTextColor(LCD_COLOR_BLACK);

    while (1) {
        BSP_TS_GetState(&TS_State);
        if (TS_State.TouchDetected) {
            HAL_Delay(150); // Debounce
            uint16_t tx = 240 - TS_State.X;
            uint16_t ty = TS_State.Y;

            // Prüfe Zahlen-Buttons
            for (uint8_t i = 1; i <= 9; i++) {
                uint16_t row = (i - 1) / 3;
                uint16_t col = (i - 1) % 3;
                uint16_t x = start_x + col * (btn_w + gap);
                uint16_t y = start_y + row * (btn_h + gap);
                if (tx >= x && tx <= x + btn_w && ty >= y && ty <= y + btn_h) {
                    if (pin_index < PIN_MAX_LEN) {
                        new_pin[pin_index++] = '0' + i;
                        new_pin[pin_index] = '\0';
                    }
                }
            }

            // Taste 0
            uint16_t x0 = start_x + btn_w + gap;
            uint16_t y0 = start_y + 3 * (btn_h + gap);
            if (tx >= x0 && tx <= x0 + btn_w && ty >= y0 && ty <= y0 + btn_h) {
                if (pin_index < PIN_MAX_LEN) {
                    new_pin[pin_index++] = '0';
                    new_pin[pin_index] = '\0';
                }
            }

            // OK-Taste
            uint16_t ok_x = start_x;
            uint16_t ok_y = start_y + 3 * (btn_h + gap);
            if (tx >= ok_x && tx <= ok_x + btn_w && ty >= ok_y && ty <= ok_y + btn_h) {
                if (pin_index >= PIN_MIN_LEN) {
                    return new_pin;
                } else {
                    BSP_LCD_DisplayStringAt(0, 10, (uint8_t *)"PIN zu kurz!", CENTER_MODE);
                }
            }

            // CLR-Taste
            uint16_t clr_x = start_x + 2 * (btn_w + gap);
            uint16_t clr_y = start_y + 3 * (btn_h + gap);
            if (tx >= clr_x && tx <= clr_x + btn_w && ty >= clr_y && ty <= clr_y + btn_h) {
                memset(new_pin, 0, sizeof(new_pin));
                pin_index = 0;
                BSP_LCD_FillRect(0, 0, xSize, 30);  // PIN-Anzeige löschen
            }

            // Eingabeanzeige aktualisieren
            BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
            BSP_LCD_FillRect(0, 0, xSize, 30);
            BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
            BSP_LCD_DisplayStringAt(0, 3, (uint8_t *)new_pin, LEFT_MODE);
        }
    }
}
