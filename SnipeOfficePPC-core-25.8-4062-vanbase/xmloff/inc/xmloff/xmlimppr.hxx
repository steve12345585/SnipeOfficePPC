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
#ifndef _XMLOFF_XMLIMPPR_HXX
#define _XMLOFF_XMLIMPPR_HXX

#include "sal/config.h"
#include "xmloff/dllapi.h"
#include "sal/types.h"
#include <tools/solar.h>
#include <com/sun/star/xml/sax/XAttributeList.hpp>
#include <com/sun/star/beans/XPropertySet.hpp>
#include <com/sun/star/beans/PropertyValue.hpp>
#include <com/sun/star/beans/XMultiPropertySet.hpp>
#include <com/sun/star/beans/XTolerantMultiPropertySet.hpp>

#include <vector>
#include <xmloff/uniref.hxx>

struct XMLPropertyState;
class XMLPropertySetMapper;

namespace rtl { class OUString; }

class SvXMLUnitConverter;
class SvXMLNamespaceMap;
class SvXMLImport;

/** This struct is used as an optional parameter to the static
 * _FillPropertySet() methods.
 *
 * It should not be used in any other context.
 */
struct _ContextID_Index_Pair
{
    sal_Int16 nContextID;
    sal_Int32 nIndex;
};

class XMLOFF_DLLPUBLIC SvXMLImportPropertyMapper : public UniRefBase
{
    UniReference< SvXMLImportPropertyMapper> mxNextMapper;

    SvXMLImport& rImport;   // access to error handling

    SAL_DLLPRIVATE SvXMLImportPropertyMapper(SvXMLImportPropertyMapper &);
        // not defined
    SAL_DLLPRIVATE void operator =(SvXMLImportPropertyMapper &); // not defined

protected:

    UniReference< XMLPropertySetMapper > maPropMapper;
    SvXMLImport& GetImport() const { return rImport;}

public:

    SvXMLImportPropertyMapper(
            const UniReference< XMLPropertySetMapper >& rMapper,
            SvXMLImport& rImport);
    virtual ~SvXMLImportPropertyMapper();

    // Add a ImportPropertyMapper at the end of the import mapper chain.
    // The added mapper MUST not be used outside the Mapper chain any longer,
    // because its PropertyMapper will be replaced.
    void ChainImportMapper(
        const UniReference< SvXMLImportPropertyMapper>& rMapper );

    /** fills the given itemset with the attributes in the given list
      * the map is only searched within the range
      * [nStartIdx, nEndIdx[
      */
    void importXML(
            ::std::vector< XMLPropertyState >& rProperties,
            ::com::sun::star::uno::Reference<
                    ::com::sun::star::xml::sax::XAttributeList > xAttrList,
            const SvXMLUnitConverter& rUnitConverter,
            const SvXMLNamespaceMap& rNamespaceMap,
            sal_uInt32 nPropType,
            sal_Int32 nStartIdx, sal_Int32 nEndIdx ) const;

    /** this method is called for every item that has the MID_FLAG_SPECIAL_ITEM_IMPORT flag set */
    virtual bool handleSpecialItem(
            XMLPropertyState& rProperty,
            ::std::vector< XMLPropertyState >& rProperties,
            const ::rtl::OUString& rValue,
            const SvXMLUnitConverter& rUnitConverter,
            const SvXMLNamespaceMap& rNamespaceMap ) const;

    /** This method is called when all attributes have benn processed. It may be used to remove items that are incomplete */
    virtual void finished(
            ::std::vector< XMLPropertyState >& rProperties,
            sal_Int32 nStartIndex, sal_Int32 nEndIndex ) const;

    void CheckSpecialContext(
            const ::std::vector< XMLPropertyState >& aProperties,
            const ::com::sun::star::uno::Reference<
                    ::com::sun::star::beans::XPropertySet > rPropSet,
            _ContextID_Index_Pair* pSpecialContextIds ) const;

    sal_Bool FillPropertySet(
            const ::std::vector< XMLPropertyState >& aProperties,
            const ::com::sun::star::uno::Reference<
                    ::com::sun::star::beans::XPropertySet > rPropSet,
            _ContextID_Index_Pair* pSpecialContextIds = NULL ) const;

    void FillPropertySequence(
            const ::std::vector< XMLPropertyState >& aProperties,
            ::com::sun::star::uno::Sequence< ::com::sun::star::beans::PropertyValue >& rValues )
            const;

    inline const UniReference< XMLPropertySetMapper >&
        getPropertySetMapper() const;



    /** implementation helper for FillPropertySet: fill an XPropertySet.
     * Exceptions will be asserted. */
    static sal_Bool _FillPropertySet(
        const ::std::vector<XMLPropertyState> & rProperties,
        const ::com::sun::star::uno::Reference<
            ::com::sun::star::beans::XPropertySet> & rPropSet,
        const ::com::sun::star::uno::Reference<
            ::com::sun::star::beans::XPropertySetInfo> & rPropSetInfo,
        const UniReference<XMLPropertySetMapper> & rPropMapper,
        SvXMLImport& rImport,

        // parameter for use by txtstyli.cxx; allows efficient
        // catching the combined characters property
        _ContextID_Index_Pair* pSpecialContextIds = NULL );

    /** implementation helper for FillPropertySet: fill an XMultiPropertySet.
     * If unsuccessul, set return value. */
    static sal_Bool _FillMultiPropertySet(
        const ::std::vector<XMLPropertyState> & rProperties,
        const ::com::sun::star::uno::Reference<
            ::com::sun::star::beans::XMultiPropertySet> & rMultiPropSet,
        const ::com::sun::star::uno::Reference<
            ::com::sun::star::beans::XPropertySetInfo> & rPropSetInfo,
        const UniReference<XMLPropertySetMapper> & rPropMapper,

        // parameters for use by txtstyli.cxx; allows efficient
        // catching the combined characters property
        _ContextID_Index_Pair* pSpecialContextIds = NULL );

    /** implementation helper for FillPropertySet: fill an XTolerantMultiPropertySet.
     * If unsuccessul, set return value. */
    static sal_Bool _FillTolerantMultiPropertySet(
        const ::std::vector<XMLPropertyState> & rProperties,
        const ::com::sun::star::uno::Reference<
            ::com::sun::star::beans::XTolerantMultiPropertySet> & rTolPropSet,
        const UniReference<XMLPropertySetMapper> & rPropMapper,
        SvXMLImport& rImport,

        // parameters for use by txtstyli.cxx; allows efficient
        // catching the combined characters property
        _ContextID_Index_Pair* pSpecialContextIds = NULL );


    static void _PrepareForMultiPropertySet(
        const ::std::vector<XMLPropertyState> & rProperties,
        const ::com::sun::star::uno::Reference<
            ::com::sun::star::beans::XPropertySetInfo> & rPropSetInfo,
        const UniReference<XMLPropertySetMapper> & rPropMapper,
        _ContextID_Index_Pair* pSpecialContextIds,
        ::com::sun::star::uno::Sequence< ::rtl::OUString >& rNames,
        ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Any >& rValues);
};


inline const UniReference< XMLPropertySetMapper >&
    SvXMLImportPropertyMapper::getPropertySetMapper() const
{
    return maPropMapper;
}

#endif  //  _XMLOFF_XMLIMPPR_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
