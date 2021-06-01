#include "stm32h7xx_hal.h"
#include "rc522.h"
#include "rc522_app.h"

#include <stdio.h>
#include <string.h>
#include "usart.h"
#include "sntp_client.h"

#include "rtc.h"
#include <stdio.h>
#include <stdlib.h>
#include "OLED_SSD1306_Task.h"

RTC_DateTypeDef ZerosDate={0,0,0,0};
RTC_TimeTypeDef ZerosTime={0,0,0,0,0,0,0,0};


//Tag firstTag;
static PersonInformation *firstPersonInfo = NULL;  //it will be allocate on head by malloc


void RC522_appSetup()
{
	MFRC522_Init();
}

TagStatus SearchTag(Tag *TagToCheck,PersonInformation **FoundPerson)
{
//TODO:	mozna dodac jakis timeout co by sie mi nie zblokowalo calkiem,
	//TODO ale to watchdoga pasuje wsadzic ewentualnie
	PersonInformation *curr;
	curr=firstPersonInfo;
	while(curr!=NULL)
	{
		if(
					*TagToCheck->UID  ==*curr->SavedTag->UID&&
					*TagToCheck->UID+1==*curr->SavedTag->UID+1&&
					*TagToCheck->UID+2==*curr->SavedTag->UID+2&&
					*TagToCheck->UID+3==*curr->SavedTag->UID+3&&
					*TagToCheck->UID+4==*curr->SavedTag->UID+4
			)
		{

			*FoundPerson=curr;
			return  TagExisist;
		}
			curr=curr->next;
	}
	return TagErr;
}
TagStatus AddNewPerson(Tag *TagToSave,PersonInformation **Person)
{

		PersonInformation *NewPerson;
		NewPerson=firstPersonInfo;
		HAL_NVIC_DisableIRQ(TIM2_IRQn); //TIM2 interrupt is wake up timer for LWIP lib
		//__disable_irq();   //malloc is used by LWIP --enter critical section its rly important, without it I had erros
		if((firstPersonInfo=malloc(sizeof(PersonInformation)))==NULL)
		{
			return TagErr;
		}

		Tag *NewTag=malloc(sizeof(Tag));
			if(NewTag==NULL)
			{
			return TagErr;
			}

		NewTag->UID=malloc(UID_SIZE);  // TODO here also i should check errors...
		NewTag->ReadName=malloc(BLOCK_MAX_LEN);
		NewTag->ReadFamily=malloc(BLOCK_MAX_LEN);
		//__enable_irq(); 		//--leave critical section
		HAL_NVIC_EnableIRQ(TIM2_IRQn);

		memcpy(NewTag->UID,TagToSave->UID,5);
		strcpy(NewTag->ReadName,TagToSave->ReadName);
		strcpy(NewTag->ReadFamily,TagToSave->ReadFamily);
		firstPersonInfo->SavedTag=NewTag;


		HAL_RTC_GetTime(&hrtc,&firstPersonInfo->LoginTimes.SavedTime1, RTC_FORMAT_BIN);
		HAL_RTC_GetDate(&hrtc,&firstPersonInfo->LoginTimes.SavedData1, RTC_FORMAT_BIN);
		firstPersonInfo->LoginTimes.SavedTime2=firstPersonInfo->LoginTimes.SavedTime1;
		firstPersonInfo->LoginTimes.SavedData2=firstPersonInfo->LoginTimes.SavedData1;
		firstPersonInfo->LogoutTimes.SavedTime1=ZerosTime;
		firstPersonInfo->LogoutTimes.SavedData1=ZerosDate;
		firstPersonInfo->LogoutTimes.SavedTime2=ZerosTime;
		firstPersonInfo->LogoutTimes.SavedData2=ZerosDate;

		firstPersonInfo->Status=LoggedIn;
		firstPersonInfo->next=NewPerson;
		*Person=firstPersonInfo;
		return TagOK;
}

