// DirectoryCatalog.cc

// This file is part of bes, A C++ back-end server implementation framework
// for the OPeNDAP Data Access Protocol.

// Copyright (c) 2004,2005 University Corporation for Atmospheric Research
// Author: Patrick West <pwest@ucar.org>
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

#include "sys/types.h"
#include "sys/stat.h"
#include "dirent.h"
#include "stdio.h"

#include <sstream>

using std::stringstream ;

#include "DirectoryCatalog.h"
#include "TheDODSKeys.h"
#include "DODSTextInfo.h"
#include "DODSResponseException.h"

DirectoryCatalog::DirectoryCatalog( const string &key )
{
    bool found = false ;
    _rootDir = TheDODSKeys::TheKeys()->get_key( key, found ) ;
    if( !found || _rootDir == "" )
    {
	string serr = "DirectoryCatalog - unable to load root directory key "
		      + key + " from initialization file" ;
	DODSResponseException e( serr ) ;
	throw e ;
    }

    DIR *dip = opendir( _rootDir.c_str() ) ;
    if( dip == NULL )
    {
	string serr = "DirectoryCatalog - unable to load root directory "
	              + _rootDir ;
	DODSResponseException e( serr ) ;
	throw e ;
    }
    closedir( dip ) ;
}

DirectoryCatalog::~DirectoryCatalog( )
{
}

bool
DirectoryCatalog::show_catalog( const string &container, DODSTextInfo *info )
{
    string newdir ;
    if( container == "" )
    {
	newdir = _rootDir ;
    }
    else
    {
	newdir = _rootDir + "/" + container ;
    }
    DIR *dip = opendir( newdir.c_str() ) ;
    if( dip != NULL )
    {
	struct dirent *dit;
	while( ( dit = readdir( dip ) ) != NULL )
	{
	    struct stat buf;
	    string dirEntry = dit->d_name ;
	    if( dirEntry != "." && dirEntry != ".." )
	    {
		string fullPath = newdir + "/" + dirEntry ;
		stat( fullPath.c_str(), &buf ) ;

		// look at the mode and determine if this is a directory
		if ( S_ISDIR( buf.st_mode ) )
		{
		    info->add_data( "        <dataset container=\"true\">\n" ) ;

		    stringstream snm ;
		    snm << "            <name>" << dirEntry << "</name>\n" ;
		    info->add_data( snm.str() ) ;

		    off_t sz = buf.st_size ;
		    stringstream ssz ;
		    ssz << "            <size>" << sz << "</size>\n" ;
		    info->add_data( ssz.str() ) ;

		    // %T = %H:%M:%S
		    // %F = %Y-%m-%d
		    time_t mod = buf.st_mtime ;
		    struct tm *stm = gmtime( &mod ) ;
		    char mdate[64] ;
		    strftime( mdate, 64, "%F", stm ) ;
		    char mtime[64] ;
		    strftime( mtime, 64, "%T", stm ) ;
		    info->add_data( "            <lastmodified>\n" ) ;
		    stringstream sdt ;
		    sdt << "                <date>" << mdate << "</date>\n" ;
		    info->add_data( sdt.str() ) ;
		    stringstream stt ;
		    stt << "                <time>" << mtime << "</time>\n" ;
		    info->add_data( stt.str() ) ;
		    info->add_data( "            </lastmodified>\n" ) ;

		    info->add_data( "        </dataset>\n" ) ;
		}
		else if ( S_ISREG( buf.st_mode ) )
		{
		    info->add_data( "        <dataset container=\"false\">\n" ) ;

		    stringstream snm ;
		    snm << "            <name>" << dirEntry << "</name>\n" ;
		    info->add_data( snm.str() ) ;

		    off_t sz = buf.st_size ;
		    stringstream ssz ;
		    ssz << "            <size>" << sz << "</size>\n" ;
		    info->add_data( ssz.str() ) ;

		    // %T = %H:%M:%S
		    // %F = %Y-%m-%d
		    time_t mod = buf.st_mtime ;
		    struct tm *stm = gmtime( &mod ) ;
		    char mdate[64] ;
		    strftime( mdate, 64, "%F", stm ) ;
		    char mtime[64] ;
		    strftime( mtime, 64, "%T", stm ) ;
		    info->add_data( "            <lastmodified>\n" ) ;
		    stringstream sdt ;
		    sdt << "                <date>" << mdate << "</date>\n" ;
		    info->add_data( sdt.str() ) ;
		    stringstream stt ;
		    stt << "                <time>" << mtime << "</time>\n" ;
		    info->add_data( stt.str() ) ;
		    info->add_data( "            </lastmodified>\n" ) ;

		    info->add_data( "        </dataset>\n" ) ;
		}
	    }
	}
	closedir( dip ) ;
    }
    else
    {
	struct stat buf;
	int statret = stat( newdir.c_str(), &buf ) ;
	if ( statret == 0 && S_ISREG( buf.st_mode ) )
	{
	    info->add_data( "        <dataset container=\"false\">\n" ) ;

	    stringstream snm ;
	    snm << "            <name>" << container << "</name>\n" ;
	    info->add_data( snm.str() ) ;

	    off_t sz = buf.st_size ;
	    stringstream ssz ;
	    ssz << "            <size>" << sz << "</size>\n" ;
	    info->add_data( ssz.str() ) ;

	    // %T = %H:%M:%S
	    // %F = %Y-%m-%d
	    time_t mod = buf.st_mtime ;
	    struct tm *stm = gmtime( &mod ) ;
	    char mdate[64] ;
	    strftime( mdate, 64, "%F", stm ) ;
	    char mtime[64] ;
	    strftime( mtime, 64, "%T", stm ) ;
	    info->add_data( "            <lastmodified>\n" ) ;
	    stringstream sdt ;
	    sdt << "                <date>" << mdate << "</date>\n" ;
	    info->add_data( sdt.str() ) ;
	    stringstream stt ;
	    stt << "                <time>" << mtime << "</time>\n" ;
	    info->add_data( stt.str() ) ;
	    info->add_data( "            </lastmodified>\n" ) ;

	    info->add_data( "        </dataset>\n" ) ;
	}
	else
	{
	    return false ;
	}
    }

    return true ;
}

// $Log: DirectoryCatalog.cc,v $
