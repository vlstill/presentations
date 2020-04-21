#include <future>
#include <cassert>

// divine check -std=c++14 --relaxed-memory tso FILE

volatile int x, y;
volatile int rx, ry;

void thread1() {
    y = 1;
    rx = x;
}

void thread2() {
    x = 1;
    ry = y;
}

int main() {
    std::thread t1( thread1 );
    thread2();
    t1.join();
    assert( rx == 1 || ry == 1 );
}

