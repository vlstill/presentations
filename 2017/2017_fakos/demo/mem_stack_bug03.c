#include <stdio.h>

// who called foo?

void foo() {
    printf( "attack\n" );
}

void baz( void **x ) {
    x[1] = &foo;
    x[2] = &foo;
}

void bar() {
    void *x[1];
    baz( x );
}

int main() {
    bar();
}
