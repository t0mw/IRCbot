#include <iostream>
#include "../ircbot.hpp"
#include "www_stuff.hpp"

using namespace std;

void print_wwwtitle( string nick, string channel, string msg, IRC *circ ) {

    if( msg.find( "http" ) != string::npos  || msg.find( "www" ) != string::npos ) {

        string link = get_link( msg );
        if( link.empty() ) {
            #ifdef DEBUG
                cout << "Got empty link.\n";
            #endif
            return;
        }

        string title = get_title( link );
        if( title.empty() ) {
            #ifdef DEBUG
                cout << "Got empty title for link: " << link << "\n";
            #endif
            return;
        }

        int delete_pos = 0;
        while( ( delete_pos = title.find("&quot;") ) >= 0 ) title.erase( delete_pos, 6 );
        while( ( delete_pos = title.find("&#39;") ) >= 0 ) title.erase( delete_pos, 5 );

        string buf = "PRIVMSG ";
        buf += channel;
        buf += " :";
        buf += title;
        buf += "\r\n";
        circ->send_raw( buf );
    }

}
