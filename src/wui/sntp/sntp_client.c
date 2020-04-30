#include "sntp.h"
#include "sntp_client.h"
#include "wui_api.h"

//#define _gettimeofday(x, y) get_time_of_day(x, y)
//int gettimeofday( struct timeval *tv, void *tzvp )
//{
//    uint64_t t = xTaskGetTickCount();  // get uptime in miliseconds
//    tv->tv_sec = (time_t)(t / 1000);  // convert to seconds
//    tv->tv_usec = (suseconds_t)( t % 1000 ) * 1000;  // get remaining
//    return 0;
//}

static ip_addr_t ntp_server;  // testing ntp server located in Prague

void sntp_client_init(void)
{
  sntp_setoperatingmode(SNTP_OPMODE_POLL);

  //  sntp_servermode_dhcp(1); /* get SNTP server via DHCP */

    // In LwIP init netif_default is set to eth0, BUT
    // When DHCP is enabled, it supplies addrs after this is called -> send NTP request from 0.0.0.0
    if(ipaddr_aton("195.113.144.238", &ntp_server)){
      sntp_setserver(0, &ntp_server);
    }
  sntp_init();
}