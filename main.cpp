#include "ircbot.hpp"
#include "BotFunctor.hpp"
#include "modules/PrintWWWTitle.hpp"
#include "modules/UserMemo.hpp"
#include <cstdio>
#include <iostream>
#include <stdexcept>

class EchoFunctor : public BotFunctor {

	public:

		void operator() ( string msg, IRCBot *circ ) {

			std::cout << "EchoFunctor start.\n";

			try {

				circ->parser.parse( msg );

				if( circ->parser.get_command() == "PRIVMSG" ) {

					std::cout << "\tCommand: " << circ->parser.get_command() << std::endl <<
								 "\tChannel: " << circ->parser.get_param(0) << std::endl <<
								 "\tMessage: " << circ->parser.get_param(1) << std::endl;
					circ->send_pm( circ->parser.get_param(0), circ->parser.get_param(1) );

					if( circ->parser.get_param(1) == "quit" ) {
						circ->quit();
					}

				}

			} catch( std::exception &e ) {
				printf( "EchoFunctor error: %s\n", e.what() );
			}

			std::cout << "EchoFunctor end.\n";

		}

};

/*
	TODO:
		- UTF-8 support
		- logging class
*/
int main() {

	IRCBot irc_bot;

	try {

		irc_bot.set_server( "holmes.freenode.net" );
		irc_bot.set_nick( "test94384" );

		irc_bot.add_channel( "#chujemuje" );
		irc_bot.register_handler( "#chujemuje", new EchoFunctor );
		irc_bot.register_handler( "#chujemuje", new UserMemo );

		irc_bot.add_channel( "#mujechuje" );
		irc_bot.register_handler( "#mujechuje", new PrintWWWTitle );

		irc_bot.start();

	} catch( std::exception &e ) {

		printf( "Error: %s\n", e.what() );

	}

	return 0;

}
