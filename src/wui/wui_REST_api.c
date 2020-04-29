/*
 * wui_api.c
 * \brief
 *
 *  Created on: Jan 24, 2020
 *      Author: joshy <joshymjose[at]gmail.com>
 */

#include "wui_REST_api.h"

#include "wui.h"
#include "filament.h"
#include "wui_vars.h"
#include "progress_data_wrapper.h"

#define BDY_WUI_API_BUFFER_SIZE 512

// for data exchange between wui thread and HTTP thread
static wui_vars_t wui_vars_copy;

void get_telemetry_for_connect(char *data, const uint32_t buf_len) {

    osStatus status = osMutexWait(wui_thread_mutex_id, osWaitForever);
    if (status == osOK) {
        wui_vars_copy = wui_vars;
    }
    osMutexRelease(wui_thread_mutex_id);

    float z_pos_mm = wui_vars_copy.pos[Z_AXIS_POS];
    const char *filament_material = filaments[get_filament()].name;

    if (!wui_vars_copy.sd_printing) {
        snprintf(data, buf_len, "{"
                                "\"temp_nozzle\":%.2f,"
                                "\"temp_bed\":%.2f,"
                                "\"target_nozzle\":%.2f,"
                                "\"target_bed\":%.2f,"
                                "\"p_fan\":%d,"
                                "\"material\":\"%s\","
                                "\"pos_z_mm\":%.2f,"
                                "\"printing_speed\":%d,"
                                "\"flow_factor\":%d"
                                "}",
            wui_vars_copy.temp_nozzle, wui_vars_copy.temp_bed,
            wui_vars_copy.target_nozzle, wui_vars_copy.target_bed,
            wui_vars_copy.fan_speed, filament_material,
            z_pos_mm, wui_vars_copy.print_speed, wui_vars_copy.flow_factor);

        return;
    }

    uint8_t percent_done;
    uint32_t estimated_time;
    char time_2_end[9], print_time[13];
    if (is_percentage_valid(wui_vars_copy.print_dur)) {
        estimated_time = progress_time2end(wui_vars_copy.print_speed);
        percent_done = progress_get_percentage();
        progress_format_time2end(time_2_end, wui_vars_copy.print_speed);
    } else {
        estimated_time = 0;
        strlcpy(time_2_end, "N/A", 4);
        percent_done = wui_vars_copy.sd_precent_done;
    }

    print_dur_to_string(print_time, sizeof(print_time), wui_vars_copy.print_dur);

    snprintf(data, buf_len, "{"
                            "\"temp_nozzle\":%.2f,"
                            "\"temp_bed\":%.2f,"
                            "\"target_nozzle\":%.2f,"
                            "\"target_bed\":%.2f,"
                            "\"p_fan\":%d,"
                            "\"material\":\"%s\","
                            "\"pos_z_mm\":%.2f,"
                            "\"printing_speed\":%d,"
                            "\"flow_factor\":%d,"
                            "\"progress\":%d,"
                            "\"print_dur\":\"%s\"," // OctoPrint API ?
                            "\"time_est\":\"%s\","
                            "\"printing_time\":%ld," // Connect
                            "\"estimated_time\":%u,"
                            "\"project_name\":\"%s\","
                            "\"state\":\"PRINTING\""
                            "}",
        wui_vars_copy.temp_nozzle, wui_vars_copy.temp_bed,
        wui_vars_copy.target_nozzle, wui_vars_copy.target_bed,
        wui_vars_copy.fan_speed, filament_material,
        z_pos_mm, wui_vars_copy.print_speed, wui_vars_copy.flow_factor,
        percent_done, print_time, time_2_end,
        wui_vars_copy.print_dur, estimated_time,
        wui_vars_copy.gcode_name);
}

void get_telemetry_for_local(char *data, const uint32_t buf_len) {

    osStatus status = osMutexWait(wui_thread_mutex_id, osWaitForever);
    if (status == osOK) {
        wui_vars_copy = wui_vars;
    }
    osMutexRelease(wui_thread_mutex_id);

    int32_t actual_nozzle = (int32_t)(wui_vars_copy.temp_nozzle);
    int32_t actual_heatbed = (int32_t)(wui_vars_copy.temp_bed);
    double z_pos_mm = (double)wui_vars_copy.pos[Z_AXIS_POS];
    uint16_t print_speed = (uint16_t)(wui_vars_copy.print_speed);
    uint16_t flow_factor = (uint16_t)(wui_vars_copy.flow_factor);
    const char *filament_material = filaments[get_filament()].name;

    if (!wui_vars_copy.sd_printing) {
        snprintf(data, buf_len, "{"
                                "\"temp_nozzle\":%d,"
                                "\"temp_bed\":%d,"
                                "\"material\":\"%s\","
                                "\"pos_z_mm\":%.2f,"
                                "\"printing_speed\":%d,"
                                "\"flow_factor\":%d"
                                "}",
            actual_nozzle, actual_heatbed, filament_material,
            z_pos_mm, print_speed, flow_factor);
        return;
    }

    uint8_t percent_done;
    char time_2_end[9], print_time[13];
    if (is_percentage_valid(wui_vars_copy.print_dur)) {
        percent_done = progress_get_percentage();
        progress_format_time2end(time_2_end, wui_vars_copy.print_speed);
    } else {
        strlcpy(time_2_end, "N/A", 4);
        percent_done = wui_vars_copy.sd_precent_done;
    }

    print_dur_to_string(print_time, sizeof(print_time), wui_vars_copy.print_dur);

    snprintf(data, buf_len, "{"
                            "\"temp_nozzle\":%ld,"
                            "\"temp_bed\":%ld,"
                            "\"material\":\"%s\","
                            "\"pos_z_mm\":%.2f,"
                            "\"printing_speed\":%d,"
                            "\"flow_factor\":%d,"
                            "\"progress\":%d,"
                            "\"print_dur\":\"%s\","
                            "\"time_est\":\"%s\","
                            "\"project_name\":\"%s\""
                            "}",
        actual_nozzle, actual_heatbed, filament_material,
        z_pos_mm, print_speed, flow_factor,
        percent_done, print_time, time_2_end, wui_vars_copy.gcode_name);
}
