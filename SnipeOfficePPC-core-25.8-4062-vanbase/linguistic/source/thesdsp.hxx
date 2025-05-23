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

#ifndef _LINGUISTIC_THESDSP_HXX_
#define _LINGUISTIC_THESDSP_HXX_

#include <com/sun/star/uno/Reference.h>
#include <com/sun/star/uno/Sequence.h>
#include <com/sun/star/beans/XPropertySet.hpp>
#include <com/sun/star/beans/XPropertyAccess.hpp>
#include <com/sun/star/beans/XPropertyChangeListener.hpp>
#include <com/sun/star/lang/XComponent.hpp>
#include <com/sun/star/lang/XInitialization.hpp>
#include <com/sun/star/lang/XServiceDisplayName.hpp>

#include <com/sun/star/lang/XServiceInfo.hpp>
#include <com/sun/star/linguistic2/XThesaurus.hpp>

#include <uno/lbnames.h>            // CPPU_CURRENT_LANGUAGE_BINDING_NAME macro, which specify the environment type
#include <cppuhelper/implbase1.hxx> // helper for implementations
#include <cppuhelper/implbase5.hxx> // helper for implementations
#include <cppuhelper/interfacecontainer.h>

#include <osl/mutex.hxx>

#include <boost/shared_ptr.hpp>
#include <map>

#include "lngopt.hxx"



class ThesaurusDispatcher :
    public cppu::WeakImplHelper1
    <
        ::com::sun::star::linguistic2::XThesaurus
    >,
    public LinguDispatcher
{
    typedef boost::shared_ptr< LangSvcEntries_Thes >                LangSvcEntries_Thes_Ptr_t;
    typedef std::map< LanguageType, LangSvcEntries_Thes_Ptr_t >     ThesSvcByLangMap_t;
    ThesSvcByLangMap_t      aSvcMap;

    ::com::sun::star::uno::Reference<
        ::com::sun::star::beans::XPropertySet >     xPropSet;

    // disallow copy-constructor and assignment-operator for now
    ThesaurusDispatcher(const ThesaurusDispatcher &);
    ThesaurusDispatcher & operator = (const ThesaurusDispatcher &);

    inline ::com::sun::star::uno::Reference<
        ::com::sun::star::beans::XPropertySet >
            GetPropSet();

    void    ClearSvcList();

public:
    ThesaurusDispatcher();
    virtual ~ThesaurusDispatcher();

    // XSupportedLocales
    virtual ::com::sun::star::uno::Sequence<
            ::com::sun::star::lang::Locale > SAL_CALL
        getLocales()
            throw(::com::sun::star::uno::RuntimeException);
    virtual sal_Bool SAL_CALL
        hasLocale( const ::com::sun::star::lang::Locale& aLocale )
            throw(::com::sun::star::uno::RuntimeException);

    // XThesaurus
    virtual ::com::sun::star::uno::Sequence<
            ::com::sun::star::uno::Reference<
                ::com::sun::star::linguistic2::XMeaning > > SAL_CALL
        queryMeanings( const ::rtl::OUString& aTerm,
                const ::com::sun::star::lang::Locale& aLocale,
                const ::com::sun::star::beans::PropertyValues& aProperties )
            throw(::com::sun::star::lang::IllegalArgumentException,
                  ::com::sun::star::uno::RuntimeException);

    // LinguDispatcher
    virtual void
        SetServiceList( const ::com::sun::star::lang::Locale &rLocale,
                const ::com::sun::star::uno::Sequence<
                    rtl::OUString > &rSvcImplNames );
    virtual ::com::sun::star::uno::Sequence< rtl::OUString >
        GetServiceList( const ::com::sun::star::lang::Locale &rLocale ) const;
    virtual DspType
        GetDspType() const;
};


inline ::com::sun::star::uno::Reference<
    ::com::sun::star::beans::XPropertySet >
        ThesaurusDispatcher::GetPropSet()
{
    return xPropSet.is() ?
        xPropSet : xPropSet = linguistic::GetLinguProperties();
}



#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
