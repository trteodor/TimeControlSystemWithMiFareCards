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



#include "main.h"
#include "stm32h7xx_hal.h"

#include "GFX_BW.h"
#include <stdlib.h> // for abs() function
#include <string.h>

#define _swap_int(a, b) { int t = a; a = b; b = t; }


const uint8_t* font;
uint8_t size = 1;

//here put your extern function prototype
extern void SSD1306_DrawPixel(GFX_td *GFXstr,int16_t x, int16_t y, uint8_t Color);
extern uint8_t SSD1306_ReadPixelColor(GFX_td *GFXstr,int16_t x, int16_t y);
extern void SSD1306_Clear(GFX_td *GFXstr,int16_t x, int16_t y);

__attribute__((weak)) void GFX_DrawPixel(GFX_td *GFXstr,int16_t x, int16_t y, uint8_t Color)
{
	SSD1306_DrawPixel(GFXstr,x,y,Color);   //here call
}

__attribute__((weak)) int GFX_ReadPixel(GFX_td *GFXstr,int16_t x, int16_t y)
{
	return SSD1306_ReadPixelColor(GFXstr,x,y);
}
__attribute__((weak)) void GFX_ClearBuffer(GFX_td *GFXstr,int16_t x, int16_t y)
{
	 SSD1306_Clear(GFXstr,x,y);
}

GFX_td* GFX_CreateWindow(int sizeWidth, int sizeHigh)
{
	GFX_td *WindowPointer= malloc(sizeof(GFX_td) );

	WindowPointer->WindowHeigh=sizeHigh;
	WindowPointer->WindowWidth=sizeWidth;
	WindowPointer->OutBuffer=calloc(sizeHigh* (sizeWidth/DividerToPixel),1 );

	return WindowPointer;
}



GFX_td* GFX_CreateScreen()
{
	GFX_td *ScrenPointer= malloc(sizeof(GFX_td) );

	ScrenPointer->WindowHeigh=LCDHEIGHT;
	ScrenPointer->WindowWidth=LCDWIDTH;
	ScrenPointer->OutBuffer=calloc(LCDHEIGHT* (LCDWIDTH/DividerToPixel),1 );

	return ScrenPointer;
}

void GFX_PutWindow(GFX_td *InBuf,GFX_td *ScrOutBuf,int place_x_put, int place_y_put)
{
uint8_t color;

			for(uint32_t j=0; j<InBuf->WindowWidth; j++)
			{
				for(uint32_t i=0; i<InBuf->WindowHeigh; i++)
				{
					color=GFX_ReadPixel(InBuf,j,i);
					GFX_DrawPixel(ScrOutBuf,place_x_put+j, place_y_put+i, color);
				}
			}

}

void GFX_DrawChar(GFX_td *GFXstr,int x, int y, char chr, uint8_t color, uint8_t background)
{
	if(chr > 0x7E) return; // chr > '~'

	for(uint8_t i=0; i<font[1]; i++ )
	{
        uint8_t line = (uint8_t)font[(chr-0x20) * font[1] + i + 2];

        for(int8_t j=0; j<font[0]; j++, line >>= 1)
        {
            if(line & 1)
            {
            	if(size == 1)
            		GFX_DrawPixel(GFXstr,x+i, y+j, color);
            	else
            		GFX_DrawFillRectangle(GFXstr,x+i*size, y+j*size, size, size, color);
            }
            else if(background == 0)
            {
            	if(size == 1)
					GFX_DrawPixel(GFXstr,x+i, y+j, background);
				else
					GFX_DrawFillRectangle(GFXstr,x+i*size, y+j*size, size, size, background);
            }
        }
    }
}

void GFX_DrawString(GFX_td *GFXstr,int x, int y, char* str, uint8_t color, uint8_t background)
{
	int x_tmp = x;
	char znak;
	znak = *str;
	while(*str++)
	{
		GFX_DrawChar(GFXstr,x_tmp, y, znak, color, background);
		x_tmp += ((uint8_t)font[1] * size) + 1;
		if(background == 0)
		{
			for(uint8_t i=0; i<(font[0]*size); i++)
			{
				GFX_DrawPixel(GFXstr,x_tmp-1, y+i, background);
			}
		}
		znak = *str;
	}
}


