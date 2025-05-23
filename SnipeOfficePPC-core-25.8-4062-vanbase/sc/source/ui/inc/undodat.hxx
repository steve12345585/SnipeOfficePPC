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

#ifndef SC_UNDODAT_HXX
#define SC_UNDODAT_HXX

#include "undobase.hxx"
#include "rangeutl.hxx"     // ScArea
#include "rangelst.hxx"     // ScRangeListRef
#include "sortparam.hxx"
#include "queryparam.hxx"
#include "subtotalparam.hxx"
#include "pivot.hxx"

#include <boost/scoped_ptr.hpp>

class ScDocShell;
class ScDocument;
class ScOutlineTable;
class ScRangeName;
class ScDBData;
class ScDBCollection;
class ScDPObject;
class SdrUndoAction;
class ScMarkData;

//----------------------------------------------------------------------------

class ScUndoDoOutline: public ScSimpleUndo
{
public:
                    TYPEINFO();
                    ScUndoDoOutline( ScDocShell* pNewDocShell,
                            SCCOLROW nNewStart, SCCOLROW nNewEnd, SCTAB nNewTab,
                            ScDocument* pNewUndoDoc, sal_Bool bNewColumns,
                            sal_uInt16 nNewLevel, sal_uInt16 nNewEntry, sal_Bool bNewShow );
    virtual         ~ScUndoDoOutline();

    virtual void    Undo();
    virtual void    Redo();
    virtual void    Repeat(SfxRepeatTarget& rTarget);
    virtual sal_Bool    CanRepeat(SfxRepeatTarget& rTarget) const;

    virtual rtl::OUString GetComment() const;

private:
    SCCOLROW        nStart;
    SCCOLROW        nEnd;
    SCTAB           nTab;
    ScDocument*     pUndoDoc;
    sal_Bool            bColumns;
    sal_uInt16          nLevel;
    sal_uInt16          nEntry;
    sal_Bool            bShow;
};


class ScUndoMakeOutline: public ScSimpleUndo
{
public:
                    TYPEINFO();
                    ScUndoMakeOutline( ScDocShell* pNewDocShell,
                            SCCOL nStartX, SCROW nStartY, SCTAB nStartZ,
                            SCCOL nEndX, SCROW nEndY, SCTAB nEndZ,
                            ScOutlineTable* pNewUndoTab,
                            sal_Bool bNewColumns, sal_Bool bNewMake );
    virtual         ~ScUndoMakeOutline();

    virtual void    Undo();
    virtual void    Redo();
    virtual void    Repeat(SfxRepeatTarget& rTarget);
    virtual sal_Bool    CanRepeat(SfxRepeatTarget& rTarget) const;

    virtual rtl::OUString GetComment() const;

private:
    ScAddress       aBlockStart;
    ScAddress       aBlockEnd;
    ScOutlineTable* pUndoTable;
    sal_Bool            bColumns;
    sal_Bool            bMake;
};


class ScUndoOutlineLevel: public ScSimpleUndo
{
public:
                    TYPEINFO();
                    ScUndoOutlineLevel( ScDocShell* pNewDocShell,
                            SCCOLROW nNewStart, SCCOLROW nNewEnd, SCTAB nNewTab,
                            ScDocument* pNewUndoDoc, ScOutlineTable* pNewUndoTab,
                            sal_Bool bNewColumns, sal_uInt16 nNewLevel );
    virtual         ~ScUndoOutlineLevel();

    virtual void    Undo();
    virtual void    Redo();
    virtual void    Repeat(SfxRepeatTarget& rTarget);
    virtual sal_Bool    CanRepeat(SfxRepeatTarget& rTarget) const;

    virtual rtl::OUString GetComment() const;

private:
    SCCOLROW        nStart;
    SCCOLROW        nEnd;
    SCTAB           nTab;
    ScDocument*     pUndoDoc;
    ScOutlineTable* pUndoTable;
    sal_Bool            bColumns;
    sal_uInt16          nLevel;
};


class ScUndoOutlineBlock: public ScSimpleUndo
{
public:
                    TYPEINFO();
                    ScUndoOutlineBlock( ScDocShell* pNewDocShell,
                            SCCOL nStartX, SCROW nStartY, SCTAB nStartZ,
                            SCCOL nEndX, SCROW nEndY, SCTAB nEndZ,
                            ScDocument* pNewUndoDoc, ScOutlineTable* pNewUndoTab,
                            sal_Bool bNewShow );
    virtual         ~ScUndoOutlineBlock();

    virtual void    Undo();
    virtual void    Redo();
    virtual void    Repeat(SfxRepeatTarget& rTarget);
    virtual sal_Bool    CanRepeat(SfxRepeatTarget& rTarget) const;

