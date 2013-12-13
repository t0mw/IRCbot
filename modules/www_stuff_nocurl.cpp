#include <iostream>
#include <cstdlib>
#include <cstring>
#include <fstream>

#include "www_stuff.hpp"

using namespace std;

string parsetitle( string data ) {
	string out;
	int start = 0, end = 0;

	start = data.find( "<title>" );
	end = data.find( "</title>" );

	if( start < 0 || end < 0 ) return string();

	out = data.substr( start + 7, end - start - 7 );	// 7 == strlen( "<title>" )

	return out;
}

string download_single_url( string url ) {

	string ret;

	////////////////////////////////////////////////////////////////////// test

	#ifdef DEBUG
		cout << "DLOAD URL: " << url << endl;
	#endif
	
	// > < | ` $ \ " '
	if( url.find(">") != string::npos || url.find("<") != string::npos || url.find("|") != string::npos || url.find("`") != string::npos || url.find("$") != string::npos || url.find("\\") != string::npos || url.find("\"") != string::npos || url.find("'") != string::npos ) return string();

	int pos = 0;
	if( pos = url.find("https") ) {
		url.replace( pos, pos + 5, "http" );
	}
	
	string exec = "wget -O plik.txt -q -t 1 -T 5 ";
	exec += url;
	
	#ifdef DEBUG
		cout << "EXEC: " << exec << endl;
	#endif
	
	system( exec.c_str() );
	
	ifstream file;
	file.open( "plik.txt" );
	string buf;
	
	while( !file.eof() ) {
		getline( file, buf );
		ret += buf;
	}
	
	cout << endl << ret << endl;
	
	system( "rm plik.txt" );
	
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
	
		// cout << "\n\n\n" << "Got link: `" << link << "`\n\n\n";
	
		return link;
		
	} else {
		return string();
	}

}



