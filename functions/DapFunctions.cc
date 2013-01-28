// DapFunctions.cc

// This file is part of bes, A C++ back-end server implementation framework
// for the OPeNDAP Data Access Protocol.

// Copyright (c) 2013 OPeNDAP, Inc.
// Author: James Gallagher <jgallagher@opendap.org>
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
// You can contact OPeNDAP, Inc. at PO Box 112, Saunderstown, RI. 02874-0112.

#include <iostream>

using std::endl;

#include "DapFunctions.h"
#include "ce_functions.h"

#if 0
#include "BESDapRequestHandler.h"
#include "BESRequestHandlerList.h"

#include "BESDapNames.h"
#include "BESResponseHandlerList.h"

#include "BESDASResponseHandler.h"
#include "BESDMRResponseHandler.h"
#include "BESDDSResponseHandler.h"
#include "BESDataResponseHandler.h"
#include "BESDDXResponseHandler.h"
#include "BESDataDDXResponseHandler.h"

#include "BESCatalogResponseHandler.h"

#include "BESServiceRegistry.h"

#include "BESDapTransmit.h"
#include "BESTransmitter.h"
#include "BESReturnManager.h"
#include "BESTransmitterNames.h"
#endif

#include "BESServerFunctionsList.h"

#include "BESDebug.h"
#if 0
#include "BESInternalFatalError.h"
#include "BESExceptionManager.h"
#include "BESDapError.h"
#endif

void DapFunctions::initialize(const string &modname)
{
    BESDEBUG( "dap_functions", "Initializing DAP Functions:" << endl );

    BESServerFunctionsList::TheList()->add_function("grid", libdap::function_grid);
    BESServerFunctionsList::TheList()->add_function("geogrid", libdap::function_geogrid);
    BESServerFunctionsList::TheList()->add_function("linear_scale", libdap::function_linear_scale);
    BESServerFunctionsList::TheList()->add_function("version", libdap::func_version);

    BESDEBUG( "dap_functions", "Done initializing DAP Functions" << endl );
}

void DapFunctions::terminate(const string &modname)
{
    BESDEBUG( "dap_functions", "Removing DAP Modules (this does nothing)." << endl );
}

/** @brief dumps information about this object
  *
  * Displays the pointer value of this instance
  *
  * @param strm C++ i/o stream to dump the information to
  */
void DapFunctions::dump(ostream &strm) const
{
    strm << BESIndent::LMarg << "DapFunctions::dump - (" << (void *) this << ")" << endl;
}

extern "C" {
BESAbstractModule *maker()
{
    return new DapFunctions;
}
}

