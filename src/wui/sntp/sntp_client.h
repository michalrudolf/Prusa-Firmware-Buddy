#ifndef SNTP_HANDLE_H
#define SNTP_HANDLE_H

#include <stdint.h>

#define TIME_STR_MAX_LEN 32


void sntp_set_system_time(uint32_t sec);
void sntp_client_init(void);


#endif //SNTP_HANDLE_H