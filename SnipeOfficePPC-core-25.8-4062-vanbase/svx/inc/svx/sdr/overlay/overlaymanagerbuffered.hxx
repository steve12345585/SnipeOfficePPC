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

#ifndef _SDR_OVERLAY_OVERLAYMANAGERBUFFERED_HXX
#define _SDR_OVERLAY_OVERLAYMANAGERBUFFERED_HXX

#include <svx/sdr/overlay/overlaymanager.hxx>
#include <basegfx/range/b2irange.hxx>
#include <vcl/virdev.hxx>

//////////////////////////////////////////////////////////////////////////////

namespace sdr
{
    namespace overlay
    {
        class OverlayManagerBuffered : public OverlayManager
        {
        protected:
            // The VirtualDevice for draw window content buffering, this
            // is the view content without overlay
            VirtualDevice                           maBufferDevice;

            // #i73602# The VirtualDevice for OverlayPaint buffering. This
            // is an extra device to avoid flickering of overlay paints
            VirtualDevice                           maOutputBufferDevice;

            // Timer for buffering
            Timer                                   maBufferTimer;

            // Range for buffering (in pixel to be independent from mapMode)
            basegfx::B2IRange                       maBufferRememberedRangePixel;

            // bitfield
            // Flag to decide if PreRendering shall be used for overlay refreshes.
            // Default is false.
            unsigned                                mbRefreshWithPreRendering : 1;

            // link for timer
            DECL_LINK(ImpBufferTimerHandler, AutoTimer*);

            // Internal methods for buffering
            void ImpPrepareBufferDevice();
            void ImpRestoreBackground() const ;
            void ImpRestoreBackground(const Region& rRegionPixel) const;
            void ImpSaveBackground(const Region& rRegion, OutputDevice* pPreRenderDevice = 0L);

            // when handing over another OverlayManager at construction, the OverlayObjects
            // will be taken over from it. The new one will have added all OverlayObjects
            // while the handed over one will have none
            OverlayManagerBuffered(
                OutputDevice& rOutputDevice,
                OverlayManager* pOldOverlayManager,
                bool bRefreshWithPreRendering);
            virtual ~OverlayManagerBuffered();

        public:
            static rtl::Reference<OverlayManager> create(OutputDevice& rOutputDevice,
                OverlayManager* pOldOverlayManager = 0, bool bRefreshWithPreRendering = false);

            // complete redraw
            virtual void completeRedraw(const Region& rRegion, OutputDevice* pPreRenderDevice = 0L) const;

            // flush. Do buffered updates.
            virtual void flush();

            // #i68597# part of content gets copied, react on it
            virtual void copyArea(const Point& rDestPt, const Point& rSrcPt, const Size& rSrcSize);

            // restore part of background. Implemented form buffered versions only.
            virtual void restoreBackground(const Region& rRegion) const;

            // invalidate the given range at local OutputDevice
            virtual void invalidateRange(const basegfx::B2DRange& rRange);

            // access to RefreshWithPreRendering Flag
            bool DoRefreshWithPreRendering() const { return mbRefreshWithPreRendering; }
        };
    } // end of namespace overlay
} // end of namespace sdr

//////////////////////////////////////////////////////////////////////////////

#endif //_SDR_OVERLAY_OVERLAYMANAGERBUFFERED_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
