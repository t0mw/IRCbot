#ifndef __MEMO_HPP__
#define __MEMO_HPP__

#include "../BotFunctor.hpp"
#include <string>
#include <vector>

#define MEMO_MAX 20 // max messages in memo

using namespace std;

class BotMemo : public BotFunctor {

	public:

		BotMemo( BotFunctor::handler_types type ) : BotFunctor( type ) {

		}

		void operator() ( string nick, string channel, string msg, IRC *circ );
		void operator() ( string nick, string channel, IRC *circ );
		void operator() ( string recvd_line, IRC *circ );

	private:

		void memo_add( string by_nick, string to_nick, string memo );

		struct user_memo {
			string by_nick;
			string to_nick;
			string memo;
		} search_in_names;

		vector<struct user_memo> memo_list;


};

#endif
