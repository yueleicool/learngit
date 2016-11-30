#include <pthread.h>  
#include <stdio.h>  
#include <stdlib.h>  
  
#define NUM 6  
  
void *thread_function(void *arg);  
  
int main()  
{  
    int res;  
    pthread_t a_thread[NUM];  
    void *thread_result;  
    long index;  
  
    for (index = 0; index < NUM; ++index) {  
        res = pthread_create(&a_thread[index], NULL, thread_function, (void *)index);  
        if (res != 0)  
        {  
            perror("Thread create failed!");  
            exit(EXIT_FAILURE);  
        }  
        sleep(1);  
    }  
  
    printf("Waiting for threads to finished...\n");  
  
    for (index = NUM - 1; index >= 0; --index)  
    {  
        res = pthread_join(a_thread[index], &thread_result);  
        if (res == 0)  
        {  
            printf("Picked up a thread:%d\n", index + 1);  
        }  
        else  
        {  
            perror("pthread_join failed\n");  
        }  
    }  
  
    printf("All done\n");  
      
    exit(EXIT_SUCCESS);  
}  
  
void *thread_function(void *arg)  
{  
    long my_number = (long)arg;  
    int rand_num;  
  
    printf("thread_function is running. Argument was %d\n", my_number);  
    rand_num = 1 + (int)(9.0 * rand()/(RAND_MAX + 1.0));  
    sleep(rand_num);  
    printf("Bye from %d\n", my_number);  
    pthread_exit(NULL);  
}  
