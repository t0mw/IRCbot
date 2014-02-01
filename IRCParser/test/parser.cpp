#include "IRCParser.hpp"
#include <iostream>
#include <cstdio>
#include <string>
#include <stdexcept>
#include <string>
#include <vector>

int main( void ) {

	std::string msg = ":hujumuju123!~asdf3f@91.123.221.8 JOIN #chujemuje\r\n"; // :author_nick!author_ident@author_host PRIVMSG #chujmuj :noelo\r\n";
	std::string msg2 = ":barjavel.freenode.net MODE #chujemuje +ns\r\n"; // :host.internety.org.pl NOTICE #ckurwy :asdf ghxcz\r\n";
	
	IRCParser p;
	
	p.parse( msg );
	std::cout << "SRC: " << p.get_src() << ", CMD: " << p.get_command() << std::endl;
	std::cout << "Nick: " << p.get_src_nick() << ", Ident: " << p.get_src_ident() << ", Host: " << p.get_src_host() << std::endl;
	std::cout << "Server: " << p.get_src_server() << std::endl;
	
	for( int i = 0; i < p.get_param_count(); ++i ) {
		std::cout << p.get_param(i) << std::endl;
	}
	
	p.reset();
	
	p.parse( msg2 );
	std::cout << "SRC: " << p.get_src() << ", CMD: " << p.get_command() << std::endl;
	std::cout << "Nick: " << p.get_src_nick() << ", Ident: " << p.get_src_ident() << ", Host: " << p.get_src_host() << std::endl;
	std::cout << "Server: " << p.get_src_server() << std::endl;

	for( int i = 0; i < p.get_param_count(); ++i ) {
		std::cout << p.get_param(i) << std::endl;
	}

	return 0;

}