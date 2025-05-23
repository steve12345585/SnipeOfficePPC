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

#ifndef XML_SETTINGS_EXPORT_CONTEXT_HXX
#define XML_SETTINGS_EXPORT_CONTEXT_HXX

#include <com/sun/star/lang/XMultiServiceFactory.hpp>

#include "xmloff/xmltoken.hxx"

//........................................................................
namespace xmloff
{
//........................................................................

    //====================================================================
    //= XMLExporter
    //====================================================================
    class SAL_NO_VTABLE XMLSettingsExportContext
    {
    public:
        virtual void    AddAttribute( enum ::xmloff::token::XMLTokenEnum i_eName,
                                      const ::rtl::OUString& i_rValue ) = 0;
        virtual void    AddAttribute( enum ::xmloff::token::XMLTokenEnum i_eName,
                                      enum ::xmloff::token::XMLTokenEnum i_eValue ) = 0;

        virtual void    StartElement( enum ::xmloff::token::XMLTokenEnum i_eName,
                                      const sal_Bool i_bIgnoreWhitespace ) = 0;
        virtual void    EndElement(   const sal_Bool i_bIgnoreWhitespace ) = 0;

        virtual void    Characters( const ::rtl::OUString& i_rCharacters ) = 0;

        virtual ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory >
                        GetServiceFactory() const = 0;

    protected:
        ~XMLSettingsExportContext() {}
    };

//........................................................................
} // namespace xmloff
//........................................................................

#endif // XML_SETTINGS_EXPORT_CONTEXT_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
