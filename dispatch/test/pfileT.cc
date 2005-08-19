// pfileT.C

#include <iostream>

using std::cerr ;
using std::cout ;
using std::endl ;

#include "pfileT.h"
#include "DODSContainerPersistenceFile.h"
#include "DODSContainer.h"
#include "DODSException.h"
#include "DODSTextInfo.h"

int pfileT::
run(void)
{
    cout << endl << "*****************************************" << endl;
    cout << "Entered pfileT::run" << endl;
    int retVal = 0;

    cout << endl << "*****************************************" << endl;
    cout << "Try to get one called File" << endl;
    try
    {
	DODSContainerPersistenceFile cpf( "File" ) ;
	cerr << "opened file File, shouldn't have" << endl ;
	return 1 ;
    }
    catch( DODSException &ex )
    {
	cout << "couldn't get File, good, because" << endl ;
	cout << ex.get_error_description() << endl ;
    }

    cout << endl << "*****************************************" << endl;
    cout << "Try to get one called FileNot" << endl;
    try
    {
	DODSContainerPersistenceFile cpf( "FileNot" ) ;
	cerr << "opened file FileNot, shouldn't have" << endl ;
	return 1 ;
    }
    catch( DODSException &ex )
    {
	cout << "couldn't get FileNot, good, because" << endl ;
	cout << ex.get_error_description() << endl ;
    }

    cout << endl << "*****************************************" << endl;
    cout << "Try to get one called FileTooMany" << endl;
    try
    {
	DODSContainerPersistenceFile cpf( "FileTooMany" ) ;
	cerr << "opened file FileTooMany, shouldn't have" << endl ;
	return 1 ;
    }
    catch( DODSException &ex )
    {
	cout << "couldn't get FileTooMany, good, because" << endl ;
	cout << ex.get_error_description() << endl ;
    }

    cout << endl << "*****************************************" << endl;
    cout << "Try to get one called FileTooFew" << endl;
    try
    {
	DODSContainerPersistenceFile cpf( "FileTooFew" ) ;
	cerr << "opened file FileTooFew, shouldn't have" << endl ;
	return 1 ;
    }
    catch( DODSException &ex )
    {
	cout << "couldn't get FileTooFew, good, because" << endl ;
	cout << ex.get_error_description() << endl ;
    }

    cout << endl << "*****************************************" << endl;
    cout << "Get one called File1" << endl;
    try
    {
	DODSContainerPersistenceFile cpf( "File1" ) ;
	cout << "opened file File1, good" << endl ;
    }
    catch( DODSException &ex )
    {
	cerr << "couldn't get File1 because" << endl ;
	cerr << ex.get_error_description() << endl ;
	return 1 ;
    }

    DODSContainerPersistenceFile cpf( "File1" ) ;
    char s[10] ;
    char r[10] ;
    char c[10] ;
    for( int i = 1; i < 6; i++ )
    {
	sprintf( s, "sym%d", i ) ;
	sprintf( r, "real%d", i ) ;
	sprintf( c, "type%d", i ) ;
	cout << endl << "*****************************************" << endl;
	cout << "Looking for " << s << endl;
	DODSContainer d( s ) ;
	cpf.look_for( d ) ;
	if( d.is_valid() )
	{
	    if( d.get_real_name() == r && d.get_container_type() == c )
	    {
		cout << "found " << s << endl ;
	    }
	    else
	    {
		cerr << "found " << s << " but real = " << r
		     << " and container = " << c << endl ;
		return 1 ;
	    }
	}
	else
	{
	    cerr << "couldn't find " << s << endl ;
	    return 1 ;
	}
    }

    cout << endl << "*****************************************" << endl;
    cout << "Looking for thingy" << endl;
    DODSContainer d( "thingy" ) ;
    cpf.look_for( d ) ;
    if( d.is_valid() )
    {
	cerr << "found thingy" << endl ;
	return 1 ;
    }
    else
    {
	cout << "didn't find thingy, good" << endl ;
    }

    cout << endl << "*****************************************" << endl;
    cout << "show containers" << endl;
    DODSTextInfo info( false ) ;
    cpf.show_containers( info ) ;
    info.print( stdout ) ;

    cout << endl << "*****************************************" << endl;
    cout << "Returning from pfileT::run" << endl;

    return retVal;
}

int
main(int argC, char **argV) {
    putenv( "DODS_INI=./persistence_file_test.ini" ) ;
    Application *app = new pfileT();
    return app->main(argC, argV);
}
