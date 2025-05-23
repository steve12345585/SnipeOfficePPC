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


#include "backbuffer.hxx"


namespace vclcanvas
{
    BackBuffer::BackBuffer( const OutputDevice& rRefDevice,
                            bool                bMonochromeBuffer ) :
        maVDev( new VirtualDevice( rRefDevice,
                                   bMonochromeBuffer ) )
    {
        if( !bMonochromeBuffer )
        {
            // #i95645#
#if defined( QUARTZ )
            // use AA on VCLCanvas for Mac
            maVDev->SetAntialiasing( ANTIALIASING_ENABLE_B2DDRAW | maVDev->GetAntialiasing() );
#else
            // switch off AA for WIN32 and UNIX, the VCLCanvas does not look good with it and
            // is not required to do AA. It would need to be adapted to use it correctly
            // (especially gradient painting). This will need extra work.
            maVDev->SetAntialiasing( maVDev->GetAntialiasing() & ~ANTIALIASING_ENABLE_B2DDRAW);
#endif
        }
    }

    OutputDevice& BackBuffer::getOutDev()
    {
        return maVDev.get();
    }

    const OutputDevice& BackBuffer::getOutDev() const
    {
        return maVDev.get();
    }

    void BackBuffer::setSize( const ::Size& rNewSize )
    {
        maVDev->SetOutputSizePixel( rNewSize );
    }

}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
