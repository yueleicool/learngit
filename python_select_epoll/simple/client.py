#coding:utf-8
import socket,select
#xiaorui.cc
host = "10.0.2.15"
port = 50000
s = socket.socket(socket.AF_INET,socket.SOCK_STREAM)
s.connect((host,port))
s.send("coming from select client")
s.close()
