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

#ifndef _TYPEDETECTION_EXPORT_HXX
#define _TYPEDETECTION_EXPORT_HXX

#include <com/sun/star/xml/sax/XWriter.hpp>
#include <com/sun/star/io/XOutputStream.hpp>

#include "xmlfilterjar.hxx"

class TypeDetectionExporter
{
public:
    TypeDetectionExporter( com::sun::star::uno::Reference< com::sun::star::uno::XComponentContext >& mxContext );

    void doExport(com::sun::star::uno::Reference < com::sun::star::io::XOutputStream > xOS,  const XMLFilterVector& rFilters );

private:
    void addProperty( com::sun::star::uno::Reference< com::sun::star::xml::sax::XWriter > xWriter, const rtl::OUString& rName, const rtl::OUString& rValue );
    void addLocaleProperty( com::sun::star::uno::Reference< com::sun::star::xml::sax::XWriter > xWriter, const rtl::OUString& rName, const rtl::OUString& rValue );

    com::sun::star::uno::Reference< com::sun::star::uno::XComponentContext > mxContext;
};

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
