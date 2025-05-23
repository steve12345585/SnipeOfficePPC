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
#ifndef INCLUDED__XMLOFF_CHARCONTEXT_HXX
#define INCLUDED__XMLOFF_CHARCONTEXT_HXX

#include "xmloff/dllapi.h"
#include <com/sun/star/uno/Reference.h>
#include "xmlictxt.hxx"

class XMLOFF_DLLPUBLIC XMLCharContext : public SvXMLImportContext
{
    XMLCharContext(const XMLCharContext&);
    void operator =(const XMLCharContext&);
protected:
    sal_Int16   m_nControl;
    sal_uInt16  m_nCount;
    sal_Unicode m_c;
public:

    TYPEINFO();

    XMLCharContext(
            SvXMLImport& rImport,
            sal_uInt16 nPrfx,
            const ::rtl::OUString& rLName,
            const ::com::sun::star::uno::Reference< ::com::sun::star::xml::sax::XAttributeList > & xAttrList,
            sal_Unicode c,
            sal_Bool bCount );
    XMLCharContext(
            SvXMLImport& rImport,
            sal_uInt16 nPrfx,
            const ::rtl::OUString& rLName,
            const ::com::sun::star::uno::Reference< ::com::sun::star::xml::sax::XAttributeList > & xAttrList,
            sal_Int16 nControl );

    virtual ~XMLCharContext();

    // EndElement is called before a context will be destructed, but
    // after a elements context has been parsed. It may be used for actions
    // that require virtual methods. The default is to do nothing.
    virtual void EndElement();

    virtual void InsertControlCharacter(sal_Int16   _nControl);
    virtual void InsertString(const ::rtl::OUString& _sString);
};
// ---------------------------------------------------------------------
#endif // INCLUDED__XMLOFF_CHARCONTEXT_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
