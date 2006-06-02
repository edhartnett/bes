// DODSProcessEncodedString.cc

// This file is part of bes, A C++ back-end server implementation framework
// for the OPeNDAP Data Access Protocol.

// Copyright (c) 2004,2005 University Corporation for Atmospheric Research
// Author: Patrick West <pwest@ucar.edu> and Jose Garcia <jgarcia@ucar.edu>
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
// 
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
// You can contact University Corporation for Atmospheric Research at
// 3080 Center Green Drive, Boulder, CO 80301
 
// (c) COPYRIGHT University Corporation for Atmostpheric Research 2004-2005
// Please read the full copyright statement in the file COPYRIGHT_UCAR.
//
// Authors:
//      pwest       Patrick West <pwest@ucar.edu>
//      jgarcia     Jose Garcia <jgarcia@ucar.edu>

#include "DODSProcessEncodedString.h"

using std::cerr ;

DODSProcessEncodedString::DODSProcessEncodedString (const char *s)
{
    if (s)
    {
	string key = "" ;
	string value = "" ;
	bool getting_key_data = true ;
	size_t len = strlen( s ) ;
	for( unsigned int j = 0; j < len; j++ )
	{
	    if( getting_key_data )
	    {
		if( s[j] != '=' )
		{
		    key += s[j] ;
		}
		else
		{
		    getting_key_data = false ;
		    value = "" ;
		}
	    }
	    else
	    {
		if( s[j] != '&' )
		{
		    value += s[j] ;
		}
		else
		{
		    _entries[parseHex( key.c_str() )] = parseHex( value.c_str() ) ;
		    getting_key_data = true ;
		    key = "" ;
		}
	    }
	}
	if( getting_key_data )
	    cerr << "DODSProcessEncodedString: parse error.\n" ;
	else
	{
	    _entries[parseHex( key.c_str() )] = parseHex( value.c_str() ) ;
	}
    }
    else
    {
	cerr << "DODSProcessEncodedString: Passing NULL pointer.\n" ;
	exit( 1 ) ;
    }
}

string
DODSProcessEncodedString::parseHex( const char *s )
{ 
    char *hexstr = new char[strlen( s ) + 1] ;
    strcpy( hexstr, s ) ;

    if(hexstr == NULL || strlen( hexstr ) == 0 ) 
	return ""; 

    register unsigned int x,y; 
    for( x = 0, y = 0; hexstr[y]; x++, y++ ) 
    { 
	if( ( hexstr[x] = hexstr[y] ) == '+' ) 
	{ 
	    hexstr[x] = ' ' ;
	    continue ;
	}
	else if( hexstr[x] == '%' ) 
	{ 
	    hexstr[x] = convertHex( &hexstr[y+1] ) ; 
	    y += 2 ; 
	} 
    } 
    hexstr[x] = '\0';
    string w = hexstr ;
    delete [] hexstr ;
    return w ; 
} 

const unsigned int
DODSProcessEncodedString::convertHex( const char* what )
{ 
    //0x4f == 01001111 mask 

    register char digit; 
    digit = (what[0] >= 'A' ? ((what[0] & 0x4F) - 'A')+10 : (what[0] - '0')); 
    digit *= 16; 
    digit += (what[1] >='A' ? ((what[1] & 0x4F) - 'A')+10 : (what[1] - '0')); 

    return digit; 
} 

string
DODSProcessEncodedString::get_key( const string& s ) 
{
    map<string,string>::iterator i ;
    i = _entries.find( s ) ;
    if( i != _entries.end() )
	return (*i).second ;
    else
	return "" ;
}

// $Log: DODSProcessEncodedString.cc,v $
// Revision 1.2  2004/09/09 17:17:12  pwest
// Added copywrite information
//
// Revision 1.1  2004/06/30 20:16:24  pwest
// dods dispatch code, can be used for apache modules or simple cgi script
// invocation or opendap daemon. Built during cedar server development.
//
