// divine-cflags: -std=c++11
// -*- C++ -*- (c) 2015 Vladimír Štill <xstill@fi.muni.cz>

#include <atomic>
#include <cassert>
#include <pthread.h>

bool flag[2];
int turn;

std::atomic< bool > critical;

constexpr int other( int x ) { return x == 0 ? 1 : 0; }

template< int tid >
void *worker( void * = nullptr ) {
    while ( true ) {
        flag[ tid ] = true;
        turn = other( tid );
        while ( flag[ other( tid ) ] && turn == other( tid ) ) { }
        // critical start
        assert( !critical );
        critical = true;
        assert( critical );
        critical = false;
        // end
        flag[ tid ] = false;
    }
    return nullptr;
}

int main() {
    pthread_t t1, t2;
    pthread_create( &t1, nullptr, &worker< 0 >, nullptr );
    pthread_create( &t2, nullptr, &worker< 1 >, nullptr );
    pthread_join( t1, nullptr );
    return 0;
}
