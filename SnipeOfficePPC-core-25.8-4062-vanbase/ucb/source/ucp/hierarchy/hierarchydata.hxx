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

#ifndef _HIERARCHYDATA_HXX
#define _HIERARCHYDATA_HXX

#include <rtl/ustring.hxx>
#include <osl/mutex.hxx>
#include <com/sun/star/lang/XMultiServiceFactory.hpp>
#include <com/sun/star/uno/XComponentContext.hpp>

namespace com { namespace sun { namespace star {
    namespace container {
        class XHierarchicalNameAccess;
    }
    namespace util {
        class XOfficeInstallationDirectories;
    }
} } }

namespace hierarchy_ucp
{

//=========================================================================

class HierarchyEntryData
{
public:
    enum Type { NONE, LINK, FOLDER };

    HierarchyEntryData() : m_aType( NONE ) {}
    HierarchyEntryData( const Type & rType ) : m_aType( rType ) {}

    const rtl::OUString & getName() const { return m_aName; }
    void setName( const rtl::OUString & rName ) { m_aName = rName; }

    const rtl::OUString & getTitle() const { return m_aTitle; }
    void setTitle( const rtl::OUString & rTitle ) { m_aTitle = rTitle; }

    const rtl::OUString & getTargetURL() const { return m_aTargetURL; }
    void setTargetURL( const rtl::OUString & rURL ) { m_aTargetURL = rURL; }

    Type getType() const
    { return ( m_aType != NONE ) ? m_aType
                                 : m_aTargetURL.getLength()
                                    ? LINK
                                    : FOLDER; }
    void setType( const Type & rType ) { m_aType = rType; }

private:
    rtl::OUString m_aName;      // Name (language independent)
    rtl::OUString m_aTitle;     // Title (language dependent)
    rtl::OUString m_aTargetURL; // Target URL ( links only )
    Type          m_aType;      // Type
};

//=========================================================================

class HierarchyContentProvider;
class HierarchyUri;

class HierarchyEntry
{
    ::rtl::OUString m_aServiceSpecifier;
    ::rtl::OUString m_aName;
    ::rtl::OUString m_aPath;
    ::osl::Mutex    m_aMutex;
    ::com::sun::star::uno::Reference<
            ::com::sun::star::uno::XComponentContext >     m_xContext;
    ::com::sun::star::uno::Reference<
            ::com::sun::star::lang::XMultiServiceFactory > m_xConfigProvider;
    ::com::sun::star::uno::Reference<
            ::com::sun::star::container::XHierarchicalNameAccess >
                                                           m_xRootReadAccess;
    ::com::sun::star::uno::Reference<
            ::com::sun::star::util::XOfficeInstallationDirectories >
                                                           m_xOfficeInstDirs;
    sal_Bool m_bTriedToGetRootReadAccess;  // #82494#

private:
    ::rtl::OUString createPathFromHierarchyURL( const HierarchyUri & rURI );
    ::com::sun::star::uno::Reference<
            ::com::sun::star::container::XHierarchicalNameAccess >
    getRootReadAccess();

public:
    HierarchyEntry( const ::com::sun::star::uno::Reference<
                        ::com::sun::star::uno::XComponentContext >& rxContext,
                    HierarchyContentProvider* pProvider,
                    const ::rtl::OUString& rURL );

    sal_Bool hasData();

    sal_Bool getData( HierarchyEntryData& rData );

    sal_Bool setData( const HierarchyEntryData& rData, sal_Bool bCreate );

    sal_Bool move( const ::rtl::OUString& rNewURL,
                   const HierarchyEntryData& rData );

    sal_Bool remove();

    // Iteration.

    struct iterator_Impl;

    class iterator
    {
    friend class HierarchyEntry;

        iterator_Impl*  m_pImpl;

    public:
        iterator();
        ~iterator();

        const HierarchyEntryData& operator*() const;
    };

    sal_Bool first( iterator& it );
    sal_Bool next ( iterator& it );
};

} // namespace hierarchy_ucp

#endif /* !_HIERARCHYDATA_HXX */

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
