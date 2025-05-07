/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * Copyright 2000, 2010 Oracle and/or its affiliates.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * This file is part of OpenOffice.org.
 *
 * OpenOffice.org is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License version 3
 * only, as published by the Free Software Foundation.
 *
 * OpenOffice.org is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License version 3 for more details
 * (a copy is included in the LICENSE file that accompanied this code).
 *
 * You should have received a copy of the GNU Lesser General Public License
 * version 3 along with OpenOffice.org.  If not, see
 * <http://www.openoffice.org/license.html>
 * for a copy of the LGPLv3 License.
 *
 ************************************************************************/

/*TODO outline this implementation :-) */

#ifndef __FRAMEWORK_PROTOCOLS_H_
#define __FRAMEWORK_PROTOCOLS_H_

#include <rtl/ustring.hxx>

namespace framework{

//_______________________________________________________________________
/**
    some protocols must be checked during loading or dispatching URLs manually
    It can be neccessary to decide, if a URL represent a non visible content or
    a real visible component.
 */

// indicates a loadable content in general!
#define SPECIALPROTOCOL_PRIVATE           "private:"
// indicates loading of components using a model directly
#define SPECIALPROTOCOL_PRIVATE_OBJECT    "private:object"
// indicates loading of components using a stream only
#define SPECIALPROTOCOL_PRIVATE_STREAM    "private:stream"
// indicates creation of empty documents
#define SPECIALPROTOCOL_PRIVATE_FACTORY   "private:factory"
// internal protocol of the sfx project for generic dispatch funtionality
#define SPECIALPROTOCOL_SLOT              "slot:"
// external representation of the slot protocol using names instead of id's
#define SPECIALPROTOCOL_UNO               ".uno:"
// special sfx protocol to execute macros
#define SPECIALPROTOCOL_MACRO             "macro:"
// generic way to start uno services during dispatch
#define SPECIALPROTOCOL_SERVICE           "service:"
// for sending mails
#define SPECIALPROTOCOL_MAILTO            "mailto:"
// for sending news
#define SPECIALPROTOCOL_NEWS              "news:"

class ProtocolCheck
{
    public:

    //_______________________________________________________________________
    /**
        enums for well known protocols
     */
    enum EProtocol
    {
        E_UNKNOWN_PROTOCOL  ,
        E_PRIVATE           ,
        E_PRIVATE_OBJECT    ,
        E_PRIVATE_STREAM    ,
        E_PRIVATE_FACTORY   ,
        E_SLOT              ,
        E_UNO               ,
        E_MACRO             ,
        E_SERVICE           ,
        E_MAILTO            ,
        E_NEWS
    };

    //_______________________________________________________________________
    /**
        it checks, if the given URL string match one of the well known protocols.
        It returns the right enum value.
        Protocols are defined above ...
     */
    static EProtocol specifyProtocol( const ::rtl::OUString& sURL )
    {
        // because "private:" is part of e.g. "private:object" too ...
        // we must check it before all other ones!!!
        if (sURL.matchAsciiL(RTL_CONSTASCII_STRINGPARAM(SPECIALPROTOCOL_PRIVATE)))
            return E_PRIVATE;
        else
        if (sURL.matchAsciiL(RTL_CONSTASCII_STRINGPARAM(SPECIALPROTOCOL_PRIVATE_OBJECT)))
            return E_PRIVATE_OBJECT;
        else
        if (sURL.matchAsciiL(RTL_CONSTASCII_STRINGPARAM(SPECIALPROTOCOL_PRIVATE_STREAM)))
            return E_PRIVATE_STREAM;
        else
        if (sURL.matchAsciiL(RTL_CONSTASCII_STRINGPARAM(SPECIALPROTOCOL_PRIVATE_FACTORY)))
            return E_PRIVATE_FACTORY;
        else
        if (sURL.matchAsciiL(RTL_CONSTASCII_STRINGPARAM(SPECIALPROTOCOL_SLOT)))
            return E_SLOT;
        else
        if (sURL.matchAsciiL(RTL_CONSTASCII_STRINGPARAM(SPECIALPROTOCOL_UNO)))
            return E_UNO;
        else
        if (sURL.matchAsciiL(RTL_CONSTASCII_STRINGPARAM(SPECIALPROTOCOL_MACRO)))
            return E_MACRO;
        else
        if (sURL.matchAsciiL(RTL_CONSTASCII_STRINGPARAM(SPECIALPROTOCOL_SERVICE)))
            return E_SERVICE;
        else
        if (sURL.matchAsciiL(RTL_CONSTASCII_STRINGPARAM(SPECIALPROTOCOL_MAILTO)))
            return E_MAILTO;
        else
        if (sURL.matchAsciiL(RTL_CONSTASCII_STRINGPARAM(SPECIALPROTOCOL_NEWS)))
            return E_NEWS;
        else
            return E_UNKNOWN_PROTOCOL;
    }

    //_______________________________________________________________________
    /**
        it checks if given URL match the required protocol only
        It should be used instead of specifyProtocol() if only this question
        is interesting to perform the code. We must not check for all possible protocols here...
     */
    static sal_Bool isProtocol( const ::rtl::OUString& sURL, EProtocol eRequired )
    {
        sal_Bool bRet = sal_False;
        switch(eRequired)
        {
            case E_PRIVATE:
                bRet = sURL.matchAsciiL(RTL_CONSTASCII_STRINGPARAM(SPECIALPROTOCOL_PRIVATE));
                break;
            case E_PRIVATE_OBJECT:
                bRet = sURL.matchAsciiL(RTL_CONSTASCII_STRINGPARAM(SPECIALPROTOCOL_PRIVATE_OBJECT));
                break;
            case E_PRIVATE_STREAM:
                bRet = sURL.matchAsciiL(RTL_CONSTASCII_STRINGPARAM(SPECIALPROTOCOL_PRIVATE_STREAM));
                break;
            case E_PRIVATE_FACTORY:
                bRet = sURL.matchAsciiL(RTL_CONSTASCII_STRINGPARAM(SPECIALPROTOCOL_PRIVATE_FACTORY));
                break;
            case E_SLOT:
                bRet = sURL.matchAsciiL(RTL_CONSTASCII_STRINGPARAM(SPECIALPROTOCOL_SLOT));
                break;
            case E_UNO:
                bRet = sURL.matchAsciiL(RTL_CONSTASCII_STRINGPARAM(SPECIALPROTOCOL_UNO));
                break;
            case E_MACRO:
                bRet = sURL.matchAsciiL(RTL_CONSTASCII_STRINGPARAM(SPECIALPROTOCOL_MACRO));
                break;
            case E_SERVICE:
                bRet = sURL.matchAsciiL(RTL_CONSTASCII_STRINGPARAM(SPECIALPROTOCOL_SERVICE));
                break;
            case E_MAILTO:
                bRet = sURL.matchAsciiL(RTL_CONSTASCII_STRINGPARAM(SPECIALPROTOCOL_MAILTO));
                break;
            case E_NEWS:
                bRet = sURL.matchAsciiL(RTL_CONSTASCII_STRINGPARAM(SPECIALPROTOCOL_NEWS));
                break;
            default:
                bRet = sal_False;
                break;
        }
        return bRet;
    }
};

} // namespace framework

#endif // #ifndef __FRAMEWORK_PROTOCOLS_H_

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