void GFX_WriteLine(GFX_td *GFXstr,int x_start, int y_start, int x_end, int y_end, uint8_t color)
{
	int16_t steep = abs(y_end - y_start) > abs(x_end - x_start);

	    if (steep) {
	        _swap_int(x_start, y_start);
	        _swap_int(x_end, y_end);
	    }

	    if (x_start > x_end) {
	        _swap_int(x_start, x_end);
	        _swap_int(y_start, y_end);
	    }

	    int16_t dx, dy;
	    dx = x_end - x_start;
	    dy = abs(y_end - y_start);

	    int16_t err = dx / 2;
	    int16_t ystep;

	    if (y_start < y_end) {
	        ystep = 1;
	    } else {
	        ystep = -1;
	    }

	    for (; x_start<=x_end; x_start++) {
	        if (steep) {
	        	GFX_DrawPixel(GFXstr,y_start, x_start, color);
	        } else {
	        	GFX_DrawPixel(GFXstr,x_start, y_start, color);
	        }
	        err -= dy;
	        if (err < 0) {
	            y_start += ystep;
	            err += dx;
	        }
	    }
}

void GFX_DrawFastVLine(GFX_td *GFXstr,int x_start, int y_start, int h, uint8_t color)
{
	GFX_WriteLine(GFXstr,x_start, y_start, x_start, y_start+h-1, color);
}

void GFX_DrawFastHLine(GFX_td *GFXstr,int x_start, int y_start, int w, uint8_t color)
{
	GFX_WriteLine(GFXstr,x_start, y_start, x_start+w-1, y_start, color);
}

void GFX_DrawLine(GFX_td *GFXstr,int x_start, int y_start, int x_end, int y_end, uint8_t color)
{
	if(x_start == x_end){
	        if(y_start > y_end) _swap_int(y_start, y_end);
	        GFX_DrawFastVLine(GFXstr,x_start, y_start, y_end - y_start + 1, color);
	    } else if(y_start == y_end){
	        if(x_start > x_end) _swap_int(x_start, x_end);
	        GFX_DrawFastHLine(GFXstr,x_start, y_start, x_end - x_start + 1, color);
	    } else {

	    	GFX_WriteLine(GFXstr,x_start, y_start, x_end, y_end, color);
	    }
}

