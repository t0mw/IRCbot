#include "../ircbot.hpp"
#include "PrintWWWTitle.hpp"
#include "WWWDownload.hpp"
#include <iostream>
#include <stdexcept>

PrintWWWTitle::PrintWWWTitle() {

}

void PrintWWWTitle::operator() ( std::string msg, IRC *circ ) {

	circ->parser.parse( msg );

	if( circ->parser.get_command() == "PRIVMSG" ) {

		std::string channel = circ->parser.get_param(0);
		std::string msg = circ->parser.get_param(1);

		std::string url = this->get_url_from_str( msg );

		if( !url.empty() ) {
			std::string title = this->get_title_from_url( url );
			if( !title.empty() ) {
				circ->send_pm( channel, title );
			}
		}

	 }

}

std::string PrintWWWTitle::get_url_from_str( std::string &str ) {

	std::string url;

	std::string url_start[] = { "http://", "https://", "www" };

	std::size_t url_start_pos = 0;
	for( int i = 0; i < 3; ++i ) {
		url_start_pos = str.find( url_start[i] );
		if( url_start_pos != std::string::npos ) {

			while( isgraph( str[ url_start_pos ] ) ) {
				url += str[url_start_pos];
				++url_start_pos;
			}

			break;

		}
	}

	return url;

}

std::string PrintWWWTitle::get_title_from_url( std::string &url ) {

	std::string www_page = WWWDownload::download_url( url );

	std::string title;

	size_t title_start_pos = www_page.find( "<title>" );
	title_start_pos += 7; // Ommit "<title>"
	size_t title_end_pos = www_page.find( "</title>" );

	if( title_start_pos != std::string::npos && title_end_pos != std::string::npos ) {

		while( isprint( www_page[title_start_pos] ) && title_start_pos != title_end_pos ) {
			title += www_page[title_start_pos];
			++title_start_pos;
		}

	}

	return title;

}
