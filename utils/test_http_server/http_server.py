import http_server_test_funcs as test
import time

def main():
    global test.test_cnt
    args_count = len(sys.argv)
    args_list = sys.argv

    # check script argument
    if args_count == 2:
        try:
            ip = ipaddress.ip_address(sys.argv[1])    
        except ValueError:
            print('Address is invalid: %s' % sys.argv[1])
            return
        test.init(args_list[1])
    else:
        print("\nRun again and add argument IP of tested printer\nexample: python HTTP_server_tests.py 10.24.230.10\n")
        return

    while 1:
        test.test_get_telemetry()
        if test.test_cnt != 0:
            test.test_loop()

        time.sleep(1)

main()