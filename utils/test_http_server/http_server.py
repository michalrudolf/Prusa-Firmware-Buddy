import requests
import logging
import sys
import time
from datetime import datetime

TELEMETRY_URL = ""
GCODE_URL = ""
ADMIN_URL = ""

def main():

    global TELEMETRY_URL, GCODE_URL, ADMIN_URL

    args_count = len(sys.argv)
    args_list = sys.argv
    test_valid = 0
    my_printer_ip = ''

    # logging init
    logging.basicConfig(filename='server_tests/errors.log', filemode='w', level=logging.ERROR)

    # check argument
    if args_count == 2:
        my_printer_ip = args_list[1]
    else:
        print("\nRun again and add argument IP of tested printer\nexample: python HTTP_server_tests.py 10.24.230.10\n")
        return

    gcode_autohome = '{"command":"G28"}'
    gcode_moving = '{"command":"G1 X50 Y100", "command":"G1 X100 Y50", "command":"G1 X60 Y90", "command":"G1 X100 Y50", "command":"G1 X70 Y80"}'
    gcode_name_mistake = '{"comman":"G28"}'
    gcode_code_mistake = '{"command":"G"}'

    admin_valid = '{"connect_ip":"10.24.230.10","connect_key":"01234567890123456789","connect_name":"P3D"}'
    admin_not_valid_ip = '{"connect_ip":"1000.24.230.10","connect_key":"11234567890123456789","connect_name":"PRUSA3D"}'
    admin_longer_str = '{"connect_ip":"10.24.230.10","connect_key":"11114567890123456789123456","connect_name":"PRUSA3D01234567890123456789"}'

    post_header = "'Content-type: application/json'"

    TELEMETRY_URL = 'http://' + my_printer_ip + '/api/telemetry'
    GCODE_URL = 'http://' + my_printer_ip + '/api/g-code'
    ADMIN_URL = 'http://' + my_printer_ip + '/admin.html'

    while 1:
        
        test_get_telemetry()
        test_post_gcode()
        print("sleep")
        time.sleep(1)

def test_get_telemetry():
    response = requests.get(TELEMETRY_URL)
    res_dic = response.json()
    if not isinstance(res_dic, dict):
        print("Couldn't decode json structure of Telemetry")
        return

    test_telemetry_response(res_dic)

def test_post_gcode():
    pass

def test_telemetry_response(response_dic):
    
    if 'temp_nozzle' not in response_dic or not isinstance(response_dic['temp_nozzle'], int):
        test_failed(str(response_dic), "Telemetry")
        return
    if 'temp_bed' not in response_dic or not isinstance(response_dic['temp_bed'], int):
        test_failed(str(response_dic), "Telemetry")
        return
    if 'material' not in response_dic or not isinstance(response_dic['material'], str):
        test_failed(str(response_dic), "Telemetry")
        return
    if 'pos_z_mm' not in response_dic or not isinstance(response_dic['pos_z_mm'], float):
        test_failed(str(response_dic), "Telemetry")
        return
    if 'printing_speed' not in response_dic or not isinstance(response_dic['printing_speed'], int):
        test_failed(str(response_dic), "Telemetry")
        return
    if 'flow_factor' not in response_dic or not isinstance(response_dic['flow_factor'], int):
        test_failed(str(response_dic), "Telemetry")
        return

# if test fails it logs the info in error output file "connect_tests_results.txt"
def test_failed(data, name):
    now = datetime.now()
    logging.error(str(now) + " :: Test " + name + " failed:\n" + data + "\n")

main()