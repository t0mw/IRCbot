#include "../ircbot.hpp"
#include "UserMemo.hpp"
#include <iostream>
#include <cstring>

UserMemo::UserMemo() {

	this->memo_max = 20;
	this->names_wait_flag = false;

}

void UserMemo::operator() ( std::string recvd, IRC *circ ) {

	if( memo_list.size() >= this->memo_max ) {
		memo_list.clear();
		return;
	}

	circ->parser.parse( recvd );

	if( circ->parser.get_command() == "PRIVMSG" ) {

		this->memo_send( circ->parser.get_src_nick(), circ );

		std::string memo_to_parse = circ->parser.get_param(1);

		std::string to_nick;
		int i = 0;
		for( ; isgraph( memo_to_parse[i] ) && memo_to_parse[i] != ':'; ++i ) {
			to_nick += memo_to_parse[i];
		}

		if( memo_to_parse[i] != ':' ) {
			return;
		}

		std::string memo_str = "<";
		memo_str += circ->parser.get_src_nick();
		memo_str += "> ";
		memo_str += memo_to_parse;

		this->memo_list.insert( std::pair< std::string, std::string >( to_nick, memo_str ) );

	} else if( circ->parser.get_command() == "JOIN" ) {

		this->memo_send( circ->parser.get_src_nick(), circ );

	}

}

void UserMemo::memo_send( std::string nick, IRC *circ ) {

	auto memos_for_nick = this->memo_list.equal_range( nick );
	for( auto it = memos_for_nick.first; it != memos_for_nick.second; ++it ) {
		circ->send_pm( it->first, it->second );
		this->memo_list.erase( it );
	}

}

void UserMemo::set_memo_max( int new_max ) {

	this->memo_max = new_max;

}
