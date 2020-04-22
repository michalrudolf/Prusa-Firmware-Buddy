#include "wui_request_parser.h"
#include <string.h>

#define CMD_LIMIT               10 // number of commands accepted in low level command response

static HTTPC_COMMAND_STATUS parse_high_level_cmd(char *json, uint32_t len) {
    char cmd_str[200];
    uint32_t ret_code = httpc_json_parser(json, len, cmd_str);
    if (ret_code) {
        return CMD_REJT_CMD_STRUCT;
    } else {
        return CMD_ACCEPTED;
    }
}

static HTTPC_COMMAND_STATUS parse_low_level_cmd(const char *request, httpc_header_info *h_info_ptr) {

    char gcode_str[CMD_LIMIT][MAX_REQ_MARLIN_SIZE] = { 0 };

    if (h_info_ptr->content_lenght <= 2) {
        return CMD_REJT_CMD_STRUCT;
    }

    uint32_t cmd_count = 0;
    uint32_t index = 0;
    const char *line_start_addr = request;
    const char *line_end_addr;
    uint32_t i = 0;

    do {
        cmd_count++;
        if (CMD_LIMIT < cmd_count) {
            return CMD_REJT_GCODES_LIMI; // return if more than 10 codes in the response
        }

        while ((i < h_info_ptr->content_lenght) && (request[i] != '\0') && (request[i] != '\r') && (request[i + 1] != '\n')) {
            i++;
        }

        line_end_addr = request + i;
        uint32_t str_len = line_end_addr - line_start_addr;
        strlcpy(gcode_str[index++], line_start_addr, str_len + 1);
        line_start_addr = line_end_addr + 2;
        i = i + 2; // skip the end of line chars

    } while (i < h_info_ptr->content_lenght);

    for (uint32_t cnt = 0; cnt < cmd_count; cnt++) {
        send_request_to_wui(gcode_str[cnt]);
    }

    return CMD_ACCEPTED;
}

HTTPC_COMMAND_STATUS parse_http_reply(char *reply, uint32_t reply_len, httpc_header_info *h_info_ptr) {
    HTTPC_COMMAND_STATUS cmd_status = CMD_UNKNOWN;
    if (0 == h_info_ptr->command_id) {
        return CMD_REJT_CMD_ID;
    }
    if (TYPE_JSON == h_info_ptr->content_type) {
        cmd_status = parse_high_level_cmd(reply, reply_len);
    } else if (TYPE_GCODE == h_info_ptr->content_type) {
        cmd_status = parse_low_level_cmd(reply, h_info_ptr);
    } else {
        cmd_status = CMD_REJT_CONT_TYPE;
    }
    return cmd_status;
}