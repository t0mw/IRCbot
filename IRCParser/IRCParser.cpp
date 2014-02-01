#include "IRCParser.hpp"
#include <stdexcept>

void IRCParser::parse( std::string &input ) {

	this->reset();
	this->parser_state = READ_START;
	
	for( auto irc_char : input ) {
								
		switch( this->parser_state ) {
		
			case READ_START: {
					if( irc_char != ':' ) {
						this->reset();
						throw std::invalid_argument( "Invalid IRC frame (no starting ':')." );
					} else {
						this->parser_state = READ_SRC;
					}
				}
				break;
				
			case READ_SRC: {
					if( isgraph( irc_char ) ) {
						this->src += irc_char;
					} else if( irc_char != ' ' ) {
						this->reset();
						throw std::invalid_argument( "Invalid IRC frame (no space after source)." );
					} else {
						this->parse_src();
						this->parser_state = READ_CMD;
					}
				}
				break;

			case READ_CMD: {
					if( isalnum( irc_char ) ) {
						this->command += irc_char;
					} else if( irc_char != ' ' ) {
						this->reset();
						throw std::invalid_argument( "Invalid IRC frame (no space after command)." );
					} else {
						this->parser_state = READ_PARAMS;
					}
				}
				break;
				
			case READ_PARAMS: {
					static std::string tmp_param;
					if( irc_char == ':' ) {
						this->parser_state = READ_PARAMS_TRAILING;
						tmp_param.clear();
					} else if( isgraph( irc_char ) ) {
						tmp_param += irc_char;
					} else if( irc_char == ' ' || irc_char == '\r' ) {
						this->params.push_back( tmp_param );
						tmp_param.clear();
						if( irc_char == '\r' ) {
							this->parser_state = READ_END;
						}
					} else {
						this->reset();
						throw std::invalid_argument( "Invalid IRC frame (no space after param)." );
					}
				}
				break;
				
			case READ_PARAMS_TRAILING: {
					static std::string last_param;
					if( isprint( irc_char ) ) {
						last_param += irc_char;
					} else if( irc_char != '\r' ) {
						this->reset();
						last_param.clear();
						throw std::invalid_argument( "Invalid IRC frame (no carriage return char at the end of frame)." );
					} else {
						this->params.push_back( last_param );
						this->parser_state = READ_END;
						last_param.clear();
					}
				}
				break;
				
			case READ_END: {
					if( irc_char != '\n' ) {
						this->reset();
						throw std::invalid_argument( "Invalid IRC frame (no new line char at the end of frame)." );
					} else {
						return;
					}
				}
				break;
				
		}
		
	}
	
}

std::string IRCParser::get_src() {

	return this->src;

}

std::string IRCParser::get_command() {

	return this->command;

}

std::string IRCParser::get_param( int param_index ) {

	if( this->get_param_count() < param_index )
		return std::string("");

	return this->params[ param_index ];

}

int IRCParser::get_param_count() {

	return this->params.size();

}

std::string IRCParser::get_src_nick() {
	
	return this->src_nick;
	
}

std::string IRCParser::get_src_ident() {

	return this->src_ident;

}

std::string IRCParser::get_src_host() {

	return this->src_host;

}

std::string IRCParser::get_src_server() {

	return this->src_server;

}

void IRCParser::reset() {

	this->parser_state = READ_START;
	this->src.clear();
	this->command.clear();
	std::vector< std::string >().swap( this->params );
	
	src_nick.clear();
	src_ident.clear();
	src_host.clear();
	src_server.clear();

}

void IRCParser::parse_src() {

	int src_parser_state = READ_SRC_START;
	std::string tmp;
	for( auto src_char : this->src ) {
	
		switch( src_parser_state ) {
		
			case READ_SRC_START: {
					if( src_char == '.' ) {
						this->src_server = tmp;
						this->src_server += src_char;
						src_parser_state = READ_SRC_SERVER;
					} else if( src_char == '!' ) {
						this->src_nick = tmp;
						src_parser_state = READ_SRC_IDENT;
					} else {
						tmp += src_char;
					}
				}
				break;
				
			case READ_SRC_SERVER: {
					this->src_server += src_char;
				}
				break;
				
			case READ_SRC_IDENT: {
					if( src_char != '@' ) {
						this->src_ident += src_char;
					} else {
						src_parser_state = READ_SRC_HOST;
					}
				}
				break;
				
			case READ_SRC_HOST: {
					this->src_host += src_char;
				}
				break;

		}
	
	}

}
