#ifndef __PRINTWWWTITLE_HPP__
#define __PRINTWWWTITLE_HPP__

#include "../BotFunctor.hpp"
#include "../IRCBot.hpp"

class PrintWWWTitle : public BotFunctor {

	public:

		PrintWWWTitle();

		void operator() ( std::string msg, IRCBot *circ );

	private:

		std::string get_url_from_str( std::string &str );
		std::string get_title_from_url( std::string &url );

};

#endif
