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
#ifndef SW_TOX_HXX
#define SW_TOX_HXX

#include <cppuhelper/weakref.hxx>

#include <i18npool/lang.h>
#include <svl/poolitem.hxx>
#include <tools/string.hxx>
#include <osl/diagnose.h>

#include <editeng/svxenum.hxx>
#include <swtypes.hxx>
#include <toxe.hxx>
#include <calbck.hxx>

#include <vector> // #i21237#

namespace com { namespace sun { namespace star {
    namespace text { class XDocumentIndexMark; }
} } }

class SwTOXType;
class SwTOXMark;
class SwTxtTOXMark;
class SwDoc;

class SwTOXMarks : public std::vector<SwTOXMark*> {};

/*--------------------------------------------------------------------
     Description:  Entry of content index, alphabetical index or user defined index
 --------------------------------------------------------------------*/

class SW_DLLPUBLIC SwTOXMark
    : public SfxPoolItem
    , public SwModify
{
    friend void _InitCore();
    friend class SwTxtTOXMark;

    String aAltText;    // Text of caption is different.
    String aPrimaryKey, aSecondaryKey;

    // three more strings for phonetic sorting
    String aTextReading;
    String aPrimaryKeyReading;
    String aSecondaryKeyReading;

    SwTxtTOXMark* pTxtAttr;

    sal_uInt16  nLevel;
    sal_Bool    bAutoGenerated : 1;     // generated using a concordance file
    sal_Bool    bMainEntry : 1;         // main entry emphasized by character style

    ::com::sun::star::uno::WeakReference<
        ::com::sun::star::text::XDocumentIndexMark> m_wXDocumentIndexMark;

    SwTOXMark();                    // to create the dflt. atr. in _InitCore

protected:
    // SwClient
   virtual void Modify( const SfxPoolItem* pOld, const SfxPoolItem* pNew );

public:
    TYPEINFO();   // rtti

    // single argument ctors shall be explicit.
    explicit SwTOXMark( const SwTOXType* pTyp );
    virtual ~SwTOXMark();

    SwTOXMark( const SwTOXMark& rCopy );
    SwTOXMark& operator=( const SwTOXMark& rCopy );

    // "pure virtual methods" of SfxPoolItem
    virtual int             operator==( const SfxPoolItem& ) const;
    virtual SfxPoolItem*    Clone( SfxItemPool* pPool = 0 ) const;

    void InvalidateTOXMark();

    String                  GetText() const;

    inline sal_Bool             IsAlternativeText() const;
    inline const String&    GetAlternativeText() const;

    inline void             SetAlternativeText( const String& rAlt );

    // content or user defined index
    inline void             SetLevel(sal_uInt16 nLevel);
    inline sal_uInt16           GetLevel() const;

    // for alphabetical index only
    inline void             SetPrimaryKey(const String& rStr );
    inline void             SetSecondaryKey(const String& rStr);
    inline void             SetTextReading(const String& rStr);
    inline void             SetPrimaryKeyReading(const String& rStr );
    inline void             SetSecondaryKeyReading(const String& rStr);

    inline const String&    GetPrimaryKey() const;
    inline const String&    GetSecondaryKey() const;
    inline const String&    GetTextReading() const;
    inline const String&    GetPrimaryKeyReading() const;
    inline const String&    GetSecondaryKeyReading() const;

    sal_Bool                    IsAutoGenerated() const {return bAutoGenerated;}
    void                    SetAutoGenerated(sal_Bool bSet) {bAutoGenerated = bSet;}

    sal_Bool                    IsMainEntry() const {return bMainEntry;}
    void                    SetMainEntry(sal_Bool bSet) { bMainEntry = bSet;}

    inline const SwTOXType*    GetTOXType() const;

    const SwTxtTOXMark* GetTxtTOXMark() const   { return pTxtAttr; }
          SwTxtTOXMark* GetTxtTOXMark()         { return pTxtAttr; }

    SW_DLLPRIVATE ::com::sun::star::uno::WeakReference<
        ::com::sun::star::text::XDocumentIndexMark> const& GetXTOXMark() const
            { return m_wXDocumentIndexMark; }
    SW_DLLPRIVATE void SetXTOXMark(::com::sun::star::uno::Reference<
                    ::com::sun::star::text::XDocumentIndexMark> const& xMark)
            { m_wXDocumentIndexMark = xMark; }
    void DeRegister() { GetRegisteredInNonConst()->Remove( this ); }
    void RegisterToTOXType( SwTOXType& rMark );
    static void InsertTOXMarks( SwTOXMarks& aMarks, const SwTOXType& rType );
};

