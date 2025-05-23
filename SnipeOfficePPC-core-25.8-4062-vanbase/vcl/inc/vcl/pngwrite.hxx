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

#ifndef _SV_PNGWRITE_HXX
#define _SV_PNGWRITE_HXX

#include <com/sun/star/uno/Sequence.hxx>
#include <com/sun/star/beans/PropertyValue.hpp>
#include <vcl/dllapi.h>
#include <vcl/bitmapex.hxx>
#include <vector>

// -------------
// - PNGWriter -
// -------------

namespace vcl
{
    class PNGWriterImpl;

    class VCL_DLLPUBLIC PNGWriter
    {
        PNGWriterImpl*          mpImpl;

    public:

        PNGWriter( const BitmapEx& BmpEx,
            const ::com::sun::star::uno::Sequence< ::com::sun::star::beans::PropertyValue >* pFilterData = NULL );
        ~PNGWriter();

        sal_Bool Write( SvStream& rStm );

        // additional method to be able to modify all chunk before they are stored
        struct ChunkData
        {
            sal_uInt32                  nType;
            std::vector< sal_uInt8 >    aData;
        };
        std::vector< vcl::PNGWriter::ChunkData >& GetChunks();
    };
}

#endif // _SV_PNGWRITE_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
