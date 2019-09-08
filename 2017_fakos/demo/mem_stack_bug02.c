#include <stdio.h>

// something spooky on the stack

int foo( int *array ) {
    return array[0] + array[1];
}

int main() {
    int x = 1;
    int y = 2;
    int z = 3;
    printf( "foo returned %d\n", foo( &y ) );
}
