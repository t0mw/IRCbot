#ifndef __IRCPARSER_HPP__
#define __IRCPARSER_HPP__

#include <string>
#include <vector>

class IRCParser {

	public:
	
		void parse( std::string &input );

		std::string get_src();
		std::string get_command();
		std::string get_param( int param_index );
		int get_param_count();
		
		std::string get_src_nick();
		std::string get_src_ident();
		std::string get_src_host();
		
		std::string get_src_server();
		
		void reset();
		
	private:
	
		void parse_src();
	
		enum parser_states { READ_START, READ_SRC, READ_CMD, READ_PARAMS, READ_PARAMS_TRAILING,
							READ_END, READ_SRC_START, READ_SRC_SERVER, READ_SRC_NICK, READ_SRC_IDENT, READ_SRC_HOST };
		int parser_state;
	
		std::string src;
		std::string command;
		std::vector< std::string > params;
		
		std::string src_nick;
		std::string src_ident;
		std::string src_host;
		std::string src_server;

};

#endif
