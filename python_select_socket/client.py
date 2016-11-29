#!/usr/bin/python
#coding=utf-8
#使用utf-8在命令行窗口里才能运行  
import socket  
import time  
   
#getpeername()获得socket对方的地址  
   
server=('10.0.2.15',21345)  
   
msg=['hello','welcome','xiaoming','zhangsan','list','liuliu']  
   
socks=[]  
for i in range(10):  
    sock=socket.socket(socket.AF_INET,socket.SOCK_STREAM)  
    socks.append(sock)  
       
for s in socks:  
    s.connect(server)  
   
counter =0 
for m in msg:  
    for s in socks:  
        s.send('%d send %s' %(counter,m))  
        counter=counter+1 
    for s in socks:  
        data =s.recv(1024)  
        print '%s echo %s' %(s.getpeername(),data)  
        if not data:  
            s.close()  
    time.sleep(2)
