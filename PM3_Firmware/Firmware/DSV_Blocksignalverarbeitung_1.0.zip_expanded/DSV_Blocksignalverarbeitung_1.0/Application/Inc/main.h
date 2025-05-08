/************************************************************************************/
/*
                             M A I N. H

 ------------------------------------------------------------------------------------

  (c) Copyright       ZHAW
                      Technikumstr. 9
                      CH-8200 Winterthur

 ------------------------------------------------------------------------------------

  Subsystem         : APP (Application)
  Module            : main
  Type of SW-Unit   : H File

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
  * @file    Audio/Audio_playback_and_record/Inc/main.h 
  * @author  MCD Application Team
  * @version V1.2.1
  * @date    14-April-2017
  * @brief   Header for main.c module
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

#ifndef __MAIN_H
#define __MAIN_H

/*** Imported external Objects ******************************************************/
/************************************************************************************/
#include "stdio.h"
#include "usbh_core.h"
#include "usbh_diskio.h"
#include "stm32f769i_discovery_audio.h"
#include "ff.h"
#include "ff_gen_drv.h"


/*** Exported Objects ***************************************************************/
/************************************************************************************/

/*** Constants **********************************************************************/
#define FILEMGR_LIST_DEPDTH                        24
#define FILEMGR_FILE_NAME_SIZE                     40
#define FILEMGR_FULL_PATH_SIZE                     256
#define FILEMGR_MAX_LEVEL                          4    
#define FILETYPE_DIR                               0
#define FILETYPE_FILE                              1

/*** Types **************************************************************************/
/* Application State Machine Structure */
typedef enum {
  APPLICATION_IDLE = 0,
  APPLICATION_START,
  APPLICATION_READY,
  APPLICATION_DISCONNECT,
}AUDIO_ApplicationTypeDef;

typedef struct _FILELIST_LineTypeDef {
  uint8_t type;
  uint8_t name[FILEMGR_FILE_NAME_SIZE];
}FILELIST_LineTypeDef;

typedef struct _FILELIST_FileTypeDef {
  FILELIST_LineTypeDef  file[FILEMGR_LIST_DEPDTH] ;
  uint16_t              ptr; 
}FILELIST_FileTypeDef;


/*** Global variables ***************************************************************/
extern USBH_HandleTypeDef hUSBHost;
extern AUDIO_ApplicationTypeDef appli_state;
extern FATFS USBH_fatfs;

extern char user_text_line_mode_1[USER_TEXT_LENGTH + 1];
extern char user_text_line_mode_2[USER_TEXT_LENGTH + 1];
extern char user_text_line_mode_3[USER_TEXT_LENGTH + 1];
extern char user_text_line_mode_4[USER_TEXT_LENGTH + 1];
extern char user_text_line_mode_5[USER_TEXT_LENGTH + 1];


#endif /* __MAIN_H */


