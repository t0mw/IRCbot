#include "ircbot.hpp"
#include <iostream>
#include <cstdio>
#include <cstring>
#include <stdexcept>

#ifdef _WIN32
	#include <winsock2.h>
#else
	#include <unistd.h>
	#include <sys/types.h>
	#include <sys/socket.h>
	#include <netinet/in.h>
	#include <arpa/inet.h>
	#include <netdb.h>
#endif

IRC::IRC() {

	this->exit_flag = false;
	this->prolog_end = false;

}

IRC::~IRC() {

	for( auto it : this->handlers ) {
		delete it.second;
	}

}

void IRC::set_server( string server, string services_id_msg, string motd_end_line ) {

	this->server_host = server;
	this->services_id_msg = services_id_msg;
	this->motd_end_line = motd_end_line;

}

void IRC::set_nick( string new_nick ) {

	this->nick = new_nick;

}

void IRC::set_services_passwd( string newpass ) {

	this->services_passwd = newpass;

}

bool IRC::add_channel( string new_channel ) {

	for( auto it = this->channels.begin(); it != this->channels.end(); ++it ) {
		if( *it == new_channel ) {
			return false;
		}
	}

	this->channels.push_back( new_channel );
	return true;

}

void IRC::register_handler( string channel, BotFunctor *fctr ) {

	// Check if channel in channels list.
	for( auto it = this->channels.begin(); it != this->channels.end(); ++it ) {
		if( *it == channel ) {
			this->handlers.insert( std::pair< string, BotFunctor *>( channel, fctr ) );
			return;
		}
	}

	throw std::runtime_error( "register_handler(): channel not in list." );

}

void IRC::send_raw( string data ) {

	#ifdef _WIN32
		send( this->sockfd, data.c_str(), data.length(), 0 );
	#else
		write( this->sockfd, data.c_str(), data.length() );
	#endif

}

void IRC::send_pm( string dest, string message ) {

	string msg;
	msg += "PRIVMSG ";
	msg += dest;
	msg += " :";
	msg += message;
	msg += "\r\n";

	this->send_raw( msg );

}

#ifndef _WIN32
void IRC::daemon_mode( bool a ) {

	this->daemon_mode_flag = a;

}
#endif

bool IRC::init_irc_conn() {

	bot_log( "Connecting to host %s with nick %s... ", this->server_host.c_str(), this->nick.c_str() );
	this->server_connect();
	bot_log( "done.\n" );

	bot_log( "Setting user & nick... " );

	string ircprot_buf;
	ircprot_buf = "USER ";
	ircprot_buf += this->nick;
	ircprot_buf += " asd asd :usr\nNICK ";
	ircprot_buf += this->nick;
	ircprot_buf += "\r\n";

	this->send_raw( ircprot_buf );

	bot_log( "done.\n" );

	return true;

}

