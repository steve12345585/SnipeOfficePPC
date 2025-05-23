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

#ifndef _XMLOFF_PROPMAPPINGTYPES_HXX
#define _XMLOFF_PROPMAPPINGTYPES_HXX

#include <tools/solar.h>
#include <com/sun/star/uno/Any.hxx>
#include <xmloff/xmltoken.hxx>
#include <unotools/saveopt.hxx>

///////////////////////////////////////////////////////////////////////////
//
/** Represents a property with its API-name, its XML-name and the type of
    its value.
*/
struct XMLPropertyMapEntry
{
    const sal_Char* msApiName;      /// Property-Name
    sal_Int32       nApiNameLength; /// length of property name
    sal_uInt16      mnNameSpace;    /** declares the Namespace in wich this
                                        property exists */
    enum ::xmloff::token::XMLTokenEnum meXMLName;       /// XML-Name
    sal_uInt32      mnType;         /** Bit 0..23 : XML-Type of its value
                                        Bit 24..31: The flags specifies
                                                    additional rules how
                                                    to im/export the porperty */
    sal_Int16       mnContextId;    /// User defined id for context filtering
    SvtSaveOptions::ODFDefaultVersion   mnEarliestODFVersionForExport;// no export when the used ODF version is lower than this
};

///////////////////////////////////////////////////////////////////////////
//
/** Smart struct to transport an Any with an index to the appropriate
    property-name
*/
struct XMLPropertyState
{
    sal_Int32                  mnIndex;
    ::com::sun::star::uno::Any maValue;

    XMLPropertyState( sal_Int32 nIndex )
        : mnIndex( nIndex ) {}
    XMLPropertyState( sal_Int32 nIndex, const ::com::sun::star::uno::Any& rValue )
        : mnIndex( nIndex ), maValue( rValue ) {}
    XMLPropertyState( const XMLPropertyState& rPropState )
        : mnIndex( rPropState.mnIndex ), maValue( rPropState.maValue ) {}
};

#endif // _XMLOFF_PROPMAPPINGTYPES_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
