/* Klient protokołu SNTP wg RFC 4330 */

/* To jest błąd: lwip/netif.h musi włączony przed lwip/dns.h. */
#include <lwip/netif.h>
#include <lwip/dns.h>
#include <lwip/udp.h>
#include <string.h>
#include <sntp_client.h>

#include "lwip.h"
#include "lwip/prot/dhcp.h"
#include "lwip/dhcp.h"


#include "usart.h"
#include <stdio.h>
#include <time.h>
#include "rtc.h"

#define NTP_PORT     123
#define NTP_TO_UNIX  2208988800U

PACK_STRUCT_BEGIN
struct ntp_timestamp {
  PACK_STRUCT_FIELD(uint32_t Seconds);
  PACK_STRUCT_FIELD(uint32_t SecondsFraction);
} PACK_STRUCT_STRUCT;
PACK_STRUCT_END

PACK_STRUCT_BEGIN
struct ntp_msg {
  PACK_STRUCT_FIELD(uint8_t LI_VN_Mode);
  PACK_STRUCT_FIELD(uint8_t Stratum);
  PACK_STRUCT_FIELD(uint8_t Poll);
  PACK_STRUCT_FIELD(int8_t Precision);
  PACK_STRUCT_FIELD(uint32_t RootDelay);
  PACK_STRUCT_FIELD(uint32_t RootDispersion);
  PACK_STRUCT_FIELD(uint8_t ReferenceIdentifier[4]);
  PACK_STRUCT_FIELD(struct ntp_timestamp ReferenceTimestamp);
  PACK_STRUCT_FIELD(struct ntp_timestamp OriginateTimestamp);
  PACK_STRUCT_FIELD(struct ntp_timestamp ReceiveTimestamp);
  PACK_STRUCT_FIELD(struct ntp_timestamp TransmitTimestamp);
} PACK_STRUCT_STRUCT;
PACK_STRUCT_END

#define NTP_LI_NO_WARNING          0
#define NTP_LI_ALARM_CONDITION     3
#define NTP_VERSION                4
#define NTP_MODE_CLIENT            3
#define NTP_MODE_SERVER            4
#define NTP_STRATUM_KISS_OF_DEATH  0
#define NTP_STRATUM_MAX            15

#define ntp_leap_indicator(pdu) ((uint8_t)((pdu)->LI_VN_Mode >> 6))
#define ntp_version(pdu) ((uint8_t)(((pdu)->LI_VN_Mode >> 3) & 0x7))
#define ntp_mode(pdu) ((uint8_t)((pdu)->LI_VN_Mode & 0x7))
#define ntp_compose_li_vn_mode(li, vn, mode)            \
 (((li) << 6) | (((vn) << 3) & 0x38) | ((mode) & 0x7))

struct sntp_client {
  struct sntp_client *next;
  struct udp_pcb     *pcb;
  int                timer;
  int                *status;
};

/* Czekaj na odpowiedź maksymalnie 4 sekundy. */
#define SNTP_TIMEOUT  20

static struct sntp_client *firstClient = NULL;

//void (*dns_found_callback)(const char *name, const ip_addr_t *ipaddr, void *callback_arg);
void SNTPrequest(const char *name, const ip_addr_t *ipaddr, void *status);
void SNTPreply (void *arg, struct udp_pcb *pcb, struct pbuf *p,const ip_addr_t *addr, u16_t port);

void SetAndPrintOnUartRTCFromSecond(uint32_t sec);

extern struct netif gnetif;

