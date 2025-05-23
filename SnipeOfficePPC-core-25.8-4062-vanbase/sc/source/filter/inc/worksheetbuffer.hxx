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

#ifndef OOX_XLS_WORKSHEETBUFFER_HXX
#define OOX_XLS_WORKSHEETBUFFER_HXX

#include <utility>
#include "oox/helper/refmap.hxx"
#include "oox/helper/refvector.hxx"
#include "workbookhelper.hxx"

namespace com { namespace sun { namespace star {
    namespace i18n { class XCharacterClassification; }
} } }

namespace oox {
namespace xls {

// ============================================================================

/** Contains data from the 'sheet' element describing a sheet in the workbook. */
struct SheetInfoModel
{
    ::rtl::OUString     maRelId;        /// Relation identifier for the sheet substream.
    ::rtl::OUString     maName;         /// Original name of the sheet.
    sal_Int64           mnBiffHandle;   /// BIFF record handle of the sheet substream.
    sal_Int32           mnSheetId;      /// Sheet identifier.
    sal_Int32           mnState;        /// Visibility state.

    explicit            SheetInfoModel();
};

// ============================================================================

/** Stores information about all sheets in a spreadsheet document.

    Information about sheets includes the sheet name, the visibility state, and
    for the OOXML filter, the relation identifier of the sheet used to obtain
    the related worksheet fragment.
 */
class WorksheetBuffer : public WorkbookHelper
{
public:
    explicit            WorksheetBuffer( const WorkbookHelper& rHelper );

    /** Imports the attributes of a sheet element. */
    void                importSheet( const AttributeList& rAttribs );
    /** Imports the SHEET record from the passed BIFF12 stream. */
    void                importSheet( SequenceInputStream& rStrm );
    /** Inserts a new empty sheet into the document. Looks for an unused name.
         @return  Index of the new sheet in the Calc document. */
    sal_Int16           insertEmptySheet( const ::rtl::OUString& rPreferredName, bool bVisible );

    /** Returns the number of original sheets contained in the workbook. */
    sal_Int32           getWorksheetCount() const;
    /** Returns the OOXML relation identifier of the specified worksheet. */
    ::rtl::OUString     getWorksheetRelId( sal_Int32 nWorksheet ) const;

    /** Returns the Calc index of the specified worksheet. */
    sal_Int16           getCalcSheetIndex( sal_Int32 nWorksheet ) const;
    /** Returns the finalized name of the specified worksheet. */
    ::rtl::OUString     getCalcSheetName( sal_Int32 nWorksheet ) const;

    /** Returns the Calc index of the sheet with the passed original worksheet name. */
    sal_Int16           getCalcSheetIndex( const ::rtl::OUString& rWorksheetName ) const;
    /** Returns the finalized name of the sheet with the passed worksheet name. */
    ::rtl::OUString     getCalcSheetName( const ::rtl::OUString& rWorksheetName ) const;
    /** Converts sSheetNameRef (e.g. '#SheetName!A1' to '#SheetName.A1' )
        if sSheetNameRef doesn't start with '#' it is ignored and not modified
    */
    void                convertSheetNameRef( ::rtl::OUString& sSheetNameRef ) const;

private:
    struct SheetInfo : public SheetInfoModel
    {
        ::rtl::OUString     maCalcName;
        ::rtl::OUString     maCalcQuotedName;
        sal_Int16           mnCalcSheet;

        explicit            SheetInfo( const SheetInfoModel& rModel, sal_Int16 nCalcSheet, const ::rtl::OUString& rCalcName );
    };

    typedef ::std::pair< sal_Int16, ::rtl::OUString > IndexNamePair;

    /** Creates a new sheet in the Calc document. Does not insert anything in the own lists. */
    IndexNamePair       createSheet( const ::rtl::OUString& rPreferredName, sal_Int32 nSheetPos, bool bVisible );
    /** Creates a new sheet in the Calc document and inserts the related SheetInfo. */
    void                insertSheet( const SheetInfoModel& rModel );

private:
    typedef RefVector< SheetInfo > SheetInfoVector;
    SheetInfoVector     maSheetInfos;

    typedef RefMap< ::rtl::OUString, SheetInfo, IgnoreCaseCompare > SheetInfoMap;
    SheetInfoMap        maSheetInfosByName;
};

// ============================================================================

} // namespace xls
} // namespace oox

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
