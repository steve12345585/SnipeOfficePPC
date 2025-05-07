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

#ifndef SC_XMLEXPORTDATAPILOT_HXX
#define SC_XMLEXPORTDATAPILOT_HXX

#include <com/sun/star/sheet/XSpreadsheetDocument.hpp>
#include <rtl/ustring.hxx>
#include "global.hxx"
#include "xmloff/xmltoken.hxx"

class ScXMLExport;
class ScDocument;
class ScDPSaveDimension;
class ScDPSaveData;
class ScDPDimensionSaveData;
class ScDPSaveGroupDimension;
class ScDPSaveNumGroupDimension;
struct ScDPNumGroupInfo;
struct ScQueryParam;
struct ScQueryEntry;

class ScXMLExportDataPilot
{
    ScXMLExport&        rExport;
    ScDocument*         pDoc;

    rtl::OUString getDPOperatorXML(const ScQueryOp aFilterOperator, const bool bUseRegularExpressions) const;
    void WriteDPCondition(const ScQueryEntry& aQueryEntry, bool bIsCaseSensitive, bool bUseRegularExpressions);
    void WriteDPFilter(const ScQueryParam& aQueryParam);

    void WriteFieldReference(ScDPSaveDimension* pDim);
    void WriteSortInfo(ScDPSaveDimension* pDim);
    void WriteAutoShowInfo(ScDPSaveDimension* pDim);
    void WriteLayoutInfo(ScDPSaveDimension* pDim);
    void WriteSubTotals(ScDPSaveDimension* pDim);
    void WriteMembers(ScDPSaveDimension* pDim);
    void WriteLevels(ScDPSaveDimension* pDim);
    void WriteDatePart(sal_Int32 nPart);
    void WriteNumGroupInfo(const ScDPNumGroupInfo& pGroupInfo);
    void WriteGroupDimAttributes(const ScDPSaveGroupDimension* pGroupDim);
    void WriteGroupDimElements(ScDPSaveDimension* pDim, const ScDPDimensionSaveData* pDimData);
    void WriteNumGroupDim(const ScDPSaveNumGroupDimension* pNumGroupDim);
    void WriteDimension(ScDPSaveDimension* pDim, const ScDPDimensionSaveData* pDimData);
    void WriteDimensions(ScDPSaveData* pDPSave);

    void WriteGrandTotal(::xmloff::token::XMLTokenEnum eOrient, bool bVisible, const ::rtl::OUString* pGrandTotal);

public:
    ScXMLExportDataPilot(ScXMLExport& rExport);
    ~ScXMLExportDataPilot();
    void WriteDataPilots(const com::sun::star::uno::Reference <com::sun::star::sheet::XSpreadsheetDocument>& xSpreaDoc);
};

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
