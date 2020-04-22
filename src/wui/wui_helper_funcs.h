#ifndef WUI_HELPER_FUNCS_H
#define WUI_HELPER_FUNCS_H

#include "cmsis_os.h"

#define MAX_REQ_MARLIN_SIZE 100
#define MAX_REQ_BODY_SIZE   512

#define MAX_STATE_LEN  10
#define MAX_REASON_LEN 50

void send_request_to_wui(const char *request);

#endif //WUI_HELPER_FUNCS_H
