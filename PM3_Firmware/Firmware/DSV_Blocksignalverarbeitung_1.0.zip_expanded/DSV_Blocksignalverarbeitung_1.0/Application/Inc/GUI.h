/************************************************************************************/
/*
                                    G U I . H

 ------------------------------------------------------------------------------------

  (c) Copyright       ZHAW
                      Technikumstr. 9
                      CH-8200 Winterthur

 ------------------------------------------------------------------------------------

  Subsystem         : APP (Application)
  Module            : GUI
  Type of SW-Unit   : H File

 ------------------------------------------------------------------------------------

  Description:
  ------------
  Graphical user interface functions.
 ------------------------------------------------------------------------------------

*/
/************************************************************************************/
#ifndef APPLICATION_INC_GUI_H_
#define APPLICATION_INC_GUI_H_

/*** Imported external Objects ******************************************************/
/************************************************************************************/
#include <stdio.h>
#include <stdint.h>
#include "LAB_CONFIG.h"


/*** Exported Objects ***************************************************************/
/************************************************************************************/

/*** Constants **********************************************************************/

/*** Types **************************************************************************/
// Page 1: settings
// Page 2: Oszilloskope
// Page 3: Spektrum Analyzer
// Page 4: Funktionsgenerator
// Page 5: Board Info
typedef enum {PAGE1, PAGE2, PAGE3, PAGE4, PAGE5} menue_page_t;


// waveform generator selection
typedef enum {WAV_SINUS, WAV_RECTANGLE, WAV_SAWTOOTH} wav_gen_t;


/*** Events *************************************************************************/

/*** Global variables ***************************************************************/

// page handling
extern menue_page_t menue_page;
extern volatile uint32_t user_button;
extern volatile uint32_t user_mode;

// wav gen
extern float wav_gen_output_frequency;
extern wav_gen_t wav_gen;

// volume handling
extern float audio_input_volume_dB;
extern float audio_output_volume_dB;

// buffers to read data from the codec
extern volatile int32_t plotBufferCH1_in[PLOT_IO_DATA_BUFFER + 10];
extern volatile int32_t plotBufferCH2_in[PLOT_IO_DATA_BUFFER + 10];
extern volatile int32_t plotBufferCH1_out[PLOT_IO_DATA_BUFFER + 10];
extern volatile int32_t plotBufferCH2_out[PLOT_IO_DATA_BUFFER + 10];
extern volatile uint32_t fetchNewPlotDataNow;
extern volatile uint32_t newPlotDataReady;

/*** Functions **********************************************************************/

/***********************************************************************************/
/*
	initGUI

 	Initialize the GUI module.
*/
/***********************************************************************************/
void initGUI(void);

/***********************************************************************************/
/*
	initPageX

 	Draw the entire GUI of the according page onto the screen.
*/
/***********************************************************************************/
void initPage1(void);
void initPage2(void);
void initPage3(void);
void initPage4(void);
void initPage5(void);


/***********************************************************************************/
/*
	checkButtonsPageX

 	Test for all the possible inputs on the page X.
*/
/***********************************************************************************/
void checkButtonsPage1(void);
void checkButtonsPage2(void);
void checkButtonsPage3(void);
void checkButtonsPage4(void);
void checkButtonsPage5(void);

/***********************************************************************************/
/*
	updatePageX

 	Update only the changed information on the page X.
*/
/***********************************************************************************/
void updatePage1(void);
void updatePage2(void);
void updatePage3(void);
void updatePage4(void);
void updatePage5(void);

/***********************************************************************************/
/*
	diverse

 	Page switch handling.
*/
/***********************************************************************************/
void drawTitle(void);
void checkPageSwitchButtons(void);

#endif /* APPLICATION_INC_GUI_H_ */
