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
#ifndef SW_EXPFLD_HXX
#define SW_EXPFLD_HXX

#include "swdllapi.h"
#include <fldbas.hxx>
#include <cellfml.hxx>
#include <set>
#include <vector>

class SfxPoolItem;
class SwTxtNode;
class SwFrm;
struct SwPosition;
class SwTxtFld;
class SwDoc;
class SwFmtFld;
class _SetGetExpFlds;
class SwEditShell;

/// Forward declaration: get "BodyTxtNode" for exp.fld in Fly's headers/footers/footnotes.
const SwTxtNode* GetBodyTxtNode( const SwDoc& pDoc, SwPosition& rPos,
                                 const SwFrm& rFrm );

void ReplacePoint(String& sTmpName, bool bWithCommandType = false);

struct _SeqFldLstElem
{
    String sDlgEntry;
    sal_uInt16 nSeqNo;

    _SeqFldLstElem( const String& rStr, sal_uInt16 nNo )
        : sDlgEntry( rStr ), nSeqNo( nNo )
    {}
};

class SW_DLLPUBLIC SwSeqFldList
{
    std::vector<_SeqFldLstElem*> maData;
public:
    ~SwSeqFldList()
    {
        for( std::vector<_SeqFldLstElem*>::const_iterator it = maData.begin(); it != maData.end(); ++it )
            delete *it;
    }

    bool InsertSort(_SeqFldLstElem* pNew);
    bool SeekEntry(const _SeqFldLstElem& rNew, sal_uInt16* pPos) const;

    sal_uInt16 Count() { return maData.size(); }
    _SeqFldLstElem* operator[](sal_uInt16 nIndex) { return maData[nIndex]; }
    const _SeqFldLstElem* operator[](sal_uInt16 nIndex) const { return maData[nIndex]; }
    void Clear() { maData.clear(); }
};

class SwGetExpFieldType : public SwValueFieldType
{
public:
        SwGetExpFieldType(SwDoc* pDoc);
        virtual SwFieldType*    Copy() const;

        /** Overlay, because get-field cannot be changed and therefore
         does not need to be updated. Update at changing of set-values! */
protected:
   virtual void Modify( const SfxPoolItem* pOld, const SfxPoolItem *pNew );
};

class SW_DLLPUBLIC SwGetExpField : public SwFormulaField
{
    String          sExpand;
    bool            bIsInBodyTxt;
    sal_uInt16          nSubType;

    bool            bLateInitialization; // #i82544#

    virtual String              Expand() const;
    virtual SwField*            Copy() const;

public:
    SwGetExpField( SwGetExpFieldType*, const String& rFormel,
                   sal_uInt16 nSubType = nsSwGetSetExpType::GSE_EXPR, sal_uLong nFmt = 0);

    virtual void                SetValue( const double& rVal );
    virtual void                SetLanguage(sal_uInt16 nLng);

    inline const String&        GetExpStr() const;
    inline void                 ChgExpStr(const String& rExpand);

    /// Called by formating.
    inline bool                 IsInBodyTxt() const;

    /// Set by UpdateExpFlds where node position is known.
    inline void                 ChgBodyTxtFlag( bool bIsInBody );

    /** For fields in header/footer/footnotes/flys:
     Only called by formating!! */
    void                        ChangeExpansion( const SwFrm&, const SwTxtFld& );

    virtual String      GetFieldName() const;

    /// Change formula.
    virtual rtl::OUString GetPar2() const;
    virtual void        SetPar2(const rtl::OUString& rStr);

    virtual sal_uInt16  GetSubType() const;
    virtual void        SetSubType(sal_uInt16 nType);
    virtual bool        QueryValue( com::sun::star::uno::Any& rVal, sal_uInt16 nWhich ) const;
    virtual bool        PutValue( const com::sun::star::uno::Any& rVal, sal_uInt16 nWhich );

    static sal_uInt16   GetReferenceTextPos( const SwFmtFld& rFmt, SwDoc& rDoc, unsigned nHint = 0);
    // #i82544#
    void                SetLateInitialization() { bLateInitialization = true;}
};

inline void SwGetExpField::ChgExpStr(const String& rExpand)
    { sExpand = rExpand;}

inline const String& SwGetExpField::GetExpStr() const
    { return sExpand;   }

 /// Called by formating.
inline bool SwGetExpField::IsInBodyTxt() const
    { return bIsInBodyTxt; }

 /// Set by UpdateExpFlds where node position is known.
inline void SwGetExpField::ChgBodyTxtFlag( bool bIsInBody )
    { bIsInBodyTxt = bIsInBody; }

class SwSetExpField;

