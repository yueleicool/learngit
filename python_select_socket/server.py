#!/usr/bin/python
#coding=utf-8
import socket  
import select  
import Queue  
   
server=('10.0.2.15',21345)  
#SOCK_STREAM（流套接字）、SOCK_DGRAM（数据报文套接字）、AF_INET（IPv4）      
sock=socket.socket(socket.AF_INET,socket.SOCK_STREAM)  
#阻塞与端口复用前后顺序可换  
sock.setblocking(False)  
#SOL_SOCKET（套接字描述符）、SO_REUSEADDR（端口复用）  
sock.setsockopt(socket.SOL_SOCKET,socket.SO_REUSEADDR,1)  
sock.bind(server)  
sock.listen(5)  
   
rlists=[sock]  
wlists=[]  
msg_que={}  
   
timeout=10 
   
while True:   
    rs,ws,es=select.select(rlists,wlists,rlists,timeout)  
   
    if not(rs or ws or es):  
        print 'timeout...' 
        continue 
       
    #读部分  
    for s in rs:  
        #看s是否是本机上用于监听的socket，是则接受连接，不是则接收数据  
        if s is sock:  
            conn,addr=s.accept()  
            #conn、addr分别是所接收到的socket对象和对方端口  
            print 'connect by',addr  
            conn.setblocking(False)  
            rlists.append(conn)  
            #使用字典将conn与一个队列相对应  
            msg_que[conn]=Queue.Queue()              
        else:  
            data =s.recv(1024)  
            if data:  
                print data  
                msg_que[s].put(data)  
                if s not in wlists:  
                    wlists.append(s)  
            else:  
                if s in wlists:  
                    wlists.remove(s)  
                rlists.remove(s)  
                s.close  
                del msg_que[s]  
    #写部分             
    for  s in ws:  
        try:  
            #get_nowait()跟get(0)一样  
            msg=msg_que[s].get_nowait()  
        except Queue.Empty:  
            print 'msg empty' 
            wlists.remove(s)  
        else:  
            s.send(msg)  
    #异常部分      
    for s in es:  
        print 'except',s.getpeername()  
        if s in rlists:  
            rlists.remove(s)  
        if s in wlists:  
            wlists.remove(s)          
        s.close  
        del msg_que[s]  
