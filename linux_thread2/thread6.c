#include <stdio.h>  
#include <stdlib.h>  
#include <errno.h>  
#include <unistd.h>  
#include <pthread.h>  
pthread_mutex_t g_mutex;  
int g_lock_var = 0;  
void* thread1( void *arg )  
{  
    int i, ret;  
    time_t end_time;  
    end_time = time(NULL) + 10;  
    while( time(NULL) < end_time ) {  
        ret = pthread_mutex_trylock( &g_mutex );  
        if( EBUSY == ret ) {  
            printf( "thread1: the varible is locked by thread2.\n" );  
        } else {  
            printf( "thread1: lock the variable!\n" );  
            ++g_lock_var;  
            pthread_mutex_unlock( &g_mutex );  
        }  
        sleep(1);  
    }  
    return NULL;  
}  
void* thread2( void *arg )  
{  
    int i;  
    time_t end_time;  
    end_time = time(NULL) + 10;  
    while( time(NULL) < end_time ) {  
        pthread_mutex_lock( &g_mutex );  
        printf( "thread2: lock the variable!\n" );  
        ++g_lock_var;  
        sleep(1);  
        pthread_mutex_unlock( &g_mutex );  
    }  
    return NULL;  
}  
int main( int argc, char *argv[] )  
{  
    int i;  
    pthread_t pth1,pth2;  
    pthread_mutex_init( &g_mutex, NULL );  
    pthread_create( &pth1, NULL, thread1, NULL );  
    pthread_create( &pth2, NULL, thread2, NULL );  
    pthread_join( pth1, NULL );  
    pthread_join( pth2, NULL );  
    pthread_mutex_destroy( &g_mutex );  
    printf( "g_lock_var = %d\n", g_lock_var );  
    return 0;                              
}  
