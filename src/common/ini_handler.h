// INI file handler (ini_handler.h)
#ifndef INI_HANDLER_H
#define INI_HANDLER_H

#include <stdint.h>
#define MAX_INI_SIZE 200

uint8_t ini_save_file(const char *ini_save_str);
uint8_t ini_load_file(void *user_struct);

char ini_file_str[MAX_INI_SIZE];

#endif //INI_HANDLER_H