/*--------------------------------------------------------------------
     Description:  index types
 --------------------------------------------------------------------*/

class SwTOXType : public SwModify
{
public:
    SwTOXType(TOXTypes eTyp, const String& aName);

    // @@@ public copy ctor, but no copy assignment?
    SwTOXType(const SwTOXType& rCopy);

    inline  const String&   GetTypeName() const;
    inline TOXTypes         GetType() const;

private:
    String          aName;
    TOXTypes        eType;

    // @@@ public copy ctor, but no copy assignment?
    SwTOXType & operator= (const SwTOXType &);
};

/*--------------------------------------------------------------------
     Description:  Structure of the index lines
 --------------------------------------------------------------------*/

#define FORM_TITLE              0
#define FORM_ALPHA_DELIMITTER   1
#define FORM_PRIMARY_KEY        2
#define FORM_SECONDARY_KEY      3
#define FORM_ENTRY              4

/*
 Pattern structure

 <E#> - entry number                    <E# CharStyleName,PoolId>
 <ET> - entry text                      <ET CharStyleName,PoolId>
 <E>  - entry text and number           <E CharStyleName,PoolId>
 <T>  - tab stop                        <T,,Position,Adjust>
 <C>  - chapter info n = {0, 1, 2, 3, 4 } values of SwChapterFormat <C CharStyleName,PoolId>
 <TX> - text token                      <X CharStyleName,PoolId, TOX_STYLE_DELIMITERTextContentTOX_STYLE_DELIMITER>
 <#>  - Page number                     <# CharStyleName,PoolId>
 <LS> - Link start                      <LS>
 <LE> - Link end                        <LE>
 <A00> - Authority entry field          <A02 CharStyleName, PoolId>
 */

// These enum values are stored and must not be changed!
enum FormTokenType
{
    TOKEN_ENTRY_NO,
    TOKEN_ENTRY_TEXT,
    TOKEN_ENTRY,
    TOKEN_TAB_STOP,
    TOKEN_TEXT,
    TOKEN_PAGE_NUMS,
    TOKEN_CHAPTER_INFO,
    TOKEN_LINK_START,
    TOKEN_LINK_END,
    TOKEN_AUTHORITY,
    TOKEN_END
};

struct SW_DLLPUBLIC SwFormToken
{
    String          sText;
    String          sCharStyleName;
    SwTwips         nTabStopPosition;
    FormTokenType   eTokenType;
    sal_uInt16          nPoolId;
    SvxTabAdjust    eTabAlign;
    sal_uInt16          nChapterFormat;     //SwChapterFormat;
    sal_uInt16          nOutlineLevel;//the maximum permitted outline level in numbering
    sal_uInt16          nAuthorityField;    //enum ToxAuthorityField
    sal_Unicode     cTabFillChar;
    sal_Bool        bWithTab;      // sal_True: do generate tab
                                   // character only the tab stop
                                   // #i21237#

    SwFormToken(FormTokenType eType ) :
        nTabStopPosition(0),
        eTokenType(eType),
        nPoolId(USHRT_MAX),
        eTabAlign( SVX_TAB_ADJUST_LEFT ),
        nChapterFormat(0 /*CF_NUMBER*/),
        nOutlineLevel(MAXLEVEL),   //default to maximum outline level
        nAuthorityField(0 /*AUTH_FIELD_IDENTIFIER*/),
        cTabFillChar(' '),
        bWithTab(sal_True)  // #i21237#
    {}

