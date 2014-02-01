#include <iostream>
#include <cstring>
#include "../ircbot.hpp"

using namespace std;

void BotMemo::memo_add( string by_nick, string to_nick, string memo ) {

	struct user_memo new_memo;
	new_memo.by_nick = by_nick;
	new_memo.to_nick = to_nick;
	new_memo.memo = memo;
	memo_list.push_back( new_memo );

}

void BotMemo::operator() ( string recvd, IRC *circ ) {

	if( ! strstr( recvd.c_str(), "PRIVMSG" ) && ! strstr( recvd.c_str(), "NOTICE" ) ) {
		if( recvd.find( search_in_names.to_nick ) == string::npos ) { // nick not in /names
			memo_add( search_in_names.by_nick, search_in_names.to_nick, search_in_names.memo );
		}
	}

}

void BotMemo::operator() ( string nick, string channel, string msg, IRC *circ ) {

	if( memo_list.size() >= MEMO_MAX ) { // memo too big
		memo_list.clear();
		return;
	}

	// check if memo
	int end = 0;    // nick end ( nick: ... )
	while( msg[end] != ':' ) { if( msg[end] == ' ' ) { end = -1; break; } end++; }

	if( end == 0 ) return;  // should be 1 min

	if( end != -1 ) {

		if( msg[end+1] != ' ' ) return;

		string to_nick = msg.substr( 0, end );
		if( to_nick != nick ) {
			// check if nick in channel
			search_in_names.by_nick = nick;
			search_in_names.to_nick = to_nick;
			search_in_names.memo = msg;

			circ->register_handler( channel, this ); // MAX LINES

			string sbuf = "NAMES "; sbuf += channel; sbuf += "\r\n";
			circ->send_raw( sbuf );
		}
	}

	if( msg.find( "queue print" ) != string::npos && nick == "widmo" ) {
		for( vector<struct user_memo>::iterator it = memo_list.begin(); it != memo_list.end(); it++ ) {
			string tosend = "PRIVMSG widmo :to_nick: ";
			tosend += it->to_nick;
			tosend += ", by_nick: ";
			tosend += it->by_nick;
			tosend += ", memo: ";
			tosend += it->memo;
			tosend += "\r\n";
			circ->send_raw( tosend );
		}
		circ->send_raw( string("PRIVMSG widmo :- ende -\r\n") );
	}


}

void BotMemo::operator() ( string nick, string channel, IRC *circ ) {

	if( memo_list.empty() ) return;

	for( vector<struct user_memo>::iterator it = memo_list.begin(); it != memo_list.end(); it++ ) {
		if( it->to_nick == nick ) {
			memostr += "<";
			memostr += it->by_nick;
			memostr += "> ";
			memostr += it->memo;
			circ->send_pm( nick, memostr );

			it = memo_list.erase( it );
			if( it == memo_list.end() ) it--;
		}
	}

}
