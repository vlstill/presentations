#include <stdlib.h>
#include <stdio.h>

int main() {
    puts( "calling malloc" );
    int *array = malloc( 32 );
    printf( "malloc returned %p\n", array );
    if ( array == 0 ) {
        puts( "malloc failed, try re-running with -o nofail:malloc" );
    }
    array[ 4 ] = 16;
}