    String GetString() const;
};

struct SwFormTokenEqualToFormTokenType
{
    FormTokenType eType;

    SwFormTokenEqualToFormTokenType(FormTokenType _eType) : eType(_eType) {}
    bool operator()(const SwFormToken & rToken)
    {
        return rToken.eTokenType == eType;
    }
};

/**
   Functor that appends the string representation of a given token to a string.

   @param _rText    string to append the string representation to
   @param rToken    token whose string representation is appended
*/
struct SwFormTokenToString
{
    String & rText;
    SwFormTokenToString(String & _rText) : rText(_rText) {}
    void operator()(const SwFormToken & rToken) { rText += rToken.GetString(); }
};

/// Vector of tokens.
typedef std::vector<SwFormToken> SwFormTokens;

/**
   Helper class that converts vectors of tokens to strings and vice
   versa.
 */
class SW_DLLPUBLIC SwFormTokensHelper
{
    /// the tokens
    SwFormTokens aTokens;

    /**
       Builds a token from its string representation.

       @sPattern          the whole pattern
       @nCurPatternPos    starting position of the token

       @return the token
     */
    SW_DLLPRIVATE SwFormToken BuildToken( const String & sPattern,
                                          xub_StrLen & nCurPatternPos ) const;

    /**
       Returns the string of a token.

       @param sPattern    the whole pattern
       @param nStt        starting position of the token

       @return   the string representation of the token
    */
    SW_DLLPRIVATE String SearchNextToken( const String & sPattern,
                                          xub_StrLen nStt ) const;

    /**
       Returns the type of a token.

       @param sToken     the string representation of the token
       @param pTokenLen  return parameter the length of the head of the token

       If pTokenLen is non-NULL the length of the token's head is
       written to *pTokenLen

       @return the type of the token
    */
    SW_DLLPRIVATE FormTokenType GetTokenType(const String & sToken,
                                             xub_StrLen * pTokenLen) const;

public:
    /**
       contructor

       @param rTokens       vector of tokens
    */
    SwFormTokensHelper(const SwFormTokens & rTokens) : aTokens(rTokens) {}

    /**
       constructor

       @param rStr   string representation of the tokens
    */
    SwFormTokensHelper(const String & rStr);

    /**
       Returns vector of tokens.

       @return vector of tokens
    */
    const SwFormTokens & GetTokens() const { return aTokens; }
};

class SW_DLLPUBLIC SwForm
{
    SwFormTokens    aPattern[ AUTH_TYPE_END + 1 ]; // #i21237#
    String  aTemplate[ AUTH_TYPE_END + 1 ];

    TOXTypes    eType;
    sal_uInt16      nFormMaxLevel;

    sal_Bool    bGenerateTabPos : 1;
    sal_Bool    bIsRelTabPos : 1;
    sal_Bool    bCommaSeparated : 1;

public:
    SwForm( TOXTypes eTOXType = TOX_CONTENT );
    SwForm( const SwForm& rForm );

    SwForm& operator=( const SwForm& rForm );

    inline void SetTemplate(sal_uInt16 nLevel, const String& rName);
    inline const String&    GetTemplate(sal_uInt16 nLevel) const;

    // #i21237#
    void    SetPattern(sal_uInt16 nLevel, const SwFormTokens& rName);
    void    SetPattern(sal_uInt16 nLevel, const String& rStr);
    const SwFormTokens& GetPattern(sal_uInt16 nLevel) const;

    // fill tab stop positions from template to pattern
    // #i21237#
    void                    AdjustTabStops(SwDoc& rDoc,
                                           sal_Bool bInsertNewTabStops = sal_False);

    inline TOXTypes GetTOXType() const;
    inline sal_uInt16   GetFormMax() const;

