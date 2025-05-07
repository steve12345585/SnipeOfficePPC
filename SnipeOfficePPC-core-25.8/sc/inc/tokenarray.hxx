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

#ifndef SC_TOKENARRAY_HXX
#define SC_TOKENARRAY_HXX

#include "formula/token.hxx"
#include "scmatrix.hxx"
#include <tools/solar.h>
#include "scdllapi.h"
#include <formula/tokenarray.hxx>

struct ScRawToken;
struct ScSingleRefData;
struct ScComplexRefData;
class ScMatrix;

class SC_DLLPUBLIC ScTokenArray : public formula::FormulaTokenArray
{
    friend class ScCompiler;
    bool                    ImplGetReference( ScRange& rRange, bool bValidOnly ) const;

public:
    ScTokenArray();
    /// Assignment with references to ScToken entries (not copied!)
    ScTokenArray( const ScTokenArray& );
   virtual ~ScTokenArray();
    ScTokenArray* Clone() const;    /// True copy!

    /// Exactly and only one range (valid or deleted)
    bool    IsReference( ScRange& rRange ) const;
    /// Exactly and only one valid range (no #REF!s)
    bool    IsValidReference( ScRange& rRange ) const;


                            /** Determines the extent of direct adjacent
                                references. Only use with real functions, e.g.
                                GetOuterFuncOpCode() == ocSum ! */
    bool                    GetAdjacentExtendOfOuterFuncRefs( SCCOLROW& nExtend,
                                const ScAddress& rPos, ScDirection );

    formula::FormulaToken* AddRawToken( const ScRawToken& );
    virtual bool AddFormulaToken(const com::sun::star::sheet::FormulaToken& _aToken,formula::ExternalReferenceHelper* _pRef);
    virtual formula::FormulaToken* AddOpCode( OpCode eCode );
    /** ScSingleRefToken with ocPush. */
    formula::FormulaToken* AddSingleReference( const ScSingleRefData& rRef );
    /** ScSingleRefOpToken with ocMatRef. */
    formula::FormulaToken* AddMatrixSingleReference( const ScSingleRefData& rRef );
    formula::FormulaToken* AddDoubleReference( const ScComplexRefData& rRef );
    formula::FormulaToken* AddRangeName( sal_uInt16 n, bool bGlobal );
    formula::FormulaToken* AddDBRange( sal_uInt16 n );
    formula::FormulaToken* AddExternalName( sal_uInt16 nFileId, const String& rName );
    formula::FormulaToken* AddExternalSingleReference( sal_uInt16 nFileId, const String& rTabName, const ScSingleRefData& rRef );
    formula::FormulaToken* AddExternalDoubleReference( sal_uInt16 nFileId, const String& rTabName, const ScComplexRefData& rRef );
    formula::FormulaToken* AddMatrix( const ScMatrixRef& p );
    /** ScSingleRefOpToken with ocColRowName. */
    formula::FormulaToken* AddColRowName( const ScSingleRefData& rRef );
    virtual formula::FormulaToken* MergeArray( );

    /** Merge very last SingleRef+ocRange+SingleRef combination into DoubleRef
        and adjust pCode array, or do nothing if conditions not met.
        Unconditionally returns last token from the resulting pCode array, or
        NULL if there is no pCode (which actually would be caller's fault). */
    formula::FormulaToken* MergeRangeReference( const ScAddress & rPos );

    /// Assignment with references to ScToken entries (not copied!)
    ScTokenArray& operator=( const ScTokenArray& );

    /// Make 3D references point to old referenced position even if relative
    void            ReadjustRelative3DReferences(
                                const ScAddress& rOldPos,
                                const ScAddress& rNewPos );

    /**
     * Make all absolute references external references pointing to the old document
     *
     * @param pOldDoc old document
     * @param pNewDoc new document
     * @param rPos position of the cell to determine if the reference is in the copied area
     * @param bRangeName set for range names, range names have special handling for absolute sheet ref + relative col/row ref
     */
    void ReadjustAbsolute3DReferences( const ScDocument* pOldDoc, const ScDocument* pNewDoc, const ScAddress& rPos, bool bRangeName = false );

    /**
     * Make all absolute references pointing to the copied range if the range is copied too
     * @param bCheckCopyArea should references pointing into the copy area be adjusted independently from being absolute, should be true only for copy&paste between documents
     */
    void AdjustAbsoluteRefs( const ScDocument* pOldDoc, const ScAddress& rOldPos, const ScAddress& rNewPos, bool bRangeName = false, bool bCheckCopyArea = false );
};

#endif // SC_TOKENARRAY_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
