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

#ifndef _XMLOFF_XMLASTPL_IMPL_HXX
#define _XMLOFF_XMLASTPL_IMPL_HXX

#include <boost/ptr_container/ptr_set.hpp>
#include <sal/types.h>
#include <rtl/ustring.hxx>
#include <set>
#include <vector>
#include <com/sun/star/uno/Reference.h>
#include <com/sun/star/xml/sax/XAttributeList.hpp>
#include <com/sun/star/xml/sax/XDocumentHandler.hpp>
#include <o3tl/sorted_vector.hxx>
#include <xmloff/maptype.hxx>
#include <xmloff/xmlexppr.hxx>

class SvXMLAutoStylePoolP;
class SvXMLAutoStylePoolParentsP_Impl;
typedef std::set<rtl::OUString> SvXMLAutoStylePoolNamesP_Impl;
class SvXMLExportPropertyMapper;
class SvXMLExport;

#define MAX_CACHE_SIZE 65536

///////////////////////////////////////////////////////////////////////////////
//
// Implementationclass for stylefamily-information
//

typedef ::rtl::OUString* OUStringPtr;
typedef ::std::vector< OUStringPtr > SvXMLAutoStylePoolCache_Impl;

class XMLFamilyData_Impl
{
public:
    SvXMLAutoStylePoolCache_Impl        *pCache;
    sal_uInt32                          mnFamily;
    ::rtl::OUString                     maStrFamilyName;
    UniReference < SvXMLExportPropertyMapper >  mxMapper;

    SvXMLAutoStylePoolParentsP_Impl*    mpParentList;
    SvXMLAutoStylePoolNamesP_Impl*      mpNameList;
    sal_uInt32                          mnCount;
    sal_uInt32                          mnName;
    ::rtl::OUString                     maStrPrefix;
    sal_Bool                            bAsFamily;

public:
    XMLFamilyData_Impl( sal_Int32 nFamily, const ::rtl::OUString& rStrName,
            const UniReference < SvXMLExportPropertyMapper > &  rMapper,
            const ::rtl::OUString& rStrPrefix, sal_Bool bAsFamily = sal_True );

    XMLFamilyData_Impl( sal_Int32 nFamily ) :
        pCache( 0 ),
        mnFamily( nFamily ), mpParentList( NULL ),
        mpNameList( NULL ), mnCount( 0 ), mnName( 0 )

    {}
    ~XMLFamilyData_Impl();

    friend bool operator<(const XMLFamilyData_Impl& r1, const XMLFamilyData_Impl& r2);

    void ClearEntries();
};

typedef boost::ptr_set<XMLFamilyData_Impl> XMLFamilyDataList_Impl;

///////////////////////////////////////////////////////////////////////////////
//
// Properties of a pool
//

class SvXMLAutoStylePoolPropertiesP_Impl
{
    ::rtl::OUString                     msName;
    ::std::vector< XMLPropertyState >   maProperties;
    sal_uInt32                          mnPos;

public:

    SvXMLAutoStylePoolPropertiesP_Impl( XMLFamilyData_Impl& rFamilyData, const ::std::vector< XMLPropertyState >& rProperties );

    ~SvXMLAutoStylePoolPropertiesP_Impl()
    {
    }

    const ::rtl::OUString& GetName() const { return msName; }
    const ::std::vector< XMLPropertyState >& GetProperties() const { return maProperties; }
    sal_uInt32 GetPos() const { return mnPos; }

    void SetName( const ::rtl::OUString& rNew ) { msName = rNew; }
};

typedef SvXMLAutoStylePoolPropertiesP_Impl* SvXMLAutoStylePoolPropertiesPPtr;
typedef ::std::vector< SvXMLAutoStylePoolPropertiesPPtr > SvXMLAutoStylePoolPropertiesPList_Impl;

///////////////////////////////////////////////////////////////////////////////
//
// Parents of AutoStylePool's
//

class SvXMLAutoStylePoolParentP_Impl
{
    ::rtl::OUString                         msParent;
    SvXMLAutoStylePoolPropertiesPList_Impl  maPropertiesList;

public:

