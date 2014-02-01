#ifndef BOTFUNCTOR_HPP
#define BOTFUNCTOR_HPP

#include "ircbot.hpp"
#include <string>

using namespace std;

class IRC;

class BotFunctor {

	public:

		BotFunctor();
		virtual ~BotFunctor();

		virtual void operator() ( string recvd_line, IRC *circ ) = 0;

};

#endif // BOTFUNCTOR_HPP