#if USING_RECTANGLE == 1
void GFX_DrawRectangle(GFX_td *GFXstr,int x, int y, uint16_t w, uint16_t h, uint8_t color)
{

    GFX_DrawFastHLine(GFXstr,x, y, w, color);
    GFX_DrawFastHLine(GFXstr,x, y+h-1, w, color);
    GFX_DrawFastVLine(GFXstr,x, y, h, color);
    GFX_DrawFastVLine(GFXstr,x+w-1, y, h, color);

}
#endif
#if USING_FILL_RECTANGLE == 1
void GFX_DrawFillRectangle(GFX_td *GFXstr,int x, int y, uint16_t w, uint16_t h, uint8_t color)
{
    for (int i=x; i<x+w; i++) {
    	GFX_DrawFastVLine(GFXstr,i, y, h, color);
    }

}
#endif
#if USING_CIRCLE == 1
void GFX_DrawCircle(GFX_td *GFXstr,int x0, int y0, uint16_t r, uint8_t color)
{
    int16_t f = 1 - r;
    int16_t ddF_x = 1;
    int16_t ddF_y = -2 * r;
    int16_t x = 0;
    int16_t y = r;

    GFX_DrawPixel(GFXstr,x0  , y0+r, color);
    GFX_DrawPixel(GFXstr,x0  , y0-r, color);
    GFX_DrawPixel(GFXstr,x0+r, y0  , color);
    GFX_DrawPixel(GFXstr,x0-r, y0  , color);

    while (x<y) {
        if (f >= 0) {
            y--;
            ddF_y += 2;
            f += ddF_y;
        }
        x++;
        ddF_x += 2;
        f += ddF_x;

        GFX_DrawPixel(GFXstr,x0 + x, y0 + y, color);
        GFX_DrawPixel(GFXstr,x0 - x, y0 + y, color);
        GFX_DrawPixel(GFXstr,x0 + x, y0 - y, color);
        GFX_DrawPixel(GFXstr,x0 - x, y0 - y, color);
        GFX_DrawPixel(GFXstr,x0 + y, y0 + x, color);
        GFX_DrawPixel(GFXstr,x0 - y, y0 + x, color);
        GFX_DrawPixel(GFXstr,x0 + y, y0 - x, color);
        GFX_DrawPixel(GFXstr,x0 - y, y0 - x, color);
    }

}
#endif
#ifdef CIRCLE_HELPER
void GFX_DrawCircleHelper(GFX_td *GFXstr, int x0, int y0, uint16_t r, uint8_t cornername, uint8_t color)
{
    int16_t f     = 1 - r;
    int16_t ddF_x = 1;
    int16_t ddF_y = -2 * r;
    int16_t x     = 0;
    int16_t y     = r;

    while (x<y) {
        if (f >= 0) {
            y--;
            ddF_y += 2;
            f     += ddF_y;
        }
        x++;
        ddF_x += 2;
        f     += ddF_x;
        if (cornername & 0x4) {
            GFX_DrawPixel(GFXstr,x0 + x, y0 + y, color);
            GFX_DrawPixel(GFXstr,x0 + y, y0 + x, color);
        }
        if (cornername & 0x2) {
            GFX_DrawPixel(GFXstr,x0 + x, y0 - y, color);
            GFX_DrawPixel(GFXstr,x0 + y, y0 - x, color);
        }
        if (cornername & 0x8) {
            GFX_DrawPixel(GFXstr,x0 - y, y0 + x, color);
            GFX_DrawPixel(GFXstr,x0 - x, y0 + y, color);
        }
        if (cornername & 0x1) {
            GFX_DrawPixel(GFXstr,x0 - y, y0 - x, color);
            GFX_DrawPixel(GFXstr,x0 - x, y0 - y, color);
        }
    }
}
#endif
#ifdef FILL_CIRCLE_HELPER
void GFX_DrawFillCircleHelper(GFX_td *GFXstr,int x0, int y0, uint16_t r, uint8_t cornername, int16_t delta, uint8_t color)
{

    int16_t f     = 1 - r;
    int16_t ddF_x = 1;
    int16_t ddF_y = -2 * r;
    int16_t x     = 0;
    int16_t y     = r;

    while (x<y) {
        if (f >= 0) {
            y--;
            ddF_y += 2;
            f     += ddF_y;
        }
        x++;
        ddF_x += 2;
        f     += ddF_x;

        if (cornername & 0x1) {
            GFX_DrawFastVLine(GFXstr,x0+x, y0-y, 2*y+1+delta, color);
            GFX_DrawFastVLine(GFXstr,x0+y, y0-x, 2*x+1+delta, color);
        }
        if (cornername & 0x2) {
            GFX_DrawFastVLine(GFXstr,x0-x, y0-y, 2*y+1+delta, color);
            GFX_DrawFastVLine(GFXstr,x0-y, y0-x, 2*x+1+delta, color);
        }
    }
}
#endif
#if USING_FILL_CIRCLE == 1
void GFX_DrawFillCircle(GFX_td *GFXstr,int x0, int y0, uint16_t r, uint8_t color)
{

	GFX_DrawFastVLine(GFXstr,x0, y0-r, 2*r+1, color);
    GFX_DrawFillCircleHelper(GFXstr,x0, y0, r, 3, 0, color);
}
#endif
#if USING_ROUND_RECTANGLE == 1
void GFX_DrawRoundRectangle(GFX_td *GFXstr,int x, int y, uint16_t w, uint16_t h, uint16_t r, uint8_t color)
{
	GFX_DrawFastHLine(GFXstr,x+r  , y    , w-2*r, color); // Top
    GFX_DrawFastHLine(GFXstr,x+r  , y+h-1, w-2*r, color); // Bottom
    GFX_DrawFastVLine(GFXstr,x    , y+r  , h-2*r, color); // Left
    GFX_DrawFastVLine(GFXstr,x+w-1, y+r  , h-2*r, color); // Right
    // draw four corners
    GFX_DrawCircleHelper(GFXstr,x+r    , y+r    , r, 1, color);
    GFX_DrawCircleHelper(GFXstr,x+w-r-1, y+r    , r, 2, color);
    GFX_DrawCircleHelper(GFXstr,x+w-r-1, y+h-r-1, r, 4, color);
    GFX_DrawCircleHelper(GFXstr,x+r    , y+h-r-1, r, 8, color);
}
#endif
#if USING_FILL_ROUND_RECTANGLE == 1
void GFX_DrawFillRoundRectangle(GFX_td *GFXstr,int x, int y, uint16_t w, uint16_t h, uint16_t r, uint8_t color)
{
    // smarter version

	GFX_DrawFillRectangle(GFXstr,x+r, y, w-2*r, h, color);

    // draw four corners
	GFX_DrawFillCircleHelper(GFXstr,x+w-r-1, y+r, r, 1, h-2*r-1, color);
	GFX_DrawFillCircleHelper(GFXstr,x+r    , y+r, r, 2, h-2*r-1, color);
}
#endif
#if USING_TRIANGLE == 1
void GFX_DrawTriangle(GFX_td *GFXstr,int x0, int y0, int x1, int y1, int x2, int y2, uint8_t color)
{
	GFX_DrawLine(GFXstr,x0, y0, x1, y1, color);
    GFX_DrawLine(GFXstr,x1, y1, x2, y2, color);
    GFX_DrawLine(GFXstr,x2, y2, x0, y0, color);
}
#endif
#if USING_FILL_TRIANGLE == 1
void GFX_DrawFillTriangle(GFX_td *GFXstr,int x0, int y0, int x1, int y1, int x2, int y2, uint8_t color)
{

    int16_t a, b, y, last;

    // Sort coordinates by Y order (y2 >= y1 >= y0)
    if (y0 > y1) {
    	_swap_int(y0, y1); _swap_int(x0, x1);
    }
    if (y1 > y2) {
    	_swap_int(y2, y1); _swap_int(x2, x1);
    }
    if (y0 > y1) {
    	_swap_int(y0, y1); _swap_int(x0, x1);
    }

    if(y0 == y2) { // Handle awkward all-on-same-line case as its own thing
        a = b = x0;
        if(x1 < a)      a = x1;
        else if(x1 > b) b = x1;
        if(x2 < a)      a = x2;
        else if(x2 > b) b = x2;
        GFX_DrawFastHLine(GFXstr,a, y0, b-a+1, color);
        return;
    }

    int16_t
    dx01 = x1 - x0,
    dy01 = y1 - y0,
    dx02 = x2 - x0,
    dy02 = y2 - y0,
    dx12 = x2 - x1,
    dy12 = y2 - y1;
    int32_t
    sa   = 0,
    sb   = 0;

    // For upper part of triangle, find scanline crossings for segments
    // 0-1 and 0-2.  If y1=y2 (flat-bottomed triangle), the scanline y1
    // is included here (and second loop will be skipped, avoiding a /0
    // error there), otherwise scanline y1 is skipped here and handled
    // in the second loop...which also avoids a /0 error here if y0=y1
    // (flat-topped triangle).
    if(y1 == y2) last = y1;   // Include y1 scanline
    else         last = y1-1; // Skip it

    for(y=y0; y<=last; y++) {
        a   = x0 + sa / dy01;
        b   = x0 + sb / dy02;
        sa += dx01;
        sb += dx02;
        /* longhand:
        a = x0 + (x1 - x0) * (y - y0) / (y1 - y0);
        b = x0 + (x2 - x0) * (y - y0) / (y2 - y0);
        */
        if(a > b) _swap_int(a,b);
        GFX_DrawFastHLine(GFXstr,a, y, b-a+1, color);
    }

    // For lower part of triangle, find scanline crossings for segments
    // 0-2 and 1-2.  This loop is skipped if y1=y2.
    sa = dx12 * (y - y1);
    sb = dx02 * (y - y0);
    for(; y<=y2; y++) {
        a   = x1 + sa / dy12;
        b   = x0 + sb / dy02;
        sa += dx12;
        sb += dx02;
        /* longhand:
        a = x1 + (x2 - x1) * (y - y1) / (y2 - y1);
        b = x0 + (x2 - x0) * (y - y0) / (y2 - y0);
        */
        if(a > b) _swap_int(a,b);
        GFX_DrawFastHLine(GFXstr,a, y, b-a+1, color);
    }
}
#endif

