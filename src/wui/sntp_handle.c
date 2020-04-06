#include <time.h>
#include "lwip/apps/sntp.h"
#include "lwip.h"
#include "sntp_handle.h"

void
sntp_set_system_time(u32_t sec, char * dest)
{
  struct tm current_time_val;
  time_t current_time = (time_t)sec;

  localtime_r(&current_time, &current_time_val);
  strftime(dest, TIME_STR_MAX_LEN, "%d.%m.%Y %H:%M:%S", &current_time_val);
}

void
sntp_example_init(void)
{
  sntp_setoperatingmode(SNTP_OPMODE_POLL);
#if LWIP_DHCP
  sntp_servermode_dhcp(1); /* get SNTP server via DHCP */
#else /* LWIP_DHCP */
#if LWIP_IPV4
  sntp_setserver(0, netif_ip_gw4(netif_default));
#endif /* LWIP_IPV4 */
#endif /* LWIP_DHCP */
  sntp_init();
}