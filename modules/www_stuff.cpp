#include <iostream>
#include <cstdlib>
#include <cstring>
#include <curl/curl.h>

#include "www_stuff.hpp"

using namespace std;

string parsetitle( string data ) {

    string ret;
	int start = 0, end = 0;

	start = data.find( "<title>" );
	end = data.find( "</title>" );

	if( start < 0 || end < 0 ) return string();

    for( int i = start; i < end; i++ ) {
        if( data[i] == '\n' || data[i] == '\r' ) {  // newline filter
            end = i-1;
            break;
        }
    }

    ret = data.substr( start + 7, end - start - 7 );	// 7 == strlen( "<title>" )

    return ret;

}

/* CURL callback, do not touch */
size_t curl_strsave(char *ptr, size_t size, size_t nmemb, void *userdata) {
	
	char *buf;
	buf = (char*)malloc( size*nmemb + 1 );
	strncpy(buf, ptr, size*nmemb + 1);
	buf[ size*nmemb ] = '\0';

	*( (string *)userdata ) += buf;
	
	free( buf );
	buf = NULL;
	
	return size * nmemb;
	
}

string download_single_url( string url ) {
	
	string ret;

	CURL *curl_fd;
	curl_fd = curl_easy_init();

	if(curl_fd) {

		#ifdef CURL_DEBUG
			curl_easy_setopt( curl_fd, CURLOPT_VERBOSE, 1 );
		#endif

		struct curl_slist *headers = NULL;
		
		// headers = curl_slist_append( headers, "User-Agent: Mozilla/5.0 (Windows NT 6.1; rv:19.0) Gecko/20100101 Firefox/19.0" );
		// headers = curl_slist_append( headers, "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8" );
		// headers = curl_slist_append( headers, "Accept-Language: pl,en-us;q=0.7,en;q=0.3" );
		// headers = curl_slist_append( headers, "Connection: keep-alive" );*/
		
		curl_easy_setopt( curl_fd, CURLOPT_HTTPHEADER, headers );
		curl_easy_setopt( curl_fd, CURLOPT_URL, url.c_str() );
		curl_easy_setopt( curl_fd, CURLOPT_FOLLOWLOCATION, 1 );
		curl_easy_setopt( curl_fd, CURLOPT_WRITEFUNCTION, curl_strsave );
		curl_easy_setopt( curl_fd, CURLOPT_WRITEDATA, &ret );

        // curl_easy_setopt( curl_fd, CURLOPT_PROXY, "127.0.0.1:9050" );
        // curl_easy_setopt( curl_fd, CURLOPT_PROXYTYPE, CURLPROXY_SOCKS5_HOSTNAME );

		curl_easy_setopt( curl_fd, CURLOPT_SSL_VERIFYPEER, 0 );
		curl_easy_setopt( curl_fd, CURLOPT_SSL_VERIFYHOST, 0 );

		curl_easy_perform( curl_fd );
		curl_easy_cleanup( curl_fd );

	} else {

		return NULL;

	}
	
	return ret;
	
}

string get_title( string url ) {

	string data = download_single_url( url );
	if( data.empty() ) return data;
	string www_title = parsetitle( data );
	
	return www_title;
	
}

string get_link( string data ) {

	int ss;
	string link;
	if( (ss = data.find( "http://" )) >= 0 || ( ss = data.find( "https://" )) >= 0 || ( ss = data.find( "www" ) ) >= 0 ) {
	
		int end = ss;
		while( data[end] != ' ' && data[end] != '\r' && data[end] != '\n' && data[end] != '\0' ) end++;
		
		link = data.substr( ss, end-ss );
		
		return link;
		
	} else {
		return string();
	}

}



