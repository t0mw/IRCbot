#include <iostream>
#include <pthread.h>
#include <cstdio>
#include "ircbot.hpp"

void *task( void *args ) {

    ((IRC *)args)->start();

}

int main() {

    IRC *tab[10];
    pthread_t threads[10];
    for( int i = 0; i < 10; ++i ) {
        tab[i] = new IRC;
        char nick[20] = { 0 };
        sprintf( nick, "jeste_nr_%d", i );
        tab[i]->set_nick( nick );
        tab[i]->set_server("irc.freenode.net");
        tab[i]->add_channel("#chujemuje");
        pthread_create( threads+i, NULL, task, tab[i] );
    }
    
    while( getchar() != 'Q' );
    
    for( int i = 0; i < 10; ++i ) {
        delete tab[i];
    }

    return 0;

}
