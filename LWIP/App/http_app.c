/*
 * http_app.c
 *
 *  Created on: May 29, 2021
 *      Author: tktof
 */
#include <stdio.h>
#include <string.h>
// including httpd.h [- HTTPd #1 -]
#include "lwip/apps/httpd.h"

// we include this library to be able to use boolean variables for SSI
#include <Stdbool.h>
#include "rtc.h"

bool LD1ON = false; // this variable will indicate if the LD3 LED on the board is ON or not
bool LD2ON = false; // this variable will indicate if our LD2 LED on the board is ON or not
// just declaring the function for the compiler [= CGI #2 =]
const char* LedCGIhandler(int iIndex, int iNumParams, char *pcParam[],char *pcValue[]);
// in our SHTML file <form method="get" action="/leds.cgi"> [= CGI #3 =]
const tCGI LedCGI = { "/leds.cgi", LedCGIhandler };
// [= CGI #4 =]
tCGI theCGItable[1];
// just declaring SSI handler function [* SSI #1 *]
u16_t mySSIHandler(int iIndex, char *pcInsert, int iInsertLen);
// [* SSI #2 *]
#define numSSItags 3
// [* SSI #3 *]
char const *theSSItags[numSSItags] = { "tag1", "tag2" , "tag3"};
// the actual function for handling CGI [= CGI #5 =]
const char* LedCGIhandler(int iIndex, int iNumParams, char *pcParam[],char *pcValue[])
{
              uint32_t i = 0;
                       if (iIndex == 0) {
                           //turning the LED lights off
                           HAL_GPIO_WritePin(LED_GREEN_GPIO_Port, LED_GREEN_Pin, GPIO_PIN_RESET);
                           // we put this variable to false to indicate that the LD2 LED on the board is not ON
                           LD2ON = false;
                           HAL_GPIO_WritePin(LED_RED_GPIO_Port, LED_RED_Pin, GPIO_PIN_RESET);
                           // we put this variable to false to indicate that the LD* LED on the board is not ON
                           LD1ON = false;
                       }
                       for (i = 0; i < iNumParams; i++) {
                           if (strcmp(pcParam[i], "led") == 0)
                           {
                               if (strcmp(pcValue[i], "1") == 0) {
                            	   HAL_GPIO_WritePin(LED_GREEN_GPIO_Port, LED_GREEN_Pin, GPIO_PIN_SET);
                                   // LD3 LED (red) on the board is ON!
                                   LD1ON = true;
                               }
                               else if (strcmp(pcValue[i], "2") == 0) {
                            	   HAL_GPIO_WritePin(LED_RED_GPIO_Port, LED_RED_Pin, GPIO_PIN_SET);
                                   // LD2 LED (blue) on the board is ON!
                                   LD2ON = true;
                               }
                           }

                       }
                       // the extension .shtml for SSI to work
                       return "/index.shtml";
} // END [= CGI #5 =]

 // function to initialize CGI [= CGI #6 =]
void myCGIinit(void)
{
		//add LED control CGI to the table
		theCGItable[0] = LedCGI;
		//give the table to the HTTP server
		http_set_cgi_handlers(theCGItable, 1);
} // END [= CGI #6 =]

// the actual function for SSI [* SSI #4 *]
u16_t mySSIHandler(int iIndex, char *pcInsert, int iInsertLen)
{
if (iIndex == 0) {
       if (LD1ON == false) {
         char myStr1[] = "<input value=\"1\" name=\"led\" type=\"checkbox\">";
          strcpy(pcInsert, myStr1);
             return strlen(myStr1);
          }
        else if (LD1ON == true) {
   // since the LD3 red LED on the board is ON we make its checkbox checked!
    char myStr1[] =
        "<input value=\"1\" name=\"led\" type=\"checkbox\" checked>";
            strcpy(pcInsert, myStr1);
            return strlen(myStr1);
           }
     }
else if (iIndex == 1)
       {
        if (LD2ON == false) {
         char myStr2[] = "<input value=\"2\" name=\"led\" type=\"checkbox\">";
            strcpy(pcInsert, myStr2);
              return strlen(myStr2);
        }
         else if (LD2ON == true) {
           // since the LD2 blue LED on the board is ON we make its checkbox checked!
       char myStr2[] =
          "<input value=\"2\" name=\"led\" type=\"checkbox\" checked>";
           strcpy(pcInsert, myStr2);
          return strlen(myStr2);
       }
   }
else if(iIndex ==2)
{
	  	  HAL_RTC_GetTime(&hrtc, &RtcTime, RTC_FORMAT_BIN);
	 	  Milliseconds = ((RtcTime.SecondFraction-RtcTime.SubSeconds)/((float)RtcTime.SecondFraction+1) * 100);
	 	  HAL_RTC_GetDate(&hrtc, &RtcDate, RTC_FORMAT_BIN);

	 	 sprintf((char*)pcInsert, "Czas na RTC w Mikrokontrolerze z SNTP gum.gov.pl Date: %02d.%02d.20%02d Time: %02d:%02d:%02d:%02d\n\r",
					  RtcDate.Date, RtcDate.Month, RtcDate.Year, RtcTime.Hours, RtcTime.Minutes, RtcTime.Seconds, Milliseconds);
	          return strlen(pcInsert);
}
 return 0;
}
// function to initialize SSI [* SSI #5 *]
void mySSIinit(void) {
  http_set_ssi_handler(mySSIHandler, (char const**) theSSItags,numSSItags);
}





