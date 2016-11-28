#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <sys/types.h>      
#include <sys/socket.h>
#include<errno.h>
#include<fcntl.h>
#define _BACKLOG_ 5
#define _BUF_SIZE_ 5  //数据缓冲区大小
#define _MAX_ 64   //事件就绪队列存储空间

typedef struct _data_buf
{
	int fd;
	char buf[_BUF_SIZE_];
}data_buf_t,*data_buf_p;

static int set_no_block(int fd) //
{
	int old_fl=fcntl(fd,F_GETFL);//获取当前文件描述符的标志
	if(old_fl<0)
	{
		perror("perror");
		return -1;
	}
	if(fcntl(fd,F_SETFL,old_fl|O_NONBLOCK))//重新设置文件描的标志，设置为非阻塞模式
	{
		perror("fcntl");
		return -1;
	}

	return 0;
}

//读取数据
ssize_t myread(int fd,char *buf,size_t size)
{
	assert(buf);
	int index=0;
	ssize_t ret=0;
//如果读取的数据为0，则远端关闭连接，直接返回0
//如果为非0，不管是大于0还是小于0，都要进入循环

	while(ret=read(fd,buf+index,size-index))
	{
		if((ret<0)&&(errno==EAGAIN))   //说明读取完毕，
		{
			printf("read errno:%d\n",errno);
			perror("read");
			break;
		}
		index += ret;
	}
	return (ssize_t)index;//返回读取的数据量
}

//写入数据
ssize_t mywrite(int fd,char *buf,size_t size)
{
	assert(buf);
	int index=0;
	ssize_t ret=0;
//如果写入的数据为0，则远端关闭连接，直接返回0
//如果为非0，不管是大于0还是小于0，都要进入循环

	while(ret=write(fd,buf+index,size-index))
	{
		if(errno==EAGAIN)//说明写入完毕，
		{
			printf("write errno:%d\n",errno);
			perror("write");
			break;
		}
		index += ret;
	}
	return (ssize_t)index;//返回写入的数据量
}


static void usage(const char* proc)  //命令行参数的格式判断
{
	printf("usage:%s[ip][port]\n",proc);
}

static int start(int port,char *ip)
{
	assert(ip);
	int sock=socket(AF_INET,SOCK_STREAM,0);
	if(sock<0)
	{
		perror("socket");
		exit(1);
	}
      //设置本地的网络地址信息
	struct sockaddr_in local;
	local.sin_port=htons(port);
	local.sin_family=AF_INET;
	local.sin_addr.s_addr=inet_addr(ip);

	int opt=1;  //调用这个函数使当server首先断开连接的时候避免进入一个TIME——WAIT的等待时间
	setsockopt(sock,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt));

	if(bind(sock,(struct sockaddr*)&local,sizeof(local))<0)
	{
		perror("bind");
		exit(2);
	}

	if(listen(sock,_BACKLOG_)<0)
	{
		perror("listen");
		exit(3);
	}
	return sock;
}


