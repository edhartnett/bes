// BESServerHandler.cc

// This file is part of bes, A C++ back-end server implementation framework
// for the OPeNDAP Data Access Protocol.

// Copyright (c) 2004-2009 University Corporation for Atmospheric Research
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

// (c) COPYRIGHT University Corporation for Atmospheric Research 2004-2005
// Please read the full copyright statement in the file COPYRIGHT_UCAR.
//
// Authors:
//      pwest       Patrick West <pwest@ucar.edu>
//      jgarcia     Jose Garcia <jgarcia@ucar.edu>

#include <unistd.h>    // for getpid fork sleep
#include <sys/types.h>
#include <sys/socket.h>
#include <signal.h>
#include <sys/wait.h>  // for waitpid

#include <cstring>
#include <cstdlib>
#include <cerrno>
#include <sstream>
#include <iostream>

using std::ostringstream ;
using std::cout ;
using std::endl ;
using std::cerr ;
using std::flush ;

#include "BESServerHandler.h"
#include "Connection.h"
#include "Socket.h"
#include "BESXMLInterface.h"
#include "TheBESKeys.h"
#include "BESInternalError.h"
#include "ServerExitConditions.h"
#include "BESUtil.h"
#include "PPTStreamBuf.h"
#include "PPTProtocol.h"
#include "BESDebug.h"
#include "BESStopWatch.h"

BESServerHandler::BESServerHandler()
{
    bool found = false ;
    try
    {
	TheBESKeys::TheKeys()->get_value( "BES.ProcessManagerMethod",
					  _method, found ) ;
    }
    catch( BESError &e )
    {
	cerr << "Unable to determine method to handle clients, "
	     << "single or multiple as defined by BES.ProcessManagerMethod"
	     << ": " << e.get_message() << endl ;
	exit( SERVER_EXIT_FATAL_CAN_NOT_START ) ;
    }
    if( _method != "multiple" && _method != "single" )
    {
	cerr << "Unable to determine method to handle clients, "
	     << "single or multiple as defined by BES.ProcessManagerMethod"
	     << endl ;
	exit( SERVER_EXIT_FATAL_CAN_NOT_START ) ;
    }
}

// *** I'm not sure that we need to fork twice. jhrg 11/14/05
// The reason that we fork twice is explained in Advanced Programming in the
// Unit Environment by W. Richard Stevens. In the 'multiple' case we don't
// want to leave any zombie processes.
void
BESServerHandler::handle( Connection *c )
{
    if(_method=="single")
    {
	// we're in single mode, so no for and exec is needed. One
	// client connection and we are done.
	execute( c ) ;
    }
    else
    {
	// multi-process mode, so fork and exec.
	int main_process = getpid() ;
	pid_t pid ;
	if( ( pid = fork() ) < 0 )
	{
	    string error( "fork error" ) ;
	    const char* error_info = strerror( errno ) ;
	    if( error_info )
		error += " " + (string)error_info ;
	    throw BESInternalError( error, __FILE__, __LINE__ ) ;
	}
	else if( pid == 0 ) /* child process */
	{
	    pid_t pid1 ;
	    // we fork twice so we do not have zombie children
	    if( ( pid1 = fork() ) < 0 )
	    {
		// we must send a signal of immediate termination to the
		// main server
		kill( main_process, 9 ) ;
		perror( "fork error" ) ;
		exit( SERVER_EXIT_CHILD_SUBPROCESS_ABNORMAL_TERMINATION ) ;
	    }
	    else if( pid1 == 0 ) /* child of the child */
	    {
		// execute given the connection. The execute method does
		// the listen and handles input/output, etc...
		execute( c ) ;
	    }
	    sleep( 1 ) ;
	    c->closeConnection() ;
	    exit( SERVER_EXIT_CHILD_SUBPROCESS_NORMAL_TERMINATION ) ;
	}
	if( waitpid( pid, NULL, 0 ) != pid )
	{
	    string error( "waitpid error" ) ;
	    const char *error_info = strerror( errno ) ;
	    if( error_info )
		error += " " + (string)error_info ;
	    throw BESInternalError( error, __FILE__, __LINE__ ) ;
	}
	c->closeConnection() ;
    }
}

