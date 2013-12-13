#include <iostream>
#include "ircbot.hpp"
#include "modules/print_wwwtitle.hpp"
#include "modules/memo.hpp"

void print_info( string nick, string channel, string msg, IRC *circ ) {

        string buf = "PRIVMSG ";
        buf += channel;
        buf += " :Nick: -";
        buf += nick;
        buf += "- Chan: -";
        buf += channel;
        buf += "-";
        buf += " Msg: -";
        buf += msg;
        buf += "-";
        buf += "\r\n";
        circ->send_d( buf );

}

void echo_function( string nick, string channel, string msg, IRC *circ ) {

    if( msg.find("echo") != string::npos ) {
        string buf = "PRIVMSG ";
        buf += channel;
        buf += " :echo echo echo\r\n";
        circ->send_d( buf );
    }

}

void jh_test( string nick, string channel, IRC *circ ) {

    string buf = "PRIVMSG #chujemuje :nick -";
    buf += nick;
    buf += "- wlaz na kanal -";
    buf += channel;
    buf += "-\r\n";
    circ->send_d( buf );

}

void q2_handler( string nick, string channel, string msg, IRC *circ ) {

    if( nick == "widmo" ) { circ->quit(); }

}

void str_htest( string recvd, IRC *circ ) {

    string s = "PRIVMSG #chujemuje :";
    s += recvd;
    s += "\r\n";
    circ->send_d( s );

}

int main() {

    /*
        TODO :
            - IRC::is_registered()
    */

    IRC newirc;

    // newirc.daemon_mode( true );
    const string nick = "qrw2";

    newirc.set_server( "irc.freenode.net" );
    newirc.set_nick( nick );
    // newirc.set_services_passwd( "gfhRAFB4af" );

    /*
    newirc.add_channel( "#jebacirc" );
    newirc.add_pm_handler( print_wwwtitle, "#jebacirc" );

    newirc.add_channel( nick );

    newirc.add_pm_handler( memo, "#jebacirc" );
    newirc.add_join_handler( memo_join, "#jebacirc" );
    */

    // TEST STUFF
    newirc.add_channel( "#chujemuje2" );
    newirc.add_handler( print_wwwtitle, "#chujemuje2" );

    newirc.add_channel( "#chujemuje" );

    newirc.add_handler( echo_function, "#chujemuje" );
    // newirc.add_handler( q2_handler, "#chujemuje" );
    newirc.add_handler( print_info, "#chujemuje" );
    newirc.add_handler( print_wwwtitle, "#chujemuje" );
    newirc.add_handler( memo, "#chujemuje" );

    newirc.add_handler( jh_test, "#chujemuje" );
    newirc.add_handler( str_htest, "str_start", "str_stop", 4 );
    newirc.add_handler( memo_join, "#chujemuje" );

    newirc.add_channel( nick );
    newirc.add_handler( memo_print, nick ); // query

    newirc.start();

    return 0;

}
