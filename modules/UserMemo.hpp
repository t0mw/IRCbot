#ifndef __MEMO_HPP__
#define __MEMO_HPP__

#include "../BotFunctor.hpp"
#include "../IRCBot.hpp"
#include <string>
#include <map>

class UserMemo : public BotFunctor {

	public:

		UserMemo();

		void operator() ( std::string recvd_line, IRCBot *circ );
		void memo_send( std::string nick, IRCBot *circ );
		void set_memo_max( int new_max );

	private:

		unsigned int memo_max;
		bool names_wait_flag;

		// Map dest_nick : message
		std::multimap< std::string, std::string > memo_list;


};

#endif