    sal_Bool IsRelTabPos() const    {   return bIsRelTabPos; }
    void SetRelTabPos( sal_Bool b ) {   bIsRelTabPos = b;       }

    sal_Bool IsCommaSeparated() const       { return bCommaSeparated;}
    void SetCommaSeparated( sal_Bool b)     { bCommaSeparated = b;}

    static sal_uInt16 GetFormMaxLevel( TOXTypes eType );

    static const sal_Char*  aFormEntry;             // <E>
    static sal_uInt8 nFormEntryLen;                      // 3 characters
    static const sal_Char*  aFormTab;               // <T>
    static sal_uInt8 nFormTabLen;                        // 3 characters
    static const sal_Char*  aFormPageNums;          // <P>
    static sal_uInt8 nFormPageNumsLen;                   // 3 characters
    static const sal_Char* aFormLinkStt;            // <LS>
    static sal_uInt8 nFormLinkSttLen;                    // 4 characters
    static const sal_Char* aFormLinkEnd;            // <LE>
    static sal_uInt8 nFormLinkEndLen;                    // 4 characters
    static const sal_Char*  aFormEntryNum;          // <E#>
    static sal_uInt8 nFormEntryNumLen;                   // 4 characters
    static const sal_Char*  aFormEntryTxt;          // <ET>
    static sal_uInt8 nFormEntryTxtLen;                   // 4 characters
    static const sal_Char*  aFormChapterMark;       // <C>
    static sal_uInt8 nFormChapterMarkLen;                // 3 characters
    static const sal_Char*  aFormText;              // <TX>
    static sal_uInt8 nFormTextLen;                       // 4 characters
    static const sal_Char*  aFormAuth;              // <Axx> xx - decimal enum value
    static sal_uInt8 nFormAuthLen;                       // 3 characters
};

/*--------------------------------------------------------------------
     Description: Content to create indexes of
 --------------------------------------------------------------------*/

typedef sal_uInt16 SwTOXElement;
namespace nsSwTOXElement
{
    const SwTOXElement TOX_MARK             = 1;
    const SwTOXElement TOX_OUTLINELEVEL     = 2;
    const SwTOXElement TOX_TEMPLATE         = 4;
    const SwTOXElement TOX_OLE              = 8;
    const SwTOXElement TOX_TABLE            = 16;
    const SwTOXElement TOX_GRAPHIC          = 32;
    const SwTOXElement TOX_FRAME            = 64;
    const SwTOXElement TOX_SEQUENCE         = 128;
}

typedef sal_uInt16 SwTOIOptions;
namespace nsSwTOIOptions
{
    const SwTOIOptions TOI_SAME_ENTRY       = 1;
    const SwTOIOptions TOI_FF               = 2;
    const SwTOIOptions TOI_CASE_SENSITIVE   = 4;
    const SwTOIOptions TOI_KEY_AS_ENTRY     = 8;
    const SwTOIOptions TOI_ALPHA_DELIMITTER = 16;
    const SwTOIOptions TOI_DASH             = 32;
    const SwTOIOptions TOI_INITIAL_CAPS     = 64;
}

//which part of the caption is to be displayed
enum SwCaptionDisplay
{
    CAPTION_COMPLETE,
    CAPTION_NUMBER,
    CAPTION_TEXT
};

typedef sal_uInt16 SwTOOElements;
namespace nsSwTOOElements
{
    const SwTOOElements TOO_MATH            = 0x01;
    const SwTOOElements TOO_CHART           = 0x02;
    const SwTOOElements TOO_CALC            = 0x08;
    const SwTOOElements TOO_DRAW_IMPRESS    = 0x10;
    const SwTOOElements TOO_OTHER           = 0x80;
}

#define TOX_STYLE_DELIMITER ((sal_Unicode)0x01)

/*--------------------------------------------------------------------
     Description:  Class for all indexes
 --------------------------------------------------------------------*/