    virtual rtl::OUString GetComment() const;

private:
    ScAddress       aBlockStart;
    ScAddress       aBlockEnd;
    ScDocument*     pUndoDoc;
    ScOutlineTable* pUndoTable;
    sal_Bool            bShow;
};


class ScUndoRemoveAllOutlines: public ScSimpleUndo
{
public:
                    TYPEINFO();
                    ScUndoRemoveAllOutlines( ScDocShell* pNewDocShell,
                            SCCOL nStartX, SCROW nStartY, SCTAB nStartZ,
                            SCCOL nEndX, SCROW nEndY, SCTAB nEndZ,
                            ScDocument* pNewUndoDoc, ScOutlineTable* pNewUndoTab );
    virtual         ~ScUndoRemoveAllOutlines();

    virtual void    Undo();
    virtual void    Redo();
    virtual void    Repeat(SfxRepeatTarget& rTarget);
    virtual sal_Bool    CanRepeat(SfxRepeatTarget& rTarget) const;

    virtual rtl::OUString GetComment() const;

private:
    ScAddress       aBlockStart;
    ScAddress       aBlockEnd;
    ScDocument*     pUndoDoc;
    ScOutlineTable* pUndoTable;
};


class ScUndoAutoOutline: public ScSimpleUndo
{
public:
                    TYPEINFO();
                    ScUndoAutoOutline( ScDocShell* pNewDocShell,
                            SCCOL nStartX, SCROW nStartY, SCTAB nStartZ,
                            SCCOL nEndX, SCROW nEndY, SCTAB nEndZ,
                            ScDocument* pNewUndoDoc, ScOutlineTable* pNewUndoTab );
    virtual         ~ScUndoAutoOutline();

    virtual void    Undo();
    virtual void    Redo();
    virtual void    Repeat(SfxRepeatTarget& rTarget);
    virtual sal_Bool    CanRepeat(SfxRepeatTarget& rTarget) const;

    virtual rtl::OUString GetComment() const;

private:
    ScAddress       aBlockStart;
    ScAddress       aBlockEnd;
    ScDocument*     pUndoDoc;
    ScOutlineTable* pUndoTable;
};


class ScUndoSubTotals: public ScDBFuncUndo
{
public:
                    TYPEINFO();
                    ScUndoSubTotals( ScDocShell* pNewDocShell, SCTAB nNewTab,
                            const ScSubTotalParam& rNewParam, SCROW nNewEndY,
                            ScDocument* pNewUndoDoc, ScOutlineTable* pNewUndoTab,
//                          ScDBData* pNewData,
                            ScRangeName* pNewUndoRange, ScDBCollection* pNewUndoDB );
    virtual         ~ScUndoSubTotals();

    virtual void    Undo();
    virtual void    Redo();
    virtual void    Repeat(SfxRepeatTarget& rTarget);
    virtual sal_Bool    CanRepeat(SfxRepeatTarget& rTarget) const;

    virtual rtl::OUString GetComment() const;

private:
    SCTAB           nTab;
    ScSubTotalParam aParam;                         // The original passed parameter
    SCROW           nNewEndRow;                     // Size of result
    ScDocument*     pUndoDoc;
    ScOutlineTable* pUndoTable;
//  ScDBData*       pUndoDBData;
    ScRangeName*    pUndoRange;
    ScDBCollection* pUndoDB;
};


class ScUndoSort: public ScDBFuncUndo
{
public:
                    TYPEINFO();
                    ScUndoSort( ScDocShell* pNewDocShell, SCTAB nNewTab,
                            const ScSortParam& rParam,
                            ScDocument* pNewUndoDoc,
                            ScDBCollection* pNewUndoDB, const ScRange* pDest = NULL );
    virtual         ~ScUndoSort();

    virtual void    Undo();
    virtual void    Redo();
    virtual void    Repeat(SfxRepeatTarget& rTarget);
    virtual sal_Bool    CanRepeat(SfxRepeatTarget& rTarget) const;

    virtual rtl::OUString GetComment() const;

private:
    SCTAB           nTab;
    ScSortParam     aSortParam;
    ScDocument*     pUndoDoc;
    ScDBCollection* pUndoDB;                // due to source and target range
    sal_Bool            bDestArea;
    ScRange         aDestRange;
};


class ScUndoQuery: public ScDBFuncUndo
{
public:
                    TYPEINFO();
                    ScUndoQuery( ScDocShell* pNewDocShell, SCTAB nNewTab,
                            const ScQueryParam& rParam, ScDocument* pNewUndoDoc,
                            ScDBCollection* pNewUndoDB, const ScRange* pOld,
                            sal_Bool bSize, const ScRange* pAdvSrc );
    virtual         ~ScUndoQuery();

