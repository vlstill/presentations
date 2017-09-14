#include <thread>
#include <cassert>

// divine verify -std=c++14 -o nofail:malloc FILE

volatile int x;

void thread() {
    ++x;
}

int main() {
    assert( x == 0 ); // globals are initialized
    std::thread t1( thread ),
                t2( thread );
    assert( x <= 2 );
    assert( x >= 0 );
    t1.join();
    t2.join();
    assert( x == 2 );
}
