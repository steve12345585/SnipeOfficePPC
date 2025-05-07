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

#ifndef SC_DOCOPTIO_HXX
#define SC_DOCOPTIO_HXX

#include <unotools/configitem.hxx>
#include <svl/poolitem.hxx>
#include <svl/itemprop.hxx>
#include "scdllapi.h"
#include "scmod.hxx"
#include "optutil.hxx"

#include "formula/grammar.hxx"

class SC_DLLPUBLIC ScDocOptions
{
    double fIterEps;                    // epsilon value dazu
    sal_uInt16 nIterCount;              // number
    sal_uInt16 nPrecStandardFormat;     // precision for standard format
    sal_uInt16 nDay;                    // Null date:
    sal_uInt16 nMonth;
    sal_uInt16 nYear;
    sal_uInt16 nYear2000;               // earlier 19YY is assumed, 20YY otherwise (if only YY of year is given)
    sal_uInt16 nTabDistance;            // distance of standard tabs
    sal_Bool   bIsIgnoreCase;           // ignore case for comparisons?
    sal_Bool   bIsIter;                 // iterations for circular refs
    sal_Bool   bCalcAsShown;            // calculate as shown (wrt precision)
    sal_Bool   bMatchWholeCell;         // search criteria must match the whole cell
    sal_Bool   bDoAutoSpell;            // auto-spelling
    sal_Bool   bLookUpColRowNames;      // determine column-/row titles automagically
    sal_Bool   bFormulaRegexEnabled;    // regular expressions in formulas enabled
public:
                ScDocOptions();
                ScDocOptions( const ScDocOptions& rCpy );
                ~ScDocOptions();

    sal_Bool   IsLookUpColRowNames() const  { return bLookUpColRowNames; }
    void   SetLookUpColRowNames( sal_Bool bVal ) { bLookUpColRowNames = bVal; }
    sal_Bool   IsAutoSpell() const          { return bDoAutoSpell; }
    void   SetAutoSpell( sal_Bool bVal )    { bDoAutoSpell = bVal; }
    sal_Bool   IsMatchWholeCell() const     { return bMatchWholeCell; }
    void   SetMatchWholeCell( sal_Bool bVal ){ bMatchWholeCell = bVal; }
    sal_Bool   IsIgnoreCase() const         { return bIsIgnoreCase; }
    void   SetIgnoreCase( sal_Bool bVal )   { bIsIgnoreCase = bVal; }
    sal_Bool   IsIter() const               { return bIsIter; }
    void   SetIter( sal_Bool bVal )         { bIsIter = bVal; }
    sal_uInt16 GetIterCount() const         { return nIterCount; }
    void   SetIterCount( sal_uInt16 nCount) { nIterCount = nCount; }
    double GetIterEps() const           { return fIterEps; }
    void   SetIterEps( double fEps )    { fIterEps = fEps; }

    void   GetDate( sal_uInt16& rD, sal_uInt16& rM, sal_uInt16& rY ) const
                                        { rD = nDay; rM = nMonth; rY = nYear;}
    void   SetDate (sal_uInt16 nD, sal_uInt16 nM, sal_uInt16 nY)
                                        { nDay = nD; nMonth = nM; nYear = nY; }
    sal_uInt16 GetTabDistance() const { return nTabDistance;}
    void   SetTabDistance( sal_uInt16 nTabDist ) {nTabDistance = nTabDist;}

    void        ResetDocOptions();

    inline const ScDocOptions&  operator=( const ScDocOptions& rOpt );
    inline bool                 operator==( const ScDocOptions& rOpt ) const;
    inline bool                 operator!=( const ScDocOptions& rOpt ) const;

    sal_uInt16  GetStdPrecision() const { return nPrecStandardFormat; }
    void        SetStdPrecision( sal_uInt16 n ) { nPrecStandardFormat = n; }

    sal_Bool    IsCalcAsShown() const       { return bCalcAsShown; }
    void    SetCalcAsShown( sal_Bool bVal ) { bCalcAsShown = bVal; }

