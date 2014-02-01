#ifndef __BOTFUNCTOR_HPP__
#define __BOTFUNCTOR_HPP__

#include "IRCBot.hpp"
#include <string>

using namespace std;

class IRCBot;

class BotFunctor {

	public:

		BotFunctor();
		virtual ~BotFunctor();

		virtual void operator() ( string recvd_line, IRCBot *circ ) = 0;

};

#endif // BOTFUNCTOR_HPP
