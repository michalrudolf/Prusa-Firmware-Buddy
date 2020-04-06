#ifndef SNTP_HANDLE_H
#define SNTP_HANDLE_H

#define TIME_STR_MAX_LEN 32
void sntp_example_init(void);

void sntp_set_system_time(u32_t sec, char * dest);

#endif //SNTP_HANDLE_H