const double sinus_LUT[] =
{// 0* to 90* = 91 valuse
		0.0000,
		0.0175,
		0.0349,
		0.0523,
		0.0698,
		0.0872,
		0.1045,
		0.1219,
		0.1392,
		0.1564,
		0.1736,
		0.1908,
		0.2079,
		0.2250,
		0.2419,
		0.2588,
		0.2756,
		0.2924,
		0.3090,
		0.3256,
		0.3420,
		0.3584,
		0.3746,
		0.3907,
		0.4067,
		0.4226,
		0.4384,
		0.4540,
		0.4695,
		0.4848,
		0.5000,
		0.5150,
		0.5299,
		0.5446,
		0.5592,
		0.5736,
		0.5878,
		0.6018,
		0.6157,
		0.6293,
		0.6428,
		0.6561,
		0.6691,
		0.6820,
		0.6947,
		0.7071,
		0.7193,
		0.7314,
		0.7431,
		0.7547,
		0.7660,
		0.7771,
		0.7880,
		0.7986,
		0.8090,
		0.8192,
		0.8290,
		0.8387,
		0.8480,
		0.8572,
		0.8660,
		0.8746,
		0.8829,
		0.8910,
		0.8988,
		0.9063,
		0.9135,
		0.9205,
		0.9272,
		0.9336,
		0.9397,
		0.9455,
		0.9511,
		0.9563,
		0.9613,
		0.9659,
		0.9703,
		0.9744,
		0.9781,
		0.9816,
		0.9848,
		0.9877,
		0.9903,
		0.9925,
		0.9945,
		0.9962,
		0.9976,
		0.9986,
		0.9994,
		0.9998,
		1.0000,
};

