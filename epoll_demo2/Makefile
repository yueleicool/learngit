.PHONY:all
all:epoll_client epoll_server
epoll_client:epoll_client.c
	gcc -o $@ $^
epoll_server:epoll_server.c
	gcc -o $@ $^
.PHONY:clean
clean:
	rm -f epoll_client epoll_server
