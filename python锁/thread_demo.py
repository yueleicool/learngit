#!/usr/bin/env python  
  
import threading  
import time  
import Queue  
  
def producter(name,queue,lock):  
    event.clear()  
    print '%s start to product...' % name  
    queue.put('something')  
    time.sleep(3)  
    print 'product something'  
    event.set()  
    event.wait()  
  
def consumer(name,queue,lock):  
    print '%s start to consume...' % name  
    event.wait()  
    queue.get()  
    print 'consume something'  
    event.set()  
  
  
lock = threading.Lock()  
queue = Queue.Queue(10)  
event = threading.Event()  
threads = []  
threadsc = []  
for i in range(10):  
    t = threading.Thread(target=producter,args=('kel%s' % i,queue,lock))  
    threads.append(t)  
for i in range(10):  
    threads[i].start()  
for i in range(10):  
    t = threading.Thread(target=consumer,args=('smile%s' % i,queue,lock))  
    threadsc.append(t)  
for i in range(10):  
    threadsc[i].start() 
