#ifndef __IRCBOT_HPP__
#define __IRCBOT_HPP__

#include "BotFunctor.hpp"
#include "IRCParser/IRCParser.hpp"
#include <string>
#include <vector>
#include <map>

using namespace std;

// #define VERBOSE // print error, connect and join info, etc
// #define SUPER_VERBOSE // shows all IRC messages

class BotFunctor;

class IRC {

	public:

		IRC();
		~IRC();

		void set_server( string server, string services_id_msg = "are now identified", string motd_end_line = ":End of /MOTD command." );
		void set_nick( string );
		void set_services_passwd( string );

		bool add_channel( string );

		void register_handler( string channel, BotFunctor *fctr );

		void send_raw( string );
		void send_pm( string dest, string message );

		void server_connect();

		void start();

		#ifndef WIN32
			void daemon_mode( bool ); // true: run in background
		#endif

		// Quits ircbot. Can be called by handlers.
		void quit();

		IRCParser parser;

	private:

		bool init_irc_conn();
		bool services_id();
		void join_channels();

		string get_channel_from_str( char *buf , int buf_len);

		// Change to stream
		void bot_log( const char *format, ... );

		bool prolog_end; // true if IRC server prolog/motd end.
		bool exit_flag; // used by quit() to exit main loop ( in IRC::start() )

		string motd_end_line;
		string services_id_msg;

		string server_host;
		string nick;
		string services_passwd; // optional

		#ifndef WIN32
			bool daemon_mode_flag;
		#endif

		int sockfd;

		multimap<string, BotFunctor *> handlers;
		vector<string> channels;

};

#endif // IRCBOT_H
