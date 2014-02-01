#ifndef __PRINT_WWWTITLE_HPP__
#define __PRINT_WWWTITLE_HPP__

#include "../BotFunctor.hpp"
#include "../ircbot.hpp"

// #define HTTPS_DISABLED_WORKAROUND

class PrintWWWTitle : public BotFunctor {

	public:

		PrintWWWTitle();

		void operator() ( std::string msg, IRC *circ );

	private:

		std::string get_url_from_str( std::string &str );
		std::string get_title_from_url( std::string &url );
		std::string download_url( std::string &url );

		static size_t curl_cback( char *ptr, size_t size, size_t nmemb, void *userdata );

};

#endif
