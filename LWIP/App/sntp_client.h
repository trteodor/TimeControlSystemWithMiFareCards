#ifndef _SNTP_CLIENT_H
#define _SNTP_CLIENT_H 1

#define SNTP_TIMER_MS 200

#define SNTP_NOT_RUNNING     2
#define SNTP_SYNCHRONIZED    1
#define SNTP_IN_PROGRESS     0
#define SNTP_DNS_ERROR      -1
#define SNTP_MEMORY_ERROR   -2
#define SNTP_UDP_ERROR      -3
#define SNTP_NO_RESPONSE    -4
#define SNTP_RESPONSE_ERROR -5

void SNTPclientStart(const char *, int *);
void SNTPtimer(void);

#endif
