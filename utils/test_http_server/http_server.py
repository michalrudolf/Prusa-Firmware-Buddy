import ipaddress
import requests
import logging
import sys
import time
from datetime import datetime

# admin_valid = '{"connect_ip":"10.24.230.10","connect_key":"01234567890123456789","connect_name":"P3D"}'
# admin_not_valid_ip = '{"connect_ip":"1000.24.230.10","connect_key":"11234567890123456789","connect_name":"PRUSA3D"}'
# admin_longer_str = '{"connect_ip":"10.24.230.10","connect_key":"11114567890123456789123456","connect_name":"PRUSA3D01234567890123456789"}'

# global variables
IP_ADDR = ""
test_cnt = 0
tests_off = 0
json_tests = {}

# gcode structures
G_AUTOHOME = {"command":"G28"}
G_MOVES = {"command":"G1 X50 Y100", "command":"G1 X100 Y50", "command":"G1 X60 Y90", "command":"G1 X100 Y50", "command":"G1 X70 Y80"}

def init(ip_addr):
    global json_tests, test_cnt, IP_ADDR

    # set ip address
    IP_ADDR = 'http://' + ip_addr

    # load JSON tests
    json_file = open("/server_tests/tests.json", "r")
    json_obj = json.load(json_file)
    json_file.close()

    json_tests = json_obj['tests']

    # disable switched off tests
    for test in json_tests:
        if "switch" in test:
            if "off" in test["switch"]:
                json_tests.remove(test)
    
    if len(json_tests) == 0:
        print("All tests switched off")
    
    test_cnt = len(json_tests)

    # logging init
    logging.basicConfig(filename='server_tests/errors.log', filemode='w', level=logging.ERROR)

def main():
    global test_cnt
    args_count = len(sys.argv)
    args_list = sys.argv

    # check script argument
    if args_count == 2:
        try:
            ip = ipaddress.ip_address(sys.argv[1])    
        except ValueError:
            print('Address is invalid: %s' % sys.argv[1])
            return
        init(args_list[1])
    else:
        print("\nRun again and add argument IP of tested printer\nexample: python HTTP_server_tests.py 10.24.230.10\n")
        return

    while 1:

        test_get_telemetry()
        if test_cnt != 0:
            test_loop()

        time.sleep(1)

def test_loop():
    global json_tests

    
    name = json_tests[test_curr]['name']

    result_dic = send_request(name)
    if len(result_dic) != 0:
        test_response(result_dic)

    if (test_curr + 1) >= test_cnt :
            test_curr = 0
            off_test_cnt = 0
        else:
            test_curr += 1

def send_request(name):
    global test_curr, test_cnt, json_tests, IP_ADDR
    
    test_header = json_tests[test_curr]['request']['header']
    headers = {}
    res_dic = {}
    ip_addr = IP_ADDR + json_tests[test_curr]['uri']

    if 'token' in test_header:
        headers.update({"Printer-Token" : str(test_header['token'])})

    if 'GET' in test_header['method']:
        if len(headers) != 0:
            response = requests.get(url = ip_addr, headers = headers)
        else:
            response = requests.get(url = ip_addr)
    elif 'POST' in test_header['method']:
        if len(headers) != 0:
            response = requests.post(url = ip_addr, headers = headers, json = json_tests['request']['body'])
        else:
            response = requests.post(url = ip_addr, json = json_tests['request']['body'])
    else:
        test_failed(str(json_tests[test_curr]), name + " has unsupported test method...")
        return {}

    try:
        res_dic = response.json()
    except ValueError:
        test_failed(str(res_dic), "Response parsing to JSON")
        return {}
    
    return res_dic

def test_response(result_dic):
    pass

def test_get_telemetry():
    response = requests.get(IP_ADDR + "/api/telemetry")
    try:
        res_dic = response.json()
    except ValueError:
        test_failed(str(res_dic), "Telemetry parsing to JSON")
        return

    test_telemetry_response(res_dic)

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