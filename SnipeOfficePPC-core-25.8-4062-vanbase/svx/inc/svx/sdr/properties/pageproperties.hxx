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

#ifndef _SDR_PROPERTIES_PAGEPROPERTIES_HXX
#define _SDR_PROPERTIES_PAGEPROPERTIES_HXX

#include <svx/sdr/properties/emptyproperties.hxx>

//////////////////////////////////////////////////////////////////////////////

namespace sdr
{
    namespace properties
    {
        class PageProperties : public EmptyProperties
        {
        protected:
            // create a new object specific itemset with object specific ranges.
            virtual SfxItemSet& CreateObjectSpecificItemSet(SfxItemPool& pPool);

            // Do the ItemChange, may do special handling
            virtual void ItemChange(const sal_uInt16 nWhich, const SfxPoolItem* pNewItem = 0);

            // Called after ItemChange() is done for all items.
            virtual void PostItemChange(const sal_uInt16 nWhich);

        public:
            // basic constructor
            explicit PageProperties(SdrObject& rObj);

            // constructor for copying, but using new object
            PageProperties(const PageProperties& rProps, SdrObject& rObj);

            // destructor
            virtual ~PageProperties();

            // Clone() operator, normally just calls the local copy constructor
            virtual BaseProperties& Clone(SdrObject& rObj) const;

            // get itemset. Overloaded here to allow creating the empty itemset
            // without asserting
            virtual const SfxItemSet& GetObjectItemSet() const;

            // get the installed StyleSheet
            virtual SfxStyleSheet* GetStyleSheet() const;

            // clear single item
            virtual void ClearObjectItem(const sal_uInt16 nWhich = 0);
        };
    } // end of namespace properties
} // end of namespace sdr

//////////////////////////////////////////////////////////////////////////////

#endif //_SDR_PROPERTIES_PAGEPROPERTIES_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
