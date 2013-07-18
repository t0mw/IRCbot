#ifndef __IRCBOT_HPP__
#define __IRCBOT_HPP__

#include <vector>

using namespace std;

#define VERBOSE // print error, connect and join info, etc
// #define SUPER_VERBOSE // shows all IRC messages

class IRC;

// PRIVMSG handler function
typedef void ( *pm_handler )( string nick, string channel, string msg, IRC *circ );

/*
    PRIVMSG handler vector elem.
    channel: defines the channel where handler is working.
*/
struct privmsgh_ve {
    pm_handler fptr;
    string channel;
};

// JOIN handler function
typedef void ( *join_handler )( string nick, string channel, IRC *circ );

// JOIN handler vector elem.
struct joinh_ve {
    join_handler fptr;
    string channel;
};

// String-triggered handler
typedef void ( *str_handler )( string recvd_line, IRC *circ );

/*
    String-triggered handler vector elem.
    start_str: Defines string that triggers collecting IRC messages (saved to recvd_data).
               if empty, data collection starts immediately.
    stop_str: Defines string that triggers stop of collecting data and fptr() call.
              if empty, data collection end is triggered by lines.
    lines: Number of lines of IRC messages to collect.
    recvd_data: Collected IRC messages that will be passed to fptr().
*/
struct strh_ve {
    str_handler fptr;
    string start_str;
    string stop_str;
    int lines;
    string recvd_data; // for use only in IRC::start()
};

class IRC {

    public:

        IRC();

        /*
            Sets IRC server to connect.
            server: host of IRC server.
            services_id_msg: Line send after successful auth to services.
            motd_end_line: End line of IRC servers MOTD.
        */
        void set_server( string server, string services_id_msg = "are now identified", string motd_end_line = ":End of /MOTD command." );
        void set_nick( string );
        void set_services_passwd( string ); // optional

        // Adds channel to join to.
        void add_channel( string );

        // Adds PRIVMSG handler and binds it to channel (must be in channels list (add_channel)).
        void add_handler( pm_handler, string channel );
        void add_handler( join_handler, string channel );

        /*
            Adds string-triggered handler. Handler works always only once (deleted from queue if got stop condition)
            params info: read struct strh_ve description.
        */
        void add_handler( str_handler fptr, string start_str, string stop_str, int lines );

        // Sends RAW IRC protocol messages to server.
        void send_d( string );
        void send_d( char* );

        // connects to server
        bool server_connect();

        // Starts the bot.
        void start();

        #ifndef WIN32
            void daemon_mode( bool ); // true: run in background
        #endif

        // Quits server. Can be called by handlers.
        void quit();

    private:

        // Gets IRC message and parses it for PRIVMSG handlers.
        void get_d( string recvd_line, string &nick, string &channel, string &msg );

        // Gets IRC message and parses it for JOIN handlers.
        void get_join_d( string recvd_line, string &nick, string &channel );

        // bool is_registered( string nick );   // TODO

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

        vector< struct privmsgh_ve > pm_handlers;
        vector< struct joinh_ve > join_handlers;
        vector< struct strh_ve > str_handlers;

        vector< string > channels;

};

#endif // IRCBOT_H
