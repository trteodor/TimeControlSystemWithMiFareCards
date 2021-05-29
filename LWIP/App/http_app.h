/*
 * http_app.h
 *
 *  Created on: May 29, 2021
 *      Author: tktof
 */

#ifndef APP_HTTP_APP_H_
#define APP_HTTP_APP_H_
extern void myCGIinit();
extern void   mySSIinit();
extern u16_t mySSIHandler(int iIndex, char *pcInsert, int iInsertLen);

#endif /* APP_HTTP_APP_H_ */
