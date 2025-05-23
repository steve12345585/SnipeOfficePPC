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

#ifndef OOX_XLS_WORKSHEETFRAGMENT_HXX
#define OOX_XLS_WORKSHEETFRAGMENT_HXX

#include "excelhandlers.hxx"

namespace oox {
namespace xls {

// ============================================================================

class DataValidationsContext : public WorksheetContextBase
{
public:
    explicit            DataValidationsContext( WorksheetFragmentBase& rFragment );

protected:
    virtual ::oox::core::ContextHandlerRef onCreateContext( sal_Int32 nElement, const AttributeList& rAttribs );
    virtual void        onCharacters( const ::rtl::OUString& rChars );
    virtual void        onEndElement();

    virtual ::oox::core::ContextHandlerRef onCreateRecordContext( sal_Int32 nRecId, SequenceInputStream& rStrm );

private:
    /** Imports the dataValidation element containing data validation settings. */
    void                importDataValidation( const AttributeList& rAttribs );
    /** Imports the DATAVALIDATION record containing data validation settings. */
    void                importDataValidation( SequenceInputStream& rStrm );

private:
    ::std::auto_ptr< ValidationModel > mxValModel;
};

// ============================================================================

class WorksheetFragment : public WorksheetFragmentBase
{
public:
    explicit            WorksheetFragment(
                            const WorksheetHelper& rHelper,
                            const ::rtl::OUString& rFragmentPath );

protected:
    virtual ::oox::core::ContextHandlerRef onCreateContext( sal_Int32 nElement, const AttributeList& rAttribs );
    virtual void        onCharacters( const ::rtl::OUString& rChars );

    virtual ::oox::core::ContextHandlerRef onCreateRecordContext( sal_Int32 nRecId, SequenceInputStream& rStrm );

    virtual const ::oox::core::RecordInfo* getRecordInfos() const;
    virtual void        initializeImport();
    virtual void        finalizeImport();

private:
    /** Imports page settings from a pageSetUpPr element. */
    void                importPageSetUpPr( const AttributeList& rAttribs );
    /** Imports the dimension element containing the used area of the sheet. */
    void                importDimension( const AttributeList& rAttribs );
    /** Imports sheet format properties from a sheetFormatPr element. */
    void                importSheetFormatPr( const AttributeList& rAttribs );
    /** Imports column settings from a col element. */
    void                importCol( const AttributeList& rAttribs );
    /** Imports a merged cell range from a mergeCell element. */
    void                importMergeCell( const AttributeList& rAttribs );
    /** Imports the hyperlink element containing a hyperlink for a cell range. */
    void                importHyperlink( const AttributeList& rAttribs );
    /** Imports individual break that is either within row or column break context. */
    void                importBrk( const AttributeList& rAttribs, bool bRowBreak );
    /** Imports the the relation identifier for the DrawingML part. */
    void                importDrawing( const AttributeList& rAttribs );
    /** Imports the the relation identifier for the legacy VML drawing part. */
    void                importLegacyDrawing( const AttributeList& rAttribs );
    /** Imports additional data for an OLE object. */
    void                importOleObject( const AttributeList& rAttribs );
    /** Imports additional data for an OCX form control. */
    void                importControl( const AttributeList& rAttribs );

    /** Imports the DIMENSION record containing the used area of the sheet. */
    void                importDimension( SequenceInputStream& rStrm );
    /** Imports sheet format properties from a SHEETFORMATPR record. */
    void                importSheetFormatPr( SequenceInputStream& rStrm );
    /** Imports column settings from a COL record. */
    void                importCol( SequenceInputStream& rStrm );
    /** Imports a merged cell range from a MERGECELL record. */
    void                importMergeCell( SequenceInputStream& rStrm );
    /** Imports a hyperlink for a cell range from a HYPERLINK record. */
    void                importHyperlink( SequenceInputStream& rStrm );
    /** Imports the BRK record for an individual row or column page break. */
    void                importBrk( SequenceInputStream& rStrm, bool bRowBreak );
    /** Imports the DRAWING record containing the relation identifier for the DrawingML part. */
    void                importDrawing( SequenceInputStream& rStrm );
    /** Imports the LEGACYDRAWING record containing the relation identifier for the VML drawing part. */
    void                importLegacyDrawing( SequenceInputStream& rStrm );
    /** Imports additional data for an OLE object. */
    void                importOleObject( SequenceInputStream& rStrm );
    /** Imports additional data for an OCX form control. */
    void                importControl( SequenceInputStream& rStrm );

    /** Imports the binary data of an embedded OLE object from the fragment with the passed ID. */
    void                importEmbeddedOleData( StreamDataSequence& orEmbeddedData, const ::rtl::OUString& rRelId );
};

} // namespace xls
} // namespace oox

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
