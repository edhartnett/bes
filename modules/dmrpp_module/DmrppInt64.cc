
// -*- mode: c++; c-basic-offset:4 -*-

// This file is part of the BES

// Copyright (c) 2016 OPeNDAP, Inc.
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
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
//
// You can contact OPeNDAP, Inc. at PO Box 112, Saunderstown, RI. 02874-0112.

#include "config.h"

#include <string>
#include <sstream>
#include <cassert>

#include <BESError.h>
#include <BESDebug.h>

#include "DmrppUtil.h"
#include "DmrppInt64.h"

using namespace libdap;
using namespace std;

namespace dmrpp {

void
DmrppInt64::_duplicate(const DmrppInt64 &)
{
}

DmrppInt64::DmrppInt64(const string &n) : Int64(n), DmrppCommon()
{
}

DmrppInt64::DmrppInt64(const string &n, const string &d) : Int64(n, d), DmrppCommon()
{
}

BaseType *
DmrppInt64::ptr_duplicate()
{
    return new DmrppInt64(*this);
}

DmrppInt64::DmrppInt64(const DmrppInt64 &rhs) : Int64(rhs), DmrppCommon(rhs)
{
    _duplicate(rhs);
}

DmrppInt64 &
DmrppInt64::operator=(const DmrppInt64 &rhs)
{
    if (this == &rhs)
    return *this;

    dynamic_cast<Int64 &>(*this) = rhs; // run Constructor=

    _duplicate(rhs);
    DmrppCommon::_duplicate(rhs);

    return *this;
}

bool
DmrppInt64::read()
{
    BESDEBUG("dmrpp", "Entering " <<__PRETTY_FUNCTION__ << " for '" << name() << "'" << endl);

    if (read_p())
        return true;

    vector<H4ByteStream> *chunk_refs = get_chunk_vec();
    if((*chunk_refs).size() == 0){
        ostringstream oss;
        oss << "DmrppInt64::read() - Unable to obtain a byteStream object for DmrppInt64 " << name()
        		<< " Without a byteStream we cannot read! "<< endl;
        throw BESError(oss.str(), BES_INTERNAL_ERROR, __FILE__, __LINE__);
    }
    else {
		BESDEBUG("dmrpp", "DmrppInt64::read() - Found H4ByteStream (chunks): " << endl);
    	for(unsigned long i=0; i<(*chunk_refs).size(); i++){
    		BESDEBUG("dmrpp", "DmrppInt64::read() - chunk[" << i << "]: " << (*chunk_refs)[i].to_string() << endl);
    	}
    }
    // For now we only handle the one chunk case.
    H4ByteStream h4bs = (*chunk_refs)[0];
    h4bs.set_rbuf_to_size();

    // Do a range get with libcurl
    // Slice 'this' to just the DmrppCommon parts. Needed because the generic
    // version of the 'write_data' callback only knows about DmrppCommon. Passing
    // in a whole object like DmrppInt32 and then using reinterpret_cast<>()
    // will leave the code using garbage memory. jhrg 11/23/16
    BESDEBUG("dmrpp", "DmrppInt64::read() - Reading  " << h4bs.get_data_url() << ": " << h4bs.get_curl_range_arg_string() << endl);
    curl_read_byte_stream(h4bs.get_data_url(), h4bs.get_curl_range_arg_string(), dynamic_cast<H4ByteStream*>(&h4bs));

    // Could use get_rbuf_size() in place of sizeof() for a more generic version.
    if (sizeof(dods_int64) != h4bs.get_bytes_read()) {
        ostringstream oss;
        oss << "DmrppInt64: Wrong number of bytes read for '" << name() << "'; expected " << sizeof(dods_int64)
            << " but found " << h4bs.get_bytes_read() << endl;
        throw BESError(oss.str(), BES_INTERNAL_ERROR, __FILE__, __LINE__);
    }

    set_value(*reinterpret_cast<dods_int64*>(h4bs.get_rbuf()));

    set_read_p(true);

    return true;

}


void DmrppInt64::dump(ostream & strm) const
{
    strm << DapIndent::LMarg << "DmrppInt64::dump - (" << (void *) this << ")" << endl;
    DapIndent::Indent();
    DmrppCommon::dump(strm);
    Int64::dump(strm);
    strm << DapIndent::LMarg << "value:    " << d_buf << endl;
    DapIndent::UnIndent();
}

} // namespace dmrpp

