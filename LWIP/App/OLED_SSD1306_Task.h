/*
 * OLED_SSD1306_Tasks.j
 *
 *This file is an example of using a prepared GFX with a display based on the SSD1306 driver
 *
 *  Created on: Apr 22, 2021
 *      Author: Teodor
 *      trteodor@gmail.com
 */

#ifndef INC_OLED_SSD1306_TASK_H_
#define INC_OLED_SSD1306_TASK_H_

#include "stdio.h"
#include "stdlib.h"
#include "string.h"

#include "gpio.h"
#include "i2c.h"
#include "rtc.h"

#include "font_8x5.h"
#include "OLED_SSD1306.h"
#include "GFX_BW.h"
#include "picture.h"

extern void OLED_Init();
extern void OLED_Task();

typedef enum
{
	ViewingPerson,
	ViewingTime,
}MachineState;
extern MachineState M_State;


extern GFX_td *MainWindow;
extern GFX_td *WindowVerScrH;
extern GFX_td *WindowVerScr;
extern GFX_td *WindowHorScr;
extern GFX_td *WindowHorScrH;
extern GFX_td *WindowVerStr;
extern GFX_td *WindowVerStrH;


#endif /* INC_OLED_SSD1306_TASK_H_ */
