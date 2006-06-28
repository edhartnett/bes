// BESHeapManager.cc

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

#include <string.h>
#include <errno.h>
#include <iostream>

using std::cerr ;
using std::cout ;
using std::endl ;

#include "BESHeapManager.h"

#define BES_MEGABYTE 1024*1024

int BESHeapManager::counter=0;

BESHeapManager::BESHeapManager()
{
    if (counter++ == 0) 
    {
	limit.rlim_cur=megabytes(3);
	limit.rlim_max=megabytes(3);
	if(setrlimit(RLIMIT_DATA, &limit)<0)
	{
	    cerr << "OPeNDAP: Could not set limit for the heap because "
		 << strerror(errno)
		 << endl;
	    exit(1);
	}
    }
}

BESHeapManager::~BESHeapManager()
{
    --counter;
}
  
unsigned long BESHeapManager::megabytes(unsigned int s) 
{
    return BES_MEGABYTE * s ;
}

void BESHeapManager::show_limits()
{
    if (getrlimit(RLIMIT_DATA, &limit)<0)
    {
	cerr << "Could not get limit" <<endl;
	exit(1);
    }
    if(limit.rlim_cur==RLIM_INFINITY)
	cout << "I have infinity soft" <<endl;
    else
	cout << "I have soft limit " <<limit.rlim_cur<<endl;
    if(limit.rlim_max==RLIM_INFINITY)
	cout << "I have infinity hard" <<endl;
    else
	cout << "I have hard limit " <<limit.rlim_max<<endl;
}

static BESHeapManager heapmanager;

#ifdef BES_TEST_MAIN
int main()
{
    cout<<"Entering in main" <<endl;
    void *p=0;
    BESHeapManager heapmanager;
    heapmanager.show_limits();
    struct mallinfo info;
    info=mallinfo();
    cout << info.arena << endl ;

    try
    {
	p= ::operator new(3*BES_MEGABYTE);
    }
    catch(bad_alloc::bad_alloc)
    {
	cout << "Got a bad alloc exception" <<endl;
    }
    info = mallinfo();
    cout << info.arena << endl ;
    ::operator delete (p);
    info = mallinfo();
    cout << info.arena << endl;
}
#endif

