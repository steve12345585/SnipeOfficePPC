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

#ifndef SD_SLIDESORTER_TRANSFERABLE_DATA_HXX
#define SD_SLIDESORTER_TRANSFERABLE_DATA_HXX

#include "sdxfer.hxx"
#include <boost/function.hpp>
#include <vector>

class SdDrawDocument;
namespace sd { namespace slidesorter {
class SlideSorterViewShell;
} }

namespace sd { namespace slidesorter { namespace controller {

/** Represent previews and other information so that they can be
    attached to an existing transferable.
*/
class TransferableData
    : public SdTransferable::UserData,
      public SfxListener
{
public:
    class Representative
    {
    public:
        Representative (const Bitmap& rBitmap, const bool bIsExcluded)
            : maBitmap(rBitmap), mbIsExcluded(bIsExcluded) {}
        Representative (const Representative& rOther)
            : maBitmap(rOther.maBitmap), mbIsExcluded(rOther.mbIsExcluded) {}
        Representative operator= (Representative const& rOther)
        {   if (&rOther != this) {maBitmap = rOther.maBitmap; mbIsExcluded = rOther.mbIsExcluded; }
            return *this;
        }

        Bitmap maBitmap;
        bool mbIsExcluded;
    };

    static SdTransferable* CreateTransferable (
        SdDrawDocument* pSrcDoc,
        ::sd::View* pWorkView,
        sal_Bool bInitOnGetData,
        SlideSorterViewShell* pViewShell,
        const ::std::vector<TransferableData::Representative>& rRepresentatives);

    static ::boost::shared_ptr<TransferableData> GetFromTransferable (const SdTransferable* pTransferable);

    TransferableData (
        SlideSorterViewShell* pViewShell,
        const ::std::vector<TransferableData::Representative>& rRepresentatives);
    ~TransferableData (void);

    virtual void DragFinished (sal_Int8 nDropAction);

    const ::std::vector<Representative>& GetRepresentatives (void) const;

    /** Return the view shell for which the transferable was created.
    */
    SlideSorterViewShell* GetSourceViewShell (void) const;

private:
    SlideSorterViewShell* mpViewShell;
    const ::std::vector<Representative> maRepresentatives;
    typedef ::std::vector<boost::function<void(sal_uInt8)> > CallbackContainer;

    virtual void Notify (SfxBroadcaster& rBroadcaster, const SfxHint& rHint);
};

} } } // end of namespace ::sd::slidesorter::controller

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
