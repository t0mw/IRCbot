#include "ircbot.hpp"
#include "BotFunctor.hpp"
#include "modules/sjp_pwn_info.hpp"
#include "modules/print_wwwtitle.hpp"
#include "modules/memo.hpp"
#include <cstdio>
#include <iostream>
#include <signal.h>

class EchoFunctor : public BotFunctor {

	public:

		EchoFunctor( BotFunctor::handler_types type ) : BotFunctor( type ) {

		}

		void operator() ( string nick, string channel, string msg, IRC *circ ) {
			if( msg.find( "echo" ) != string::npos ) {
				circ->send_pm( channel, msg );
			}
		}

		void operator() ( string nick, string channel, IRC *circ ) {
			string resp = nick;
			resp += " wlaz na kanal";
			circ->send_pm( channel, resp );
		}

		void operator() ( string msg, IRC *circ ) {
			if( msg.find( "widmo" ) != string::npos ) {
				circ->send_pm( "widmo", msg );
			}
		}

};

/*
	TODO:
		- string functors
		- enum - operator `|`
		- IRC messages scalable parser class
		- UTF-8 support
*/

int main() {

	IRC newirc;

	newirc.set_server( "holmes.freenode.net" );
	newirc.set_nick( "test94384" );

	newirc.add_channel( "#chujemuje" );
	newirc.register_handler( "#chujemuje", new EchoFunctor( BotFunctor::HDLR_PM | BotFunctor::HDLR_JOIN | BotFunctor::HDLR_STR ) );

	newirc.add_channel( "#mujechuje" );

	try {

		newirc.start();

	} catch( std::exception &e ) {

		printf( "Error: %s\n", e.what() );

	}

	return 0;

}





