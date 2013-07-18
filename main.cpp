#include <iostream>
#include "ircbot.hpp"
#include "modules/print_wwwtitle.hpp"
#include "modules/memo.cpp"

void pmhandler_test( string nick, string channel, string msg, IRC *circ ) {

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

void joinhandler_test( string nick, string channel, IRC *circ ) {

    string buf = "PRIVMSG ";
	buf += channel;
	buf += " :nick -";
    buf += nick;
    buf += "- joined -";
    buf += channel;
    buf += "-\r\n";
    circ->send_d( buf );

}

void strhandler_test( string recvd, IRC *circ ) {

    string s = "PRIVMSG #channel_2 :";
    s += recvd;
    s += "\r\n";
    circ->send_d( s );

}

void bot_query( string nick, string channel, string msg, IRC *circ ) {

	if( nick == "admin" ) {
		string s = "PRIVMSG admin :hi\r\n";
		send_d( s );
	}

}

int main() {

    /*
        TODO :
            - IRC::is_registered()
    */

    IRC newirc;

    // newirc.daemon_mode( true );
	
    const string nick = "bot_nick";

    newirc.set_server( "irc.theircserver.com" );
    newirc.set_nick( nick );
    // newirc.set_services_passwd( "passwd" );

    newirc.add_channel( "#channel" );
    newirc.add_handler( print_wwwtitle, "#channel" );
	newirc.add_handler( memo, "#channel" );
	newirc.add_handler( memo_join, "#channel" );

    newirc.add_channel( "#channel_2" );
    newirc.add_handler( pmhandler_test, "#channel_2" );
    newirc.add_handler( joinhandler_test, "#channel_2" );
	
    newirc.add_handler( strhandler_test, "str_start", "str_stop", 10 );

    newirc.add_channel( nick ); // bot query
    newirc.add_handler( bot_query, nick );

    newirc.start();

    return 0;

}
