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

#ifndef _XMLOFF_EVENTOASISTCONTEXT_HXX
#define _XMLOFF_EVENTOASISTCONTEXT_HXX

#include "RenameElemTContext.hxx"

class XMLTransformerOASISEventMap_Impl;

class XMLEventOASISTransformerContext : public XMLRenameElemTransformerContext
{
public:
    TYPEINFO();

    XMLEventOASISTransformerContext( XMLTransformerBase& rTransformer,
                           const ::rtl::OUString& rQName );
    virtual ~XMLEventOASISTransformerContext();

    static XMLTransformerOASISEventMap_Impl *CreateFormEventMap();
    static XMLTransformerOASISEventMap_Impl *CreateEventMap();
    static void FlushEventMap( XMLTransformerOASISEventMap_Impl *p );
    static ::rtl::OUString GetEventName( sal_uInt16 nPrefix,
                             const ::rtl::OUString& rName,
                             XMLTransformerOASISEventMap_Impl& rMap,
                             XMLTransformerOASISEventMap_Impl* pMap2    );

    virtual void StartElement( const ::com::sun::star::uno::Reference< ::com::sun::star::xml::sax::XAttributeList >& xAttrList );
};

#endif  //  _XMLOFF_EVENTOASISTCONTEXT_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
