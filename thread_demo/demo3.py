#!/usr/bin/python
#coding=utf-8
import threading
import time
import random
 
counter = 0
#全局变量counter可能存在多线程抢占的问题  全局变量资源没有加 
class MyThread(threading.Thread):
    def __init__(self):
        threading.Thread.__init__(self)
 
    def run(self):
        global counter
        time.sleep(random.randint(1,2));
        counter += 1
        print "I am %s, set counter:%s" % (self.name, counter)
 
if __name__ == "__main__":
    for i in range(0, 2000):
        my_thread = MyThread()
        my_thread.start()
