// BESHTMLInfo.h

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

#ifndef BESHTMLInfo_h_
#define BESHTMLInfo_h_ 1

#include "BESInfo.h"

/** @brief represents an html formatted response object
 *
 * Uses the default add_data method, but overwrites print method in order to
 * set the mime type to html.
 *
 * @see BESInfo
 * @see DODSResponseObject
 */
class BESHTMLInfo : public BESInfo {
private:
    bool		_header ;
    string		_indent ;
    bool		_do_indent ;
public:
  			BESHTMLInfo( ) ;
  			BESHTMLInfo( const string &key ) ;
    virtual 		~BESHTMLInfo() ;

    virtual void	begin_response( const string &response_name ) ;
    virtual void	end_response( ) ;

    virtual void	add_tag( const string &tag_name,
                                 const string &tag_data,
				 map<string,string> *attrs = 0 ) ;
    virtual void	begin_tag( const string &tag_name,
				   map<string,string> *attrs = 0 ) ;
    virtual void	end_tag( const string &tag_name ) ;

    virtual void 	add_data( const string &s ) ;
    virtual void	add_space( unsigned long num_spaces ) ;
    virtual void	add_break( unsigned long num_breaks ) ;

    virtual void 	add_data_from_file( const string &key,
                                            const string &name ) ;
    virtual void	transmit( BESTransmitter *transmitter,
				  BESDataHandlerInterface &dhi ) ;

    static BESInfo *BuildHTMLInfo( const string &info_type ) ;
};

#endif // BESHTMLInfo_h_

