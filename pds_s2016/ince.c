#include <pthread.h>
#include <assert.h>

int x;

void *thread( void *_ ) {
    x = x + 1;
    return 0;
}

int main() {
    pthread_t t;
    pthread_create( &t, 0, thread, 0 );
    x = x + 1;
    pthread_join( t, 0 );
    assert( x == 2 );
}
