// dispatch_test.cc

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

#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <iostream>
#include <string>

using std::cerr ;
using std::endl ;
using std::string;

#include "BESWrapper.h"
#include "BESGlobalIQ.h"
#include "BESException.h"
#include "BESDataRequestInterface.h"

string
get_ip()
{
    char name[1024] ;
    gethostname( name, 1024 ) ;

    struct hostent *hp = gethostbyname( name ) ;

    char **p = hp->h_addr_list ;
    struct in_addr in;
    (void)memcpy( &in.s_addr, *p, sizeof( in.s_addr ) ) ;
    string s = inet_ntoa( in ) ;

    return s ;
}

int
main( int argC, char **argV )
{
    try
    {
	BESGlobalIQ::BESGlobalInit( argC, argV ) ;
	BESDataRequestInterface rq ;
	BESWrapper wrapper ;

	// Initialize all data request elements
	rq.server_name="twilek";
	rq.server_address="pwest";
	rq.server_protocol="HTTP";
	rq.server_port="2004";
	rq.script_name="test";
	string ip = get_ip();
	rq.user_address=ip.c_str();
	rq.user_agent="pwest";
	rq.cookie="OpenDAP.remoteuser=pwest";
	char request[256] ;
	bool isempty = false ;
	if( argC < 2 )
	{
	    return 1 ;
	}
	if( !strcmp( argV[1], "version" ) )
	{
	    sprintf( request, "show version;" ) ;
	}
	else if( !strcmp( argV[1], "help" ) )
	{
	    sprintf( request, "show help;" ) ;
	}
	else if( !strcmp( argV[1], "process" ) )
	{
	    sprintf( request, "show process;" ) ;
	}
	else if( !strcmp( argV[1], "empty" ) )
	{
	    isempty = true ;
	}
	else if( !strcmp( argV[1], "status" ) )
	{
	    sprintf( request, "status;" ) ;
	}
	else if( argC == 4 )
	{
	    sprintf( request, "get %s for %s with %s;",
	             argV[1], argV[2], argV[3] ) ;
	}
	else
	{
	    sprintf( request, "define %s as %s;", argV[2], argV[2] ) ;
	    rq.request=request;
	    wrapper.call_BES( rq ) ;

	    sprintf( request, "get %s for %s;", argV[1], argV[2] ) ;
	}
	if( isempty )
	    rq.request = 0 ;
	else
	    rq.request=request;

	wrapper.call_BES(rq);

	BESGlobalIQ::BESGlobalQuit() ;
    }
    catch( BESException &e )
    {
	cerr << "Caught Exception" << endl ;
	cerr << e.get_message() << endl ;
    }

    return 0 ;
}