    virtual void    Undo();
    virtual void    Redo();
    virtual void    Repeat(SfxRepeatTarget& rTarget);
    virtual sal_Bool    CanRepeat(SfxRepeatTarget& rTarget) const;

    virtual rtl::OUString GetComment() const;

private:
    SdrUndoAction*  pDrawUndo;
    SCTAB           nTab;
    ScQueryParam    aQueryParam;
    ScDocument*     pUndoDoc;
    ScDBCollection* pUndoDB;                // due to source and target range
    ScRange         aOldDest;
    ScRange         aAdvSource;
    sal_Bool            bIsAdvanced;
    sal_Bool            bDestArea;
    sal_Bool            bDoSize;
};


class ScUndoAutoFilter: public ScDBFuncUndo
{
private:
    ::rtl::OUString aDBName;
    bool            bFilterSet;

    void            DoChange( bool bUndo );

public:
                    TYPEINFO();
                    ScUndoAutoFilter( ScDocShell* pNewDocShell, const ScRange& rRange,
                                      const ::rtl::OUString& rName, bool bSet );
    virtual         ~ScUndoAutoFilter();

    virtual void    Undo();
    virtual void    Redo();
    virtual void    Repeat(SfxRepeatTarget& rTarget);
    virtual sal_Bool    CanRepeat(SfxRepeatTarget& rTarget) const;

    virtual rtl::OUString GetComment() const;
};


class ScUndoDBData: public ScSimpleUndo
{
public:
                    TYPEINFO();
                    ScUndoDBData( ScDocShell* pNewDocShell,
                            ScDBCollection* pNewUndoColl, ScDBCollection* pNewRedoColl );
    virtual         ~ScUndoDBData();

    virtual void    Undo();
    virtual void    Redo();
    virtual void    Repeat(SfxRepeatTarget& rTarget);
    virtual sal_Bool    CanRepeat(SfxRepeatTarget& rTarget) const;

    virtual rtl::OUString GetComment() const;

private:
    ScDBCollection* pUndoColl;
    ScDBCollection* pRedoColl;
};


class ScUndoImportData: public ScSimpleUndo
{
public:
                    TYPEINFO();
                    ScUndoImportData( ScDocShell* pNewDocShell, SCTAB nNewTab,
                            const ScImportParam& rParam, SCCOL nNewEndX, SCROW nNewEndY,
                            SCCOL nNewFormula,
                            ScDocument* pNewUndoDoc, ScDocument* pNewRedoDoc,
                            ScDBData* pNewUndoData, ScDBData* pNewRedoData );
    virtual         ~ScUndoImportData();

    virtual void    Undo();
    virtual void    Redo();
    virtual void    Repeat(SfxRepeatTarget& rTarget);
    virtual sal_Bool    CanRepeat(SfxRepeatTarget& rTarget) const;

    virtual rtl::OUString GetComment() const;

private:
    SCTAB           nTab;
    ScImportParam   aImportParam;
    SCCOL           nEndCol;
    SCROW           nEndRow;
    ScDocument*     pUndoDoc;
    ScDocument*     pRedoDoc;
    ScDBData*       pUndoDBData;
    ScDBData*       pRedoDBData;
    SCCOL           nFormulaCols;
    sal_Bool            bRedoFilled;
};


class ScUndoRepeatDB: public ScSimpleUndo
{
public:
                    TYPEINFO();
                    ScUndoRepeatDB( ScDocShell* pNewDocShell, SCTAB nNewTab,
                            SCCOL nStartX, SCROW nStartY, SCCOL nEndX, SCROW nEndY,
                            SCROW nResultEndRow, SCCOL nCurX, SCROW nCurY,
                            ScDocument* pNewUndoDoc, ScOutlineTable* pNewUndoTab,
                            ScRangeName* pNewUndoRange, ScDBCollection* pNewUndoDB,
                            const ScRange* pOldQ, const ScRange* pNewQ );
    virtual         ~ScUndoRepeatDB();

    virtual void    Undo();
    virtual void    Redo();
    virtual void    Repeat(SfxRepeatTarget& rTarget);
    virtual sal_Bool    CanRepeat(SfxRepeatTarget& rTarget) const;

    virtual rtl::OUString GetComment() const;

private:
    ScAddress       aBlockStart;
    ScAddress       aBlockEnd;
    SCROW           nNewEndRow;
    ScAddress       aCursorPos;
    ScDocument*     pUndoDoc;
    ScOutlineTable* pUndoTable;
    ScRangeName*    pUndoRange;
    ScDBCollection* pUndoDB;
    ScRange         aOldQuery;
    ScRange         aNewQuery;
    sal_Bool            bQuerySize;
};


