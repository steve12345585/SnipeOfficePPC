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

#ifndef OOX_XLS_PIVOTCACHEFRAGMENT_HXX
#define OOX_XLS_PIVOTCACHEFRAGMENT_HXX

#include "excelhandlers.hxx"

namespace oox {
namespace xls {

class PivotCache;
class PivotCacheField;

// ============================================================================

class PivotCacheFieldContext : public WorkbookContextBase
{
public:
    explicit            PivotCacheFieldContext(
                            WorkbookFragmentBase& rFragment,
                            PivotCacheField& rCacheField );

protected:
    virtual ::oox::core::ContextHandlerRef onCreateContext( sal_Int32 nElement, const AttributeList& rAttribs );
    virtual void        onStartElement( const AttributeList& rAttribs );
    virtual ::oox::core::ContextHandlerRef onCreateRecordContext( sal_Int32 nRecId, SequenceInputStream& rStrm );
    virtual void        onStartRecord( SequenceInputStream& rStrm );

private:
    PivotCacheField&    mrCacheField;
};

// ============================================================================

class PivotCacheDefinitionFragment : public WorkbookFragmentBase
{
public:
    explicit            PivotCacheDefinitionFragment(
                            const WorkbookHelper& rHelper,
                            const ::rtl::OUString& rFragmentPath,
                            PivotCache& rPivotCache );

protected:
    virtual ::oox::core::ContextHandlerRef onCreateContext( sal_Int32 nElement, const AttributeList& rAttribs );
    virtual ::oox::core::ContextHandlerRef onCreateRecordContext( sal_Int32 nRecId, SequenceInputStream& rStrm );
    virtual const ::oox::core::RecordInfo* getRecordInfos() const;
    virtual void        finalizeImport();

private:
    PivotCache&         mrPivotCache;
};

// ============================================================================

class PivotCacheRecordsFragment : public WorksheetFragmentBase
{
public:
    explicit            PivotCacheRecordsFragment(
                            const WorksheetHelper& rHelper,
                            const ::rtl::OUString& rFragmentPath,
                            const PivotCache& rPivotCache );

protected:
    virtual ::oox::core::ContextHandlerRef onCreateContext( sal_Int32 nElement, const AttributeList& rAttribs );
    virtual ::oox::core::ContextHandlerRef onCreateRecordContext( sal_Int32 nRecId, SequenceInputStream& rStrm );
    virtual const ::oox::core::RecordInfo* getRecordInfos() const;

private:
    void                startCacheRecord();
    void                importPCRecord( SequenceInputStream& rStrm );
    void                importPCRecordItem( sal_Int32 nRecId, SequenceInputStream& rStrm );

private:
    const PivotCache&   mrPivotCache;
    sal_Int32           mnColIdx;           /// Relative column index in source data.
    sal_Int32           mnRowIdx;           /// Relative row index in source data.
    bool                mbInRecord;
};

// ============================================================================
// ============================================================================

class BiffPivotCacheFragment : public BiffWorkbookFragmentBase
{
public:
    explicit            BiffPivotCacheFragment(
                            const WorkbookHelper& rHelper,
                            const ::rtl::OUString& rStrmName,
                            PivotCache& rPivotCache );

    /** Imports the entire fragment, returns true, if EOF record has been reached. */
    virtual bool        importFragment();

private:
    PivotCache&         mrPivotCache;
};

// ============================================================================

class BiffPivotCacheRecordsContext : public BiffWorksheetContextBase
{
public:
    explicit            BiffPivotCacheRecordsContext(
                            const WorksheetHelper& rHelper,
                            const PivotCache& rPivotCache );

    /** Reads the current record from stream and tries to insert a cell into
        the source data sheet. */
    virtual void        importRecord( BiffInputStream& rStrm );

private:
    void                startNextRow();

private:
    typedef ::std::vector< sal_Int32 > ColumnIndexVector;

    const PivotCache&   mrPivotCache;
    ColumnIndexVector   maUnsharedCols; /// Column indexes of all unshared cache fields.
    size_t              mnColIdx;       /// Current index into maUnsharedCols.
    sal_Int32           mnRowIdx;       /// Current row in source data (0-based).
    bool                mbHasShared;    /// True = pivot cache contains fields with shared items.
    bool                mbInRow;        /// True = a data row has been started.
};

// ============================================================================

} // namespace xls
} // namespace oox

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