void SNTPclientStart(const char *serverName, int *status) {

	  uint32_t Timeoutdhcp=HAL_GetTick();
	  struct dhcp *dhcp;
	  dhcp = netif_dhcp_data(&gnetif);
	  while(dhcp->state != DHCP_STATE_BOUND ) //blokujaco do oporu az sie polaczy... XD
	  {
		  MX_LWIP_Process ();  //wiadomo libka musi dzialac wczesniej se ja zblokowalem
		  	  	  	  	  //dobrze ze sie szybko kapnolem
		  	if( (HAL_GetTick()-Timeoutdhcp) > 10000 )  //timeout 10sek
		  	{
		  		break;
		  	}
	  }			//Ja nie wiem czego ST tego nie umieszcza np w przerwaniu od systicka :/
	  	  	  	  	  	  //no Najlepiej to RTOSik odzielny task i po zawodach :) i tak w RTOSie moze to robia nwm nie sprawdzalem



	ip_addr_t serverIP;
    err_t err;



  err = dns_gethostbyname(serverName, &serverIP, SNTPrequest, (void*)status);

  if (err == ERR_OK) {
    /* Odpowiedź odczytana z pamięci podręcznej. */

    SNTPrequest(serverName, &serverIP, (void*)status);

    *status = SNTP_IN_PROGRESS;
  }
  else if (err == ERR_INPROGRESS)
    *status = SNTP_IN_PROGRESS;
  else
    *status = SNTP_DNS_ERROR;
}

/* Usuń klientów, którzy czekają zbyt długo. */
void SNTPtimer() {
  struct sntp_client *curr, *prev, *next;

  curr = firstClient;
  prev = NULL;
  while (curr) {
    if (--(curr->timer) < 0) {
      if (curr->pcb)
        udp_remove(curr->pcb);
      if (prev)
        next = prev->next = curr->next;
      else
        next = firstClient = curr->next;
      if (*(curr->status) == SNTP_IN_PROGRESS)
        *(curr->status) = SNTP_NO_RESPONSE;
      mem_free(curr);
      curr = next;
    }
    else {
      prev = curr;
      curr = curr->next;

    }
  }
}

void SNTPrequest(const char *name, const ip_addr_t *ipaddr, void *status)
{
  struct sntp_client *client;

  if (NULL == ipaddr) {
    *(int*)status = SNTP_DNS_ERROR;
    return;
  }
  client = mem_malloc(sizeof(struct sntp_client));
  if (client == NULL) {
    *(int*)status = SNTP_MEMORY_ERROR;
    return;
  }

  client->pcb = NULL;
  client->timer = SNTP_TIMEOUT;
  client->status = (int*)status;
  client->next = firstClient;
  firstClient = client;

  if (NULL != (client->pcb = udp_new()) &&
      ERR_OK == udp_bind(client->pcb, IP_ADDR_ANY, 0) &&
      /* Odbieraj tylko odpowiedzi od serwera, do którego
         wysłałeś żądanie. */
      ERR_OK == udp_connect(client->pcb, ipaddr, NTP_PORT)) {
    struct pbuf *p;

    p = pbuf_alloc(PBUF_TRANSPORT, sizeof(struct ntp_msg), PBUF_RAM);
    if (p) {
      struct ntp_msg *msg = p->payload;

      memset(msg, 0, sizeof(struct ntp_msg));
      msg->LI_VN_Mode = ntp_compose_li_vn_mode(NTP_LI_NO_WARNING,
                                              NTP_VERSION,
                                              NTP_MODE_CLIENT);
      udp_recv(client->pcb, SNTPreply, client);
      if (ERR_OK != udp_send(client->pcb, p)) {
        *(int*)status = SNTP_UDP_ERROR;
        /* Wystąpił błąd, można przyspieszyć usunięcie klienta. */
        client->timer = 0;
      }
      pbuf_free(p);
    }
    else {
      *(int*)status = SNTP_MEMORY_ERROR;
      /* Wystąpił błąd, można przyspieszyć usunięcie klienta. */
      client->timer = 0;
    }
  }
  else {
    *(int*)status = SNTP_UDP_ERROR;
    /* Wystąpił błąd, można przyspieszyć usunięcie klienta. */
    client->timer = 0;
  }
}

