#!/usr/bin/python
# coding : uft-8
__author__ = 'Phtih0n'
import threading, time
class MyThread(threading.Thread):
    def __init__(self):
        threading.Thread.__init__(self)
    def run(self):
        global n, lock
        time.sleep(1)
        if lock.acquire():
            print n , self.name
            n += 1
            lock.release()
if "__main__" == __name__:
    n = 1
    ThreadList = []
    lock = threading.Lock()
    for i in range(1, 200):
        t = MyThread()
        ThreadList.append(t)
    for t in ThreadList:
        t.start()
    for t in ThreadList:
        t.join()
