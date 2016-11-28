#include  <unistd.h>
#include  <sys/types.h>       /* basic system data types */
#include  <sys/socket.h>      /* basic socket definitions */
#include  <netinet/in.h>      /* sockaddr_in{} and other Internet defns */
#include  <arpa/inet.h>       /* inet(3) functions */

#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>

#define MAXLINE 1024
//typedef struct sockaddr  SA;
void handle(int connfd);

int  main(int argc, char **argv)
{
    int     listenfd, connfd;
    int  serverPort = 6888;
    int listenq = 1024;
    pid_t   childpid;
    char buf[MAXLINE];
    socklen_t socklen;

    struct sockaddr_in cliaddr, servaddr;
    socklen = sizeof(cliaddr);

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(serverPort);

    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if (listenfd < 0) {
        perror("socket error");
        return -1;
    }
    if (bind(listenfd, (struct sockaddr *) &servaddr, socklen) < 0) {
        perror("bind error");
        return -1;
    }
    if (listen(listenfd, listenq) < 0) {
        perror("listen error");    
        return -1;
    }
    printf("echo server startup,listen on port:%d\n", serverPort);
    for ( ; ; )  {
        connfd = accept(listenfd, (struct sockaddr *)&cliaddr, &socklen);
        if (connfd < 0) {
            perror("accept error");
            continue;
        }

        sprintf(buf, "accept form %s:%d\n", inet_ntoa(cliaddr.sin_addr), cliaddr.sin_port);
        printf(buf,"");
        childpid = fork();
        if (childpid == 0) { /* child process */
            close(listenfd);    /* close listening socket */
            handle(connfd);   /* process the request */
            exit (0);
        } else if (childpid > 0)  {
            close(connfd);          /* parent closes connected socket */
        } else {
            perror("fork error");
        }
    }
}


void handle(int connfd)
{
    size_t n;
    char    buf[MAXLINE];

    for(;;) {
        n = read(connfd, buf, MAXLINE);
        if (n < 0) {
            if(errno != EINTR) {
                perror("read error");
                break;
            }
        }
        if (n == 0) {
            //connfd is closed by client
            close(connfd);
            printf("client exit\n");
            break;
        }
        //client exit
        if (strncmp("exit", buf, 4) == 0) {
            close(connfd);
            printf("client exit\n");
            break;
        }
        write(connfd, buf, n); //write maybe fail,here don't process failed error
    } 
} 