double sinus(uint16_t angle)
{
	angle %= 360;
	if((angle >= 0) && (angle < 90)) return sinus_LUT[angle];
	if((angle >= 90) && (angle < 180)) return sinus_LUT[180 - angle];
	if((angle >= 180) && (angle < 270)) return -(sinus_LUT[angle - 180]);
	if((angle >= 270) && (angle < 360)) return -(sinus_LUT[180 - (angle - 180)]);
	return 0; // will be never here
}

int GFX_WindowRotate(GFX_td *Image, int inrows, int incol,uint8_t color, int angle)
{

  int c,r;//counters for rows and columns
  uint32_t sizeBuf=Image->WindowHeigh * (Image->WindowWidth/DividerToPixel);

  uint8_t *HelperBuf;
  HelperBuf=(uint8_t*) calloc(sizeBuf,1);

  	  GFX_td *HelperStruct;
  	  HelperStruct  = (GFX_td*) malloc(  sizeof(GFX_td) );

   HelperStruct->OutBuffer=HelperBuf;
   HelperStruct->WindowHeigh=inrows;
   HelperStruct->WindowWidth=incol;



  	  //rotate 90 degree anticlockwise
  if(angle == -90 || angle == 270)//rotating -90 or 270
  { for(r = 0; r < inrows; r++)
        { for(c = 0; c < incol; c++ )
                {
            int color=	GFX_ReadPixel(Image,inrows-r-1, c);
            	GFX_DrawPixel(HelperStruct,c, r, color);
                 }}

  	  Image->WindowWidth=incol;
  	  Image->WindowHeigh=inrows;
  }
if(angle == 90 || angle == -270)//rotating 90 or -270
	{  for(r = 0; r < inrows; r++)
    	{  for(c = 0; c < incol; c++ )
            {
            int color=	GFX_ReadPixel(Image,inrows-r-1, c);
            	GFX_DrawPixel(HelperStruct,incol-c-1, inrows-r, color);
            }
    	}
	  Image->WindowWidth=incol;
	  Image->WindowHeigh=inrows;
	}
if(angle == 180 || angle == -180)//rotating 180 or -180
{    for(r = 0; r < inrows; r++)
    	{ for(c = 0; c < incol; c++ )
            {
            int color=	GFX_ReadPixel(Image,r, c);
            	GFX_DrawPixel(HelperStruct,inrows-r-1, incol-c-2, color);
            }
    }
}
memset(Image->OutBuffer,0x00,sizeBuf);

memcpy(Image->OutBuffer,HelperBuf,sizeBuf);

free(HelperStruct);
free(HelperBuf);

  return 0;
}

