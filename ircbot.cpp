#include <iostream>
#include <vector>
#include <cstring>

#ifdef WIN32
	#include <winsock2.h>
#else
	#include <unistd.h>
	#include <sys/types.h>
	#include <sys/socket.h>
	#include <netinet/in.h>
	#include <arpa/inet.h>
	#include <netdb.h>
#endif

#include "ircbot.hpp"

IRC::IRC() {

    this->exit_flag = false;
    this->prolog_end = false;

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

void IRC::add_channel( string new_channel ) {

    for( vector<string>::iterator it = this->channels.begin(); it != this->channels.end(); it++ ) {
        if( *it == new_channel ) {
            #ifdef VERBOSE
                cout << "Channel " << new_channel << " already in channels list.\n" << endl;
            #endif
            return;
        }
    }

    this->channels.push_back( new_channel );

}

void IRC::add_handler( pm_handler new_handler, string channel ) {

    // check if channel in channels list
    bool chan_in_vector = false;
    for( vector<string>::iterator it = this->channels.begin(); it != this->channels.end(); it++ ) {
        if( *it == channel ) { chan_in_vector = true; break; }
    }

    if( chan_in_vector ) {
        struct privmsgh_ve ch;
        ch.fptr = new_handler;
        ch.channel = channel;
        this->pm_handlers.push_back( ch );
    } else {
        #ifdef VERBOSE
            cout << "Channel " << channel << " not found in channels list, can't add handler!\n";
        #endif
    }

}

void IRC::add_handler( join_handler jhandler, string channel ) {

    // check if channel in channels list
    bool chan_in_vector = false;
    for( vector<string>::iterator it = this->channels.begin(); it != this->channels.end(); it++ ) {
        if( *it == channel ) { chan_in_vector = true; break; }
    }

    if( chan_in_vector ) {
        struct joinh_ve ch;
        ch.fptr = jhandler;
        ch.channel = channel;
        this->join_handlers.push_back( ch );
    } else {
        #ifdef VERBOSE
            cout << "Channel " << channel << " not found in channels list, can't add join handler!\n";
        #endif
    }

}

void IRC::add_handler( str_handler fptr, string start_str, string stop_str, int lines ) {

    struct strh_ve s;
    s.start_str = start_str;
    s.stop_str = stop_str;
    s.fptr = fptr;

    // No stop trigger, set to one line.
    if( stop_str.empty() && lines < 0 ) {
        #ifdef VERBOSE
            cout << "No stop condition in string-based handler, setting to one line.\n";
        #endif
        s.lines = 1;
    } else {
        s.lines = lines;
    }

    this->str_handlers.push_back( s );

}

void IRC::send_d( string data ) {

	#ifdef WIN32
		send( this->sockfd, data.c_str(), data.length(), 0 );
	#else
		write( this->sockfd, data.c_str(), data.length() );
	#endif

}

void IRC::send_d( char *data ) {

	#ifdef WIN32
		send( this->sockfd, data, strlen(data), 0 );
	#else
		write( this->sockfd, data, strlen(data) );
	#endif

}

#ifndef WIN32
    void IRC::daemon_mode( bool a ) {

        this->daemon_mode_flag = a;

    }
#endif

void IRC::get_d( string recvd_line, string &nick, string &channel, string &msg ) {

    // check if privmsg
    // :nick!ident@host PRIVMSG #channel :message\r\n
    int start = 0;
    while( recvd_line[start] != ' ' ) start++;
    start++;
    if( recvd_line.substr( start, 8 ) == "PRIVMSG " ) {

        int end;

        // get channel
        start += 8; // after: ...PRIVMSG [start]#channel\r\n
        end = start;
        while( recvd_line[end] != ' ' ) end++;
        channel = recvd_line.substr( start, end-start );

        // get msg
        end += 2;   // after: ...PRIVMSG #channel :[end]msg\r\n
        start = end;
        while( recvd_line[end] != '\r' ) end++;
        msg = recvd_line.substr( start, end-start );

        // get nick
        start = 0;
        while( recvd_line[start] != ':' ) start++;
        start++;
        end = start;
        while( recvd_line[end] != '!' ) end++;
        nick = recvd_line.substr( start, end-start );

    } else {
        // not privmsg - not interesting
        nick = "";
        channel = "";
        msg = "";
    }

}

void IRC::get_join_d( string recvd_line, string &nick, string &channel ) {

    // check if join
    // :nick!ident@host JOIN #channel\r\n
    int start = 0;
    while( recvd_line[start] != ' ' ) start++;  // after: ...[start]JOIN #chan
    start++;
    if( recvd_line.substr( start, 5 ) == "JOIN " ) {

        int end;

        // get channel
        start += 5; // after: ...JOIN [start]#channel
        end = start;
        while( recvd_line[end] != ' ' && recvd_line[end] != '\r' ) end++;
        channel = recvd_line.substr( start, end-start );

        // get nick
        start = 0;
        while( recvd_line[start] != ':' ) start++;
        start++;
        end = start;
        while( recvd_line[end] != '!' ) end++;
        nick = recvd_line.substr( start, end-start );

    } else {
        // not join - not interesting
        nick = "";
        channel = "";
    }

}

bool IRC::server_connect() {

    #ifdef WIN32
        WORD version = MAKEWORD( 2, 0 );
        WSADATA wsaData;
        if( WSAStartup( version, &wsaData ) != 0 ) {
            #ifdef VERBOSE
                cout << "WSA init error!\n";
            #endif
            WSACleanup();
            return false;
        }

        if( LOBYTE( wsaData.wVersion) != 2 || HIBYTE( wsaData.wVersion ) != 0 ) {
            #ifdef VERBOSE
                cout << "Wrong WSA version!\n";
            #endif
            WSACleanup();
            return false;
        }
    #endif

    struct hostent *h = gethostbyname( this->server_host.c_str() );
    if( !h ) {
        #ifdef VERBOSE
            cout << "gethostbyname() error!\n";
        #endif
        return false;
    }

    this->sockfd = socket( AF_INET, SOCK_STREAM, 0 );
    struct sockaddr_in st_sin;
    st_sin.sin_family = AF_INET;
    st_sin.sin_port = htons( 6667 );
    st_sin.sin_addr = *((struct in_addr*)h->h_addr);
    memset( &(st_sin.sin_zero), '\0', 8 );

    if( connect( this->sockfd, (struct sockaddr *)&st_sin, sizeof(st_sin) ) < 0 ) {
        #ifdef VERBOSE
            cout << "Connect error!\n";
        #endif
        return false;
    }

    return true;

}

/*
	IRC main loop
    Fuck this one is huge.
*/
void IRC::start() {

	if( this->nick.empty() || this->server_host.empty() ) {
        #ifdef VERBOSE
            cout << "Nick or IRC server not set!\n";
        #endif
        return;
	}

    #ifdef VERBOSE
        cout << "Connecting to host " << this->server_host << " with nick " << this->nick << "... ";
    #endif
    bool connected = this->server_connect();
    if( connected ) {
        #ifdef VERBOSE
            cout << "done.\n";
        #endif
    } else {
        #ifdef VERBOSE
            cout << "failed!\n";
        #endif
        return;
    }
	
    #ifdef VERBOSE
        cout << "Setting user & nick... ";
    #endif

    string ircprot_buf; // stores IRC protocol messages to send

    ircprot_buf = "USER ";
    ircprot_buf += this->nick;
    ircprot_buf += " asd asd :usr\nNICK ";
    ircprot_buf += this->nick;
    ircprot_buf += "\r\n";
	
    this->send_d( ircprot_buf );
	
    #ifdef VERBOSE
        cout << "done.\n";
    #endif
	
    char buf[512];		// recv buffer, 512 RFC max irc line length
	
    #ifdef VERBOSE
        cout << "Ignoring MOTD... ";
    #endif
	
    int n = 0;  // bytes received
	#ifdef WIN32
		n = recv( this->sockfd, buf, sizeof( buf ), 0 );
	#else
		n = read( this->sockfd, buf, sizeof(buf) );
	#endif

    // main loop
	while( n > 0 ) {
		
        #ifdef SUPER_VERBOSE
            cout << buf;
		#endif
		
        // handle ping & pong
        if( strncmp( buf, "PING", 4 ) == 0 ) {
            buf[1] = 'I'; // P(I->O)NG
			this->send_d( buf );
		}
		
        // IRC server MOTD and stuff
		if( this->prolog_end == false ) {

            if( strstr( buf, this->motd_end_line.c_str() ) != NULL ) {
                #ifdef VERBOSE
                    cout << "done.\n";
                #endif

                // Services identify
                if( ! this->services_passwd.empty() ) {
                    #ifdef VERBOSE
                        cout << "Identifying to services... ";
                    #endif
                    ircprot_buf = "";
                    ircprot_buf += "PRIVMSG NickServ :identify ";
                    ircprot_buf += this->services_passwd;
                    ircprot_buf += "\r\n";

					// Wait for ID string, break if received more than max_id_checks lines without it.
                    int max_id_checks = 100;
                    #ifdef WIN32
                        while( this->send_d(ircprot_buf), ( n = recv( this->sockfd, buf, sizeof(buf), 0 ) ) > 0 ) {
                    #else
                        while( this->send_d(ircprot_buf), ( n = read( this->sockfd, buf, sizeof(buf) ) ) > 0 ) {
                    #endif
                            if( max_id_checks-- < 0 ) {
                                #ifdef VERBOSE
                                    cout << "failed!\n";
                                #endif
                                this->quit();
                                break;
                            } else if( strstr( buf, this->services_id_msg.c_str() ) ) {
                                #ifdef VERBOSE
                                    cout << "done.\n";
                                #endif
                                break;
                            }
                        }
                }

                // Joining channels
                if( ! this->channels.empty() ) {
                    for( vector<string>::iterator it = this->channels.begin(); it != this->channels.end(); it++ ) {
                        if( (*it)[0] == '#' ) {
                            #ifdef VERBOSE
                                cout << "Joining " << *it;
                            #endif
                            ircprot_buf = "";
                            ircprot_buf += "JOIN ";
                            ircprot_buf += *it;
                            ircprot_buf += "\r\n";
                            this->send_d( ircprot_buf );
                            #ifdef VERBOSE
                                cout << "... done.\n";
                            #endif
                        }
                    }
                }

				this->prolog_end = true;

				#ifndef WIN32
                    if( this->daemon_mode_flag == true ) {
                        #ifdef VERBOSE
                            cout << "Entering daemon mode.\n";
                        #endif
                        daemon( 1, 0 );
                    }
				#endif
			}

        } else {    // after MOTD

            // string-based handlers
            if( ! this->str_handlers.empty() ) {

                for( vector<strh_ve>::iterator it = this->str_handlers.begin(); it != str_handlers.end(); it++ ) {

                    if( ! it->recvd_data.empty() ) {
                        if( it->lines >= 0 ) it->lines--;
                        it->recvd_data += buf;

                        // check if stop cond occurred
                        if( ( it->lines == 0 ) || ( !it->stop_str.empty() && strstr( buf, it->stop_str.c_str() ) ) ) {
                            ((str_handler) it->fptr)( it->recvd_data, this );
                            it->recvd_data = "";
                            it->lines = -1;
                            it = str_handlers.erase( it );   // delete handler
                            if( it == str_handlers.end() ) it--; // it++ at the end of loop, cheatin'
                        }

                    } else {
                        // check if start of trigger, adding first line, so recvd_data should be empty
                        if( strstr( buf, it->start_str.c_str() ) || it->start_str.empty() ) {    // got start_str
                            it->recvd_data = buf;
                            if( it->lines >= 0 ) it->lines--;   // lines == 0: got all, -1: not set
                        }
                    }

                }

            }

            // JOIN handlers
            if( ! this->join_handlers.empty() ) {

                string nick, channel;
                this->get_join_d( buf, nick, channel );

                for( vector<joinh_ve>::iterator it = this->join_handlers.begin(); it != join_handlers.end(); it++ ) {
                    if( it->channel == channel ) {
                        ((join_handler) it->fptr)( nick, channel, this );
                    }
                }

            }

            // PRIVMSG handlers
            if( ! this->pm_handlers.empty() ) {

                string nick, channel, msg;
                this->get_d( buf, nick, channel, msg );
                if( ! nick.empty() ) {
                    for( vector<privmsgh_ve>::iterator it = this->pm_handlers.begin(); it != this->pm_handlers.end(); it++ ) {
                        if( it->channel == channel ) {  // exec handlers for msg from channel
                            ((pm_handler) it->fptr)( nick, channel, msg, this );
                        }
                    }

                }

            }
		
		}
		
        if( this->exit_flag == true ) {
            #ifdef VERBOSE
                cout << "Quit.\n";
            #endif
            break;
        }

		memset( buf, '\0', sizeof(buf) );
		#ifdef WIN32
			n = recv( this->sockfd, buf, sizeof( buf ), 0 );
		#else
			n = read( this->sockfd, buf, sizeof(buf) );
		#endif
	}
	
}

void IRC::quit() {

    this->send_d( string("QUIT :quit\r\n") );

	#ifdef WIN32
		closesocket( this->sockfd );
	#else
		close( this->sockfd );
	#endif

    this->exit_flag = true;

}