class SW_DLLPUBLIC SwSetExpFieldType : public SwValueFieldType
{
    rtl::OUString sName;
    const SwNode* pOutlChgNd;
    String      sDelim;
    sal_uInt16      nType;
    sal_uInt8       nLevel;
    sal_Bool        bDeleted;

protected:
   virtual void Modify( const SfxPoolItem* pOld, const SfxPoolItem *pNew );

public:
    SwSetExpFieldType( SwDoc* pDoc, const String& rName,
                        sal_uInt16 nType = nsSwGetSetExpType::GSE_EXPR );
    virtual SwFieldType*    Copy() const;
    virtual const rtl::OUString&   GetName() const;

    inline void             SetType(sal_uInt16 nTyp);
    inline sal_uInt16       GetType() const;

    void                    SetSeqFormat(sal_uLong nFormat);
    sal_uLong               GetSeqFormat();

    sal_Bool                IsDeleted() const       { return bDeleted; }
    void                    SetDeleted( sal_Bool b )    { bDeleted = b; }

    /// Overlay, because set-field takes care for its being updated by itself.
    inline const rtl::OUString& GetSetRefName() const;

    sal_uInt16 SetSeqRefNo( SwSetExpField& rFld );

    sal_uInt16 GetSeqFldList( SwSeqFldList& rList );
    String MakeSeqName( sal_uInt16 nSeqNo );

    /// Number sequence fields chapterwise if required.
    const String& GetDelimiter() const      { return sDelim; }
    void SetDelimiter( const String& s )    { sDelim = s; }
    sal_uInt8 GetOutlineLvl() const                 { return nLevel; }
    void SetOutlineLvl( sal_uInt8 n )           { nLevel = n; }
    void SetChapter( SwSetExpField& rFld, const SwNode& rNd );

    /** Member only for SwDoc::UpdateExpFld.
     It is needed only at runtime of sequence field types! */
    const SwNode* GetOutlineChgNd() const   { return pOutlChgNd; }
    void SetOutlineChgNd( const SwNode* p ) { pOutlChgNd = p; }

    virtual bool        QueryValue( com::sun::star::uno::Any& rVal, sal_uInt16 nWhich ) const;
    virtual bool        PutValue( const com::sun::star::uno::Any& rVal, sal_uInt16 nWhich );
};

inline void SwSetExpFieldType::SetType( sal_uInt16 nTyp )
{
        nType = nTyp;
        EnableFormat( !(nType & (nsSwGetSetExpType::GSE_SEQ|nsSwGetSetExpType::GSE_STRING)));
}

inline sal_uInt16 SwSetExpFieldType::GetType() const
    { return nType;   }

inline const rtl::OUString& SwSetExpFieldType::GetSetRefName() const
    { return sName; }


class SW_DLLPUBLIC SwSetExpField : public SwFormulaField
{
    String          sExpand;
    String          aPText;
    String          aSeqText;
    sal_Bool            bInput;
    sal_uInt16          nSeqNo;
    sal_uInt16          nSubType;

    virtual String              Expand() const;
    virtual SwField*            Copy() const;

public:
    SwSetExpField(SwSetExpFieldType*, const String& rFormel, sal_uLong nFmt = 0);

    virtual void                SetValue( const double& rVal );

    inline const String&        GetExpStr() const;

    inline void                 ChgExpStr( const String& rExpand );

    inline void                 SetPromptText(const String& rStr);
    inline const                String& GetPromptText() const;

    inline void                 SetInputFlag(sal_Bool bInp);
    inline sal_Bool                 GetInputFlag() const;

    virtual String              GetFieldName() const;

    virtual sal_uInt16              GetSubType() const;
    virtual void                SetSubType(sal_uInt16 nType);

    inline sal_Bool                 IsSequenceFld() const;

    /// Logical number, sequence fields.
    inline void                 SetSeqNumber( sal_uInt16 n )    { nSeqNo = n; }
    inline sal_uInt16           GetSeqNumber() const        { return nSeqNo; }

    /// Query name only.
    virtual const rtl::OUString& GetPar1()   const;

    /// Query formula.
    virtual rtl::OUString       GetPar2()   const;
    virtual void                SetPar2(const rtl::OUString& rStr);
    virtual bool        QueryValue( com::sun::star::uno::Any& rVal, sal_uInt16 nWhich ) const;
    virtual bool        PutValue( const com::sun::star::uno::Any& rVal, sal_uInt16 nWhich );
};

inline const String& SwSetExpField::GetExpStr() const
    { return sExpand;       }

inline void SwSetExpField::ChgExpStr( const String& rExpand )
    { sExpand = rExpand;    }

