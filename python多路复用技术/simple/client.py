#!/usr/bin/python
#coding=utf-8
import socket  
  
socCli = socket.socket()  
socCli.connect(('127.0.0.1', 8888))  
while True:  
    data = raw_input("input str:")  
    socCli.send(data.encode()) 
