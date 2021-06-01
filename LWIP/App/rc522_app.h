/*
 * rc_522_app.h
 *
 *  Created on: May 30, 2021
 *      Author: tktof
 */

#ifndef APP_RC522_APP_H_
#define APP_RC522_APP_H_

typedef enum
{
	TagNew,
	TagExisist,
	TagOK,
	TagErr,
}TagStatus;
typedef enum
{
	LoggedOut=0,
	LoggedIn=1,
}PersonStatus;
typedef struct
{
	RTC_TimeTypeDef SavedTime1;
	RTC_DateTypeDef SavedData1;
	RTC_TimeTypeDef SavedTime2;
	RTC_DateTypeDef SavedData2;
}DoubleTimeAndDate;
typedef struct
{
		 char *UID;
		 char *ReadName;
		 char *ReadFamily;
}Tag;
typedef struct PersonInformation
{
	  struct PersonInformation		*next;  //for Person Chain! :)
	  Tag					*SavedTag;
	  DoubleTimeAndDate 	LoginTimes;
	  DoubleTimeAndDate 	LogoutTimes;
	  PersonStatus			Status;
}PersonInformation;

extern void CreateStringLogg(char *PrepareString);
extern void RC522_appTask();
extern void RC522_appSetup();

#endif /* APP_RC522_APP_H_ */
