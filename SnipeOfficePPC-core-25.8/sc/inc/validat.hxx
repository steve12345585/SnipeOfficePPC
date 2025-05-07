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

#ifndef SC_VALIDAT_HXX
#define SC_VALIDAT_HXX

#include "conditio.hxx"
#include <com/sun/star/sheet/TableValidationVisibility.hpp>
#include "scdllapi.h"

namespace ValidListType = ::com::sun::star::sheet::TableValidationVisibility;

class ScPatternAttr;
class ScTokenArray;
class ScTypedStrData;

enum ScValidationMode
{
    SC_VALID_ANY,
    SC_VALID_WHOLE,
    SC_VALID_DECIMAL,
    SC_VALID_DATE,
    SC_VALID_TIME,
    SC_VALID_TEXTLEN,
    SC_VALID_LIST,
    SC_VALID_CUSTOM
};

enum ScValidErrorStyle
{
    SC_VALERR_STOP,
    SC_VALERR_WARNING,
    SC_VALERR_INFO,
    SC_VALERR_MACRO
};

//
// Entry for validation (only one condition exists)
//

class SC_DLLPUBLIC ScValidationData : public ScConditionEntry
{
    sal_uInt32          nKey;               // index in attributes

    ScValidationMode    eDataMode;
    sal_Bool                bShowInput;
    sal_Bool                bShowError;
    ScValidErrorStyle   eErrorStyle;
    sal_Int16           mnListType;         // selection list type: none, unsorted, sorted.
    String              aInputTitle;
    String              aInputMessage;
    String              aErrorTitle;
    String              aErrorMessage;

    sal_Bool                bIsUsed;            // temporary during saving

    sal_Bool            DoMacro( const ScAddress& rPos, const String& rInput,
                                ScFormulaCell* pCell, Window* pParent ) const;

    sal_Bool            DoScript( const ScAddress& rPos, const String& rInput,
                                ScFormulaCell* pCell, Window* pParent ) const;

    using ScConditionEntry::operator==;

public:
            ScValidationData( ScValidationMode eMode, ScConditionMode eOper,
                                const String& rExpr1, const String& rExpr2,
                                ScDocument* pDocument, const ScAddress& rPos,
                                const String& rExprNmsp1 = EMPTY_STRING, const String& rExprNmsp2 = EMPTY_STRING,
                                formula::FormulaGrammar::Grammar eGrammar1 = formula::FormulaGrammar::GRAM_DEFAULT,
                                formula::FormulaGrammar::Grammar eGrammar2 = formula::FormulaGrammar::GRAM_DEFAULT );
            ScValidationData( ScValidationMode eMode, ScConditionMode eOper,
                                const ScTokenArray* pArr1, const ScTokenArray* pArr2,
                                ScDocument* pDocument, const ScAddress& rPos );
            ScValidationData( const ScValidationData& r );
            ScValidationData( ScDocument* pDocument, const ScValidationData& r );
    virtual ~ScValidationData();

    ScValidationData* Clone() const     // real copy
                    { return new ScValidationData( GetDocument(), *this ); }
    ScValidationData* Clone(ScDocument* pNew) const
                    { return new ScValidationData( pNew, *this ); }

    void            ResetInput();
    void            ResetError();
    void            SetInput( const String& rTitle, const String& rMsg );
    void            SetError( const String& rTitle, const String& rMsg,
                                ScValidErrorStyle eStyle );

    sal_Bool            GetInput( String& rTitle, String& rMsg ) const
                        { rTitle = aInputTitle; rMsg = aInputMessage; return bShowInput; }
    sal_Bool            GetErrMsg( String& rTitle, String& rMsg, ScValidErrorStyle& rStyle ) const;

    sal_Bool            HasErrMsg() const       { return bShowError; }

    ScValidationMode GetDataMode() const    { return eDataMode; }

    inline sal_Int16 GetListType() const                { return mnListType; }
    inline void     SetListType( sal_Int16 nListType )  { mnListType = nListType; }

