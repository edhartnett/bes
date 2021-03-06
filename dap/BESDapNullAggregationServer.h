// BESDapNullAggregationServer.h

// This file is part of bes, A C++ back-end server implementation framework
// for the OPeNDAP Data Access Protocol.

// Copyright (c) 2015 OPeNDAP, Inc.
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

#ifndef I_BESDapNullAggregationServer_h
#define I_BESDapNullAggregationServer_h 1

#include "BESAggregationServer.h"       // in dispatch

/** @brief When called, print out information about the DataHanderInterface object
 *
 * Aggregation is an inherent capability of the BES framework. However,
 * the command, the data, etc., are all bundled in the DHI object's
 * 'data' map. This specialization of AggregationServer will print out
 * all of the available information about the DHI.
 *
 * @see BESDataHandlerInterface
 * @see BESAggregationServer
 */
class BESDapNullAggregationServer: public BESAggregationServer
{
protected:
    BESDapNullAggregationServer( const string &name ): BESAggregationServer( name )
    {
    }

public:
    /**
     * This 'static' constructor is used to build an instance that can be
     * registered with the 'AggFactory' in the DapModule so that the define
     * command will find the AggregationServer instance.
     */
    static BESAggregationServer *NewBESDapNullAggregationServer( const string &name )
    {
        return new BESDapNullAggregationServer(name);
    }

    virtual ~BESDapNullAggregationServer( void )
    {
    }

    // This method is abstract in the parent class
    virtual void aggregate(BESDataHandlerInterface &dhi);

    virtual void dump(ostream &strm) const;
};

#endif // I_BESDapNullAggregationServer_h

