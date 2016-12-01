.PHONY:all
all:tcp udp arp1 arp2 
tcp:
	gcc -o tcp tcp.c -lnet
udp:
	gcc -o udp udp.c -lnet
arp1:
	gcc -o arp1 arp1.c -lnet
arp2:
	gcc -o arp2 arp2.c -lnet 
clean:  
	rm -f tcp udp arp1 arp2 
