#ifndef WUI_REQUEST_PARSER_H
#define WUI_REQUEST_PARSER_H

#include "http_client.h"
#include "httpd.h"

HTTPC_COMMAND_STATUS parse_http_reply(char *reply, uint32_t reply_len, httpc_header_info *h_info_ptr);

#endif // WUI_REQUEST_PARSER_H