    SvXMLAutoStylePoolParentP_Impl( const ::rtl::OUString & rParent ) :
        msParent( rParent )
    {
    }

    ~SvXMLAutoStylePoolParentP_Impl();

    sal_Bool Add( XMLFamilyData_Impl& rFamilyData, const ::std::vector< XMLPropertyState >& rProperties, ::rtl::OUString& rName, bool bDontSeek = false );

    sal_Bool AddNamed( XMLFamilyData_Impl& rFamilyData, const ::std::vector< XMLPropertyState >& rProperties, const ::rtl::OUString& rName );

    ::rtl::OUString Find( const XMLFamilyData_Impl& rFamilyData, const ::std::vector< XMLPropertyState >& rProperties ) const;

    const ::rtl::OUString& GetParent() const { return msParent; }

    const SvXMLAutoStylePoolPropertiesPList_Impl& GetPropertiesList() const
    {
        return maPropertiesList;
    }
};

struct SvXMLAutoStylePoolParentPCmp_Impl
{
    bool operator()( SvXMLAutoStylePoolParentP_Impl* const& lhs, SvXMLAutoStylePoolParentP_Impl* const& rhs) const
    {
        return lhs->GetParent().compareTo( rhs->GetParent() ) < 0;
    }
};
class SvXMLAutoStylePoolParentsP_Impl : public o3tl::sorted_vector<SvXMLAutoStylePoolParentP_Impl*, SvXMLAutoStylePoolParentPCmp_Impl>
{
public:
    ~SvXMLAutoStylePoolParentsP_Impl() { DeleteAndDestroyAll(); }
};

///////////////////////////////////////////////////////////////////////////////
//
// Implementationclass of SvXMLAutoStylePool
//

class SvXMLAutoStylePoolP_Impl
{
    SvXMLExport& rExport;

    XMLFamilyDataList_Impl      maFamilyList;

public:

    SvXMLAutoStylePoolP_Impl( SvXMLExport& rExport );
    ~SvXMLAutoStylePoolP_Impl();

    SvXMLExport& GetExport() const { return rExport; }

    void AddFamily( sal_Int32 nFamily, const ::rtl::OUString& rStrName,
        const UniReference < SvXMLExportPropertyMapper > & rMapper,
        const ::rtl::OUString& rStrPrefix, sal_Bool bAsFamily = sal_True );
    void SetFamilyPropSetMapper( sal_Int32 nFamily,
        const UniReference < SvXMLExportPropertyMapper > & rMapper );
    void RegisterName( sal_Int32 nFamily, const ::rtl::OUString& rName );
    void GetRegisteredNames(
        com::sun::star::uno::Sequence<sal_Int32>& aFamilies,
        com::sun::star::uno::Sequence<rtl::OUString>& aNames );

    sal_Bool Add( ::rtl::OUString& rName, sal_Int32 nFamily,
                const ::rtl::OUString& rParent,
                const ::std::vector< XMLPropertyState >& rProperties,
                sal_Bool bCache = sal_False,
                bool bDontSeek = false );
    sal_Bool AddNamed( const ::rtl::OUString& rName, sal_Int32 nFamily,
                const ::rtl::OUString& rParent,
                const ::std::vector< XMLPropertyState >& rProperties );

    ::rtl::OUString Find( sal_Int32 nFamily, const ::rtl::OUString& rParent,
                          const ::std::vector< XMLPropertyState >& rProperties ) const;

    void exportXML( sal_Int32 nFamily,
        const ::com::sun::star::uno::Reference< ::com::sun::star::xml::sax::XDocumentHandler > & rHandler,
        const SvXMLUnitConverter& rUnitConverter,
        const SvXMLNamespaceMap& rNamespaceMap,
        const SvXMLAutoStylePoolP *pAntiImpl) const;

    void ClearEntries();
};

struct SvXMLAutoStylePoolPExport_Impl
{
    const ::rtl::OUString                   *mpParent;
    const SvXMLAutoStylePoolPropertiesP_Impl    *mpProperties;
};

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
