// agglistT.C

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

#include <cppunit/TextTestRunner.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/extensions/HelperMacros.h>

using namespace CppUnit ;

#include <iostream>
#include <cstdlib>

using std::cerr ;
using std::cout ;
using std::endl ;

#include "BESAggFactory.h"
#include "BESTextInfo.h"
#include "BESError.h"
#include "TestAggServer.h"
#include "TheBESKeys.h"
#include <test_config.h>

class agglistT: public TestFixture {
private:

public:
    agglistT() {}
    ~agglistT() {}

    static BESAggregationServer *
    h1( string name )
    {
	return new TestAggServer( name ) ;
    }

    static BESAggregationServer *
    h2( string name )
    {
	return new TestAggServer( name ) ;
    }

    static BESAggregationServer *
    h3( string name )
    {
	return new TestAggServer( name ) ;
    }

    void setUp()
    {
	string bes_conf = (string)TEST_SRC_DIR + "/empty.ini" ;
	TheBESKeys::ConfigFile = bes_conf ;
    }

    void tearDown()
    {
    }

    CPPUNIT_TEST_SUITE( agglistT ) ;

    CPPUNIT_TEST( do_test ) ;

    CPPUNIT_TEST_SUITE_END() ;

    void do_test()
    {
	cout << "*****************************************" << endl;
	cout << "Entered agglistT::run" << endl;

	cout << "*****************************************" << endl;
	cout << "Adding three handlers to the list" << endl ;
	try
	{
	    CPPUNIT_ASSERT( BESAggFactory::TheFactory()->add_handler( "h1", agglistT::h1 ) ) ;
	    CPPUNIT_ASSERT( BESAggFactory::TheFactory()->add_handler( "h2", agglistT::h2 ) ) ;
	    CPPUNIT_ASSERT( BESAggFactory::TheFactory()->add_handler( "h3", agglistT::h3 ) ) ;
	}
	catch( BESError &e )
	{
	    cerr << e.get_message() << endl ;
	    CPPUNIT_ASSERT( !"Failed to add aggregation servers to list" ) ;
	}

	cout << "*****************************************" << endl;
	cout << "Find the three handlers" << endl ;
	try
	{
	    BESAggregationServer *s = 0 ;
	    s = BESAggFactory::TheFactory()->find_handler( "h1" ) ;
	    CPPUNIT_ASSERT( s ) ;
	    s = BESAggFactory::TheFactory()->find_handler( "h2" ) ;
	    CPPUNIT_ASSERT( s ) ;
	    s = BESAggFactory::TheFactory()->find_handler( "h3" ) ;
	    CPPUNIT_ASSERT( s ) ;
	}
	catch( BESError &e )
	{
	    cerr << e.get_message() << endl ;
	    CPPUNIT_ASSERT( !"Failed to find aggregation servers" ) ;
	}

	cout << "*****************************************" << endl;
	cout << "Remove handler h2" << endl ;
	try
	{
	    bool removed = BESAggFactory::TheFactory()->remove_handler( "h2" ) ;
	    CPPUNIT_ASSERT( removed ) ;
	}
	catch( BESError &e )
	{
	    cerr << e.get_message() << endl ;
	    CPPUNIT_ASSERT( !"Failed to remove aggregation server h2" ) ;
	}

	cout << "*****************************************" << endl;
	cout << "Find the two handlers" << endl ;
	try
	{
	    BESAggregationServer *s = 0 ;
	    s = BESAggFactory::TheFactory()->find_handler( "h1" ) ;
	    CPPUNIT_ASSERT( s ) ;
	    s = BESAggFactory::TheFactory()->find_handler( "h2" ) ;
	    CPPUNIT_ASSERT( !s ) ;
	    s = BESAggFactory::TheFactory()->find_handler( "h3" ) ;
	    CPPUNIT_ASSERT( s ) ;
	}
	catch( BESError &e )
	{
	    cerr << "Failed to find aggregation servers" << endl ;
	    cerr << e.get_message() << endl ;
	}

	cout << "*****************************************" << endl;
	cout << "Show handler names registered" << endl;
	string registered = BESAggFactory::TheFactory()->get_handler_names() ;
	string expected = "h1, h3" ;
	CPPUNIT_ASSERT( registered == expected ) ;

	cout << "*****************************************" << endl;
	cout << "Returning from agglistT::run" << endl;
    }
} ;

CPPUNIT_TEST_SUITE_REGISTRATION( agglistT ) ;

int 
main( int, char** )
{
    CppUnit::TextTestRunner runner ;
    runner.addTest( CppUnit::TestFactoryRegistry::getRegistry().makeTest() ) ;

    bool wasSuccessful = runner.run( "", false )  ;

    return wasSuccessful ? 0 : 1 ;
}