TagStatus ActualizatePerson(PersonInformation *Person)
{
	if(Person->Status==LoggedOut)
	{
		Person->Status=LoggedIn;
		Person->LoginTimes.SavedTime2=Person->LoginTimes.SavedTime1;
		Person->LoginTimes.SavedData2=Person->LoginTimes.SavedData1;
		HAL_RTC_GetTime(&hrtc,&Person->LoginTimes.SavedTime1, RTC_FORMAT_BIN);
		HAL_RTC_GetDate(&hrtc,&Person->LoginTimes.SavedData1, RTC_FORMAT_BIN);
		return TagOK;
	}
	if(Person->Status==LoggedIn)
	{
		Person->Status=LoggedOut;
		Person->LogoutTimes.SavedTime2=Person->LogoutTimes.SavedTime1;
		Person->LogoutTimes.SavedData2=Person->LogoutTimes.SavedData1;
		HAL_RTC_GetTime(&hrtc,&Person->LogoutTimes.SavedTime1, RTC_FORMAT_BIN);
		HAL_RTC_GetDate(&hrtc,&Person->LogoutTimes.SavedData1, RTC_FORMAT_BIN);
		return TagOK;
	}
return TagOK;
}
void CreateStringLogg(char *PrepareString)
{
	PersonInformation *curr;
	uint32_t pointeroffset=0;
	uint32_t str_size=0;
	UNUSED(str_size);
	uint32_t LoggedInStat=0;
	uint32_t LoggedOutStat=0;

		curr=firstPersonInfo;
		while(curr!=NULL)
			{
			//TODO zrobic funkcje zeby mozna bylo zapisywac kilka logowan a nie taki cyrk...
			if(curr->Status==LoggedIn)
				{
					str_size=sprintf(PrepareString+pointeroffset,"<br><strong>Zalogowany: %s  %s</strong> , GodzLog: %02d:%02d:%02d  Data: Y:%02dM:%02dD:%02d  "
							" PopGodzLog: %02d:%02d:%02d PopData: Y%02d:M%02d:D%02d "

							" <br> GodzWylogowania: %02d:%02d:%02d DataWylogowania: Y:%02dM:%02dD:%02d "
							" GodzPopWylogowania: %02d:%02d:%02d DataPopWylogowania: Y:%02dM:%02dD:%02d "

								,curr->SavedTag->ReadName,curr->SavedTag->ReadFamily,
								curr->LoginTimes.SavedTime1.Hours,curr->LoginTimes.SavedTime1.Minutes,curr->LogoutTimes.SavedTime1.Seconds,
								curr->LoginTimes.SavedData1.Year,curr->LoginTimes.SavedData1.Month,curr->LoginTimes.SavedData1.Date,
								curr->LoginTimes.SavedTime2.Hours,curr->LoginTimes.SavedTime2.Minutes,curr->LogoutTimes.SavedTime2.Seconds,
								curr->LoginTimes.SavedData2.Year,curr->LoginTimes.SavedData2.Month,curr->LoginTimes.SavedData2.Date,

								curr->LogoutTimes.SavedTime1.Hours,curr->LogoutTimes.SavedTime1.Minutes,curr->LogoutTimes.SavedTime1.Seconds,
								curr->LogoutTimes.SavedData1.Year,curr->LogoutTimes.SavedData1.Month,curr->LogoutTimes.SavedData1.Date,
								curr->LogoutTimes.SavedTime2.Hours,curr->LogoutTimes.SavedTime2.Minutes,curr->LogoutTimes.SavedTime2.Seconds,
								curr->LogoutTimes.SavedData2.Year,curr->LogoutTimes.SavedData2.Month,curr->LogoutTimes.SavedData2.Date
					);
														pointeroffset=pointeroffset+str_size;
														LoggedInStat++;
				}
				else
				{
					str_size=sprintf(PrepareString+pointeroffset,"<br><strong>Wylogowany: %s  %s</strong> , GodzLog: %02d:%02d:%02d  Data: Y:%02dM:%02dD:%02d  "
							" PopGodzLog: %02d:%02d:%02d PopData: Y%02d:M%02d:D%02d "

							" <br> GodzWylogowania: %02d:%02d:%02d DataWylogowania: Y:%02dM:%02dD:%02d "
							" GodzPopWylogowania: %02d:%02d:%02d DataPopWylogowania: Y:%02dM:%02dD:%02d "

								,curr->SavedTag->ReadName,curr->SavedTag->ReadFamily,
								curr->LoginTimes.SavedTime1.Hours,curr->LoginTimes.SavedTime1.Minutes,curr->LogoutTimes.SavedTime1.Seconds,
								curr->LoginTimes.SavedData1.Year,curr->LoginTimes.SavedData1.Month,curr->LoginTimes.SavedData1.Date,
								curr->LoginTimes.SavedTime2.Hours,curr->LoginTimes.SavedTime2.Minutes,curr->LogoutTimes.SavedTime2.Seconds,
								curr->LoginTimes.SavedData2.Year,curr->LoginTimes.SavedData2.Month,curr->LoginTimes.SavedData2.Date,

								curr->LogoutTimes.SavedTime1.Hours,curr->LogoutTimes.SavedTime1.Minutes,curr->LogoutTimes.SavedTime1.Seconds,
								curr->LogoutTimes.SavedData1.Year,curr->LogoutTimes.SavedData1.Month,curr->LogoutTimes.SavedData1.Date,
								curr->LogoutTimes.SavedTime2.Hours,curr->LogoutTimes.SavedTime2.Minutes,curr->LogoutTimes.SavedTime2.Seconds,
								curr->LogoutTimes.SavedData2.Year,curr->LogoutTimes.SavedData2.Month,curr->LogoutTimes.SavedData2.Date
					);
											pointeroffset=pointeroffset+str_size;
											LoggedOutStat++;
				}

				if(curr->next!= NULL)
					{
					curr=curr->next;
					}
				else
				{
					break;
				}
			}
		sprintf(PrepareString+pointeroffset,"<br>ZalogowaniLacznie:%lu<br>WylogowaniLacznie%lu<br>",LoggedInStat,LoggedOutStat);
}