inline void  SwSetExpField::SetPromptText(const String& rStr)
    { aPText = rStr;        }

inline const String& SwSetExpField::GetPromptText() const
    { return aPText;        }

inline void SwSetExpField::SetInputFlag(sal_Bool bInp)
    { bInput = bInp; }

inline sal_Bool SwSetExpField::GetInputFlag() const
    { return bInput; }

inline sal_Bool SwSetExpField::IsSequenceFld() const
    { return 0 != (nsSwGetSetExpType::GSE_SEQ & ((SwSetExpFieldType*)GetTyp())->GetType()); }


class SwInputFieldType : public SwFieldType
{
    SwDoc* pDoc;
public:
    SwInputFieldType( SwDoc* pDoc );

    virtual SwFieldType* Copy() const;

    SwDoc* GetDoc() const { return pDoc; }
};


class SW_DLLPUBLIC SwInputField : public SwField
{
    rtl::OUString aContent;
    String  aPText;
    String  aHelp;
    String  aToolTip;
    sal_uInt16  nSubType;

    virtual String          Expand() const;
    virtual SwField*        Copy() const;

public:
    /// Direct input via dialog; delete old value.
    SwInputField(SwInputFieldType*, const String& rContent ,
                 const String& rPrompt, sal_uInt16 nSubType = 0,
                 sal_uLong nFmt = 0);

    virtual String          GetFieldName() const;

    /// Content
    virtual const rtl::OUString&   GetPar1() const;
    virtual void            SetPar1(const rtl::OUString& rStr);

    /// aPromptText
    virtual rtl::OUString   GetPar2() const;
    virtual void            SetPar2(const rtl::OUString& rStr);

    virtual String          GetHelp() const;
    virtual void            SetHelp(const String & rStr);

    virtual String          GetToolTip() const;
    virtual void            SetToolTip(const String & rStr);

    virtual sal_Bool            isFormField() const;

    virtual sal_uInt16          GetSubType() const;
    virtual void            SetSubType(sal_uInt16 nSub);
    virtual bool        QueryValue( com::sun::star::uno::Any& rVal, sal_uInt16 nWhich ) const;
    virtual bool        PutValue( const com::sun::star::uno::Any& rVal, sal_uInt16 nWhich );
};

/*--------------------------------------------------------------------
    Description: Sorted list of input fields and DropDown fields
 --------------------------------------------------------------------*/

class SwInputFieldList
{
public:
    SwInputFieldList( SwEditShell* pShell, sal_Bool bBuildTmpLst = sal_False );
    ~SwInputFieldList();

    sal_uInt16      Count() const;
    SwField*    GetField(sal_uInt16 nId);

    void        GotoFieldPos(sal_uInt16 nId);
    void        PushCrsr();
    void        PopCrsr();

    /** Put all that are new into SortLst for updating. @return count.
     (For Glossary: only update its input-fields).
     Compare TmpLst with current fields. */
    sal_uInt16      BuildSortLst();

    void        RemoveUnselectedFlds();

private:
    SwEditShell*              pSh;
    _SetGetExpFlds*           pSrtLst;
    std::set<const SwTxtFld*> aTmpLst;
};

 /// Implementation in tblcalc.cxx.
class SwTblFieldType : public SwValueFieldType
{
public:
    SwTblFieldType(SwDoc* pDocPtr);
    virtual SwFieldType* Copy() const;
};


class SwTblField : public SwValueField, public SwTableFormula
{
    String      sExpand;
    sal_uInt16      nSubType;

    virtual String      Expand() const;
    virtual SwField*    Copy() const;

    /// Search TextNode containing the field.
    virtual const SwNode* GetNodeOfFormula() const;

    String GetCommand();

public:
    SwTblField( SwTblFieldType*, const String& rFormel,
                sal_uInt16 nSubType = 0, sal_uLong nFmt = 0);

    virtual void        SetValue( const double& rVal );
    virtual sal_uInt16      GetSubType() const;
    virtual void        SetSubType(sal_uInt16 nType);

    const String&       GetExpStr() const               { return sExpand; }
    void                ChgExpStr(const String& rStr)   { sExpand = rStr; }

    void                CalcField( SwTblCalcPara& rCalcPara );

    virtual String      GetFieldName() const;

    /// The formula.
    virtual rtl::OUString GetPar2()   const;
    virtual void        SetPar2(const rtl::OUString& rStr);
    virtual bool        QueryValue( com::sun::star::uno::Any& rVal, sal_uInt16 nWhich ) const;
    virtual bool        PutValue( const com::sun::star::uno::Any& rVal, sal_uInt16 nWhich );
};


#endif // SW_EXPFLD_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
