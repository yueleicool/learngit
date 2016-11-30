#include <stdio.h>  
#include <stdlib.h>  
#include <pthread.h>  
#define BUFFER_SIZE 5  
pthread_mutex_t g_mutex;  
pthread_cond_t g_cond;  
typedef struct {  
    char buf[BUFFER_SIZE];  
    int count;  
} buffer_t;  
buffer_t g_share = {"", 0};  
char g_ch = 'A';  
void* producer( void *arg )  
{  
    printf( "Producer starting.\n" );  
    while( g_ch != 'Z' ) {  
        pthread_mutex_lock( &g_mutex );  
        if( g_share.count < BUFFER_SIZE ) {  
            g_share.buf[g_share.count++] = g_ch++;  
            printf( "Prodcuer got char[%c]\n", g_ch - 1 );  
            if( BUFFER_SIZE == g_share.count ) {  
                printf( "Producer signaling full.\n" );  
                pthread_cond_signal( &g_cond );  
            }  
        }  
        pthread_mutex_unlock( &g_mutex );  
    }  
    printf( "Producer exit.\n" );  
    return NULL;  
}  
void* consumer( void *arg )  
{  
    int i;  
    printf( "Consumer starting.\n" );  
    while( g_ch != 'Z' ) {  
        pthread_mutex_lock( &g_mutex );  
        printf( "Consumer waiting\n" );  
        pthread_cond_wait( &g_cond, &g_mutex );  
        printf( "Consumer writing buffer\n" );  
        for( i = 0; g_share.buf[i] && g_share.count; ++i ) {  
            putchar( g_share.buf[i] );  
            --g_share.count;  
        }  
        putchar('\n');  
        pthread_mutex_unlock( &g_mutex );  
    }  
    printf( "Consumer exit.\n" );  
    return NULL;  
}  
int main( int argc, char *argv[] )  
{  
    pthread_t ppth, cpth;  
    pthread_mutex_init( &g_mutex, NULL );  
    pthread_cond_init( &g_cond, NULL );  
    pthread_create( &cpth, NULL, consumer, NULL );  
    pthread_create( &ppth, NULL, producer, NULL );  
    pthread_join( ppth, NULL );  
    pthread_join( cpth, NULL );  
    pthread_mutex_destroy( &g_mutex );  
    pthread_cond_destroy( &g_cond );  
    return 0;  
}  
