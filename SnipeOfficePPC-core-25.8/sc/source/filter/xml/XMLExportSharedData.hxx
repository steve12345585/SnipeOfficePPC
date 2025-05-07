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

#ifndef SC_XMLEXPORTSHAREDDATA_HXX
#define SC_XMLEXPORTSHAREDDATA_HXX

#include "global.hxx"
#include <com/sun/star/drawing/XDrawPage.hpp>

#include <vector>
#include <list>

struct ScMyDrawPage
{
    com::sun::star::uno::Reference<com::sun::star::drawing::XDrawPage> xDrawPage;
    bool bHasForms;

    ScMyDrawPage() : bHasForms(false) {}
};

typedef std::list< com::sun::star::uno::Reference<com::sun::star::drawing::XShape> > ScMyTableXShapes;
typedef std::vector<ScMyTableXShapes> ScMyTableShapes;
typedef std::vector<ScMyDrawPage> ScMyDrawPages;

class ScMyShapesContainer;
class ScMyDetectiveObjContainer;
struct ScMyShape;
class ScMyNoteShapesContainer;

class ScMySharedData
{
    std::vector<sal_Int32>      nLastColumns;
    std::vector<sal_Int32>      nLastRows;
    ScMyTableShapes*            pTableShapes;
    ScMyDrawPages*              pDrawPages;
    ScMyShapesContainer*        pShapesContainer;
    ScMyDetectiveObjContainer*  pDetectiveObjContainer;
    ScMyNoteShapesContainer*    pNoteShapes;
    sal_Int32                   nTableCount;
public:
    ScMySharedData(const sal_Int32 nTableCount);
    ~ScMySharedData();

    void SetLastColumn(const sal_Int32 nTable, const sal_Int32 nCol);
    void SetLastRow(const sal_Int32 nTable, const sal_Int32 nRow);
    sal_Int32 GetLastColumn(const sal_Int32 nTable) const;
    sal_Int32 GetLastRow(const sal_Int32 nTable) const;
    void AddDrawPage(const ScMyDrawPage& aDrawPage, const sal_Int32 nTable);
    void SetDrawPageHasForms(const sal_Int32 nTable, bool bHasForms);
    com::sun::star::uno::Reference<com::sun::star::drawing::XDrawPage> GetDrawPage(const sal_Int32 nTable);
    bool HasDrawPage() const { return pDrawPages != NULL; }
    bool HasForm(const sal_Int32 nTable, com::sun::star::uno::Reference<com::sun::star::drawing::XDrawPage>& xDrawPage);
    void AddNewShape(const ScMyShape& aMyShape);
    void SortShapesContainer();
    ScMyShapesContainer* GetShapesContainer() { return pShapesContainer; }
    bool HasShapes();
    void AddTableShape(const sal_Int32 nTable, const com::sun::star::uno::Reference<com::sun::star::drawing::XShape>& xShape);
    ScMyTableShapes* GetTableShapes() { return pTableShapes; }
    ScMyDetectiveObjContainer* GetDetectiveObjContainer() { return pDetectiveObjContainer; }
    void AddNoteObj(const com::sun::star::uno::Reference<com::sun::star::drawing::XShape>& xShape, const ScAddress& rPos);
    void SortNoteShapes();
    ScMyNoteShapesContainer* GetNoteShapes() { return pNoteShapes; }
};

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
