// BESContainerStorageFile.cc

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

#include <sstream>
#include <fstream>
#include <iostream>

using std::stringstream ;
using std::ifstream ;

#include "BESContainerStorageFile.h"
#include "BESContainer.h"
#include "TheBESKeys.h"
#include "BESContainerStorageException.h"
#include "BESInfo.h"

/** @brief pull container information from the specified file
 *
 * Constructs a BESContainerStorageFile from a file specified by
 * a key in the dods initialization file. The key is constructed using the
 * name of this persistent store.
 *
 * OPeNDAP.Container.Persistence.File.&lt;name&gt;
 *
 * where &lt;name&gt; is the name of this persistent store.
 *
 * The containers are then read into memory. The format of the file is as
 * follows.
 *
 * &lt;symbolic_name&gt; &lt;real_name&gt; &lt;data type&gt;
 *
 * where the symbolic name is the symbolic name of the container, the
 * &lt;real_name&gt; represents the physical location of the data, such as
 * a file, and the &lt;data type&gt; is the type of data being represented,
 * such as netcdf, cedar, etc...
 *
 * One container per line, can not span multiple lines
 *
 * @param n name of this persistent store
 * @throws BESContainerStorageException if the file can not be opened or
 * if there is an error in reading in the container information.
 * @see BESContainerStorage
 * @see BESContainer
 * @see BESContainerStorageException
 */
BESContainerStorageFile::BESContainerStorageFile( const string &n )
    : BESContainerStorage( n )
{
    string key = "OPeNDAP.Container.Persistence.File." + n ;
    bool found = false ;
    string my_file = TheBESKeys::TheKeys()->get_key( key, found ) ;
    if( my_file == "" )
    {
	string s = key + " not defined in key file" ;
	throw BESContainerStorageException( s, __FILE__, __LINE__ ) ;
    }

    ifstream persistence_file( my_file.c_str() ) ;
    if( !persistence_file )
    {
	string s = "Unable to open persistence file " + my_file ;
	throw BESContainerStorageException( s, __FILE__, __LINE__ ) ;
    }

    char cline[80] ;

    while( !persistence_file.eof() )
    {
	stringstream strm ;
	persistence_file.getline( cline, 80 ) ;
	if( !persistence_file.eof() )
	{
	    strm << cline ;
	    BESContainerStorageFile::container *c =
		new BESContainerStorageFile::container ;
	    strm >> c->_symbolic_name ;
	    strm >> c->_real_name ;
	    strm >> c->_container_type ;
	    string dummy ;
	    strm >> dummy ;
	    if( c->_symbolic_name == "" ||
		c->_real_name == "" ||
		c->_container_type == "" )
	    {
		delete c ;
		string s = "Incomplete container persistence line in file "
			   + my_file ;
		throw BESContainerStorageException( s, __FILE__, __LINE__ ) ;
	    }
	    if( dummy != "" )
	    {
		delete c ;
		string s = "Too many fields in persistence file "
			   + my_file ;
		throw BESContainerStorageException( s, __FILE__, __LINE__ ) ;
	    }
	    _container_list[c->_symbolic_name] = c ;
	}
    }
    persistence_file.close() ;
}

BESContainerStorageFile::~BESContainerStorageFile()
{
    BESContainerStorageFile::Container_citer i = _container_list.begin() ;
    BESContainerStorageFile::Container_citer ie = _container_list.end() ;
    for( ; i != ie; i++ )
    {
	BESContainerStorageFile::container *c = (*i).second ;
	delete c ;
    }
}

/** @brief looks for the specified container in the list of containers loaded
 * from the file.
 *
 * If a match is made with the symbolic name found in the container then the
 * information is stored in the passed container object and the is_valid flag
 * is set to true. If not found, then is_valid is set to false.
 *
 * @param d container to look for and, if found, store the information in.
 * @see BESContainer
 */
void
BESContainerStorageFile::look_for( BESContainer &d )
{
    d.set_valid_flag( false ) ;
    BESContainerStorageFile::Container_citer i ;
    i = _container_list.find( d.get_symbolic_name() ) ;
    if( i != _container_list.end() )
    {
	BESContainerStorageFile::container *c = (*i).second;
	d.set_real_name( c->_real_name ) ;
	d.set_container_type( c->_container_type ) ;
	d.set_valid_flag( true ) ;
    }
}

/** @brief adds a container with the provided information
 *
 * This method adds a container to the persistence store with the
 * specified information. This functionality is not currently supported for
 * file persistence.
 *
 * @param s_name symbolic name for the container
 * @param r_name real name for the container
 * @param type type of data represented by this container
 */
void
BESContainerStorageFile::add_container( const string &,
				     const string &,
				     const string & )
{
    throw BESContainerStorageException( "Unable to add a container to a file, not yet implemented", __FILE__, __LINE__ ) ;
}

/** @brief removes a container with the given symbolic name
 *
 * This method removes a container to the persistence store with the
 * given symbolic name. It deletes the container. The container is NOT
 * removed from the file from which it was loaded, however.
 *
 * @param s_name symbolic name for the container
 * @return true if successfully removed and false otherwise
 */
bool
BESContainerStorageFile::del_container( const string &s_name )
{
    bool ret = false ;
    BESContainerStorageFile::Container_iter i ;
    i = _container_list.find( s_name ) ;
    if( i != _container_list.end() )
    {
	BESContainerStorageFile::container *c = (*i).second;
	_container_list.erase( i ) ;
	delete c ;
	ret = true ;
    }
    return ret ;
}

/** @brief removes all container
 *
 * This method removes all containers from the persistent store. It does
 * not delete the real data behind the container.
 *
 * @return true if successfully removed and false otherwise
 */
bool
BESContainerStorageFile::del_containers( )
{
    while( _container_list.size() != 0 )
    {
	Container_iter ci = _container_list.begin() ;
	BESContainerStorageFile::container *c = (*ci).second;
	_container_list.erase( ci ) ;
	if( c )
	{
	    delete c ;
	}
    }
    return true ;
}

/** @brief show information for each container in this persistent store
 *
 * For each container in this persistent store, add infomation about each of
 * those containers. The information added to the information object
 * includes a line for each container within this persistent store which 
 * includes the symbolic name, the real name, and the data type, 
 * separated by commas.
 *
 * In the case of this persistent store all of the containers loaded from
 * the file specified by the key
 * OPeNDAP.Container.Persistence.File.&lt;store_name&gt;
 * is added to the information object.
 *
 * @param info object to store the container and persistent store information into
 * @see BESInfo
 */
void
BESContainerStorageFile::show_containers( BESInfo &info )
{
    info.add_tag( "name", get_name() ) ;
    BESContainerStorageFile::Container_citer i ;
    i = _container_list.begin() ;
    for( i = _container_list.begin(); i != _container_list.end(); i++ )
    {
	info.begin_tag( "container" ) ;
	BESContainerStorageFile::container *c = (*i).second;
	string sym = c->_symbolic_name ;
	info.add_tag( "symbolicName", sym ) ;
	string real = c->_real_name ;
	info.add_tag( "realName", real ) ;
	string type = c->_container_type ;
	info.add_tag( "dataType", type ) ;
	info.end_tag( "container" ) ;
    }
}

