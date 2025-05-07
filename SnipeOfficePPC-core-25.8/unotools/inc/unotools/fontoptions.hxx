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
#ifndef INCLUDED_unotools_FONTOPTIONS_HXX
#define INCLUDED_unotools_FONTOPTIONS_HXX

#include "unotools/unotoolsdllapi.h"
#include <sal/types.h>
#include <osl/mutex.hxx>
#include <rtl/ustring.hxx>
#include <unotools/options.hxx>

/*-************************************************************************************************************//**
    @short          forward declaration to our private date container implementation
    @descr          We use these class as internal member to support small memory requirements.
                    You can create the container if it is neccessary. The class which use these mechanism
                    is faster and smaller then a complete implementation!
*//*-*************************************************************************************************************/

class SvtFontOptions_Impl;

/*-************************************************************************************************************//**
    @short          collect informations about font features
    @descr          -

    @implements     -
    @base           -

    @devstatus      ready to use
*//*-*************************************************************************************************************/

class UNOTOOLS_DLLPUBLIC SvtFontOptions: public utl::detail::Options
{
    public:
        /*-****************************************************************************************************//**
            @short      standard constructor and destructor
            @descr      This will initialize an instance with default values.
                        We implement these class with a refcount mechanism! Every instance of this class increase it
                        at create and decrease it at delete time - but all instances use the same data container!
                        He is implemented as a static member ...

            @seealso    member m_nRefCount
            @seealso    member m_pDataContainer

            @param      -
            @return     -

            @onerror    -
        *//*-*****************************************************************************************************/

         SvtFontOptions();
        virtual ~SvtFontOptions();

        /*-****************************************************************************************************//**
            @short      interface methods to get and set value of config key "org.openoffice.Office.Common/Font"
            @descr      These values defines different states of font handling.

                        ..ReplacementTable..()  =>  Determines if the list of font replacements is applied or not.

                        ..FontHistory..()       =>  The last 5 fonts will be shown and the last one will be the
                                                    first name on the list. These will be displayed in the Font name
                                                    combo box on the Object bar.

                        ..FontWYSIWYG..()       =>  With this option the names of the selectable fonts
                                                    (for example, the fonts in the Font field in the object bar)
                                                    will be formatted as the current font.

            @seealso    -

            @param      -
            @return     -

            @onerror    No error should occurre!
        *//*-*****************************************************************************************************/

        sal_Bool    IsFontHistoryEnabled        (                   ) const ;
        void        EnableFontHistory           ( sal_Bool bState   )       ;

        sal_Bool    IsFontWYSIWYGEnabled        (                   ) const ;
        void        EnableFontWYSIWYG           ( sal_Bool bState   )       ;
    private:

        /*-****************************************************************************************************//**
            @short      return a reference to a static mutex
            @descr      These class use his own static mutex to be threadsafe.
                        We create a static mutex only for one ime and use at different times.

            @seealso    -

            @param      -
            @return     A reference to a static mutex member.

            @onerror    -
        *//*-*****************************************************************************************************/

        UNOTOOLS_DLLPRIVATE static ::osl::Mutex& impl_GetOwnStaticMutex();
    private:

        /*Attention

            Don't initialize these static member in these header!
            a) Double dfined symbols will be detected ...
            b) and unresolved externals exist at linking time.
            Do it in your source only.
         */

        static SvtFontOptions_Impl*         m_pDataContainer    ;   /// impl. data container as dynamic pointer for smaller memory requirements!
        static sal_Int32                    m_nRefCount         ;   /// internal ref count mechanism

};      // class SvtFontOptions

#endif  // #ifndef INCLUDED_unotools_FONTOPTIONS_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
