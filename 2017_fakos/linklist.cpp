#include <utility>

template< typename T >
struct LinkList {

    ~LinkList() {
        while ( begin ) {
            auto n = begin->next;
            delete begin;
            begin = n;
        }
    }

    struct Node {
        template< typename TT >
        Node( TT &val ) : val( std::forward< TT >( val ) ) { }

        T val;
        Node *prev = nullptr;
        Node *next = nullptr;
    };

    void push_back( const T &v ) {
        if ( end ) {
            end->next = new Node( v );
            end->next->prev = end;
            end = end->next;
        } else {
            begin = end = new Node( v );
        }
    }

    void push_front( const T &v ) {
        if ( begin ) {
            begin->prev = new Node( v );
            begin->prev->next = begin;
            begin = begin->prev;
        } else {
            begin = end = new Node( v );
        }
    }

    Node *begin = nullptr;
    Node *end = nullptr;
};

int main() {
    LinkList< int > ll;
    ll.push_front( 1 );
    ll.push_front( 2 );
    ll.push_back( 3 );
    ll.push_back( 4 );
    ll.push_front( 5 );
    return 0;
}
