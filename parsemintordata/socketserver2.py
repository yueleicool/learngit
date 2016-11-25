#!/usr/bin/python
#coding=utf-8
import SocketServer
from SocketServer import StreamRequestHandler as SRH
from time import ctime
import time
import sys,os
import json
import MySQLdb
import parsemintordata as pmd
from MySqlConn import Mysql


def daemonize(stdin='/dev/null',stdout= '/dev/null', stderr= '/var/log/yueleierror.log'):
    #Perform first fork.
    try:
        pid = os.fork()
        if pid > 0:
              sys.exit(0) #first parent out
    except OSError, e:
        sys.stderr.write("fork #1 failed: (%d) %s\n" %(e.errno, e.strerror))
        sys.exit(1)
 
    #从母体环境脱离
    os.chdir("/")
    os.umask(0)
    os.setsid()
    #执行第二次fork
    try:
        pid = os.fork()
        if pid > 0:
            sys.exit(0) #second parent out
    except OSError, e:
        sys.stderr.write("fork #2 failed: (%d) %s]n" %(e.errno,e.strerror))
        sys.exit(1)
 
    #进程已经是守护进程了，重定向标准文件描述符
    for f in sys.stdout, sys.stderr: f.flush()
    si = file(stdin, 'r')
    so = file(stdout,'a+')
    se = file(stderr,'a+',0)
    os.dup2(si.fileno(), sys.stdin.fileno())
    os.dup2(so.fileno(), sys.stdout.fileno())
    os.dup2(se.fileno(), sys.stderr.fileno())

def uchar_checksum(data):
  length=len(data)
  checksum=0
  for i in range(0,length):
    checksum += ord(data[i])
    checksum &=0xFF
  return checksum

class Servers(SRH):  
    def handle(self):  
        print 'got connection from ',self.client_address  
        #self.wfile.write('connection %s:%s at %s succeed!' % (host,port,ctime()))  
        #self.wfile.write(senddata)  
        data=''
        while True:  
            recvdata = self.request.recv(1024)  
            if not recvdata:   
                break 
 
            if data=='' and recvdata[:2] !='\x40\x40':
               continue

            if data=='' and recvdata[:2] !='\x40\x40' and recvdata[-2:] == '\x23\x23':
               continue

            if recvdata[:2] == '\x40\x40' and recvdata[-2:] == '\x23\x23':
                data=recvdata
            
            if recvdata[:2] == '\x40\x40' and recvdata[-2:] != '\x23\x23':
                data+=recvdata
                continue
            
            if recvdata[:2] != '\x40\x40' and recvdata[-2:] != '\x23\x23':
                data+=recvdata
                continue
            
            if recvdata[:2] != '\x40\x40' and recvdata[-2:] == '\x23\x23':
                data+=recvdata
            
            if data!='' and data[:2] == '\x40\x40' and data[-2:] == '\x23\x23':
                #print data
                #if ord(data[26])==2:
                dd=data[2:-2]
                arr=dd.split('##@@')
                desadd=''
                for i in arr:
                    s=''
                    for j in i[16:22][::-1]:
                        s+=hex(ord(j))[2:].zfill(2)
                    desadd = s
                if True:
                    recvdate=''
                    for i in arr:
                        s=''
                        for j in i[4:10][::-1]:
                            s+=str(ord(j)).zfill(2)
                        recvdate = s
                    resadd=''
                    for i in arr:
                        s=''
                        for j in i[10:16][::-1]:
                            s+=hex(ord(j))[2:].zfill(2)
                        resadd = s

                    codenumber = ''
                    jsoninfo = ''

                    if ord(data[27]) == 2:
                        tmpinfo = pmd.parse_typeflag_02(data[27:-3])
                        codenumber = tmpinfo['codenumber']
                        jsoninfo = json.dumps(tmpinfo)

                    if ord(data[27]) == 204:
                        tmpinfo = pmd.parse_typeflag_204(data[27:-3])
                        codenumber = tmpinfo['codenumber']
                        jsoninfo = json.dumps(tmpinfo)

                    if ord(data[27]) == 206:
                        tmpinfo = pmd.parse_typeflag_206(data[27:-3])
                        codenumber = tmpinfo['codenumber']
                        jsoninfo = json.dumps(tmpinfo)

                    if ord(data[27]) == 1:
                        tmpinfo = pmd.parse_typeflag_01(data[27:-3])
                        codenumber = ''
                        jsoninfo = json.dumps(tmpinfo)
                    
                    if ord(data[27]) == 24:
                        tmpinfo = pmd.parse_typeflag_24(data[27:-3])
                        codenumber = ''
                        jsoninfo = json.dumps(tmpinfo)

                    if ord(data[27]) == 205:
                        tmpinfo = pmd.parse_typeflag_205(data[27:-3])
                        codenumber = ''
                        jsoninfo = json.dumps(tmpinfo)

                    mysql = Mysql()
                    sql = "insert into mintordata(mintortime,resadd,desadd,datainfo,controlunit,typeflag,codenumber,jsoninfo) values(%s,%s,%s,%s,%s,%s,%s,%s)"
                    values_info = (recvdate,resadd,desadd,data[27:-3],ord(data[26]),ord(data[27]),codenumber,jsoninfo)
                    results_value = mysql.insertOne(sql,values_info)
                    mysql.dispose()
                    #cur.execute('insert into mintordata(mintortime,resadd,desadd,datainfo,controlunit,typeflag,codenumber,jsoninfo) values(%s,%s,%s,%s,%s,%s,%s,%s)',(recvdate,resadd,desadd,data[27:-3],ord(data[26]),ord(data[27]),codenumber,jsoninfo))
                    #conn.commit()
                    print "RECV from ", self.client_address[0]," at ",self.client_address[1] , " recvdata at ",recvdate,"  resadd at ",resadd," desadd at ",desadd
                    #senddata=data[:6]+second+minute+hour+day+month+year+data[18:24]+data[12:18]+data[24:26]+'\x03'
                    senddata=data[:6]+second+minute+hour+day+month+year+data[18:24]+data[12:18]+'\x00\x00'+'\x03'
                    #senddata=data[:6]+second+minute+hour+day+month+year+data[18:24]+data[12:18]+data[24:26]+'\x04'+data[27:-3]
                    checkstr=chr(uchar_checksum(senddata[2:]))
                    senddata+=checkstr+'##'
                    self.request.send(senddata) 
                    data='' 

if __name__ == "__main__":
  daemonize()
  #conn=MySQLdb.connect(host="120.55.119.87",user="root",passwd="ZYHzyhDDKJddkj",db="mintorinfo",charset="utf8")

  #cur=conn.cursor()

  host = '115.29.210.103'
  port = 10119 
  addr = (host,port)  
  senddata=''

  year=chr(int(time.strftime("%Y",time.localtime())[2:]))
  month=chr(int(time.strftime("%m",time.localtime())))
  day=chr(int(time.strftime("%d",time.localtime())))
  hour=chr(int(time.strftime("%H",time.localtime())))
  minute=chr(int(time.strftime("%M",time.localtime())))
  second=chr(int(time.strftime("%S",time.localtime())))
  print 'server is running....'  
  server = SocketServer.ThreadingTCPServer(addr,Servers)  
  server.serve_forever()
