#ifndef BOTFUNCTOR_HPP
#define BOTFUNCTOR_HPP

#include "ircbot.hpp"
#include <string>

using namespace std;

class IRC;

class BotFunctor {

	public:

		enum handler_types {
			HDLR_PM = 1, HDLR_JOIN = 2, HDLR_STR = 4
		};

		BotFunctor( BotFunctor::handler_types new_handler_type );
		virtual ~BotFunctor();

		int get_handler_type();

		virtual void operator() ( string nick, string channel, string msg, IRC *circ );
		virtual void operator() ( string nick, string channel, IRC *circ );
		virtual void operator() ( string recvd_line, IRC *circ );

		friend BotFunctor::handler_types operator|( BotFunctor::handler_types ht1, BotFunctor::handler_types ht2 );

	private:

		int handler_type;

};

#endif // BOTFUNCTOR_HPP