    /** Returns true, if the validation cell will show a selection list.
        @descr  Use this instead of GetListType() which returns the raw property
        regardless of the validation type. */
    bool            HasSelectionList() const;
    /** Tries to fill the passed collection with list validation entries.
        @descr  Fills the list only, if this is a list validation and IsShowList() is enabled.
        @param rStrings  (out-param) The string list to fill with list validation entires.
        @return  true = rStrings has been filled with at least one entry. */
    bool FillSelectionList(std::vector<ScTypedStrData>& rStrings, const ScAddress& rPos) const;

                    //  with string: during input, with cell: for detective / RC_FORCED
    sal_Bool            IsDataValid( const String& rTest, const ScPatternAttr& rPattern,
                                    const ScAddress& rPos ) const;
    sal_Bool            IsDataValid( ScBaseCell* pCell, const ScAddress& rPos ) const;

                    // TRUE -> break
    sal_Bool            DoError( Window* pParent, const String& rInput, const ScAddress& rPos ) const;
    void            DoCalcError( ScFormulaCell* pCell ) const;

    sal_Bool            IsEmpty() const;
    sal_uInt32      GetKey() const          { return nKey; }
    void            SetKey(sal_uInt32 nNew) { nKey = nNew; }    // only if not inserted!

    void            SetUsed(sal_Bool bSet)      { bIsUsed = bSet; }
    sal_Bool            IsUsed() const          { return bIsUsed; }

    sal_Bool            EqualEntries( const ScValidationData& r ) const;    // for undo

    //  sort (using PTRARR) by index
    //  operator== only for sorting
    sal_Bool operator ==( const ScValidationData& r ) const { return nKey == r.nKey; }
    sal_Bool operator < ( const ScValidationData& r ) const { return nKey <  r.nKey; }

private:
    /** Tries to fill the passed collection with list validation entries.
        @descr  Fills the list only if it is non-NULL,
        @param pStrings  (out-param) Optionally NULL, string list to fill with list validation entires.
        @param pCell     can be NULL if it is not necessary to which element in the list is selected.
        @param rPos      the base address for relative references.
        @param rTokArr   Formula token array.
        @param rMatch    (out-param) the index of the first item that matched, -1 if nothing matched.
        @return  true = Cell range found, rRange is valid, or an error entry stuffed into the list if pCell==NULL. */
    bool GetSelectionFromFormula(
        std::vector<ScTypedStrData>* pStrings, ScBaseCell* pCell, const ScAddress& rPos,
        const ScTokenArray& rTokArr, int& rMatch) const;

    /** Tests, if pCell is equal to what the passed token array represents. */
    bool            IsEqualToTokenArray( ScBaseCell* pCell, const ScAddress& rPos, const ScTokenArray& rTokArr ) const;

    /** Tests, if contents of pCell occur in cell range referenced by own formula, or in a string list. */
    bool            IsListValid( ScBaseCell* pCell, const ScAddress& rPos ) const;
};

//
//  list of contitions:
//

typedef ScValidationData* ScValidationDataPtr;

SV_DECL_PTRARR_SORT(ScValidationEntries_Impl, ScValidationDataPtr, SC_COND_GROW)

class ScValidationDataList : public ScValidationEntries_Impl
{
public:
        ScValidationDataList() {}
        ScValidationDataList(const ScValidationDataList& rList);
        ScValidationDataList(ScDocument* pNewDoc, const ScValidationDataList& rList);
        ~ScValidationDataList() {}

    void    InsertNew( ScValidationData* pNew )
                { if (!Insert(pNew)) delete pNew; }

    ScValidationData* GetData( sal_uInt32 nKey );

    void    CompileXML();
    void    UpdateReference( UpdateRefMode eUpdateRefMode,
                                const ScRange& rRange, SCsCOL nDx, SCsROW nDy, SCsTAB nDz );
    void    UpdateMoveTab( SCTAB nOldPos, SCTAB nNewPos );

    sal_Bool    operator==( const ScValidationDataList& r ) const;      // for ref-undo
};

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
