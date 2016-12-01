.PHONY:all
all:main 
main:main.o ip.o tcp.o ether.o 
main.o:
	gcc -c main.c
tcp.o:
	gcc -c tcp.c
ip.o:
	gcc -c ip.c
ether.o:
	gcc -c ether.c  
main:
	gcc -g -Wall -o main main.o ip.o tcp.o ether.o -lpcap
clean:  
	rm -f main *.o 
