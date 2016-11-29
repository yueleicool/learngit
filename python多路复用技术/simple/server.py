#!/usr/bin/python
#coding=utf-8
import socket  
  
SOCKET_FAMILY = socket.AF_INET  
SOCKET_TYPE = socket.SOCK_STREAM  
  
sockServer = socket.socket()  
sockServer.bind(('0.0.0.0', 8888))  
sockServer.listen(5)  
  
while True:  
    cliobj, addr = sockServer.accept()  
    while True:  
        recvdata = cliobj.recv(1024)  
        if recvdata:  
            print(recvdata.decode())  
        else:  
            cliobj.close()  
            break  