class SW_DLLPUBLIC SwTOXBase : public SwClient
{
    SwForm      aForm;              // description of the lines
    String      aName;              // unique name
    String      aTitle;             // title

    String      sMainEntryCharStyle; // name of the character style applied to main index entries

    String      aStyleNames[MAXLEVEL]; // (additional) style names TOX_CONTENT, TOX_USER
    String      sSequenceName;      // FieldTypeName of a caption sequence

    LanguageType    eLanguage;
    String          sSortAlgorithm;

    union {
        sal_uInt16      nLevel;             // consider outline levels
        sal_uInt16      nOptions;           // options of alphabetical index
    } aData;

    sal_uInt16      nCreateType;        // sources to create the index from
    sal_uInt16      nOLEOptions;        // OLE sources
    SwCaptionDisplay eCaptionDisplay;   //
    sal_Bool        bProtected : 1;         // index protected ?
    sal_Bool        bFromChapter : 1;       // create from chapter or document
    sal_Bool        bFromObjectNames : 1;   // create a table or object index
                                    // from the names rather than the caption
    sal_Bool        bLevelFromChapter : 1; // User index: get the level from the source chapter
public:
    SwTOXBase( const SwTOXType* pTyp, const SwForm& rForm,
               sal_uInt16 nCreaType, const String& rTitle );
    SwTOXBase( const SwTOXBase& rCopy, SwDoc* pDoc = 0 );
    virtual ~SwTOXBase();

    virtual bool GetInfo( SfxPoolItem& rInfo ) const;

    // a kind of CopyCtor - check if the TOXBase is at TOXType of the doc.
    // If not, so create it an copy all other used things. The return is this
    SwTOXBase& CopyTOXBase( SwDoc*, const SwTOXBase& );

    const SwTOXType*    GetTOXType() const; //

    sal_uInt16              GetCreateType() const;      // creation types

    const String&       GetTOXName() const {return aName;}
    void                SetTOXName(const String& rSet) {aName = rSet;}

    const String&       GetTitle() const;           // Title
    const String&       GetTypeName() const;        // Name
    const SwForm&       GetTOXForm() const;         // description of the lines

    void                SetCreate(sal_uInt16);
    void                SetTitle(const String& rTitle);
    void                SetTOXForm(const SwForm& rForm);

    TOXTypes            GetType() const;

    const String&       GetMainEntryCharStyle() const {return sMainEntryCharStyle;}
    void                SetMainEntryCharStyle(const String& rSet)  {sMainEntryCharStyle = rSet;}

    // content index only
    inline void             SetLevel(sal_uInt16);                   // consider outline level
    inline sal_uInt16           GetLevel() const;

    // alphabetical index only
    inline sal_uInt16           GetOptions() const;                 // alphabetical index options
    inline void             SetOptions(sal_uInt16 nOpt);

    // index of objects
    sal_uInt16      GetOLEOptions() const {return nOLEOptions;}
    void        SetOLEOptions(sal_uInt16 nOpt) {nOLEOptions = nOpt;}

    // index of objects

    // user defined index only
    inline void             SetTemplateName(const String& rName); // Absatzlayout beachten

    const String&           GetStyleNames(sal_uInt16 nLevel) const
                                {
                                OSL_ENSURE( nLevel < MAXLEVEL, "Which level?");
                                return aStyleNames[nLevel];
                                }
    void                    SetStyleNames(const String& rSet, sal_uInt16 nLevel)
                                {
                                OSL_ENSURE( nLevel < MAXLEVEL, "Which level?");
                                aStyleNames[nLevel] = rSet;
                                }
    sal_Bool                    IsFromChapter() const { return bFromChapter;}
    void                    SetFromChapter(sal_Bool bSet) { bFromChapter = bSet;}

    sal_Bool                    IsFromObjectNames() const {return bFromObjectNames;}
    void                    SetFromObjectNames(sal_Bool bSet) {bFromObjectNames = bSet;}

