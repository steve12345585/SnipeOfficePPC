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

#ifndef SD_SLIDESORTER_VIEW_LAYERED_DEVICE_HXX
#define SD_SLIDESORTER_VIEW_LAYERED_DEVICE_HXX

#include "view/SlsILayerPainter.hxx"
#include "SlideSorter.hxx"

#include <tools/gen.hxx>
#include <vcl/region.hxx>
#include <vcl/virdev.hxx>

#include <boost/noncopyable.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <vector>

namespace sd { namespace slidesorter { namespace view {

/** A simple wrapper around an OutputDevice that provides support for
    independent layers and buffering.
    Each layer may contain any number of painters.
*/
class LayeredDevice
    : public ::boost::enable_shared_from_this<LayeredDevice>

{
public:
    LayeredDevice (const SharedSdWindow& rpTargetWindow);
    ~LayeredDevice (void);

    void Invalidate (
        const Rectangle& rInvalidationBox,
        const sal_Int32 nLayer);
    void InvalidateAllLayers (
        const Rectangle& rInvalidationBox);
    void InvalidateAllLayers (
        const Region& rInvalidationRegion);

    void RegisterPainter (
        const SharedILayerPainter& rPainter,
        const sal_Int32 nLayer);

    void RemovePainter (
        const SharedILayerPainter& rPainter,
        const sal_Int32 nLayer);

    bool HandleMapModeChange (void);
    void Repaint (const Region& rRepaintRegion);

    void Resize (void);

    void Dispose (void);

private:
    SharedSdWindow mpTargetWindow;
    class LayerContainer;
    ::boost::scoped_ptr<LayerContainer> mpLayers;
    ::boost::scoped_ptr<VirtualDevice> mpBackBuffer;
    MapMode maSavedMapMode;

    void RepaintRectangle (const Rectangle& rRepaintRectangle);
};



} } } // end of namespace ::sd::slidesorter::view

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
