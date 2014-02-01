#include "../ircbot.hpp"
#include "print_wwwtitle.hpp"
#include <curl/curl.h>
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

	std::string www_page = this->download_url( url );

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

std::string PrintWWWTitle::download_url( std::string &url ) {

	string ret;

	CURL *curl_fd;
	curl_fd = curl_easy_init();

	if( curl_fd ) {

		#ifdef CURL_DEBUG
			curl_easy_setopt( curl_fd, CURLOPT_VERBOSE, 1 );
		#endif

		#ifdef HTTPS_DISABLED_WORKAROUND
			int pos = 0;
			if( (pos = url.find("https")) >= 0 ) {
				url.replace( pos, pos + 5, "http" );
			}
		#endif

		struct curl_slist *headers = NULL;
		// headers = curl_slist_append( headers, "User-Agent: Mozilla/5.0 (Windows NT 6.1; rv:19.0) Gecko/20100101 Firefox/19.0" );
		// headers = curl_slist_append( headers, "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8" );
		// headers = curl_slist_append( headers, "Accept-Language: pl,en-us;q=0.7,en;q=0.3" );
		// headers = curl_slist_append( headers, "Connection: keep-alive" );*/

		curl_easy_setopt( curl_fd, CURLOPT_HTTPHEADER, headers );
		curl_easy_setopt( curl_fd, CURLOPT_URL, url.c_str() );
		curl_easy_setopt( curl_fd, CURLOPT_FOLLOWLOCATION, 0 );
		curl_easy_setopt( curl_fd, CURLOPT_WRITEFUNCTION, PrintWWWTitle::curl_cback );
		curl_easy_setopt( curl_fd, CURLOPT_WRITEDATA, &ret );

		curl_easy_setopt( curl_fd, CURLOPT_TIMEOUT, 20 );
		curl_easy_setopt( curl_fd, CURLOPT_CONNECTTIMEOUT, 20 );

		// curl_easy_setopt( curl_fd, CURLOPT_PROXY, "127.0.0.1:9050" );
		// curl_easy_setopt( curl_fd, CURLOPT_PROXYTYPE, CURLPROXY_SOCKS5_HOSTNAME );

		curl_easy_setopt( curl_fd, CURLOPT_SSL_VERIFYPEER, 0 );
		curl_easy_setopt( curl_fd, CURLOPT_SSL_VERIFYHOST, 0 );

		curl_easy_perform( curl_fd );
		curl_easy_cleanup( curl_fd );

	} else {

		throw std::runtime_error( "CURL object init failed." );

	}

	return ret;

}

size_t PrintWWWTitle::curl_cback( char *ptr, size_t size, size_t nmemb, void *userdata ) {

	char *buf;
	buf = (char*)malloc( size*nmemb + 1 );
	strncpy(buf, ptr, size*nmemb + 1);
	buf[ size*nmemb ] = '\0';

	*( (string *)userdata ) += buf;

	free( buf );
	buf = NULL;

	return size * nmemb;

}
