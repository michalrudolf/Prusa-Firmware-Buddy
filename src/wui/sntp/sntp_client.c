#include <time.h>
#include <stdlib.h>
#include "lwip.h"
#include "sntp.h"
#include "sntp_client.h"

//#define _gettimeofday(x, y) get_time_of_day(x, y)
//int gettimeofday( struct timeval *tv, void *tzvp )
//{
//    uint64_t t = xTaskGetTickCount();  // get uptime in miliseconds
//    tv->tv_sec = (time_t)(t / 1000);  // convert to seconds
//    tv->tv_usec = (suseconds_t)( t % 1000 ) * 1000;  // get remaining
//    return 0;
//}


static ip_addr_t ntp_server;
void sntp_set_system_time(uint32_t sec)
{
  //char buf[32];
  //struct tm current_time_val;
  //time_t current_time = (time_t)sec;

  //localtime_s(&current_time_val, &current_time);

  //strftime(buf, sizeof(buf), "%d.%m.%Y %H:%M:%S", &current_time_val);
  
  uint32_t real_time = sec;
  real_time++;

}

void sntp_client_init(void)
{
  sntp_setoperatingmode(SNTP_OPMODE_POLL);

  //uint8_t netif_type = eeprom_get_var(EEVAR_LAN_FLAG).ui8;
  // netif is set up with dhcp
  //if ((netif_type & LAN_EEFLG_TYPE) == 0){
  //  sntp_servermode_dhcp(1); /* get SNTP server via DHCP */
  //} else {
    // in LwIP init netif_default is set to eth0
    if(ipaddr_aton("195.113.144.201", &ntp_server)){
      sntp_setserver(0, &ntp_server);
    }
  //}
  sntp_init();
}