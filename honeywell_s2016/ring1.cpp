// divine-cflags: -std=c++11

#include <cassert>
#include <atomic>
#include <pthread.h>
#include <cstdlib>

template< typename T, size_t size_ = 4 >
struct Ring {
    static constexpr size_t size = size_;

    void enqueue( T x ) {
        while ( std::abs( read - write ) >= long( size ) ) { }
        data[ write % size ] = x;
        write = (write + 1) % (2 * size);
    }

    bool empty() const { return read == write; }

    T dequeue() {
        while ( empty() ) { }
        T x = data[ read % size ];
        read = (read + 1) % (2 * size);
        return x;
    }

  private:
    long read{ 0 };
    long write{ 0 };
    T data[ size ];
};

Ring< int > queue;

int main() {

    pthread_t writer;
    pthread_create( &writer, nullptr, [] ( void * ) -> void * {
            for ( int i = 1; i <= 10; ++i )
                queue.enqueue( i );
            return nullptr;
        }, nullptr );
    for ( int i = 1; i <= 10; ++i ) {
        int x = queue.dequeue();
        assert( x == i );
    }
    pthread_join( writer, nullptr );
    assert( queue.empty() );
};