void
BESServerHandler::execute( Connection *c )
{
    ostringstream strm ;
    string ip = c->getSocket()->getIp() ;
    strm << "ip " << ip << ", port " << c->getSocket()->getPort() ;
    string from = strm.str() ;

    map<string,string> extensions ;

    // we loop continuously waiting for messages. The only way we exit
    // this loop is 1. we receive a status of exit from the client, 2.
    // the client drops the connection, the process catches the signal
    // and exits, 3. a fatal error has occurred in the server so exit,
    // 4. the server process is killed.
    for(;;)
    {
	ostringstream ss ;

	bool done = false ;
	while( !done )
	    done = c->receive( extensions, &ss ) ;

	// The server has been sent a message that the client is exiting
	// and closing the connection. So exit this process.
	if( extensions["status"] == c->exit() )
	{
	    c->closeConnection() ;
	    exit( CHILD_SUBPROCESS_READY ) ;
	}

	// This is code that was in place for the string commands. With xml
	// documents everything is taken care of by libxml2. This should be
	// happening in the Interface class before passing to the parser, if
	// need be. pwest 06 Feb 2009
	//string cmd_str = BESUtil::www2id( ss.str(), "%", "%20" ) ;
	string cmd_str = ss.str() ;
	BESDEBUG( "server", "BESServerHandler::execute - command = "
			    << cmd_str << endl ) ;

	BESStopWatch *sw = 0 ;
	if( BESISDEBUG( "timing" ) )
	{
	    sw = new BESStopWatch() ;
	    sw->start() ;
	}

	int descript = c->getSocket()->getSocketDescriptor() ;

	unsigned int bufsize = c->getSendChunkSize() ;
	PPTStreamBuf fds( descript, bufsize ) ;
	std::streambuf *holder ;
	holder = cout.rdbuf() ;
	cout.rdbuf( &fds ) ;

	BESXMLInterface cmd( cmd_str, &cout ) ;
	int status = cmd.execute_request( from ) ;

	if( status == 0 )
	{
	    fds.finish() ;

	    cout.rdbuf( holder ) ;

	    BESDEBUG( "server", "BESServerHandler::execute - "
				<< "executed successfully" << endl ) ;

	    if( BESISDEBUG( "timing" ) )
	    {
		if( sw && sw->stop() )
		{
		    BESDEBUG( "timing",
			      "BESServerHandler::execute - executed in "
			      << sw->seconds() << " seconds and "
			      << sw->microseconds() << " microseconds"
			      << endl ) ;
		}
		else
		{
		    BESDEBUG( "timing", "BESServerHandler::execute - "
					<< "no timing available" << endl ) ;
		}
	    }
	}
	else
	{
	    // an error has occurred.
	    BESDEBUG( "server", "BESServerHandler::execute - "
				<< "error occurred" << endl ) ;

	    // flush what we have in the stream to the client
	    cout << flush ;

	    // Send the extension status=error to the client so that it
	    // can reset.
	    map<string,string> extensions ;
	    extensions["status"] = "error" ;
	    if( status == BES_INTERNAL_FATAL_ERROR )
	    {
		extensions["exit"] = "true" ;
	    }
	    c->sendExtensions( extensions ) ;

	    // transmit the error message. finish_with_error will transmit
	    // the error
	    cmd.finish_with_error( status ) ;

	    // we are finished, send the last chunk
	    fds.finish() ;

	    // reset the streams buffer
	    cout.rdbuf( holder ) ;

	    // If the status is fatal, then we want to exit. Otherwise,
	    // continue, wait for the next request.
	    switch (status)
	    {
		case BES_INTERNAL_FATAL_ERROR:
		    {
			cout << "BES server " << getpid()
			     << ": Status not OK, dispatcher returned value "
			     << status << endl ;
			c->closeConnection() ;
			exit( CHILD_SUBPROCESS_READY ) ;
		    }
		    break;
		case BES_INTERNAL_ERROR:
		case BES_SYNTAX_USER_ERROR:
		case BES_FORBIDDEN_ERROR:
		case BES_NOT_FOUND_ERROR:
		default:
		    break;
	    }
	}

	if( sw ) delete sw;
    }
}

/** @brief dumps information about this object
 *
 * Displays the pointer value of this instance
 *
 * @param strm C++ i/o stream to dump the information to
 */
void
BESServerHandler::dump( ostream &strm ) const
{
    strm << BESIndent::LMarg << "BESServerHandler::dump - ("
			     << (void *)this << ")" << endl ;
    BESIndent::Indent() ;
    strm << BESIndent::LMarg << "server method: " << _method << endl ;
    BESIndent::UnIndent() ;
}

