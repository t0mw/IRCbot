#include <iostream>
#include <cstring>
#include "../ircbot.hpp"

using namespace std;

#define MEMO_MAX 20 // max messages in memo

struct user_memo {
    string by_nick;
    string to_nick;
    string memo;
} search_in_names;

vector< struct user_memo > memo_list;

void memo_add( string by_nick, string to_nick, string memo ) {

    struct user_memo new_memo;
    new_memo.by_nick = by_nick;
    new_memo.to_nick = to_nick;
    new_memo.memo = memo;
    memo_list.push_back( new_memo );

}

void memo_names( string recvd, IRC *circ ) {

    if( ! strstr( recvd.c_str(), "PRIVMSG" ) && ! strstr( recvd.c_str(), "NOTICE" ) ) {
        if( recvd.find( search_in_names.to_nick ) == string::npos ) { // nick not in /names
            memo_add( search_in_names.by_nick, search_in_names.to_nick, search_in_names.memo );
        }
    }

}

void memo( string nick, string channel, string msg, IRC *circ ) {

    if( memo_list.size() >= MEMO_MAX ) { // memo too big
        memo_list.clear();
        // return;
    }

    // check if memo
    int end = 0;    // nick end ( nick: ... )
    while( msg[end] != ':' ) { if( msg[end] == ' ' ) { end = -1; break; } end++; }

    if( end == 0 ) return;  // should be 1 min

    if( end != -1 ) {
        string to_nick = msg.substr( 0, end );
        if( to_nick != nick ) {
            // check if nick in channel
            search_in_names.by_nick = nick;
            search_in_names.to_nick = to_nick;
            search_in_names.memo = msg;

            circ->add_handler( memo_names, "", "End of /NAMES list", -1 );

            string sbuf = "NAMES "; sbuf += channel; sbuf += "\r\n";
            circ->send_d( sbuf );
        }
    }

}

void memo_join( string nick, string channel, IRC *circ ) {

    if( memo_list.empty() ) return;

    for( vector<struct user_memo>::iterator it = memo_list.begin(); it != memo_list.end(); it++ ) {
        if( it->to_nick == nick ) {
            string memostr = "PRIVMSG ";
            memostr += nick;
            memostr += " :<";
            memostr += it->by_nick;
            memostr += "> ";
            memostr += it->memo;
            memostr += "\r\n";
            circ->send_d( memostr );

            it = memo_list.erase( it );
            if( it == memo_list.end() ) it--;
        }
    }

}
