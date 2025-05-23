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

#ifndef _SDR_CONTACT_OBJECTCONTACTOFOBJLISTPAINTER_HXX
#define _SDR_CONTACT_OBJECTCONTACTOFOBJLISTPAINTER_HXX

#include <svx/sdr/contact/objectcontact.hxx>
#include "svx/svxdllapi.h"
#include <svx/svdpage.hxx>

//////////////////////////////////////////////////////////////////////////////
// predeclarations

class SdrPage;
class SdrObject;

//////////////////////////////////////////////////////////////////////////////

namespace sdr
{
    namespace contact
    {
        class SVX_DLLPUBLIC ObjectContactPainter : public ObjectContact
        {
        protected:
            // Hierarchy access methods
            virtual sal_uInt32 GetPaintObjectCount() const = 0;
            virtual ViewContact& GetPaintObjectViewContact(sal_uInt32 nIndex) const = 0;

        public:
            // basic constructor/destructor
            ObjectContactPainter();
            virtual ~ObjectContactPainter();
        };
    } // end of namespace contact
} // end of namespace sdr

//////////////////////////////////////////////////////////////////////////////

namespace sdr
{
    namespace contact
    {
        // typedef for transferring SdrObject
        typedef ::std::vector< SdrObject* > SdrObjectVector;

        class SVX_DLLPUBLIC ObjectContactOfObjListPainter : public ObjectContactPainter
        {
        protected:
            // Target OutputDevice
            OutputDevice&                                   mrTargetOutputDevice;

            // Set StartPoint for next run, also given in constructor
            SdrObjectVector                                 maStartObjects;

            // the processed page which is the base e.g. for PageNumberFields
            const SdrPage*                                  mpProcessedPage;

            // Hierarchy access methods
            virtual sal_uInt32 GetPaintObjectCount() const;
            virtual ViewContact& GetPaintObjectViewContact(sal_uInt32 nIndex) const;

        public:
            // basic constructor/destructor
            ObjectContactOfObjListPainter(
                OutputDevice& rTargetDevice,
                const SdrObjectVector& rObjects,
                const SdrPage* pProcessedPage);
            virtual ~ObjectContactOfObjListPainter();

            // Process the whole displaying
            virtual void ProcessDisplay(DisplayInfo& rDisplayInfo);

            // VirtualDevice? Default is false
            virtual bool isOutputToVirtualDevice() const;

            // recording MetaFile? Default is false
            virtual bool isOutputToRecordingMetaFile() const;

            // pdf export? Default is false
            virtual bool isOutputToPDFFile() const;

            // access to OutputDevice. May return 0L like the default implementations do. Needs to be overloaded as needed.
            virtual OutputDevice* TryToGetOutputDevice() const;
        };
    } // end of namespace contact
} // end of namespace sdr

//////////////////////////////////////////////////////////////////////////////

namespace sdr
{
    namespace contact
    {
        class ObjectContactOfPagePainter : public ObjectContactPainter
        {
        protected:
            // the original ObjectContact this painter is working on
            ObjectContact&                                  mrOriginalObjectContact;

            // Set StartPoint for next run, also given in constructor
            SdrPageWeakRef                                  mxStartPage;

            // Hierarchy access methods
            virtual sal_uInt32 GetPaintObjectCount() const;
            virtual ViewContact& GetPaintObjectViewContact(sal_uInt32 nIndex) const;

        public:
            // basic constructor
            ObjectContactOfPagePainter(
                const SdrPage* pPage,
                ObjectContact& rOriginalObjectContact);
            virtual ~ObjectContactOfPagePainter();

            // set another page
            void SetStartPage(const SdrPage* pPage);
            const SdrPage* GetStartPage() const { return mxStartPage.get(); }

            // access to OutputDevice. May return 0L like the default implementations do. Needs to be overloaded as needed.
            virtual OutputDevice* TryToGetOutputDevice() const;
        };
    } // end of namespace contact
} // end of namespace sdr

//////////////////////////////////////////////////////////////////////////////

#endif //_SDR_CONTACT_OBJECTCONTACTOFOBJLISTPAINTER_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
