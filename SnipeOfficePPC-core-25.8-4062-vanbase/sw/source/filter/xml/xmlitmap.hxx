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

#ifndef _XMLITMAP_HXX
#define _XMLITMAP_HXX

#include <sal/types.h>
#include <tools/ref.hxx>
#include <xmloff/xmltoken.hxx>

namespace rtl { class OUString; }

#define MID_SW_FLAG_MASK                    0x0000ffff

// this flags are used in the item mapper for import and export

#define MID_SW_FLAG_SPECIAL_ITEM_IMPORT 0x80000000
#define MID_SW_FLAG_NO_ITEM_IMPORT          0x40000000
#define MID_SW_FLAG_SPECIAL_ITEM_EXPORT 0x20000000
#define MID_SW_FLAG_NO_ITEM_EXPORT          0x10000000
#define MID_SW_FLAG_SPECIAL_ITEM            0xa0000000 // both import and export
#define MID_SW_FLAG_NO_ITEM             0x50000000 // both import and export
#define MID_SW_FLAG_ELEMENT_ITEM_IMPORT 0x08000000
#define MID_SW_FLAG_ELEMENT_ITEM_EXPORT 0x04000000
#define MID_SW_FLAG_ELEMENT_ITEM            0x0c000000  // both import and export

// ---

struct SvXMLItemMapEntry
{
    sal_uInt16 nNameSpace;      // declares the Namespace in wich this item
                                // exists
    enum ::xmloff::token::XMLTokenEnum eLocalName;
                                // the local name for the item inside
                                // the Namespace (as an XMLTokenEnum)
    sal_uInt16 nWhichId;        // the WichId to identify the item
                                // in the pool
    sal_uInt32 nMemberId;       // the memberid specifies wich part
                                // of the item should be imported or
                                // exported with this Namespace
                                // and localName
};

// ---

class SvXMLItemMapEntries_impl;

/** this class manages an array of SvXMLItemMapEntry. It is
    used for optimizing the static array on startup of import
    or export */
class SvXMLItemMapEntries : public SvRefBase
{
protected:
    SvXMLItemMapEntries_impl* mpImpl;

public:
    SvXMLItemMapEntries( SvXMLItemMapEntry* pEntrys );
    virtual ~SvXMLItemMapEntries();

    SvXMLItemMapEntry* getByName( sal_uInt16 nNameSpace,
                                  const ::rtl::OUString& rString,
                                  SvXMLItemMapEntry* pStartAt = NULL ) const;
    SvXMLItemMapEntry* getByIndex( sal_uInt16 nIndex ) const;

    sal_uInt16 getCount() const;
};

SV_DECL_REF( SvXMLItemMapEntries )
SV_IMPL_REF( SvXMLItemMapEntries )


#endif  //  _XMLITMAP_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
