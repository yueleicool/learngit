#!/ust/bin/python
#coding=utf-8
#可重入锁RLock 如果一个线程遇到锁嵌套的情况该怎么办，这个嵌套是指当我一个线程在获取临界资源时，又需要再次获取。
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
        time.sleep(1);
        #time.sleep(random.randint(1,3));
        if mutex.acquire():
            counter += 1
            print "I am %s, set counter:%s" % (self.name, counter)
            if mutex.acquire():
                counter += 1
                print "I am %s, set counter:%s" % (self.name, counter)
                mutex.release()
            mutex.release()
 
if __name__ == "__main__":
    for i in range(0, 200):
        my_thread = MyThread()
        my_thread.start()
