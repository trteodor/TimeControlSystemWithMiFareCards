/*
 *
 *  *  The MIT License.
 *  Created on: 23.04.2021
 *      Author: Teodor Rosolowski
 * 	    trteodor@gmail.com
 *      Added/Deleted Few Function and modified
 *
 *		I Add, structures, dynamic allocate memory and windows. Also i modified most of features.
 *		The functions added entirely by me contain the prefix Window
 *
 * GFX_BW.h
 *			Based on:
 *					GFX_BW.h
 *  				The MIT License.
 *  				Created on: 25.05.2017
 *     				 Author: Mateusz Salamon
 *     				 www.msalamon.pl
 *     				 mateusz@msalamon.pl
 */

#ifndef GFX_BW_H_
#define GFX_BW_H_



typedef struct __GFX_td
{
	uint8_t *OutBuffer;
	uint32_t WindowWidth;
	uint32_t WindowHeigh;

}GFX_td;

#define AVR_USING 0
#define STM32_USING 1

#define WIDTH SSD1306_LCDWIDTH
#define HEIGHT SSD1306_LCDHEIGHT
#define PIXEL_BLACK BLACK
#define PIXEL_WHITE	WHITE
#define PIXEL_INVERSE	INVERSE



#define DividerToPixel 8
#define LCDWIDTH       128
#define LCDHEIGHT      64

//This value depend from your screen
		/*For example, monochrome in 1 byte is 8pixel in hor
		 * ciezko to wyjasnic, ae problem jest taki ze w kolorowym
		 * bedzie trzeba mnozyc i wtedy to makro chyba jako 1/8 wystarczy zapisac
		 *  ae nwm w sumie
		 *  ale jest makro wiec juz cos mozna podziałać
		 */

#define USING_STRINGS 1 // 0 - do not compile, 1 - compile

#define USING_IMAGE 1
#if USING_IMAGE == 1
#define USING_IMAGE_ROTATE 1
#endif





//void SSD1306_DrawPixel(GFX_td *GFXstr,int16_t x, int16_t y, uint8_t Color);

// Trygonometric graphic functions
#define USING_RECTANGLE 1
#define USING_CIRCLE 1
#define USING_FILL_CIRCLE 1
#define USING_ROUND_RECTANGLE 1
#define USING_FILL_ROUND_RECTANGLE 1
#define USING_TRIANGLE 1
#define USING_FILL_TRIANGLE 1
#if ((USING_FILL_ROUND_RECTANGLE == 0) && (USING_STRINGS == 0))
#define USING_FILL_RECTANGLE 0
#endif
#if (USING_RECTANGLE == 0) && (USING_FILL_RECTANGLE == 0) && (USING_FILL_CIRCLE == 0) && (USING_ROUND_RECTANGLE == 0) && (USING_TRIANGLE == 0) && (USING_FILL_TRIANGLE == 0)
#define USING_LINES 0
#endif

/****************************************************************/

#if (USING_FILL_ROUND_RECTANGLE == 1 || USING_STRINGS == 1)
#define USING_FILL_RECTANGLE 1
#endif
#if (USING_RECTANGLE == 1) || (USING_FILL_RECTANGLE == 1) || (USING_FILL_CIRCLE == 1) || (USING_ROUND_RECTANGLE == 1) || (USING_TRIANGLE == 1) || (USING_FILL_TRIANGLE == 1)
#define USING_LINES 1
#endif
#if USING_ROUND_RECTANGLE == 1
#define CIRCLE_HELPER
#endif
#if (USING_FILL_CIRCLE == 1) || (USING_FILL_ROUND_RECTANGLE == 1)
#define FILL_CIRCLE_HELPER
#endif


//Windows
GFX_td* GFX_CreateWindow(int sizeWidth, int sizeHigh);
GFX_td* GFX_CreateScreen();
void GFX_ClearBuffer(GFX_td *GFXstr,int16_t x, int16_t y);

int GFX_Window_Hor_ScrollRight(GFX_td *ImageIn,GFX_td *ImageOut,int inrows, int incol,uint8_t color, int numRowShift,int scrollprocessHor);
int GFX_Window_VerScrollFlow(GFX_td *ImageIn,GFX_td *ImageOut,int inrows, int incol,uint8_t color, int numRowShift,int shiftval, int HALF);
int GFX_WindowMirror(GFX_td *Image, int inrows, int incol,uint8_t color, int axis);
int GFX_WindowRotate(GFX_td *Image, int inrows, int incol,uint8_t color, int angle);
void GFX_PutWindow(GFX_td *InBuf,GFX_td *ScrOutBuf,int place_x_put, int place_y_put);

extern void GFX_DrawPixel(GFX_td *GFXstr,int16_t x, int16_t y, uint8_t Color);
void GFX_SetFont(const uint8_t* font_t);
void GFX_SetFontSize(uint8_t size_t);
uint8_t GFX_GetFontHeight(void);
uint8_t GFX_GetFontWidth(void);
uint8_t  GFX_GetFontSize(void);
void GFX_DrawChar(GFX_td *GFXstr,int x, int y, char chr, uint8_t color, uint8_t background);
void GFX_DrawString(GFX_td *GFXstr,int x, int y, char* str, uint8_t color, uint8_t background);


#if USING_LINES == 1
void GFX_DrawLine(GFX_td *GFXstr,int x_start, int y_start, int x_end, int y_end, uint8_t color);
#endif

#if USING_RECTANGLE == 1
void GFX_DrawRectangle(GFX_td *GFXstr,int x, int y, uint16_t w, uint16_t h, uint8_t color);
#endif
#if USING_FILL_RECTANGLE ==1
void GFX_DrawFillRectangle(GFX_td *GFXstr,int x, int y, uint16_t w, uint16_t h, uint8_t color);
#endif
#if USING_CIRCLE == 1
void GFX_DrawCircle(GFX_td *GFXstr,int x0, int y0, uint16_t r, uint8_t color);
#endif
#if USING_FILL_CIRCLE == 1
void GFX_DrawFillCircle(GFX_td *GFXstr,int x0, int y0, uint16_t r, uint8_t color);
#endif
#if USING_ROUND_RECTANGLE == 1
void GFX_DrawRoundRectangle(GFX_td *GFXstr,int x, int y, uint16_t w, uint16_t h, uint16_t r, uint8_t color);
#endif
#if USING_FILL_ROUND_RECTANGLE == 1
void GFX_DrawFillRoundRectangle(GFX_td *GFXstr,int x, int y, uint16_t w, uint16_t h, uint16_t r, uint8_t color);
#endif
#if USING_TRIANGLE == 1
void GFX_DrawTriangle(GFX_td *GFXstr,int x0, int y0, int x1, int y1, int x2, int y2, uint8_t color);
#endif
#if USING_FILL_TRIANGLE == 1
void GFX_DrawFillTriangle(GFX_td *GFXstr,int x0, int y0, int x1, int y1, int x2, int y2, uint8_t color);
#endif
#if USING_IMAGE == 1
#if AVR_USING ==1
void GFX_Image_P(int x, int y, uint8_t *img, uint8_t w, uint8_t h, uint8_t color);
#endif
#if STM32_USING ==1


void GFX_Image(GFX_td *GFXstr,int x, int y, const uint8_t *img, uint8_t w, uint8_t h, uint8_t color);
#if USING_IMAGE_ROTATE == 1
void GFX_ImageRotate(GFX_td *GFXstr, int x, int y, const uint8_t *img, uint8_t w, uint8_t h, uint8_t color, uint16_t angle);
#endif
#endif
#endif

#endif /* GFX_BW_H_ */
