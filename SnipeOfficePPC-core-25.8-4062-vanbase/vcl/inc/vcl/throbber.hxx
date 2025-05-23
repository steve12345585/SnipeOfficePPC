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

#ifndef VCL_THROBBER_HXX
#define VCL_THROBBER_HXX

#include "vcl/dllapi.h"
#include "vcl/imgctrl.hxx"
#include "vcl/timer.hxx"

#include <com/sun/star/graphic/XGraphic.hpp>

#include <vector>

class VCL_DLLPUBLIC Throbber : public ImageControl
{
public:
    enum ImageSet
    {
        /// no (default) images at all
        IMAGES_NONE,
        /// automatically decide between different image sets, depending on what fits best the actual size
        IMAGES_AUTO,
        /// default images, 16x16 pixels
        IMAGES_16_PX,
        /// default images, 32x32 pixels
        IMAGES_32_PX,
        /// default images, 64x64 pixels
        IMAGES_64_PX,
    };

public:
                    Throbber( Window* i_parentWindow, WinBits i_style, const ImageSet i_imageSet = IMAGES_AUTO );
                    Throbber( Window* i_parentWindow, const ResId& i_resId, const ImageSet i_imageSet = IMAGES_AUTO );
                    ~Throbber();

    // Properties
    void            setStepTime( sal_Int32 nStepTime )  { mnStepTime = nStepTime; }
    sal_Int32       getStepTime() const                 { return mnStepTime; }

    void            setRepeat( sal_Bool bRepeat )       { mbRepeat = bRepeat; }
    sal_Bool        getRepeat() const                   { return mbRepeat; }

    // animation control
    void start();
    void stop();
    bool isRunning() const;

    void setImageList( const ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Reference< ::com::sun::star::graphic::XGraphic > >& ImageList );
    void setImageList( ::std::vector< Image > const& i_images );

    // default images
    static ::std::vector< ::rtl::OUString >
        getDefaultImageURLs( const ImageSet i_imageSet );

protected:
    // Window overridables
    virtual void        Resize();

private:
    SAL_DLLPRIVATE void initImages();

private:
    ::std::vector< Image >  maImageList;

    sal_Bool    mbRepeat;
    sal_Int32   mnStepTime;
    sal_Int32   mnCurStep;
    sal_Int32   mnStepCount;
    AutoTimer   maWaitTimer;
    ImageSet    meImageSet;

    DECL_LINK( TimeOutHdl, void* );
};

#endif // VCL_THROBBER_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
