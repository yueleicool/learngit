#include<fcntl.h>
#include<sys/stat.h>
#include<stdio.h>
#include<sys/types.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<assert.h>
void read_write(int fd);
void print2file(int fd);
void print2file1(int fd);
int main(int argc,char **argv)  
{  
    // 打开文件  
    int fd = open("data.dat",O_CREAT|O_RDWR|O_TRUNC,S_IRUSR|S_IWUSR);  
    assert(fd>0);  
  
    // 通过新fd写文件  
    //read_write(fd);  
      
    // dup实现标准输入重定向到文件   
    print2file(fd);  
  
    // dup2实现标准输入重定向到文件  
    //print2file1(fd);  
      
    return 0;  
}  
  
void read_write(int fd)  
{  
    // 拷贝fd,返回当前系统最小且没有被使用的fd  
    int nfd = dup(fd);  
    printf("old:%d new:%d\n",fd,nfd); // 3 4  
  
    char buf[1024];  
    memset(buf,'\0',1024);  
  
    int n;  
  
    // 读取控制台输入流，并通过nfd写入文件  
    while((n = read(STDIN_FILENO,buf,1024))>0)  
    {  
        write(nfd,buf,n);  
    }  
}  
  
void print2file(int fd)  
{  
    close(STDOUT_FILENO);  
  
    /** 
     * 关闭标准输出流，调用dup()后返回的系统最小可用fd,此时nfd=1。此时任何目标为STDOUT_FILENO的I/O操作，如printf()等数据都会流入fd对应的文件，太神奇了。 
     * 如果fd为tcp套接字描述符，则会被发送到与客户端连接的socket上，这就是CGI的实现原理。这就能解释CGI程序中大量的printf()语句 
     */  
    int nfd = dup(fd);  
  
    // 这两条打印语句会被写入data.dat文件中  
    printf("old:%d new:%d\n",fd,nfd);  
    printf("hello world\n");  
}  
  
void print2file1(int fd)  
{  
    /** 
     * dup2可以指定拷贝后的newfd,原先的newfd会被关闭 
     * dup2(fd,STDOUT_FILENO); 相当于 cose(STDOUT_FILENO); dup(fd);  
     */  
    int nfd = dup2(fd,STDOUT_FILENO);  
    printf("old:%d new:%d\n",fd,nfd);  
    printf("dup2 test\n");  
} 
