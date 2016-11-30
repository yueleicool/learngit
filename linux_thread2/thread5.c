#include <stdio.h>  
#include <stdlib.h>  
#include <pthread.h>  
#define THREAD_COUNT 10  
pthread_key_t g_key;  
typedef struct thread_data{  
    int thread_no;  
} thread_data_t;  
void show_thread_data()  
{  
    thread_data_t *data = pthread_getspecific( g_key );  
    printf( "Thread %d \n", data->thread_no );  
}  
void* thread( void *arg )  
{  
    thread_data_t *data = (thread_data_t *)arg;  
    printf( "Start thread %d\n", data->thread_no );  
    pthread_setspecific( g_key, data );  
    show_thread_data();  
    printf( "Thread %d exit\n", data->thread_no );  
}  
void free_thread_data( void *arg )  
{  
    thread_data_t *data = (thread_data_t*)arg;  
    printf( "Free thread %d data\n", data->thread_no );  
    free( data );  
}  
int main( int argc, char *argv[] )  
{  
    int i;  
    pthread_t pth[THREAD_COUNT];  
    thread_data_t *data = NULL;  
    pthread_key_create( &g_key, free_thread_data );  
    for( i = 0; i < THREAD_COUNT; ++i ) {  
        data = malloc( sizeof( thread_data_t ) );  
        data->thread_no = i;  
        pthread_create( &pth[i], NULL, thread, data );  
    }  
    for( i = 0; i < THREAD_COUNT; ++i )  
        pthread_join( pth[i], NULL );  
    pthread_key_delete( g_key );  
    return 0;  
}  
