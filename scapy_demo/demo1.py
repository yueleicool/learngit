#!/usr/bin/env python
#--*--coding=utf-8--*--
#打印收到的数据包的源IP和TTL值
 
from scapy.all import *
 
def testTTL(pkt):
    try:
        if pkt.haslayer(IP):
            ipsrc = pkt.getlayer(IP).src
            ttl = str(pkt.ttl)
            print '[+] Pkt FROM: '+ipsrc+'   TTL: '+ttl
    except:
        pass
 
def main():
    sniff(prn=testTTL,store=0)
 
if __name__ == '__main__':
    main()