    void    SetYear2000( sal_uInt16 nVal )  { nYear2000 = nVal; }
    sal_uInt16  GetYear2000() const         { return nYear2000; }

    void    SetFormulaRegexEnabled( sal_Bool bVal ) { bFormulaRegexEnabled = bVal; }
    sal_Bool    IsFormulaRegexEnabled() const       { return bFormulaRegexEnabled; }

};

inline const ScDocOptions& ScDocOptions::operator=( const ScDocOptions& rCpy )
{
    bIsIgnoreCase       = rCpy.bIsIgnoreCase;
    bIsIter             = rCpy.bIsIter;
    nIterCount          = rCpy.nIterCount;
    fIterEps            = rCpy.fIterEps;
    nPrecStandardFormat = rCpy.nPrecStandardFormat;
    nDay                = rCpy.nDay;
    nMonth              = rCpy.nMonth;
    nYear               = rCpy.nYear;
    nYear2000           = rCpy.nYear2000;
    nTabDistance        = rCpy.nTabDistance;
    bCalcAsShown        = rCpy.bCalcAsShown;
    bMatchWholeCell     = rCpy.bMatchWholeCell;
    bDoAutoSpell        = rCpy.bDoAutoSpell;
    bLookUpColRowNames  = rCpy.bLookUpColRowNames;
    bFormulaRegexEnabled= rCpy.bFormulaRegexEnabled;

    return *this;
}

inline bool ScDocOptions::operator==( const ScDocOptions& rOpt ) const
{
    return (
                rOpt.bIsIgnoreCase          == bIsIgnoreCase
            &&  rOpt.bIsIter                == bIsIter
            &&  rOpt.nIterCount             == nIterCount
            &&  rOpt.fIterEps               == fIterEps
            &&  rOpt.nPrecStandardFormat    == nPrecStandardFormat
            &&  rOpt.nDay                   == nDay
            &&  rOpt.nMonth                 == nMonth
            &&  rOpt.nYear                  == nYear
            &&  rOpt.nYear2000              == nYear2000
            &&  rOpt.nTabDistance           == nTabDistance
            &&  rOpt.bCalcAsShown           == bCalcAsShown
            &&  rOpt.bMatchWholeCell        == bMatchWholeCell
            &&  rOpt.bDoAutoSpell           == bDoAutoSpell
            &&  rOpt.bLookUpColRowNames     == bLookUpColRowNames
            &&  rOpt.bFormulaRegexEnabled   == bFormulaRegexEnabled
            );
}

inline bool ScDocOptions::operator!=( const ScDocOptions& rOpt ) const
{
    return !(operator==(rOpt));
}

//==================================================================
// Item for preferences dialog - calculation
//==================================================================

class SC_DLLPUBLIC ScTpCalcItem : public SfxPoolItem
{
public:
                TYPEINFO();
                ScTpCalcItem( sal_uInt16 nWhich,
                              const ScDocOptions& rOpt );
                ScTpCalcItem( const ScTpCalcItem& rItem );
                ~ScTpCalcItem();

    virtual String          GetValueText() const;
    virtual int             operator==( const SfxPoolItem& ) const;
    virtual SfxPoolItem*    Clone( SfxItemPool *pPool = 0 ) const;

    const ScDocOptions& GetDocOptions() const { return theOptions; }

private:
    ScDocOptions theOptions;
};

//==================================================================
//  Config Item containing document options
//==================================================================

class ScDocCfg : public ScDocOptions
{
    ScLinkConfigItem    aCalcItem;
    ScLinkConfigItem    aLayoutItem;

    DECL_LINK( CalcCommitHdl, void* );
    DECL_LINK( LayoutCommitHdl, void* );

    com::sun::star::uno::Sequence<rtl::OUString> GetCalcPropertyNames();
    com::sun::star::uno::Sequence<rtl::OUString> GetLayoutPropertyNames();

public:
            ScDocCfg();

    void    SetOptions( const ScDocOptions& rNew );
};


#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
