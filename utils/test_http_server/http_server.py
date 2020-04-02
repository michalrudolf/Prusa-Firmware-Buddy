import ipaddress
import http_server_test_funcs as test
import sys

#   IF YOU WANT TO ADD A TEST
#       add test data in server_tests/tests.json with JSON syntax
#       check if current test implementation will test it properly:
#           http_server_test_funcs.py > send_request()  - if JSON request, dont add content-type, it is added automatically
#                                       test_response() 

def main():
    if test_argv() is 0:
        return
    
    test.init(sys.argv[1])
    test.test_loop()


def test_argv():

    args_count = len(sys.argv)

    # check if script argument is valid
    if args_count == 2:
        try:
            ip = ipaddress.ip_address(sys.argv[1])
        except ValueError:
            print('Address \'%s\' is invalid' % sys.argv[1])
            return 0
    else:
        print("\nRun again and add as an argument the IP of tested printer\nexample: python HTTP_server.py 10.24.230.10\n")
        return 0
    
    return 1

main()