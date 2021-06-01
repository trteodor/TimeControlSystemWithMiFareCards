/*
 * OLED_SSD1306_Tasks.c
 *
 * This file is an example of using a prepared GFX with a display based on the SSD1306 driver
 *
 *  Created on: Apr 22, 2021
 *      Author: Teodor
 *      trteodor@gmail.com
 */

#include "OLED_SSD1306_Task.h"

void DrawActiveElement(GFX_td *Window,int elnum, char* String);
void DrawDontActiveElement(GFX_td *Window,int elnum, char* String);
void DrawHead(GFX_td *Window,int elofnum,int allselnum, char* String);

void DrawGFXDemo();
void OLED_ActiveTask();
void DrawTime();
void DrawStMachine();

uint32_t OL_Time=0;

//Windows pointer
GFX_td *MainWindow=0;
GFX_td *WindowVerScrH=0;
GFX_td *WindowVerScr=0;
GFX_td *WindowHorScr=0;
GFX_td *WindowHorScrH=0;
GFX_td *WindowVerStr=0;
GFX_td *WindowVerStrH=0;

MachineState M_State;

void OLED_Init()
{
					MainWindow= GFX_CreateScreen();  //Create Main Bufor Frame Pointer
					WindowVerScrH      = GFX_CreateWindow(120,32);
					WindowVerScr  = GFX_CreateWindow(120,16);
					WindowHorScrH  = GFX_CreateWindow(80,16);
					WindowHorScr  = GFX_CreateWindow(80,16);
					WindowVerStr = GFX_CreateWindow(80,16);
					WindowVerStrH = GFX_CreateWindow(80,16);

					GFX_SetFont(font_8x5);
					GFX_SetFontSize(1);
					SSD1306_I2cInit(&hi2c1);
					SSD1306_Bitmap((uint8_t*)picture);
					OL_Time=HAL_GetTick();
					M_State=ViewingTime;
}
void OLED_Task()
{
	OLED_ActiveTask();
}




void OLED_ActiveTask()
{
	if( (OL_Time+200) < HAL_GetTick())  //minimum delay time is around 80ms because better wait on end data transfer on i2c line OLED data with DMA
			{

								OL_Time=HAL_GetTick();
								//DrawGFXDemo();
								DrawStMachine();
	}
}
void DrawGFXDemo()
{
	static uint32_t Counter_D=0;
		static uint32_t scrollprocessVer=0;
		static uint32_t scrollprocessHor=0;
		static uint32_t scrollprocessStr=0;
		Counter_D++;


				GFX_SetFontSize(1);
				GFX_ClearBuffer(MainWindow,LCDWIDTH, LCDHEIGHT);

				char Counter_c[20];
				sprintf(Counter_c, "ILOSC: %lu", Counter_D);
				GFX_DrawString(MainWindow,10,10, Counter_c, WHITE, BLACK);
				GFX_DrawString(MainWindow, 10, 20, "POZDRAWIAM", WHITE, BLACK);





				//Scroll Effects
				GFX_DrawString(WindowVerStrH,0,0, "PIONOWO  ", WHITE, BLACK);
				GFX_WindowRotate(WindowVerStrH, 55, 8, WHITE, 90);
		  		if(scrollprocessStr<55)
		  		{
		  			GFX_Window_VerScrollFlow(WindowVerStrH, WindowVerStr , 8, 40, WHITE, 55,scrollprocessStr,1);
		  			scrollprocessStr++;
		  		}
		  		if(scrollprocessStr>54) scrollprocessStr=0;
		  		GFX_PutWindow(WindowVerStr, MainWindow, 110,5);




		  			GFX_DrawString(WindowVerScrH,0,0, "POZDRAWIAM ALLS", WHITE, BLACK);
		  			GFX_DrawString(WindowVerScrH,0,8, "To Dziala!", WHITE, BLACK);
			  		if(scrollprocessVer<16)
			  		{
			  			GFX_Window_VerScrollFlow(WindowVerScrH, WindowVerScr , 120, 16, 1, 16,scrollprocessVer,1);
			  			scrollprocessVer++;
			  		}
			  		if(scrollprocessVer>15) scrollprocessVer=0;
			  		GFX_PutWindow(WindowVerScr, MainWindow, 5, 36);




					GFX_DrawString(WindowHorScrH,0,0, "Teodor Test", WHITE, BLACK);
			  		if(scrollprocessHor<97)
			  		{
			  			GFX_Window_Hor_ScrollRight(WindowHorScrH, WindowHorScr,60, 8,1, 70,scrollprocessHor);
			  			scrollprocessHor++;
			  		}
			  		if(scrollprocessHor>96)scrollprocessHor=0;
			  		GFX_PutWindow(WindowHorScr, MainWindow, 20, 56);
			  	//End Section Scroll Effects

			  		SSD1306_Display(MainWindow);
}

