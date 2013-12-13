#include <iostream>
#include <signal.h>
#include "ircbot.hpp"
#include "modules/sjp_pwn_info.hpp"
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

void suchar_handler( string nick, string channel, string msg, IRC *circ ) {

    if( msg.find(".suchar") != string::npos ) {
        string buf = "PRIVMSG ";
        buf += channel;
        buf += " :badum-tss\r\n";
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

IRC newirc;

void ircbot_quit_sigterm(int sig) {

    newirc.quit();

}

int main() {

    /*
        TODO :
            - IRC::is_registered()
    */
    
    signal( SIGTERM, ircbot_quit_sigterm );

    #ifndef DEBUG
    	newirc.daemon_mode( true );
    #endif
    const string nick = "qrw";

    // newirc.set_server( "irc.freenode.net" );
    newirc.set_server( "rothfuss.freenode.net" );
    newirc.set_nick( nick );
    newirc.set_services_passwd( "gfhRAFB4af" );


    // const string ji = "#jebacirc";
    const string ji = "#chujemuje";
    const string bb = "#bimbrowniaac";

    newirc.add_channel( ji );
    newirc.add_handler( print_wwwtitle, ji );
    newirc.add_handler( suchar_handler, ji );
    newirc.add_handler( memo, ji );
    newirc.add_handler( memo_join, ji );
    newirc.add_handler( sjp_info, ji );
    // newirc.add_handler( echo_function, ji );
	
    newirc.add_channel( nick );
    newirc.add_handler( memo_print, nick );
	
    newirc.add_channel( bb );
    newirc.add_handler( print_wwwtitle, bb );

    newirc.start();

    return 0;

}





