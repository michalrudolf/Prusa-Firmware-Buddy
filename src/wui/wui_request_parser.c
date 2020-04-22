#include "wui_request_parser.h"
#include "wui_helper_funcs.h"
#include <string.h>
#include "jsmn.h"
#include "dbg.h"
#include "wui.h"

#define HTTP_DUBAI_HACK     0

#if HTTP_DUBAI_HACK
    #include "version.h"
#endif
#define CMD_LIMIT           10 // number of commands accepted in low level command response

#define MAX_ACK_SIZE 16
uint32_t httpc_json_parser(char *json, uint32_t len);

static int json_cmp(const char *json, jsmntok_t *tok, const char *s) {
    if (tok->type == JSMN_STRING && (int)strlen(s) == tok->end - tok->start && strncmp(json + tok->start, s, tok->end - tok->start) == 0) {
        return 0;
    }
    return -1;
}

static HTTPC_COMMAND_STATUS parse_high_level_cmd(char *json, uint32_t len) {
    uint32_t ret_code = httpc_json_parser(json, len);
    if (ret_code) {
        return CMD_STATUS_REJT_CMD_STRUCT;
    } else {
        return CMD_STATUS_ACCEPTED;
    }
}

static HTTPC_COMMAND_STATUS parse_low_level_cmd(const char *request, httpc_header_info *h_info_ptr) {

    char gcode_str[CMD_LIMIT][MAX_REQ_MARLIN_SIZE] = { 0 };

    if (h_info_ptr->content_lenght <= 2) {
        return CMD_STATUS_REJT_CMD_STRUCT;
    }

    uint32_t cmd_count = 0;
    uint32_t index = 0;
    const char *line_start_addr = request;
    const char *line_end_addr;
    uint32_t i = 0;

    do {
        cmd_count++;
        if (CMD_LIMIT < cmd_count) {
            return CMD_STATUS_REJT_GCODES_LIMI; // return if more than 10 codes in the response
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

    wui_cmd_t cmd;
    cmd.lvl = LOW_LVL_CMD;
    for (uint32_t cnt = 0; cnt < cmd_count; cnt++) {
        strcpy(cmd.arg, gcode_str[cnt]);
        send_request_to_wui(&cmd);
    }

    return CMD_STATUS_ACCEPTED;
}

static uint8_t parse_high_cmd_args(wui_cmd_t *command, const char *json, jsmntok_t *t, int *i){
    if (json_cmp(json, &t[*i], "args") != 0 || t[*i + 1].type != JSMN_ARRAY || t[*i].size > HIGH_CMD_MAX_ARGS_CNT){
        return 1;
    }
    (*i)++;
    switch (command->high_lvl_cmd) {
        case CMD_SEND_INFO:
            strlcpy(command->arg, json + t[*i].start, (t[*i].end - t[*i].start + 1));
            (*i)++;
            break;    
        // TODO: other high level commands

        default:
        break;
    }
    return 0;
}

HTTPC_COMMAND_STATUS parse_http_reply(char *reply, uint32_t reply_len, httpc_header_info *h_info_ptr) {
    HTTPC_COMMAND_STATUS cmd_status = CMD_STATUS_UNKNOWN;
    if (0 == h_info_ptr->command_id) {
        return CMD_STATUS_REJT_CMD_ID;
    }
    if (TYPE_JSON == h_info_ptr->content_type) {
        cmd_status = parse_high_level_cmd(reply, reply_len);
    } else if (TYPE_GCODE == h_info_ptr->content_type) {
        cmd_status = parse_low_level_cmd(reply, h_info_ptr);
    } else {
        cmd_status = CMD_STATUS_REJT_CONT_TYPE;
    }
    return cmd_status;
}

uint32_t httpc_json_parser(char *json, uint32_t len) {
    uint32_t ret_code = 1; // success is 0
    int ret;
    jsmn_parser parser;
    jsmntok_t t[128]; // Just a raw value, we do not expect more that 128 tokens
    char request[MAX_REQ_MARLIN_SIZE];

    jsmn_init(&parser);
    ret = jsmn_parse(&parser, json, len, t, sizeof(t) / sizeof(jsmntok_t));

    if (ret < 1 || t[0].type != JSMN_OBJECT) {
        // Fail to parse JSON or top element is not an object
        return 1;
    }

    wui_cmd_t command;

    for (int i = 0; i < ret; i++) {
        if (json_cmp(json, &t[i], "command") == 0) {
            strlcpy(request, json + t[i + 1].start, (t[i + 1].end - t[i + 1].start + 1));
            i++;
            
            if(strcmp(request, "SEND_INFO") == 0){
                command.high_lvl_cmd = CMD_SEND_INFO;
            // TODO: other high level commands
            } else {
                command.high_lvl_cmd = CMD_UNKNOWN;
            }

            if(parse_high_cmd_args(&command, json, t, &i)){
                return 1;
            }
        }
    }
    
    send_request_to_wui(&command);
    ret_code = 0;
    return ret_code;
}

void httpd_json_parser(char *json, uint32_t len) {
    int ret;
    jsmn_parser parser;
    jsmntok_t t[128]; // Just a raw value, we do not expect more that 128 tokens
    char request_str[MAX_REQ_MARLIN_SIZE];

    jsmn_init(&parser);
    ret = jsmn_parse(&parser, json, len, t, sizeof(t) / sizeof(jsmntok_t));

    if (ret < 1 || t[0].type != JSMN_OBJECT) {
        // Fail to parse JSON or top element is not an object
        return;
    }

    for (int i = 0; i < ret; i++) {
        wui_cmd_t request;
#if HTTP_DUBAI_HACK
        if (json_cmp(json, &t[i], project_firmware_name) == 0) {
#else
        if (json_cmp(json, &t[i], "command") == 0) {
#endif //HTTP_DUBAI_HACK
            strlcpy(request.arg, json + t[i + 1].start, (t[i + 1].end - t[i + 1].start + 1));
            request.lvl = LOW_LVL_CMD;
            i++;
            _dbg("command received: %s", request.arg);
            send_request_to_wui(&request);
        } else if (json_cmp(json, &t[i], "connect_ip") == 0) {
            strlcpy(request_str, json + t[i + 1].start, t[i + 1].end - t[i + 1].start + 1);
            ip4_addr_t tmp_addr;
            if (ip4addr_aton(request_str, &tmp_addr)) {
                snprintf(request.arg, MAX_REQ_MARLIN_SIZE, "!cip %lu", tmp_addr.addr);
                request.high_lvl_cmd = CMD_SET;
                request.lvl = HIGH_LVL_CMD;
                send_request_to_wui(&request);
            }
            i++;
        } else if (json_cmp(json, &t[i], "connect_key") == 0) {
            strlcpy(request_str, json + t[i + 1].start, t[i + 1].end - t[i + 1].start + 1);
            snprintf(request.arg, MAX_REQ_MARLIN_SIZE, "!ck %s", request_str);
            request.high_lvl_cmd = CMD_SET;
            request.lvl = HIGH_LVL_CMD;
            send_request_to_wui(&request);
            i++;
        } else if (json_cmp(json, &t[i], "connect_name") == 0) {
            strlcpy(request_str, json + t[i + 1].start, t[i + 1].end - t[i + 1].start + 1);
            snprintf(request.arg, MAX_REQ_MARLIN_SIZE, "!cn %s", request_str);
            request.high_lvl_cmd = CMD_SET;
            request.lvl = HIGH_LVL_CMD;
            send_request_to_wui(&request);
            i++;
        }
    }
}