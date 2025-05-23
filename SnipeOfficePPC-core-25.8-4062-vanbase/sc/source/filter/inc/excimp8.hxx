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

#ifndef SC_EXCIMP8_HXX
#define SC_EXCIMP8_HXX

#include <string.h>

#include <boost/ptr_container/ptr_vector.hpp>

#include "imp_op.hxx"
#include "root.hxx"
#include "excscen.hxx"
#include "excdefs.hxx"
#include "ftools.hxx"
#include "queryparam.hxx"

class ScDBData;
class XclImpStream;



class ImportExcel8 : public ImportExcel
{
public:
                            ImportExcel8( XclImpRootData& rImpData, SvStream& rStrm );
    virtual                 ~ImportExcel8( void );

    virtual FltError        Read( void );

    void                    Calccount( void );              // 0x0C
    void                    Precision( void );              // 0x0E
    void                    Delta( void );                  // 0x10
    void                    Iteration( void );              // 0x11
    void                    Boundsheet( void );             // 0x85
    void                    FilterMode( void );             // 0x9B
    void                    AutoFilterInfo( void );         // 0x9D
    void                    AutoFilter( void );             // 0x9E
    void                    Scenman( void );                // 0xAE
    void                    Scenario( void );               // 0xAF
    void                    ReadBasic( void );              // 0xD3
    void                    Labelsst( void );               // 0xFD

    void                    Hlink( void );                  // 0x01B8
    void                    Codename( sal_Bool bWBGlobals );    // 0x01BA
    void                    SheetProtection( void );        // 0x0867

    virtual void            EndSheet( void );
    virtual void            PostDocLoad( void );

private:
    void                    LoadDocumentProperties();

private:
    // represents codename ( and associated modules )
    // not speficied directly in the binary format
    std::vector<rtl::OUString> maAutoGeneratedCodeNames;
    ExcScenarioList maScenList;
};



//___________________________________________________________________
// classes AutoFilterData, AutoFilterBuffer

class XclImpAutoFilterData : private ExcRoot
{
private:
    ScDBData*                   pCurrDBData;
    ScQueryParam                aParam;
    ScRange                     aCriteriaRange;
    bool                        bActive:1;
    bool                        bCriteria:1;
    bool                        bAutoOrAdvanced:1;

    void                        CreateFromDouble( rtl::OUString& rStr, double fVal );
    void                        SetCellAttribs();
    void                        InsertQueryParam();

protected:
public:
                                XclImpAutoFilterData(
                                    RootData* pRoot,
                                    const ScRange& rRange);

    inline bool                 IsActive() const    { return bActive; }
    inline bool                 IsFiltered() const  { return bAutoOrAdvanced; }
    inline SCTAB                Tab() const         { return aParam.nTab; }
    inline SCCOL                StartCol() const    { return aParam.nCol1; }
    inline SCROW                StartRow() const    { return aParam.nRow1; }
    inline SCCOL                EndCol() const      { return aParam.nCol2; }
    inline SCROW                EndRow() const      { return aParam.nRow2; }

    void                        ReadAutoFilter( XclImpStream& rStrm );

    inline void                 Activate()          { bActive = true; }
    void                        SetAdvancedRange( const ScRange* pRange );
    void                        SetExtractPos( const ScAddress& rAddr );
    inline void                 SetAutoOrAdvanced()  { bAutoOrAdvanced = true; }
    void                        Apply();
    void                        CreateScDBData();
    void                        EnableRemoveFilter();
};


class XclImpAutoFilterBuffer
{
public:

    void                        Insert( RootData* pRoot, const ScRange& rRange);
    void                        AddAdvancedRange( const ScRange& rRange );
    void                        AddExtractPos( const ScRange& rRange );
    void                        Apply();

    XclImpAutoFilterData*       GetByTab( SCTAB nTab );

private:

    boost::ptr_vector<XclImpAutoFilterData> maFilters;
};

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
