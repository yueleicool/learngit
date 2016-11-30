#include <stdio.h>  
#include <unistd.h>  
#include <stdlib.h>  
#include <pthread.h>  
#define THREAD_COUNT 12  
void show_thread_policy( int threadno )  
{  
    int policy;  
    struct sched_param param;  
    pthread_getschedparam( pthread_self(), &policy, &param );  
    switch( policy ){  
    case SCHED_OTHER:  
        printf( "SCHED_OTHER %d\n", threadno );  
        break;  
    case SCHED_RR:  
        printf( "SCHDE_RR %d\n", threadno );  
        break;  
    case SCHED_FIFO:  
        printf( "SCHED_FIFO %d\n", threadno );  
        break;  
    default:  
        printf( "UNKNOWN\n");  
    }  
}  
void* thread( void *arg )  
{  
    int i, j;  
    long threadno = (long)arg;  
    printf( "thread %d start\n", threadno );  
    sleep(1);  
    show_thread_policy( threadno );  
    for( i = 0; i < 10; ++i ) {  
        for( j = 0; j < 100000000; ++j ){}  
        printf( "thread %d\n", threadno );  
    }  
    printf( "thread %d exit\n", threadno );  
    return NULL;  
}  
int main( int argc, char *argv[] )  
{  
    long i;  
    pthread_attr_t attr[THREAD_COUNT];  
    pthread_t pth[THREAD_COUNT];  
    struct sched_param param;  
    for( i = 0; i < THREAD_COUNT; ++i )  
        pthread_attr_init( &attr[i] );  
        for( i = 0; i < THREAD_COUNT / 2; ++i ) {  
            param.sched_priority = 10;                    
            pthread_attr_setschedpolicy( &attr[i], SCHED_FIFO );  
            pthread_attr_setschedparam( &attr[i], &param );  
            pthread_attr_setinheritsched( &attr[i], PTHREAD_EXPLICIT_SCHED );  
        }  
        for( i = THREAD_COUNT / 2; i < THREAD_COUNT; ++i ) {  
            param.sched_priority = 20;                    
            pthread_attr_setschedpolicy( &attr[i], SCHED_FIFO );  
            pthread_attr_setschedparam( &attr[i], &param );  
            pthread_attr_setinheritsched( &attr[i], PTHREAD_EXPLICIT_SCHED );  
        }  
        for( i = 0; i < THREAD_COUNT; ++i )                      
            pthread_create( &pth[i], &attr[i], thread, (void*)i );                
        for( i = 0; i < THREAD_COUNT; ++i )                      
            pthread_join( pth[i], NULL );                      
        for( i = 0; i < THREAD_COUNT; ++i )                      
            pthread_attr_destroy( &attr[i] );                     
    return 0;                             
} 
