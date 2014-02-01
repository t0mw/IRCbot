#ifndef __WWWDOWNLOAD_HPP__
#define __WWWDOWNLOAD_HPP__

#include <string>

// #define HTTPS_DISABLED_WORKAROUND

class WWWDownload {

	public:

		static std::string download_url( std::string &url );

	private:

		static size_t curl_cback( char *ptr, size_t size, size_t nmemb, void *userdata );

};

#endif