    sal_Bool                    IsLevelFromChapter() const {return bLevelFromChapter;}
    void                    SetLevelFromChapter(sal_Bool bSet) {bLevelFromChapter = bSet;}

    sal_Bool                    IsProtected() const { return bProtected; }
    void                    SetProtected(sal_Bool bSet) { bProtected = bSet; }

    const String&           GetSequenceName() const {return sSequenceName;}
    void                    SetSequenceName(const String& rSet) {sSequenceName = rSet;}

    SwCaptionDisplay        GetCaptionDisplay() const { return eCaptionDisplay;}
    void                    SetCaptionDisplay(SwCaptionDisplay eSet) {eCaptionDisplay = eSet;}

    bool                    IsTOXBaseInReadonly() const;

    const SfxItemSet*       GetAttrSet() const;
    void                    SetAttrSet( const SfxItemSet& );

    LanguageType    GetLanguage() const {return eLanguage;}
    void            SetLanguage(LanguageType nLang)  {eLanguage = nLang;}

    const String&   GetSortAlgorithm()const {return sSortAlgorithm;}
    void            SetSortAlgorithm(const String& rSet) {sSortAlgorithm = rSet;}
    // #i21237#
    void AdjustTabStops(SwDoc & rDoc, sal_Bool bDefaultRightTabStop);
    SwTOXBase&          operator=(const SwTOXBase& rSource);
    void RegisterToTOXType( SwTOXType& rMark );
};


/*--------------------------------------------------------------------
     Description:  Inlines
 --------------------------------------------------------------------*/

//
//SwTOXMark
//
inline const String& SwTOXMark::GetAlternativeText() const
    {   return aAltText;    }

inline const SwTOXType* SwTOXMark::GetTOXType() const
    { return (SwTOXType*)GetRegisteredIn(); }

inline sal_Bool SwTOXMark::IsAlternativeText() const
    { return aAltText.Len() > 0; }

inline void SwTOXMark::SetAlternativeText(const String& rAlt)
{
    aAltText = rAlt;
}

inline void SwTOXMark::SetLevel( sal_uInt16 nLvl )
{
    OSL_ENSURE( !GetTOXType() || GetTOXType()->GetType() != TOX_INDEX, "Falscher Feldtyp");
    nLevel = nLvl;
}

inline void SwTOXMark::SetPrimaryKey( const String& rKey )
{
    OSL_ENSURE( GetTOXType()->GetType() == TOX_INDEX, "Falscher Feldtyp");
    aPrimaryKey = rKey;
}

inline void SwTOXMark::SetSecondaryKey( const String& rKey )
{
    OSL_ENSURE(GetTOXType()->GetType() == TOX_INDEX, "Falscher Feldtyp");
    aSecondaryKey = rKey;
}

inline void SwTOXMark::SetTextReading( const String& rTxt )
{
    OSL_ENSURE(GetTOXType()->GetType() == TOX_INDEX, "Falscher Feldtyp");
    aTextReading = rTxt;
}

inline void SwTOXMark::SetPrimaryKeyReading( const String& rKey )
{
    OSL_ENSURE(GetTOXType()->GetType() == TOX_INDEX, "Falscher Feldtyp");
    aPrimaryKeyReading = rKey;
}

inline void SwTOXMark::SetSecondaryKeyReading( const String& rKey )
{
    OSL_ENSURE(GetTOXType()->GetType() == TOX_INDEX, "Falscher Feldtyp");
    aSecondaryKeyReading = rKey;
}

inline sal_uInt16 SwTOXMark::GetLevel() const
{
    OSL_ENSURE( !GetTOXType() || GetTOXType()->GetType() != TOX_INDEX, "Falscher Feldtyp");
    return nLevel;
}

inline const String& SwTOXMark::GetPrimaryKey() const
{
    OSL_ENSURE(GetTOXType()->GetType() == TOX_INDEX, "Falscher Feldtyp");
    return aPrimaryKey;
}

