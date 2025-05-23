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

#ifndef _SDR_OVERLAY_OVERLAYMANAGER_HXX
#define _SDR_OVERLAY_OVERLAYMANAGER_HXX

#include <boost/utility.hpp>
#include <rtl/ref.hxx>
#include <svx/sdr/animation/scheduler.hxx>
#include <svx/sdr/overlay/overlayobject.hxx>
#include <vcl/mapmod.hxx>
#include <tools/color.hxx>
#include "svx/svxdllapi.h"
#include <svtools/optionsdrawinglayer.hxx>
#include <basegfx/matrix/b2dhommatrix.hxx>
#include <drawinglayer/geometry/viewinformation2d.hxx>

//////////////////////////////////////////////////////////////////////////////
// predeclarations

class OutputDevice;
class Region;

namespace sdr { namespace overlay {
    class OverlayObject;
}}

namespace basegfx {
    class B2DRange;
}

//////////////////////////////////////////////////////////////////////////////

namespace sdr
{
    namespace overlay
    {
        class SVX_DLLPUBLIC OverlayManager
            : private boost::noncopyable
            , protected ::sdr::animation::Scheduler
            , public rtl::IReference
        {
        private:
            oslInterlockedCount mnRefCount;
        protected:
            // the OutputDevice to work on, set on construction and not to be changed
            OutputDevice&                               rmOutputDevice;

            // the vector of registered OverlayObjects
            OverlayObjectVector                         maOverlayObjects;

            // Stripe support. All striped OverlayObjects use these stripe
            // values. Changes change all those objects.
            Color                                       maStripeColorA; // defaults to Color(COL_BLACK)
            Color                                       maStripeColorB; // defaults to Color(COL_WHITE)
            sal_uInt32                                  mnStripeLengthPixel; // defaults to 4L

            // hold an incarnation of Drawinglayer configuration options
            SvtOptionsDrawinglayer                      maDrawinglayerOpt;

            // hold buffered the logic length of discrete vector (1.0, 0.0) and the
            // view transformation belonging to it. Update happens in getDiscreteOne()
            basegfx::B2DHomMatrix                       maViewTransformation;
            drawinglayer::geometry::ViewInformation2D   maViewInformation2D;
            double                                      mfDiscreteOne;

            // internal
            void ImpDrawMembers(const basegfx::B2DRange& rRange, OutputDevice& rDestinationDevice) const;
            void ImpStripeDefinitionChanged();
            void impApplyRemoveActions(OverlayObject& rTarget);
            void impApplyAddActions(OverlayObject& rTarget);

            // return mfDiscreteOne to derivations, but also check for buffered local
            // ViewTransformation and evtl. correct mfDiscreteOne
            double getDiscreteOne() const;

            // when handing over another OverlayManager at construction, the OverlayObjects
            // will be taken over from it. The new one will have added all OverlayObjects
            // while the handed over one will have none
            OverlayManager(
                OutputDevice& rOutputDevice,
                OverlayManager* pOldOverlayManager);
            virtual ~OverlayManager();

        public:
            static rtl::Reference<OverlayManager> create(OutputDevice& rOutputDevice,
                OverlayManager* pOldOverlayManager = 0);

            // access to current ViewInformation2D; this call checks and evtl. updates ViewInformation2D
            const drawinglayer::geometry::ViewInformation2D getCurrentViewInformation2D() const;

            // complete redraw
            virtual void completeRedraw(const Region& rRegion, OutputDevice* pPreRenderDevice = 0) const;

            // flush. Do buffered updates.
            virtual void flush();

            // #i68597# part of content gets copied, react on it
            virtual void copyArea(const Point& rDestPt, const Point& rSrcPt, const Size& rSrcSize);

            // restore part of background. Implemented form buffered versions only.
            virtual void restoreBackground(const Region& rRegion) const;

            // get the OutputDevice
            OutputDevice& getOutputDevice() const { return rmOutputDevice; }

            // add and remove OverlayObjects
            void add(OverlayObject& rOverlayObject);
            void remove(OverlayObject& rOverlayObject);

            // invalidate the given range at local OutputDevice
            virtual void invalidateRange(const basegfx::B2DRange& rRange);

            // stripe support ColA
            Color getStripeColorA() const { return maStripeColorA; }
            void setStripeColorA(Color aNew= Color(COL_BLACK));

            // stripe support ColB
            Color getStripeColorB() const { return maStripeColorB; }
            void setStripeColorB(Color aNew = Color(COL_WHITE));

            // stripe support StripeLengthPixel
            sal_uInt32 getStripeLengthPixel() const { return mnStripeLengthPixel; }
            void setStripeLengthPixel(sal_uInt32 nNew = 5L);

            // access to maDrawinglayerOpt
            const SvtOptionsDrawinglayer& getDrawinglayerOpt() const { return maDrawinglayerOpt; }

            void InsertEvent(sdr::animation::Event* pNew) { Scheduler::InsertEvent(pNew); }

            virtual oslInterlockedCount SAL_CALL acquire();
            virtual oslInterlockedCount SAL_CALL release();
        };
    } // end of namespace overlay
} // end of namespace sdr

//////////////////////////////////////////////////////////////////////////////

#endif //_SDR_OVERLAY_OVERLAYMANAGER_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
