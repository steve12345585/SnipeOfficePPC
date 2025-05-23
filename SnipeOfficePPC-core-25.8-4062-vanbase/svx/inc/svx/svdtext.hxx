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

#ifndef _SVDTEXT_HXX
#define _SVDTEXT_HXX

#include <sal/types.h>
#include "svx/svxdllapi.h"
#include <tools/weakbase.hxx>

// --------------------------------------------------------------------

class OutlinerParaObject;
class SdrOutliner;
class SdrTextObj;
class SdrModel;
class SfxItemSet;

namespace sdr { namespace properties {
    class TextProperties;
}}

/** This class stores information about one text inside a shape.
*/

class SfxStyleSheet;
class SVX_DLLPUBLIC SdrText : public tools::WeakBase< SdrText >
{
public:
    SdrText( SdrTextObj& rObject, OutlinerParaObject* pOutlinerParaObject = 0 );
    virtual ~SdrText();

    virtual void SetModel(SdrModel* pNewModel);
    virtual void ForceOutlinerParaObject( sal_uInt16 nOutlMode );

    virtual void SetOutlinerParaObject( OutlinerParaObject* pTextObject );
    virtual OutlinerParaObject* GetOutlinerParaObject() const;

    virtual void CheckPortionInfo( SdrOutliner& rOutliner );
    virtual void ReformatText();

    // default uses GetObjectItemSet, but may be overloaded to
    // return a text-specific ItemSet
    virtual const SfxItemSet& GetItemSet() const;

    SdrModel* GetModel() const { return mpModel; }
    SdrTextObj& GetObject() const { return mrObject; }

    /** returns the current OutlinerParaObject and removes it from this instance */
    OutlinerParaObject* RemoveOutlinerParaObject();

protected:
    virtual const SfxItemSet& GetObjectItemSet();
    virtual void SetObjectItem(const SfxPoolItem& rItem);
    virtual SfxStyleSheet* GetStyleSheet() const;

private:
    OutlinerParaObject* mpOutlinerParaObject;
    SdrTextObj& mrObject;
    SdrModel* mpModel;
    bool mbPortionInfoChecked;
};

#endif //_SVDTEXT_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
