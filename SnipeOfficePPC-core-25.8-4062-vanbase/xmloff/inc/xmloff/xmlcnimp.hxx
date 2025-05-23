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

#ifndef _XMLOFF_XMLCNIMP_HXX
#define _XMLOFF_XMLCNIMP_HXX

#include "xmloff/dllapi.h"     //XMLOFF_DLLPUBLIC
#include <sal/types.h>         //sal_uInt16 and sal_Bool
#include <boost/scoped_ptr.hpp>

namespace rtl { class OUString; }
class SvXMLAttrCollection; //Forward declaration only.

class XMLOFF_DLLPUBLIC SvXMLAttrContainerData
{
private:
    boost::scoped_ptr<SvXMLAttrCollection> pimpl;

public:
    SvXMLAttrContainerData();
    SvXMLAttrContainerData(const SvXMLAttrContainerData &rCopy);
    ~SvXMLAttrContainerData();

    int  operator ==( const SvXMLAttrContainerData& rCmp ) const;

    sal_Bool AddAttr( const ::rtl::OUString& rLName, const rtl::OUString& rValue );
    sal_Bool AddAttr( const ::rtl::OUString& rPrefix, const rtl::OUString& rNamespace,
                    const ::rtl::OUString& rLName, const rtl::OUString& rValue );
    sal_Bool AddAttr( const ::rtl::OUString& rPrefix,
                  const ::rtl::OUString& rLName,
                  const ::rtl::OUString& rValue );

    size_t GetAttrCount() const;
    const rtl::OUString GetAttrNamespace( size_t i ) const;
    const rtl::OUString GetAttrPrefix( size_t i ) const;
    const rtl::OUString& GetAttrLName( size_t i ) const;
    const rtl::OUString& GetAttrValue( size_t i ) const;

    sal_uInt16 GetFirstNamespaceIndex() const;
    sal_uInt16 GetNextNamespaceIndex( sal_uInt16 nIdx ) const;
    const ::rtl::OUString& GetNamespace( sal_uInt16 i ) const;
    const ::rtl::OUString& GetPrefix( sal_uInt16 i ) const;

    sal_Bool SetAt( size_t i,
                const ::rtl::OUString& rLName, const rtl::OUString& rValue );
    sal_Bool SetAt( size_t i,
                const ::rtl::OUString& rPrefix, const rtl::OUString& rNamespace,
                const ::rtl::OUString& rLName, const rtl::OUString& rValue );
    sal_Bool SetAt( size_t i,
                const ::rtl::OUString& rPrefix,
                const ::rtl::OUString& rLName,
                const ::rtl::OUString& rValue );

    void Remove( size_t i );
};

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