void DrawStMachine()
{
static uint32_t ViewingPersonTimer=0;

	if(HAL_GetTick()-ViewingPersonTimer > 3000)
	{
		M_State=ViewingTime;
	}
	 switch(M_State)
	 {
	 case ViewingPerson:  // TODO set and create in rc522_app.c function RC522_appTask() it requires change
		 break;
	 case ViewingTime:
		 ViewingPersonTimer=HAL_GetTick();

			GFX_ClearBuffer(MainWindow,LCDWIDTH, LCDHEIGHT);

			 HAL_RTC_GetTime(&hrtc, &RtcTime, RTC_FORMAT_BIN);
			 HAL_RTC_GetDate(&hrtc, &RtcDate, RTC_FORMAT_BIN);

			 GFX_SetFontSize(2);
			 char Napis[200];
			 sprintf((char*)Napis, "D:%02d.%02d.20%02d", RtcDate.Date, RtcDate.Month,RtcDate.Year);
			 GFX_DrawString(MainWindow, 0, 0,Napis, WHITE, BLACK);
			 sprintf((char*)Napis, "T:%02d:%02d:%02d", RtcTime.Hours,RtcTime.Minutes, RtcTime.Seconds);
			 GFX_DrawString(MainWindow, 0, 20,Napis, WHITE, BLACK);
			 GFX_SetFontSize(1);
			 GFX_DrawString(MainWindow, 0, 35,"PRZYLOZ SWOJA", WHITE, BLACK);
			 GFX_DrawString(MainWindow, 0, 45,"KARTE RFID", WHITE, BLACK);
			 GFX_DrawString(MainWindow, 0, 55,"MILEGO DNIA!", WHITE, BLACK);

	 		SSD1306_Display(MainWindow);
	 break;
	 }

}

void DrawHead(GFX_td *Window,int elofnum,int allselnum, char* String)
{
	char *StringP=malloc(20);
	for(int i=0; i<20; i++)
	{
		(StringP[i]=String[i]);
	}
	GFX_SetFontSize(1);
	GFX_DrawString(Window,10,0,StringP, WHITE, BLACK);
	sprintf(StringP,"%i of %i", elofnum,allselnum);
	GFX_DrawString(Window,70,0,StringP, WHITE, BLACK);
	GFX_DrawLine(Window, 0, 15, 120, 15, WHITE);

	free(StringP);
}
void DrawActiveElement(GFX_td *Window,int elnum, char* String)
{
	int stringlong=0;
	for(; stringlong<50; stringlong++)
	{
		if(String[stringlong]=='\0')
		{
			break;
		}
	}
	GFX_SetFontSize(2);
	stringlong=stringlong*12;

	GFX_SetFontSize(2);
	GFX_DrawFillRectangle(Window, 19, elnum*16-1, stringlong, 16, WHITE);
	GFX_DrawString(Window,20,elnum*16, String, BLACK, INVERSE);
	GFX_DrawFillCircle(Window, 5, (elnum*16)+8, 5, WHITE);
}
void DrawDontActiveElement(GFX_td *Window,int elnum, char* String)
{
	int stringlong=0;
	for(; stringlong<50; stringlong++)
	{
		if(String[stringlong]=='\0')
		{
			break;
		}
	}
	GFX_SetFontSize(2);
	stringlong=stringlong*12;
	GFX_SetFontSize(2);
	GFX_DrawString(Window,20,elnum*16, String, WHITE, BLACK);
	GFX_DrawCircle(Window, 5, (elnum*16)+8, 5, WHITE);
}






