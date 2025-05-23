/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This file is part of the SnipeOffice project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * This file incorporates work covered by the following license notice:
 *
 *   Licensed to the Apache Software Foundation (ASF) under one or more
 *   contributor license agreements. See the NOTICE file distributed
 *   with this work for additional information regarding copyright
 *   ownership. The ASF licenses this file to you under the Apache
 *   License, Version 2.0 (the "License"); you may not use this file
 *   except in compliance with the License. You may obtain a copy of
 *   the License at http://www.apache.org/licenses/LICENSE-2.0 .
 */

#include <SerfMoveReqProcImpl.hxx>

#include <serf.h>

namespace http_dav_ucp
{

SerfMoveReqProcImpl::SerfMoveReqProcImpl( const char* inSourcePath,
                                          const DAVRequestHeaders& inRequestHeaders,
                                          const char* inDestinationPath,
                                          const bool inOverwrite )
    : SerfRequestProcessorImpl( inSourcePath, inRequestHeaders )
    , mDestPathStr( inDestinationPath )
    , mbOverwrite( inOverwrite )
{
}

SerfMoveReqProcImpl::~SerfMoveReqProcImpl()
{
}

serf_bucket_t * SerfMoveReqProcImpl::createSerfRequestBucket( serf_request_t * inSerfRequest )
{
    // create serf request
    serf_bucket_t *req_bkt = serf_request_bucket_request_create( inSerfRequest,
                                                                 "MOVE",
                                                                 getPathStr(),
                                                                 0,
                                                                 serf_request_get_alloc( inSerfRequest ) );

    // set request header fields
    serf_bucket_t* hdrs_bkt = serf_bucket_request_get_headers( req_bkt );
    // general header fields provided by caller
    setRequestHeaders( hdrs_bkt );

    // MOVE specific header fields
    serf_bucket_headers_set( hdrs_bkt, "Destination", mDestPathStr );
    if ( mbOverwrite )
    {
        serf_bucket_headers_set( hdrs_bkt, "Overwrite", "T" );
    }
    else
    {
        serf_bucket_headers_set( hdrs_bkt, "Overwrite", "F" );
    }

    return req_bkt;
}

void SerfMoveReqProcImpl::processChunkOfResponseData( const char* /*data*/,
                                                      apr_size_t /*len*/ )
{
    // nothing to do;
}

void SerfMoveReqProcImpl::handleEndOfResponseData( serf_bucket_t * /*inSerfResponseBucket*/ )
{
    // nothing to do;
}

} // namespace http_dav_ucp

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
