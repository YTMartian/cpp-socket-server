import os

for port in range(1234, 4321):
    print('port: ', port) 
    n, c = input('input n c: ').split(' ')
    command = 'ab -n {} -c {} -s 100 http://127.0.0.1:{}/login.html'.format(n, c, port)
    print(command)
    os.system(command)