class ScUndoDataPilot: public ScSimpleUndo
{
public:
                    TYPEINFO();
                    ScUndoDataPilot( ScDocShell* pNewDocShell,
                            ScDocument* pOldDoc, ScDocument* pNewDoc,
                            const ScDPObject* pOldObj, const ScDPObject* pNewObj,
                            sal_Bool bMove );
    virtual         ~ScUndoDataPilot();

    virtual void    Undo();
    virtual void    Redo();
    virtual void    Repeat(SfxRepeatTarget& rTarget);
    virtual sal_Bool    CanRepeat(SfxRepeatTarget& rTarget) const;

    virtual rtl::OUString GetComment() const;

private:
    ScDocument*     pOldUndoDoc;
    ScDocument*     pNewUndoDoc;
    ScDPObject*     pOldDPObject;
    ScDPObject*     pNewDPObject;
    sal_Bool            bAllowMove;
};


class ScUndoConsolidate: public ScSimpleUndo
{
public:
                        TYPEINFO();
                        ScUndoConsolidate( ScDocShell* pNewDocShell,
                                const ScArea& rArea, const ScConsolidateParam& rPar,
                                ScDocument* pNewUndoDoc, sal_Bool bReference,
                                SCROW nInsCount, ScOutlineTable* pTab,
                                ScDBData* pData );
    virtual             ~ScUndoConsolidate();

    virtual void        Undo();
    virtual void        Redo();
    virtual void        Repeat(SfxRepeatTarget& rTarget);
    virtual sal_Bool        CanRepeat(SfxRepeatTarget& rTarget) const;

    virtual rtl::OUString GetComment() const;

private:
    ScArea              aDestArea;
    ScDocument*         pUndoDoc;
    ScConsolidateParam  aParam;
    sal_Bool                bInsRef;
    SCSIZE              nInsertCount;
    ScOutlineTable*     pUndoTab;
    ScDBData*           pUndoData;
};


class ScUndoChartData: public ScSimpleUndo
{
public:
                        TYPEINFO();
                        ScUndoChartData( ScDocShell* pNewDocShell,
                                const String& rName, const ScRange& rNew,
                                bool bColHdr, bool bRowHdr, bool bAdd );
                        ScUndoChartData( ScDocShell* pNewDocShell,
                                const String& rName, const ScRangeListRef& rNew,
                                bool bColHdr, bool bRowHdr, bool bAdd );
    virtual             ~ScUndoChartData();

    virtual void        Undo();
    virtual void        Redo();
    virtual void        Repeat(SfxRepeatTarget& rTarget);
    virtual sal_Bool        CanRepeat(SfxRepeatTarget& rTarget) const;

    virtual rtl::OUString GetComment() const;

private:
    String              aChartName;
    ScRangeListRef      aOldRangeListRef;
    bool                bOldColHeaders;
    bool                bOldRowHeaders;
//  ScRange             aNewRange;
    ScRangeListRef      aNewRangeListRef;
    bool                bNewColHeaders;
    bool                bNewRowHeaders;
    bool                bAddRange;

    void                Init();
};

class ScUndoDataForm: public ScBlockUndo
{
public:
                    TYPEINFO();
                    ScUndoDataForm( ScDocShell* pNewDocShell,
                                SCCOL nStartX, SCROW nStartY, SCTAB nStartZ,
                                SCCOL nEndX, SCROW nEndY, SCTAB nEndZ,
                                const ScMarkData& rMark,
                                ScDocument* pNewUndoDoc, ScDocument* pNewRedoDoc,
                                sal_uInt16 nNewFlags,
                                ScRefUndoData* pRefData, void* pFill1, void* pFill2, void* pFill3,
                                sal_Bool bRedoIsFilled = true
                                 );
    virtual     ~ScUndoDataForm();

    virtual void    Undo();
    virtual void    Redo();
    virtual void    Repeat(SfxRepeatTarget& rTarget);
    virtual sal_Bool    CanRepeat(SfxRepeatTarget& rTarget) const;

    virtual rtl::OUString GetComment() const;

private:
    boost::scoped_ptr<ScMarkData> mpMarkData;
    ScDocument*     pUndoDoc;
    ScDocument*     pRedoDoc;
    sal_uInt16          nFlags;
    ScRefUndoData*      pRefUndoData;
    ScRefUndoData*      pRefRedoData;
    sal_uLong           nStartChangeAction;
    sal_uLong           nEndChangeAction;
    sal_Bool            bRedoFilled;

    void            DoChange( const sal_Bool bUndo );
    void            SetChangeTrack();
};


#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
