#ifndef SNTP_HANDLE_H
#define SNTP_HANDLE_H

#include <stdint.h>
#define TIME_STR_MAX_LEN 32

typedef struct {
    uint32_t secs;
} real_time_t;

static real_time_t real_time;

void sntp_buddy_init(void);
void sntp_set_system_time(uint32_t sec);

#endif //SNTP_HANDLE_H