void RC522_appTask()
{
	static uint32_t LastCardTimeFound=0;
	static uint32_t CardTime=0;
	static uint8_t  KEY[] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
	static char text[120];
	static int RCsize;

		 	 	  if( (CardTime+100< HAL_GetTick()) && (HAL_GetTick()-LastCardTimeFound > 3000) )
		 	 	  {
		 	 		uint8_t str[BLOCK_MAX_LEN]={0};
		 	 		CardTime=HAL_GetTick();
		 	 			if((MFRC522_Request(PICC_REQIDL, str)==MFRC522_OK))  //Detect Tag //In pooling mode this app work now..
		 	 			{
				 	 		uint8_t UID[5]={0};
				 	 		uint8_t ReadName[BLOCK_MAX_LEN]={0};
				 	 		uint8_t ReadFamily[BLOCK_MAX_LEN]={0};

		 	 				LastCardTimeFound=HAL_GetTick();



		 	 				if (MFRC522_Anticoll(str)==MFRC522_OK)
		 	 				{
		 	 					memcpy(UID, str, 5);
		 	 					RCsize = sprintf(text,"UID:%02X:%02X:%02X:%02X:%02X\n\r",UID[0],UID[1],UID[2],UID[3],UID[4]);
		 	 					HAL_UART_Transmit(&huart3,(uint8_t*) text, RCsize, 100);
		 	 					MFRC522_SelectTag((uint8_t*)UID);
		 	 					MFRC522_Auth(PICC_AUTHENT1A,2,KEY,(uint8_t*)UID);
		 	 					//TODO If I want write something on RFID Tag
		 	 					//RCsize = sprintf(text,"Write Block 2 on Card:%s\n\r",(char*)W);
		 	 					//HAL_UART_Transmit(&huart3, (uint8_t*)text, RCsize, 100);
		 	 					//MFRC522_WriteBlock((uint8_t)1 ,(uint8_t*)  "Monika");
		 	 					//MFRC522_WriteBlock((uint8_t)2 , (uint8_t*) "Kowalska");

		 	 					MFRC522_ReadBlock( 1, (uint8_t*)ReadName);
		 	 					MFRC522_ReadBlock( 2, (uint8_t*)ReadFamily);
		 	 					PersonInformation *Personn;
		 	 					Tag ReceivedTag;
		 	 					ReceivedTag.UID=(char*)&UID;
		 	 					ReceivedTag.ReadFamily=(char*)&ReadFamily;
		 	 					ReceivedTag.ReadName=(char*)&ReadName;

		 	 					if(TagExisist == SearchTag(&ReceivedTag,&Personn))
		 	 						{
		 	 							ActualizatePerson(Personn);
		 	 						}
		 	 						else
		 	 						{
		 	 							AddNewPerson(&ReceivedTag,&Personn);
			 	 						HAL_UART_Transmit(&huart3,(uint8_t*)"NewPersonAdded\n\r",sizeof("NewPersonAdded\n\r"), 100);
		 	 						}
		 	 					//TODO it requires change -- the machine state is in OLED_SSD1306.c file
		 	 					if(Personn->Status == LoggedIn)
		 	 					{
		 	 						GFX_ClearBuffer(MainWindow,LCDWIDTH, LCDHEIGHT);
		 	 						GFX_SetFont(font_8x5);
		 	 						GFX_SetFontSize(1);
		 	 						SSD1306_I2cInit(&hi2c1);
		 	 						char Napis[500];
		 	 						 GFX_SetFontSize(2);
		 	 						sprintf(Napis,"Zalogowano:");
		 	 						GFX_DrawString(MainWindow, 0, 0,Napis, WHITE, BLACK);
		 	 						sprintf(Napis,"%s",Personn->SavedTag->ReadName);
		 	 						GFX_DrawString(MainWindow, 0, 20,Napis, WHITE, BLACK);
		 	 						GFX_SetFontSize(1);
		 	 						sprintf(Napis,"%s",Personn->SavedTag->ReadFamily);
		 	 						GFX_DrawString(MainWindow, 0, 40,Napis, WHITE, BLACK);
		 	 						sprintf((char*)Napis, "O Godz: %02d:%02d:%02d", Personn->LogoutTimes.SavedTime1.Hours
		 	 										,Personn->LogoutTimes.SavedTime1.Minutes,Personn->LogoutTimes.SavedTime1.Seconds);
		 	 						GFX_DrawString(MainWindow, 0, 50,Napis, WHITE, BLACK);
		 	 						M_State=ViewingPerson;
		 	 						SSD1306_Display(MainWindow);

		 	 						RCsize = sprintf(text,"Zalogowano: \n\r %s \n\r %s \n\r o Godz:%02d:%02d:%02d\n\r",
		 	 								Personn->SavedTag->ReadFamily,ReceivedTag.ReadName,Personn->LogoutTimes.SavedTime1.Hours
											,Personn->LogoutTimes.SavedTime1.Minutes,Personn->LogoutTimes.SavedTime1.Seconds);
		 	 				 	 	HAL_UART_Transmit(&huart3, (uint8_t*)text,RCsize, 100);
		 	 					}
		 	 					else
		 	 					{
		 	 						GFX_ClearBuffer(MainWindow,LCDWIDTH, LCDHEIGHT);
		 	 						GFX_SetFont(font_8x5);
		 	 						GFX_SetFontSize(1);
		 	 						SSD1306_I2cInit(&hi2c1);
		 	 						char Napis[500];
		 	 						 GFX_SetFontSize(2);
		 	 						sprintf(Napis,"Wylogowano:");
		 	 						GFX_DrawString(MainWindow, 0, 0,Napis, WHITE, BLACK);
		 	 						sprintf(Napis,"%s",Personn->SavedTag->ReadName);
		 	 						GFX_DrawString(MainWindow, 0, 20,Napis, WHITE, BLACK);
		 	 						GFX_SetFontSize(1);
		 	 						sprintf(Napis,"%s",Personn->SavedTag->ReadFamily);
		 	 						GFX_DrawString(MainWindow, 0, 40,Napis, WHITE, BLACK);
		 	 						sprintf((char*)Napis, "O Godz: %02d:%02d:%02d", Personn->LogoutTimes.SavedTime1.Hours
		 	 								 	 										,Personn->LogoutTimes.SavedTime1.Minutes,Personn->LogoutTimes.SavedTime1.Seconds);
		 	 						GFX_DrawString(MainWindow, 0, 50,Napis, WHITE, BLACK);
		 	 						M_State=ViewingPerson;
		 	 						SSD1306_Display(MainWindow);

		 	 						RCsize = sprintf(text,"Wylogowano: \n\r %s \n\r %s \n\r o Godz:%02d:%02d:%02d\n\r",
		 	 								Personn->SavedTag->ReadFamily,ReceivedTag.ReadName,Personn->LogoutTimes.SavedTime1.Hours
											,Personn->LogoutTimes.SavedTime1.Minutes,Personn->LogoutTimes.SavedTime1.Seconds);
		 	 				 	 	HAL_UART_Transmit(&huart3, (uint8_t*)text,RCsize, 100);
		 	 					}

		 	 					SSD1306_I2cInit(&hi2c1);
		 	 					MFRC522_DeAuth();

		 	 					}
		 	 				}
		 	 			}
}
