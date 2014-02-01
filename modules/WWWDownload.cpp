#include "WWWDownload.hpp"
#include <curl/curl.h>
#include <stdexcept>

std::string WWWDownload::download_url( std::string &url ) {

	std::string ret;

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
		curl_easy_setopt( curl_fd, CURLOPT_WRITEFUNCTION, WWWDownload::curl_cback );
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

size_t WWWDownload::curl_cback( char *ptr, size_t size, size_t nmemb, void *userdata ) {

	char *buf;
	buf = (char*)malloc( size*nmemb + 1 );
	strncpy(buf, ptr, size*nmemb + 1);
	buf[ size*nmemb ] = '\0';

	*( (std::string *)userdata ) += buf;

	free( buf );
	buf = NULL;

	return size * nmemb;

}
