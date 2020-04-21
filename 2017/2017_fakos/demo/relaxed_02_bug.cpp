#include <thread>
#include <cassert>

// divine check -std=c++14 --relaxed-memory pso FILE

volatile int x;
volatile bool flag;

void thread() {
    x = 42;
    flag = true;
}

int main() {
    assert( x == 0 ); // globals are initialized
    std::thread t1( thread );
    if ( flag )
        assert( x == 42 );
    t1.join();
    assert( flag );
    assert( x == 42 );
}

