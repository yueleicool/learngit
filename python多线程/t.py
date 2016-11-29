#!/usr/bin/python
#coding=utf-8
import threading,time
from time import sleep, ctime
def now() :
    return str( time.strftime( '%Y-%m-%d %H:%M:%S' , time.localtime() ) )

def test(nloop, nsec):
    print 'start loop', nloop, 'at:', now()
    sleep(nsec)
    print 'loop', nloop, 'done at:', now()

def main():
    print 'starting at:',now()
    threadpool=[]

    for i in xrange(10):
        th = threading.Thread(target= test,args= (i,2))
        threadpool.append(th)

    for th in threadpool:
        th.start()

    for th in threadpool :
        threading.Thread.join( th )

    print 'all Done at:', now()

if __name__ == '__main__':
        main()
