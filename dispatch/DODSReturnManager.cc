// DODSReturnManager.cc

// This file is part of bes, A C++ back-end server implementation framework
// for the OPeNDAP Data Access Protocol.

// Copyright (c) 2004,2005 University Corporation for Atmospheric Research
// Author: Patrick West <pwest@ucar.org> and Jose Garcia <jgarcia@ucar.org>
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

#include "DODSReturnManager.h"

DODSReturnManager *DODSReturnManager::_instance = 0 ;

DODSReturnManager::DODSReturnManager()
{
}

DODSReturnManager::~DODSReturnManager()
{
    DODSReturnManager::Transmitter_iter i ;
    DODSTransmitter *t = 0 ;
    for( i = _transmitter_list.begin(); i != _transmitter_list.end(); i++ )
    {
	t = (*i).second ;
	delete t ;
    }
}

bool
DODSReturnManager::add_transmitter( const string &name,
				    DODSTransmitter *transmitter )
{
    if( find_transmitter( name ) == 0 )
    {
	_transmitter_list[name] = transmitter ;
	return true ;
    }
    return false ;
}

DODSTransmitter *
DODSReturnManager::rem_transmitter( const string &name )
{
    DODSTransmitter *ret = 0 ;
    DODSReturnManager::Transmitter_iter i ;
    i = _transmitter_list.find( name ) ;
    if( i != _transmitter_list.end() )
    {
	ret = (*i).second;
	_transmitter_list.erase( i ) ;
    }
    return ret ;
}

DODSTransmitter *
DODSReturnManager::find_transmitter( const string &name )
{
    DODSReturnManager::Transmitter_citer i ;
    i = _transmitter_list.find( name ) ;
    if( i != _transmitter_list.end() )
    {
	return (*i).second;
    }
    return 0 ;
}

DODSReturnManager *
DODSReturnManager::TheManager()
{
    if( _instance == 0 )
    {
	_instance = new DODSReturnManager ;
    }
    return _instance ;
}

