#include <pthread.h>  
#include <stdio.h>  
#include <stdlib.h>  
  
void *thread_function(void *arg);  
  
int main()  
{  
    int res;  
    pthread_t a_thread;  
    void *thread_result;  
  
    res = pthread_create(&a_thread, NULL, thread_function, NULL);  
    if (res != 0)  
    {  
        perror("Thread create failed!");  
        exit(EXIT_FAILURE);  
    }  
  
    sleep(4);  
    printf("Canceling thread...\n");  
  
    res = pthread_cancel(a_thread);  
    if (res != 0)  
    {  
        perror("Thread cancel failed!");  
        exit(EXIT_FAILURE);  
    }  
  
    printf ("Waiting for thread to finished...\n");  
      
    res = pthread_join(a_thread, &thread_result);  
    if (res != 0)  
    {  
        perror ("Thread join failed!");  
        exit(EXIT_FAILURE);  
    }  
  
    printf("Thread canceled!");  
  
    exit(EXIT_FAILURE);  
}  
  
void *thread_function(void *arg)  
{  
    int i;  
    int res;  
  
    res = pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);  
    if (res != 0)  
    {  
        perror("Thread setcancelstate failed!");  
        exit(EXIT_FAILURE);  
    }  
  
    res = pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, NULL);  
    if (res != 0)  
    {  
        perror("Thread setcanceltype failed!");  
        exit(EXIT_FAILURE);  
    }  
  
    printf("thread_function is running...\n");  
  
    for (i = 0; i < 10; i++)  
    {  
        printf("Thread is still running (%d)...\n", i);  
        sleep(1);  
    }  
    pthread_exit(0);  
}  
