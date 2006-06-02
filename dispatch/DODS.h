// DODS.h

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

#ifndef DODS_h_
#define DODS_h_ 1

#include <list>

using std::list ;

#include "DODSDataHandlerInterface.h"

class DODSException ;
class DODSTransmitter ;

typedef bool (*p_opendap_init)( DODSDataHandlerInterface &dhi ) ;
typedef int (*p_opendap_ehm)( DODSException &e, DODSDataHandlerInterface &dhi );
typedef void (*p_opendap_end)( DODSDataHandlerInterface &dhi ) ;

/** @brief Entry point into OPeNDAP, building responses to given requests.

    DODS is an abstract class providing the entry point into the retrieval
    of information using the OPeNDAP system. There are eight steps to
    retrieving a response to a given request:

    <OL>
    <LI>initialize the OPeNDAP environment</LI>
    <LI>validate the incoming information to make sure that all information
    is available to perform the query</LI>
    <LI>build the request plan to retrieve the information. A response can
    be generated for multiple files using multiple server types (cedar, cdf,
    netcdf, etc...)</LI>
    <LI>execute the request plan and build the response object</LI>
    <LI>transmit the response object</LI>
    <LI>log the status of the request</LI>
    <LI>send out report information that can be reported on by any number of
    reporters registered with the system.</LI>
    <LI>end the request</LI>
    </OL>

    The way in which the response is generated is as follows.
    A DODSResponseHandler is found that knows how to build the requested
    response object. The built-in response handlers are for the
    response objects das, dds, ddx, data, help, version. These response
    handlers are added to a response handler list during initialization.
    Additional response handlers can be added to this list. For example,
    in Cedar, response handlers are registered to build flat, tab, info,
    and stream responses.

    To build the response objects a user can make many requests. For
    example, a das object can be built using as many different files as is
    requested, say for file1,file2,file3,file4. And each of these files
    could be of a different data type. For example, file1 and file3 could
    be cedar files, file2 could be cdf file and file4 could be a netcdf
    file.

    The structure that holds all of the requested information is the
    DODSDataHandlerInterface. It holds on to a list of containers, each of
    which has the data type (cedar, cdf, nph, etc...) and the file to be
    read. The DODSDataHandlerInterface is built in the build request method.

    The response handlers know how to build the specified response object,
    such as DAS, DDS, help, status, version, etc...

    For each container in the DODSDataHandlerInterface find the
    request handler (DODSRequestHandler) for the containers data type. Each
    request handler registers functions that know how to fill in a certain
    type of response (DAS, DDS, etc...). Find that function and invoke it. So,
    for example, there is a CedarRequestHandler class that registers functions
    that knows how to fill in the different response objects from cedar files.

    Once the response object is filled it is transmitted using a specified
    DODSTransmitter.

    The status is then logged (default is to not log any status. It is up
    to derived classes of DODS to implement the log_status method.)

    The request and status are then reported. The default action is to
    pass off the reporting to DODSReporterList::TheList(), which has a list of
    registered reporters and passes off the information to each of those
    reporters. For example, if the Cedar project wants to report on any
    cedar access then it can register a reporter with 
    DODSReporterList::TheList() and if OPeNDAP wants to keep track of
    usage then it can register a reporter that will report/log all requests.

    @see DODSGlobalInit
    @see DODSKeys
    @see DODSResponseHandler 
    @see DODSRequestHandler 
    @see DODSTransmitter 
    @see DODSLog
    @see DODSReporter 
 */
class DODS
{
private:
    typedef list< p_opendap_init >::const_iterator init_citer ;
    typedef list< p_opendap_init >::iterator init_iter ;
    static list< p_opendap_init > _init_list ;

    typedef list< p_opendap_ehm >::const_iterator ehm_citer ;
    typedef list< p_opendap_ehm >::iterator ehm_iter ;
    static list< p_opendap_ehm > _ehm_list ;

    typedef list< p_opendap_end >::const_iterator end_citer ;
    typedef list< p_opendap_end >::iterator end_iter ;
    static list< p_opendap_end > _end_list ;
protected:
    DODSDataHandlerInterface	_dhi ;
    DODSTransmitter		*_transmitter ;

    virtual int			exception_manager(DODSException &e) ;
    virtual void		initialize() ;
    virtual void		validate_data_request() ;
    virtual void		build_data_request_plan() ;
    virtual void		execute_data_request_plan() ;
    virtual void		invoke_aggregation() ;
    virtual void		transmit_data() ;
    virtual void		log_status() ;
    virtual void		report_request() ;
    virtual void		end_request() ;
    virtual void		clean() ;

    				DODS() ;
    virtual			~DODS() ;
public:
    virtual int			execute_request() ;

    static void			add_init_callback( p_opendap_init init ) ;
    static void			add_ehm_callback( p_opendap_ehm ehm ) ;
    static void			add_end_callback( p_opendap_end end ) ;
} ;

#endif // DODS_h_

// $Log: DODS.h,v $
// Revision 1.4  2005/02/01 17:48:17  pwest
//
// integration of ESG into opendap
//
// Revision 1.3  2004/12/15 17:39:03  pwest
// Added doxygen comments
//
// Revision 1.2  2004/09/09 17:17:12  pwest
// Added copywrite information
//
// Revision 1.1  2004/06/30 20:16:24  pwest
// dods dispatch code, can be used for apache modules or simple cgi script
// invocation or opendap daemon. Built during cedar server development.
//
