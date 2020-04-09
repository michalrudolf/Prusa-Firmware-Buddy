#include <time.h>
#include <stdlib.h>
#include <stdbool.h>
#include "lwip.h"
#include "sntp.h"
#include "sntp_client.h"
#include "stm32f4xx_hal.h"
#include "main.h" // for hrtc -> pointer to RTC storage structure

//#define _gettimeofday(x, y) get_time_of_day(x, y)
//int gettimeofday( struct timeval *tv, void *tzvp )
//{
//    uint64_t t = xTaskGetTickCount();  // get uptime in miliseconds
//    tv->tv_sec = (time_t)(t / 1000);  // convert to seconds
//    tv->tv_usec = (suseconds_t)( t % 1000 ) * 1000;  // get remaining
//    return 0;
//}
static char sys_time[10];
static char sys_date[10];
static bool sntp_time_init;
static ip_addr_t ntp_server;  // testing ntp server located in Prague

void sntp_set_system_time(uint32_t sec)
{ 
  RTC_TimeTypeDef currTime;
  RTC_DateTypeDef currDate;

  struct tm current_time_val;
  time_t current_time = (time_t)sec;

  localtime_r(&current_time, &current_time_val);

  currTime.Seconds = current_time_val.tm_sec;
  currTime.Minutes = current_time_val.tm_min;
  currTime.Hours = current_time_val.tm_hour;
  currDate.Date = current_time_val.tm_mday;
  currDate.Month = current_time_val.tm_mon;
  currDate.Year = current_time_val.tm_year;
  currDate.WeekDay = current_time_val.tm_wday;

  HAL_RTC_SetTime(&hrtc, &currTime, RTC_FORMAT_BIN);
  HAL_RTC_SetDate(&hrtc, &currDate, RTC_FORMAT_BIN);

  sntp_time_init = true;
}

const char * sntp_get_system_time(void){

  if(sntp_time_init){
    RTC_TimeTypeDef currTime;

    HAL_RTC_GetTime(&hrtc, &currTime, RTC_FORMAT_BIN);

    snprintf(sys_time, 10, "%02d:%02d:%02d", currTime.Hours, currTime.Minutes, currTime.Seconds);
  } else {
    strcpy(sys_time, "N/A");
  }
  return sys_time;
}

const char * sntp_get_system_date(void){

  if(sntp_time_init){
    RTC_DateTypeDef currDate;

    HAL_RTC_GetDate(&hrtc, &currDate, RTC_FORMAT_BIN);

    snprintf(sys_date, 10, "%02d:%02d:%02d", currDate.Date, currDate.Month, currDate.Year);
  } else {
    strcpy(sys_date, "N/A");
  }
  return sys_date;
}

void sntp_client_init(void)
{
  sntp_time_init = false;
  sntp_setoperatingmode(SNTP_OPMODE_POLL);

  //uint8_t netif_type = eeprom_get_var(EEVAR_LAN_FLAG).ui8;
  // netif is set up with dhcp
  //if ((netif_type & LAN_EEFLG_TYPE) == 0){
  //  sntp_servermode_dhcp(1); /* get SNTP server via DHCP */
  //} else {

    // In LwIP init netif_default is set to eth0, BUT \
    // When DHCP is enabled, it supplies addrs after this is called -> set_server 0.0.0.0
    if(ipaddr_aton("195.113.144.201", &ntp_server)){
      sntp_setserver(0, &ntp_server);
    }
  //}
  sntp_init();
}