bool IRC::services_id() {

	// Services identify.
	if( !this->services_passwd.empty() ) {

		string ircprot_buf;
		ircprot_buf += "PRIVMSG NickServ :identify ";
		ircprot_buf += this->services_passwd;
		ircprot_buf += "\r\n";

		// Wait for services identification ok string, break if received more than max_id_checks lines without it.
		int max_id_checks = 100;
		int bytes_received = 0;
		char buf[512];
		#ifdef _WIN32
			while( this->send_raw(ircprot_buf), ( bytes_received = recv( this->sockfd, buf, sizeof(buf), 0 ) ) > 0 ) {
		#else
			while( this->send_raw(ircprot_buf), ( bytes_received = read( this->sockfd, buf, sizeof(buf) ) ) > 0 ) {
		#endif
			if( max_id_checks-- < 0 ) {
				break;
			} else if( strstr( buf, this->services_id_msg.c_str() ) ) {
				return true;
			}
		}
	}

	return false;

}

void IRC::join_channels() {

	string ircprot_buf;

	if( !this->channels.empty() ) {
		for( vector<string>::iterator it = this->channels.begin(); it != this->channels.end(); it++ ) {
			if( (*it)[0] == '#' ) {

				bot_log( "Joining channel %s...", (*it).c_str() );

				ircprot_buf = "";
				ircprot_buf += "JOIN ";
				ircprot_buf += *it;
				ircprot_buf += "\r\n";
				this->send_raw( ircprot_buf );

				bot_log( "done.\n" );

			}
		}
	}

}

string IRC::get_channel_from_str( char *buf, int buf_len ) {

	// Channel should be first word after #, so:

	int hash_pos = -1;

	for( int i = 0; i < buf_len; ++i ) {

		if( hash_pos == -1 ) {
			if( buf[i] == '#' ) {
				hash_pos = i;
			}
		} else {
			if( buf[i] == ' ' || buf[i] == '\r' ) {
				return string( buf+hash_pos, buf+i );
			}
		}
	}

	return string("");

}

void IRC::bot_log( const char *format, ... ) {

	#ifdef VERBOSE
		va_list args;
		va_start( args, format );
		vprintf( format, args );
		va_end( args );
	#endif

}

void IRC::server_connect() {

	#ifdef _WIN32
		WORD version = MAKEWORD( 2, 0 );
		WSADATA wsaData;
		if( WSAStartup( version, &wsaData ) != 0 ) {
			WSACleanup();
			throw runtime_error( "WSA init error!" );
		}

		if( LOBYTE( wsaData.wVersion) != 2 || HIBYTE( wsaData.wVersion ) != 0 ) {
			WSACleanup();
			throw runtime_error( "Wrong WSA version!" );
		}
	#endif

	struct hostent *h = gethostbyname( this->server_host.c_str() );
	if( !h ) {
		throw runtime_error( "gethostbyname() error!" );
	}

	this->sockfd = socket( AF_INET, SOCK_STREAM, 0 );
	struct sockaddr_in st_sin;
	st_sin.sin_family = AF_INET;
	st_sin.sin_port = htons( 6667 );
	st_sin.sin_addr = *((struct in_addr*)h->h_addr);
	memset( &(st_sin.sin_zero), '\0', 8 );

	if( connect( this->sockfd, (struct sockaddr *)&st_sin, sizeof(st_sin) ) < 0 ) {
		throw runtime_error( "connect() error!\n" );
	}

}

void IRC::start() {

	if( this->nick.empty() || this->server_host.empty() ) {
		throw runtime_error( "Nick or server host not set!" );
	}

	if( !init_irc_conn() ) {
		throw runtime_error( "IRC connection failed!" );
	}
	
	char buf[512]; // recv buffer, 512 RFC max irc line length
	
	bot_log( "Ignoring MOTD... " );

	int bytes_received = 0;  // bytes received
	#ifdef _WIN32
		bytes_received = recv( this->sockfd, buf, sizeof( buf ), 0 );
	#else
		n = read( this->sockfd, buf, sizeof(buf) );
	#endif

	// IRCBot superloop.
	while( bytes_received > 0 ) {
		
		#ifdef SUPER_VERBOSE
			cout << buf;
		#endif

		// handle ping & pong
		if( strncmp( buf, "PING", 4 ) == 0 ) {
			buf[1] = 'I'; // P(I->O)NG
			this->send_raw( buf );
		}
		
		// Ignore IRC server MOTD etc.
		if( this->prolog_end == false ) {

			if( strstr( buf, this->motd_end_line.c_str() ) != NULL ) {
				bot_log( "done.\n" ); // Ignoring motd done.

				bot_log( "Identifying to services... " );
				services_id();
				bot_log( "done.\n" );

				join_channels();

				this->prolog_end = true;

				#ifndef _WIN32
					if( this->daemon_mode_flag == true ) {
						bot_log( "Entering daemon mode.\n" );
						daemon( 1, 0 );
					}
				#endif
			}

		} else { // MOTD etc finished, start running handlers.

			if( !this->handlers.empty() ) {

				string channel;
				channel = get_channel_from_str( buf, 512 );

				auto chan_specific_handlers = this->handlers.equal_range( channel );
				for( auto it = chan_specific_handlers.first; it != chan_specific_handlers.second; ++it ) {
					bot_log( "Executing functor for chan: %s\n", it->first.c_str() );
					(*(it->second))( buf, this );
				}

			}
		
		}
		
		if( this->exit_flag == true ) {
			bot_log( "Quit.\n" );
			break;
		}

		memset( buf, '\0', sizeof(buf) );
		#ifdef _WIN32
			bytes_received = recv( this->sockfd, buf, sizeof( buf ), 0 );
		#else
			bytes_received = read( this->sockfd, buf, sizeof( buf ) );
		#endif
	}

}

void IRC::quit() {

	this->send_raw( "QUIT :quit\r\n" );

	#ifdef _WIN32
		closesocket( this->sockfd );
	#else
		close( this->sockfd );
	#endif

	this->exit_flag = true;

}
