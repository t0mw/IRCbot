#include <iostream>
#include "../ircbot.hpp"
#include "www_stuff.hpp"

using namespace std;

void sjp_info( string nick, string channel, string msg, IRC *circ ) {

    int start = 0;
    if( (start = msg.find(".sjp")) > 0 ) {

        string link = "http://sjp.pl/";

        link += msg.substr(start + 5);

        string data = download_single_url( link );

        start = data.find( "<p style=\"margin-top: .5em; font-size: medium; max-width: 30em; \">" ); // strlen - 66
        if( start < 0 ) return;
        start += 66;
        int end = start;

        // ugly hacks
        while( data[end] != '<' || data[end+1] != '/' || data[end+2] != 'p' ) end++;

        string sjpdata;
        if( end - start > SJP_MAX_DATA ) {
            sjpdata = data.substr( start, SJP_MAX_DATA );
        } else {
            sjpdata = data.substr( start, end - start );
        }

        while( ( start = sjpdata.find("<br />")) >= 0 ) {
            sjpdata.replace( start, 6, " " );
        }

        // jebac iso 8859
        short iso_chars[] =  { 0xB1, 0xA1, 0xE6, 0xC6, 0xEA, 0xCA, 0xB3, 0xA3, 0xF1, 0xD1, 0xF3, 0xD3, 0xB6, 0xA6, 0xBF, 0xAF, 0xBC, 0xAC };
        short ansi_chars[] = { 0x61, 0x41, 0x63, 0x43, 0x65, 0x45, 0x6C, 0x4C, 0x6E, 0x4E, 0x6F, 0x4F, 0x73, 0x53, 0x7A, 0x5A, 0x7A, 0x5A };

        unsigned int i = 0, j = 0;
        for( i = 0; i < sjpdata.length(); i++ ) {
            for( j = 0; j < 18; j++ ) {
                if( sjpdata[i] == (char)iso_chars[j] ) {
                    sjpdata[i] = ansi_chars[j];
                    break;
                }
            }
        }

        string buf = "PRIVMSG #";
        buf += channel;
        buf += " :";
        buf += sjpdata;
        buf += "... ";
        buf += link;
        buf += "\r\n";

        circ->send_d( buf );

    }
}

void pwn_info( string nick, string channel, string msg, IRC *circ ) {

    int start = 0;
    if( (start = msg.find(".sjp")) > 0 ) {

        string link = "http://sjp.pwn.pl/szukaj/";

        link += msg.substr(start + 5);

        int space_pos = 0;
        while( ( space_pos = link.find(" ") ) >= 0 ) link.replace( space_pos, 1, "%20" );

        cout << "Link: " << link << endl;

        string data = download_single_url( link );

        if( data.empty() ) return;

        start = data.find( "<li><div><b class=\"glowka\"><a href=\"" ); // strlen - 39 - 3 = 36
        if( start < 0 ) return;
        start += 36;

        while( data[start] != '<' || data[start+1] != '/' || data[start+2] != 'b' || data[start+3] != '>' ) start++; // </b>
        start += 5;

        int end = data.find( "</div></li>" );
        if( end < 0 ) return;

        string sjpdata = data.substr( start, end - 1 - start );

        cout << "sjpdata: " << sjpdata << endl;

        int delete_pos = 0;
        int delete_end_pos = 0;
        while( ( delete_pos = sjpdata.find("<") ) >= 0 ) {
            delete_end_pos = delete_pos;
            while( sjpdata[ delete_end_pos++ ] != '>' );
            sjpdata.erase( delete_pos, delete_end_pos - delete_pos );
        }

        while( ( delete_pos = sjpdata.find("&#171;") ) >= 0 ) sjpdata.erase( delete_pos, 6 );
        while( ( delete_pos = sjpdata.find("&#187;") ) >= 0 ) sjpdata.erase( delete_pos, 6 );
        while( ( delete_pos = sjpdata.find("&#187") ) >= 0 ) sjpdata.erase( delete_pos, 6 );
        while( ( delete_pos = sjpdata.find("&#8226;") ) >= 0 ) sjpdata.erase( delete_pos, 7 );

        // czy jest spacja na start
        if( sjpdata[0] == ' ' ) sjpdata.erase( 0, 1 );

        // -----------------   ą     Ą     ć     Ć     ę     Ę     ł     Ł     ń     Ń     ó     Ó     ś     Ś     ż     Ż     ź     Ź
        short iso_chars[] =  { 0xB1, 0xA1, 0xE6, 0xC6, 0xEA, 0xCA, 0xB3, 0xA3, 0xF1, 0xD1, 0xF3, 0xD3, 0xB6, 0xA6, 0xBF, 0xAF, 0xBC, 0xAC, 0xA0 };
        short ansi_chars[] = { 0x61, 0x41, 0x63, 0x43, 0x65, 0x45, 0x6C, 0x4C, 0x6E, 0x4E, 0x6F, 0x4F, 0x73, 0x53, 0x7A, 0x5A, 0x7A, 0x5A, ' ' };

        unsigned int i = 0, j = 0;
        for( i = 0; i < sjpdata.length(); i++ ) {
            for( j = 0; j < ( sizeof(ansi_chars)/sizeof(ansi_chars[0]) ); j++ ) {
                if( sjpdata[i] == (char)iso_chars[j] ) {
                    sjpdata[i] = ansi_chars[j];
                    break;
                }
            }
        }

        string buf = "PRIVMSG #";
        buf += channel;
        buf += " :";
        buf += sjpdata;
        buf += "... ";
        buf += link;
        buf += "\r\n";

        circ->send_d( buf );

    }
}