int GFX_WindowMirror(GFX_td *Image, int inrows, int incol,uint8_t color, int axis)
{
  int c,r;//counters for rows and columns
  uint32_t sizeBuf=Image->WindowHeigh * (Image->WindowWidth/DividerToPixel);

  uint8_t *HelperBuf;
  HelperBuf=(uint8_t*) calloc(sizeBuf,1);

  	  GFX_td *HelperStruct;
  	  HelperStruct  = (GFX_td*) malloc(  sizeof(GFX_td) );

   HelperStruct->OutBuffer=HelperBuf;
   HelperStruct->WindowHeigh=Image->WindowWidth;
   HelperStruct->WindowWidth=Image->WindowHeigh;

  	  //rotate 90 degree anticlockwise
  if(axis == 1)
  { for(r = 0; r < inrows; r++)
        { for(c = 0; c < incol; c++ )
                {
            int color=	GFX_ReadPixel(Image,r, incol-c);
            	GFX_DrawPixel(HelperStruct,r,c, color);
                 }}
  }
  if(axis == 0)
  { for(r = 0; r < inrows; r++)
        { for(c = 0; c < incol; c++ )
                {
            int color=	GFX_ReadPixel(Image,inrows-r,c);
            	GFX_DrawPixel(HelperStruct,r,c, color);
                 }}
  }

memset(Image->OutBuffer,0x00,sizeBuf);

memcpy(Image->OutBuffer,HelperBuf,sizeBuf);

free(HelperStruct);
free(HelperBuf);

  return 0;
}

int GFX_Copy(GFX_td *outBUF, int inrows, int incol, GFX_td *inBUF)
{

  int c,r;//counters for rows and columns

        for(r = 0; r < inrows; r++)
        {
                for(c = 0; c < incol; c++ )
                {
                int color=	GFX_ReadPixel(inBUF,r, c);
                	GFX_DrawPixel(outBUF,r, c, color);
                }
        }
  return 0;
}

int GFX_Window_Hor_ScrollRight(GFX_td *ImageIn,GFX_td *ImageOut,int inrows, int incol,uint8_t color, int numRowShift,int scrollprocessHor)
{
	  int c,r;//counters for rows and columns
	 int processtate=scrollprocessHor;

  	  ImageOut->WindowHeigh=incol;
  	  ImageOut->WindowWidth=inrows;

	  GFX_ClearBuffer(ImageOut,inrows, incol);

	  for(r = 0; r < inrows; r++)
	        { for(c = 0; c < incol; c++ )
	                {
	            int color=	GFX_ReadPixel(ImageIn,processtate +r,c);
	            	GFX_DrawPixel(ImageOut,r,c, color);
	                 }}
	  for(r = 0; r < processtate; r++)
	        { for(c = 0; c < incol; c++ )
	                {
	        	int color=	GFX_ReadPixel(ImageIn,r,c);
	        	GFX_DrawPixel(ImageOut,numRowShift-processtate+r,c, color);
	                 }}
	  	  //inrows-r-1, incol-c-2

	  processtate++;
  	  if(processtate>=numRowShift)
  	  {
  		  int helper=processtate;
  		processtate=0;
  		return helper;
  	  }
  	  return processtate;
}

int GFX_Window_VerScrollFlow(GFX_td *ImageIn,GFX_td *ImageOut,int inrows, int incol,uint8_t color, int numRowShift,int shiftval, int HALF)
{
	  int c,r;//counters for rows and columns
	  int processtate=shiftval;
	  	  ImageOut->WindowHeigh=incol;
	  	  ImageOut->WindowWidth=inrows;

	  GFX_ClearBuffer(ImageOut,inrows, incol);
	  for(r = 0; r < inrows; r++)
	        { for(c = 0; c < incol; c++ )
	                {
	            int color=	GFX_ReadPixel(ImageIn,r,processtate+c);
	            	GFX_DrawPixel(ImageOut,r,c, color);
	                 }}


		  for(r = 0; r < inrows; r++)
		        { for(c = 0; c < processtate; c++ )
		                {
		        	int color=	GFX_ReadPixel(ImageIn,r,c);
		        	GFX_DrawPixel(ImageOut,r,numRowShift-processtate+c, color);
		                 }}
return 0;
}



void GFX_SetFont(const uint8_t* font_t)
{
	font = font_t;
}

void GFX_SetFontSize(uint8_t size_t)
{
	if(size_t != 0)
		size = size_t;
}

uint8_t GFX_GetFontHeight(void)
{
	return font[0];
}

uint8_t GFX_GetFontWidth(void)
{
	return font[1];
}

uint8_t GFX_GetFontSize(void)
{
	return size;
}
