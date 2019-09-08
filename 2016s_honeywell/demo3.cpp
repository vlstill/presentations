// divine-cflags: -std=c++11

#include <cassert>
#include <atomic>
#include <pthread.h>

template< typename T, size_t size_ = 4 >
struct Ring {
    static constexpr size_t size = size_;

    void enqueue( T x ) {
        while ( cnt == size ) { }
        data[ write ] = x;
        cnt.fetch_add( 1, std::memory_order_relaxed );
        write = (write + 1) % size;
    }

    bool empty() const { return !cnt; }

    T dequeue() {
        while ( empty() ) { }
        T x = data[ read ];
        cnt.fetch_sub( 1, std::memory_order_relaxed );
        read = (read + 1) % size;
        return x;
    }

  private:
    size_t read{ 0 };
    size_t write{ 0 };
    std::atomic< size_t > cnt{ 0 };
    T data[ size ];
};

Ring< int > queue;

int main() {

    pthread_t writer;
    pthread_create( &writer, nullptr, [] ( void * ) -> void * {
            for ( int i = 0; i < 10; ++i )
                queue.enqueue( i );
            return nullptr;
        }, nullptr );
    for ( int i = 0; i < 10; ++i ) {
        int x = queue.dequeue();
        assert( x == i );
    }
    pthread_join( writer, nullptr );
    assert( queue.empty() );
};
