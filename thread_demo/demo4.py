#!/usr/bin/python
#coding=utf-8
#互斥锁的代码显示  全局变量多线程访问的时候对全局变量资源进行上锁
import threading
import time
import random
 
counter = 0
mutex = threading.Lock()
 
class MyThread(threading.Thread):
    def __init__(self):
        threading.Thread.__init__(self)
 
    def run(self):
        global counter, mutex
        time.sleep(random.randint(1,3));
        if mutex.acquire():
            counter += 1
            print "I am %s, set counter:%s" % (self.name, counter)
            mutex.release()
 
if __name__ == "__main__":
    for i in range(0, 100):
        my_thread = MyThread()
        my_thread.start()
