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

#ifndef _VCLCANVAS_BACKBUFFER_HXX_
#define _VCLCANVAS_BACKBUFFER_HXX_

#include <vcl/virdev.hxx>

#include <canvas/vclwrapper.hxx>
#include "outdevprovider.hxx"

#include <boost/shared_ptr.hpp>


namespace vclcanvas
{
    /// Background buffer abstraction
    class BackBuffer : public OutDevProvider
    {
    public:
        /** Create a backbuffer for given reference device

            @param bMonochromeBuffer
            When false, default depth of reference device is
            chosen. When true, the buffer will be monochrome, i.e. one
            bit deep.
         */
        BackBuffer( const OutputDevice& rRefDevice,
                    bool                bMonochromeBuffer=false );

        virtual OutputDevice&       getOutDev();
        virtual const OutputDevice& getOutDev() const;

        void setSize( const ::Size& rNewSize );

    private:
        ::canvas::vcltools::VCLObject<VirtualDevice>    maVDev;
    };

    typedef ::boost::shared_ptr< BackBuffer > BackBufferSharedPtr;
}

#endif /* #ifndef _VCLCANVAS_BACKBUFFER_HXX_ */

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
