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

#ifndef OOX_XLS_NUMBERFORMATSBUFFER_HXX
#define OOX_XLS_NUMBERFORMATSBUFFER_HXX

#include <com/sun/star/lang/Locale.hpp>
#include <svl/itemset.hxx>
#include "workbookhelper.hxx"

namespace com { namespace sun { namespace star {
    namespace util { class XNumberFormats; }
} } }

namespace oox { class PropertyMap; }

namespace oox {
namespace xls {

// ============================================================================

struct NumFmtModel
{
    ::com::sun::star::lang::Locale maLocale;
    ::rtl::OUString     maFmtCode;
    sal_Int16           mnPredefId;

    explicit            NumFmtModel();
};

// ----------------------------------------------------------------------------

/** Contains all API number format attributes. */
struct ApiNumFmtData
{
    sal_Int32           mnIndex;            /// API number format index.

    explicit            ApiNumFmtData();
};

// ----------------------------------------------------------------------------

/** Contains all data for a number format code. */
class NumberFormat : public WorkbookHelper
{
public:
    explicit            NumberFormat( const WorkbookHelper& rHelper );

    /** Sets the passed format code. */
    void                setFormatCode( const ::rtl::OUString& rFmtCode );
    /** Sets the passed format code, encoded in UTF-8. */
    void                setFormatCode(
                            const ::com::sun::star::lang::Locale& rLocale,
                            const sal_Char* pcFmtCode );
    /** Sets the passed predefined format code identifier. */
    void                setPredefinedId(
                            const ::com::sun::star::lang::Locale& rLocale,
                            sal_Int16 nPredefId );

    /** Final processing after import of all style settings. Returns the API format index. */
    sal_Int32           finalizeImport(
                            const ::com::sun::star::uno::Reference< ::com::sun::star::util::XNumberFormats >& rxNumFmts,
                            const ::com::sun::star::lang::Locale& rFromLocale );
    void                fillToItemSet( SfxItemSet& rItemSet, bool bSkipPoolDefs ) const;
    /** Writes the number format to the passed property map. */
    void                writeToPropertyMap( PropertyMap& rPropMap ) const;

private:
    NumFmtModel         maModel;
    ApiNumFmtData       maApiData;
};

typedef ::boost::shared_ptr< NumberFormat > NumberFormatRef;

// ============================================================================

class NumberFormatsBuffer : public WorkbookHelper
{
public:
    explicit            NumberFormatsBuffer( const WorkbookHelper& rHelper );

    /** Inserts a new number format. */
    NumberFormatRef     createNumFmt( sal_Int32 nNumFmtId, const ::rtl::OUString& rFmtCode );

    /** Inserts a new number format code. */
    NumberFormatRef     importNumFmt( const AttributeList& rAttribs );
    /** Inserts a new number format code from a NUMFMT record. */
    void                importNumFmt( SequenceInputStream& rStrm );

    /** Final processing after import of all style settings. */
    void                finalizeImport();

    void                fillToItemSet( SfxItemSet& rItemSet, sal_Int32 nNumFmtId, bool bSkipPoolDefs = false ) const;

    /** Writes the specified number format to the passed property map. */
    void                writeToPropertyMap( PropertyMap& rPropMap, sal_Int32 nNumFmtId ) const;
    sal_Int32           nextFreeId(){ return ++mnHighestId; }
private:
    /** Inserts built-in number formats for the current system language. */
    void                insertBuiltinFormats();

private:
    typedef RefMap< sal_Int32, NumberFormat > NumberFormatMap;

    NumberFormatMap     maNumFmts;          /// List of number formats.
    ::rtl::OUString     maLocaleStr;        /// Current office locale.
    sal_Int32           mnHighestId;
};

// ============================================================================

} // namespace xls
} // namespace oox

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
