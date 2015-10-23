#include <pthread.h>
#include <assert.h>

int x = 0, y = 0;
int read_x, read_y;

void *worker0( void *nothing ) {
    x = 1;
    read_y = y;
    return NULL;
}

void *worker1( void *nothing ) {
    y = 1;
    read_x = x;
    return NULL;
}

int main() {
    pthread_t t0, t1;
    pthread_create( &t0, NULL, &worker0, NULL );
    pthread_create( &t1, NULL, &worker1, NULL );
    pthread_join( t0, NULL );
    pthread_join( t1, NULL );
    assert( read_x == 1 || read_y == 1 );
}