inline const String& SwTOXMark::GetSecondaryKey() const
{
    OSL_ENSURE(GetTOXType()->GetType() == TOX_INDEX, "Falscher Feldtyp");
    return aSecondaryKey;
}

inline const String& SwTOXMark::GetTextReading() const
{
    OSL_ENSURE(GetTOXType()->GetType() == TOX_INDEX, "Falscher Feldtyp");
    return aTextReading;
}

inline const String& SwTOXMark::GetPrimaryKeyReading() const
{
    OSL_ENSURE(GetTOXType()->GetType() == TOX_INDEX, "Falscher Feldtyp");
    return aPrimaryKeyReading;
}

inline const String& SwTOXMark::GetSecondaryKeyReading() const
{
    OSL_ENSURE(GetTOXType()->GetType() == TOX_INDEX, "Falscher Feldtyp");
    return aSecondaryKeyReading;
}

//
//SwForm
//
inline void SwForm::SetTemplate(sal_uInt16 nLevel, const String& rTemplate)
{
    OSL_ENSURE(nLevel < GetFormMax(), "Index >= FORM_MAX");
    aTemplate[nLevel] = rTemplate;
}

inline const String& SwForm::GetTemplate(sal_uInt16 nLevel) const
{
    OSL_ENSURE(nLevel < GetFormMax(), "Index >= FORM_MAX");
    return aTemplate[nLevel];
}

inline TOXTypes SwForm::GetTOXType() const
{
    return eType;
}

inline sal_uInt16 SwForm::GetFormMax() const
{
    return nFormMaxLevel;
}


//
//SwTOXType
//
inline const String& SwTOXType::GetTypeName() const
    {   return aName;   }

inline TOXTypes SwTOXType::GetType() const
    {   return eType;   }

//
// SwTOXBase
//
inline const SwTOXType* SwTOXBase::GetTOXType() const
    { return (SwTOXType*)GetRegisteredIn(); }

inline sal_uInt16 SwTOXBase::GetCreateType() const
    { return nCreateType; }

inline const String& SwTOXBase::GetTitle() const
    { return aTitle; }

inline const String& SwTOXBase::GetTypeName() const
    { return GetTOXType()->GetTypeName();  }

inline const SwForm& SwTOXBase::GetTOXForm() const
    { return aForm; }

inline void SwTOXBase::AdjustTabStops(SwDoc & rDoc, sal_Bool bDefaultRightTabStop)
{
    aForm.AdjustTabStops(rDoc, bDefaultRightTabStop);
}

inline void SwTOXBase::SetCreate(sal_uInt16 nCreate)
    { nCreateType = nCreate; }

inline void SwTOXBase::SetTOXForm(const SwForm& rForm)
    {  aForm = rForm; }

inline TOXTypes SwTOXBase::GetType() const
    { return GetTOXType()->GetType(); }

inline void SwTOXBase::SetLevel(sal_uInt16 nLev)
{
    OSL_ENSURE(GetTOXType()->GetType() != TOX_INDEX, "Falscher Feldtyp");
    aData.nLevel = nLev;
}

inline sal_uInt16 SwTOXBase::GetLevel() const
{
    OSL_ENSURE(GetTOXType()->GetType() != TOX_INDEX, "Falscher Feldtyp");
    return aData.nLevel;
}

inline void SwTOXBase::SetTemplateName(const String& rName)
{
    OSL_FAIL("SwTOXBase::SetTemplateName obsolete");
    aStyleNames[0] = rName;
}

inline sal_uInt16 SwTOXBase::GetOptions() const
{
    OSL_ENSURE(GetTOXType()->GetType() == TOX_INDEX, "Falscher Feldtyp");
    return aData.nOptions;
}

inline void SwTOXBase::SetOptions(sal_uInt16 nOpt)
{
    OSL_ENSURE(GetTOXType()->GetType() == TOX_INDEX, "Falscher Feldtyp");
    aData.nOptions = nOpt;
}


#endif  // SW_TOX_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
