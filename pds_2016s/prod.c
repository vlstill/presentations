#include <pthread.h>

pthread_mutex_t m;

int c = 0;

void *prod( void *b ) {
    int tmp = *((int*)b);
    pthread_mutex_lock( &m );
    if ( c > 0 )
        c++;
    else {
        c = 0;
        while( tmp > 0 ) {
            c++;
            tmp--;
        }
    }
    pthread_mutex_unlock( &m );
    return 0;
}

int main() {
    pthread_t t;
    pthread_mutex_init( &m, 0 );
    pthread_create( &t, NULL, prod, NULL );
    pthread_join( t, NULL );
}
