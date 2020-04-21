// INI file handler (ini_handler.h)
#ifndef INI_HANDLER_H
#define INI_HANDLER_H

#include <stdint.h>
#define MAX_INI_SIZE 200

#define BUDDY_ENABLE_INI_LOAD_AFTER_START 0

typedef int (*buddy_ini_handler)(void* user, const char* section,
                           const char* name, const char* value);

uint8_t ini_save_file(const char *ini_save_str);
uint8_t ini_load_file(void *user_struct, buddy_ini_handler handler);

char ini_file_str[MAX_INI_SIZE];

#endif //INI_HANDLER_H