static int epoll_server(int listen_sock)
{
	int epoll_fd=epoll_create(256);//生成一个专用的epoll文件描述符
	if(epoll_fd<0)
	{
		perror("epoll_create");
		exit(1);
	}

	set_no_block(listen_sock);   // 设置sock为非阻塞

	struct epoll_event ev;//用于注册事件
	struct epoll_event ret_ev[_MAX_];//数组用于回传要处理的事件，用于存放就绪事件队列
	int ret_num=_MAX_;
	int read_num=-1;
	//ev.events=EPOLLIN;
	ev.events=EPOLLIN|EPOLLET;
	ev.data.fd=listen_sock;
	if(epoll_ctl(epoll_fd,EPOLL_CTL_ADD,listen_sock,&ev)<0)//用于控制某个文件描述符上的事件（注册，修改，删除）
	{
		perror("epoll_ctl");
		return -2;
	}

	int done=0;
	int i=0;
	int timeout=5000;//设定超时时间，-1则以阻塞方式一直等待

	struct sockaddr_in client;
	socklen_t len=sizeof(client);

	while(!done)
	{
		switch(read_num=epoll_wait(epoll_fd,ret_ev,ret_num,timeout))//用于轮寻I/O事件的发生
		{
			case 0://超时
				printf("time out\n");
				break;
			case -1://出错
				perror("epoll");
				exit(2);
			default://至少有一个事件就绪
				{
					for(i=0;i<read_num;++i)
					{
						if(ret_ev[i].data.fd==listen_sock&&(ret_ev[i].events&EPOLLIN))
						{
                            //判断是否是监听套接子，是的话获取连接请求
							int fd=ret_ev[i].data.fd;
							int new_sock=accept(fd,(struct sockaddr*)&client,&len);
							if(new_sock<0)
							{
								perror("accept");
							//	printf("%s :%d\n",)
								continue;
							}
							//set_no_block(new_sock);
							printf("get a new client...[fd]:%d [ip]:%s [port]:%d\n",listen_sock,inet_ntoa(client.sin_addr),ntohs(client.sin_port));

						//	ev.events=EPOLLIN;//将新的事件添加进epoll实例中
						    ev.events=EPOLLIN|EPOLLET;   
							ev.data.fd=new_sock;
							if(epoll_ctl(epoll_fd,EPOLL_CTL_ADD,new_sock,&ev)<0)
							{
								perror("epoll_wait");
								close(listen_sock);
							}
						}
						else  //normal sock
						{
							if(ret_ev[i].events&EPOLLIN)   //读事件就绪 
							{
								int fd=ret_ev[i].data.fd;
								data_buf_p mem=(data_buf_p)malloc(sizeof(data_buf_t));//申请空间用于同时存储文件描述符和和缓冲区地址
								if(!mem)
								{
									perror("malloc");
									continue;
								}
								mem->fd=fd;
								memset(mem->buf,'\0',sizeof(mem->buf));
							//	ssize_t _s=read(mem->fd,mem->buf,sizeof(mem -> buf)-1);
								ssize_t _s=myread(mem->fd,mem->buf,sizeof(mem->buf)-1);  //改为ET模式时，要不断的一次读完
								if(_s>0)//读取成功，输出数据
								{
									mem->buf[_s]='\0';
									printf("client: %s",mem->buf);
									fflush(stdout);

									ev.events=EPOLLOUT;//读取成功，然后关心写事件，进行回写
									ev.events=EPOLLOUT|EPOLLET;
									ev.data.ptr=mem;
									epoll_ctl(epoll_fd,EPOLL_CTL_MOD,fd,&ev);
								}
								else if(_s==0)//远端关闭连接
								{
									printf("client close...\n");
									epoll_ctl(epoll_fd,EPOLL_CTL_DEL,fd,NULL);
									close(fd);									
									free(mem);
								}
								else
								{
									printf("read error\n");
								}
							}
							else if(ret_ev[i].events&EPOLLOUT)  //写事件就绪
							{
  							//	char* msg="http/1.0 200 ok\r\n\r\nhello:bit\r\n";
									data_buf_p mem=(data_buf_p)ret_ev[i].data.ptr;
									int fd=mem->fd;
									char *buf=mem->buf;
								//	write(fd,buf,strlen(buf));
								//	write(fd,msg,strlen(msg));
								    mywrite(fd,mem->buf,sizeof(mem->buf)-1);
								
								//	epoll_ctl(epoll_fd,EPOLL_CTL_DEL,fd,NULL);//写完之后就完成一次通信，结束
								//	close(fd);
								//	free(mem);

									ev.events=EPOLLIN;     //这四行用于不断的读
									ev.data.fd=fd;
									epoll_ctl(epoll_fd,EPOLL_CTL_MOD,fd,&ev);
							}
							else{
								//....
							}
						}
					}
				}
				break;
		}
	}

}

int main(int argc,char* argv[])
{
	if(argc!=3)  //判断命令行参数正确性
	{
		usage(argv[0]);
		return 1;
	}

	//获取IP地址和端口号
	int port=atoi(argv[2]);
	char *ip=argv[1];

	//获取监听套接字
	int listen_sock=start(port,ip);

	epoll_server(listen_sock);
	close(listen_sock);//关闭文件描述符
	return 0;
}