void SNTPreply(void *arg, struct udp_pcb *pcb, struct pbuf *p,const ip_addr_t *addr, u16_t port)
{
  struct sntp_client *client = arg;
  struct ntp_msg *msg;

  /* Jeśli otrzymano poprawną odpowiedź, uaktualnij czas.
     TODO: Uwzględnić: opóźnienie transmisji pakietów, strefy
     czasowe, zmiany na czas letni i zimowy, sekundy przestępne. */
  msg = p->payload;
  if (p->len >= sizeof(struct ntp_msg) &&
      ntp_leap_indicator(msg) != NTP_LI_ALARM_CONDITION &&
      ntp_version(msg) == NTP_VERSION &&
      ntp_mode(msg) == NTP_MODE_SERVER &&
      msg->Stratum != NTP_STRATUM_KISS_OF_DEATH &&
      msg->Stratum <= NTP_STRATUM_MAX &&
      (msg->TransmitTimestamp.Seconds != 0 ||
       msg->TransmitTimestamp.SecondsFraction != 0)) {

	  SetAndPrintOnUartRTCFromSecond(msg->TransmitTimestamp.Seconds);
  }
  else
    *(client->status) = SNTP_RESPONSE_ERROR;
  pbuf_free(p);

  /* Klient już nie jest potrzebny. */
  client->timer = 0;
}


void SetAndPrintOnUartRTCFromSecond(uint32_t sec)
{
	//2hours to find problem.  3min solution...  bc little/big endian conversion
	//from big endian to little endian? or vice versa dk :D ,diagnose in debugger so don't checking it
	uint32_t num = sec;
	uint32_t b0,b1,b2,b3;
	uint32_t res;
	b0 = (num & 0x000000ff) << 24u;
	b1 = (num & 0x0000ff00) << 8u;
	b2 = (num & 0x00ff0000) >> 8u;
	b3 = (num & 0xff000000) >> 24u;

	res = b0 | b1 | b2 | b3;
	sec=res;

	#define NTP_TOJJNIX 2208988800;
    time_t rawtime = sec+7200-NTP_TOJJNIX;
    struct tm  ts;
    char       buf[80]={0};

    // Format time, "ddd yyyy-mm-dd hh:mm:ss zzz"
    ts = *localtime(&rawtime);
    strftime(buf, sizeof(buf), "%a %Y-%m-%d %H:%M:%S GMT+2 \n\r", &ts);
    char       hbuf[80]={0};
    int s_size=sprintf(hbuf,buf);
    HAL_UART_Transmit(&huart3,(uint8_t*) hbuf, s_size, 100);

	RTC_TimeTypeDef sTime;
	sTime.Seconds=ts.tm_sec;
	sTime.Hours=ts.tm_hour;
	sTime.Minutes=ts.tm_min;

	RTC_DateTypeDef sData;
	sData.Year=ts.tm_year-100;
	sData.Month=ts.tm_mon+1;
	sData.Date=ts.tm_mday;
	sData.WeekDay=ts.tm_wday;
	SetRTC(&sTime,&sData);


	uint16_t Milliseconds;
	uint8_t Message[64];
	uint8_t MessageLen;

	 HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
		  Milliseconds = ((sTime.SecondFraction-sTime.SubSeconds)/((float)sTime.SecondFraction+1) * 100);
		  HAL_RTC_GetDate(&hrtc, &sData, RTC_FORMAT_BIN);

			  MessageLen = sprintf((char*)Message, "Date: %02d.%02d.20%02d Time: %02d:%02d:%02d:%02d\n\r", sData.Date, sData.Month,
					  sData.Year, sTime.Hours,sTime.Minutes, sTime.Seconds, Milliseconds);
			  HAL_UART_Transmit(&huart3, Message, MessageLen, 100);
	HAL_GPIO_TogglePin(LED_GREEN_GPIO_Port, LED_GREEN_Pin);


}




